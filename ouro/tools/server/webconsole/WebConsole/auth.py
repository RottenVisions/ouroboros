# -*- coding: utf-8 -*-

import functools, hashlib
from django.shortcuts import render
from django.http import HttpResponse, HttpResponseBadRequest, HttpResponseRedirect
from django.core.exceptions import ObjectDoesNotExist
from django.contrib.auth.hashers import check_password, make_password
from django.conf import settings

from .models import AuthUser

def login_check(func):
	"""
	decorator.
	Check whether you have login
	"""
	@functools.wraps(func)
	def wrapper(request, *args, **kws):
		if not request.session.get( "logined", False ):
			return HttpResponseRedirect( "/wc/login" )

		# Managers want to into the normal user page you must first log out
		if request.session.get( "auth_is_admin" ):
			return HttpResponseRedirect( "/wc/logout" )
		return func( request, *args, **kws )

	return wrapper

def admin_check(func):
	"""
	decorator.
	Check whether you have login
	"""
	@functools.wraps(func)
	def wrapper(request, *args, **kws):
		if not request.session.get( "logined", False ):
			return HttpResponseRedirect( "/wc/login" )

		# Ordinary users want to go to the User Management page, you must first log out
		if not request.session.get( "auth_is_admin" ):
			return HttpResponseRedirect( "/wc/logout" )
		return func( request, *args, **kws )

	return wrapper


def login(request):
	"""
	Account login page
	"""
	if request.session.get( "logined", False ):
			return HttpResponseRedirect( "/wc/index" )

	name = request.POST.get("name", None)
	password = request.POST.get("password", None)
	context = { "next_uri" : "" }

	if name is None or password is None:
		return render(request, "WebConsole/login.html", context)

	if not name or not password:
		context["error"] = "Error: missing username or password parameters"
		return render(request, "WebConsole/login.html", context)

	try:
		user = AuthUser.objects.get(name = name)
	except ObjectDoesNotExist:
		# If you can't find a specific account number, and account password are the default values, then enter the Management page
		if name == "Admin" and password == "123456":
			pwd = make_password( "123456" )
			user = AuthUser( name = name, show_name = name, password = pwd, sys_user = "UNKNOWN", sys_uid = -1 )
			user.save()
		else:
			context["error"] = "Error: account does not exist"
			return render(request, "WebConsole/login.html", context)

	if not check_password( password, user.password ):
		context["error"] = "Error: password incorrect"
		return render(request, "WebConsole/login.html", context)

	request.session["logined"] = True
	request.session["auth_id"] = user.id
	request.session["auth_user"] = name
	request.session["auth_show_name"] = user.show_name
	request.session["sys_user"] = user.sys_user
	request.session["sys_uid"] = user.sys_uid
	request.session["auth_is_admin"] = (name == "Admin")

	if 	request.session["auth_is_admin"]:
		return HttpResponseRedirect( "/wc/user/manage" )
	else:
		request.session["ouro_root"] = user.ouro_root
		request.session["ouro_res_path"] = user.ouro_res_path
		request.session["ouro_bin_path"] = user.ouro_bin_path
		return HttpResponseRedirect( "/wc/index" )


def logout(request):
	"""
	Account logout page
	"""
	request.session.clear()

	return HttpResponseRedirect( "/wc/index" )

@admin_check
def user_manage( request ):
	"""
	Account Management
	"""
	users = AuthUser.objects.all()

	context = { "Users" : users }
	return render(request, "User/manage_user.html", context)

@admin_check
def user_add( request ):
	"""
	Zo add new users
	"""
	html_template = "User/new_user.html"
	POST = request.POST
	context = {}
	if POST.get( "commit", "" ):
		username = POST.get("username", "")
		showname = POST.get("showname", "")
		password1 = POST.get("password1", "")
		password2 = POST.get("password2", "")
		sysuser = POST.get("sysuser", "")
		sysuid = POST.get("sysuid", "")
		ouro_root = POST.get("ouro-root", "")
		ouro_res_path = POST.get("ouro-res-path", "")
		ouro_bin_path = POST.get("ouro-bin-path", "")

		if not username:
			context["error"] = "An invalid account name"
			return render(request, html_template, context)

		if not showname:
			context["error"] = "Invalid display name"
			return render(request, html_template, context)

		if not password1:
			context["error"] = "Password can't be empty"
			return render(request, html_template, context)

		if password1 != password2:
			context["error"] = "Password is incorrect"
			return render(request, html_template, context)

		try:
			sysuid = int( POST.get("sysuid", "0") )
		except:
			context["error"] = "Invalid uid"
			return render(request, html_template, context)

		if not sysuser:
			context["error"] = "The system user name cannot be empty"
			return render(request, html_template, context)

		if sysuid < 0:
			context["error"] = "Invalid uid"
			return render(request, html_template, context)

		if AuthUser.objects.filter(name = username).exists():
			context["error"] = "Account name“%s”already exists" % username
			return render(request, html_template, context)

		pwd = make_password( password1 )
		user = AuthUser( name = username, show_name = showname, password = pwd, sys_user = sysuser, sys_uid = sysuid, ouro_root = ouro_root, ouro_res_path = ouro_res_path, ouro_bin_path = ouro_bin_path )
		user.save()

		return HttpResponseRedirect( "/wc/user/manage" )
	else:
		return render(request, html_template, context)

@admin_check
def user_delete( request ):
	"""
	Delete the user
	"""
	try:
		id = int( request.GET.get( "id", "0" ) )
	except:
		return HttpResponseRedirect( "/wc/user/manage" )

	AuthUser.objects.filter(pk = id).delete()
	return HttpResponseRedirect( "/wc/user/manage" )

@admin_check
def change_pwd( request ):
	"""
	Modify the account password
	"""
	html_template = "User/change_pwd.html"

	name = request.POST.get( "username", "" )
	id = request.POST.get( "id", "" )
	commit = request.POST.get( "commit", "" )

	if not id or not name:
		return HttpResponseRedirect( "/wc/user/manage" )

	context = {}
	if not commit:
		return render(request, html_template, context)

	password1 = request.POST.get( "password1", "" )
	password2 = request.POST.get( "password2", "" )
	if not password1:
		context["error"] = "Password can't be empty"
		return render(request, html_template, context)

	if password1 != password2:
		context["error"] = "Password is incorrect"
		return render(request, html_template, context)

	if not AuthUser.objects.filter(pk = id).exists():
		context["error"] = "Account name“%s”does not exist" % name
		return render(request, html_template, context)

	pwd = make_password( password1 )
	AuthUser.objects.filter(pk = id).update( password = pwd )
	context["error"] = "Modify complete"
	return render(request, html_template, context)

@admin_check
def change_user(request,userID):
	html_template = "User/change_user.html"
	POST = request.POST
	commit = POST.get( "commit", "" )
	id = userID
	if id =="" or not id:
		context["error"] = "Invalid ID"
		return render(request, html_template, context)
	user = AuthUser.objects.get(pk = id)
	context = {}
	if commit == "" or not commit:
		showname     = user.show_name
		sysuser      = user.sys_user
		sysuid       = user.sys_uid
		ouro_root     = user.ouro_root
		ouro_res_path = user.ouro_res_path
		ouro_bin_path = user.ouro_bin_path

		context = {
			"showname": showname,
			"sysuser" : sysuser,
			"sysuid"  : sysuid,
			"ouro_root": ouro_root,
			"ouro_res_path" : ouro_res_path,
			"ouro_bin_path" : ouro_bin_path
		}
		return render(request, html_template, context)

	showname = POST.get("showname", "")
	sysuser = POST.get("sysuser", "")
	sysuid = POST.get("sysuid", "")
	ouro_root = POST.get("ouro-root", "")
	ouro_res_path = POST.get("ouro-res-path", "")
	ouro_bin_path = POST.get("ouro-bin-path", "")

	if not showname:
		context["error"] = "Invalid nickname"
		return render(request, html_template, context)

	try:
		sysuid = int( POST.get("sysuid", "0") )
	except:
		context["error"] = "Invalid uid"
		return render(request, html_template, context)

	if not sysuser:
		context["error"] = "The system user name cannot be empty"
		return render(request, html_template, context)

	if sysuid < 0:
		context["error"] = "Invalid uid"
		return render(request, html_template, context)
	AuthUser.objects.filter(pk = id).update(show_name = showname, sys_user = sysuser, sys_uid = sysuid, ouro_root = ouro_root, ouro_res_path = ouro_res_path, ouro_bin_path = ouro_bin_path )
	context["error"] = "Modify complete"
	return render(request, html_template, context)

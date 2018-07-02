#!/usr/bin/python  
# -*- coding:utf-8 -*-  

import urllib, socket
socket.setdefaulttimeout(60.0)

import tarfile, zipfile, tempfile
import os, sys, re, platform, getopt, getpass, random, time, subprocess, shutil, string
from xml.etree import ElementTree as ET
from subprocess import check_call

if platform.system() == 'Windows':
	pass
else:
	import pwd

if sys.hexversion >= 0x03000000:
	import urllib.request
	import http.client
	import configparser
	from urllib.parse import urlparse
	
	if platform.system() == 'Windows':
		import winreg
else:
	import ConfigParser as configparser
	import urlparse
	import httplib
	
	if platform.system() == 'Windows':
		import _winreg as winreg
            
# Sources and binary releases
source_url = "https://github.com/ouroboros/ouroboros/releases/latest"
bin_zip_url = "https://sourceforge.net/projects/ouroboros/files/bin/latest.zip/download"
bin_tgz_url = "https://sourceforge.net/projects/ouroboros/files/bin/latest.tar.gz/download"
bin_mysql_url = "https://sourceforge.net/projects/ouroboros/files/bin/deps/mysql-win32.msi/download"

# MySQL installation directory
mysql_root = ""
mysql_home = ""

# MySQL version information
mysql_verinfo = ""

# MySQL port
mysql_ip = ""
mysql_port = ""

# MySQL root password
mysql_root_password = ""

# MySQL OURO account name and password
mysql_ouro_name = ""
mysql_ouro_password = ""

# MySQL db name
mysql_ouro_db_name = ""

# MySQL service name
mysql_sercive_name = ""

# According to root determine the installation position
OURO_ROOT = ''
OURO_RES_PATH = ''
OURO_BIN_PATH = ''
OURO_UID = ''
ouro_res_path = ""

# The system user name password, the temporary use of installation
os_user_name = ""
os_user_passwd = ""

# Tool environment variable name
INSTALLER_EVN_NAME = 'KBT'

_zip_ouroboros_path = ""
_zip_ouroboros_dirname = ""
_install_path = ""

def hello():
	# echoSystemEnvironment()
	# echoOUROEnvironment()

	OUT_MSG("###########################################################################")
	OUT_MSG("#   installer:                                                            #")
	OUT_MSG("#        Ouroboros installation tools.                                     #")
	OUT_MSG("#        Install, Uninstall, Check the version,                           #")
	OUT_MSG("#        Environment settings, etc.                                       #")
	OUT_MSG("#   homepage:                                                             #")
	OUT_MSG("#         http://ouroboros.org                                             #")
	OUT_MSG("#   sources:                                                              #")
	OUT_MSG("#         https://github.com/ouroboros/ouroboros/*/ouro/tools/server/install #")
	OUT_MSG("###########################################################################")
	OUT_MSG("")
    
def help():
	OUT_MSG("")
	OUT_MSG("Usage:")
	OUT_MSG("\tpython installer.py [command]")
	OUT_MSG("")

	OUT_MSG("install:")
	OUT_MSG("\tInstall Ouroboros.\n")
	OUT_MSG("\tinstaller.py install: Install development environment (dependent, environment variables, etc.), From the OURO_ROOT search.")
	OUT_MSG("\tinstaller.py install=localsrc : Install from local-disk(Source code), From the OURO_ROOT search.")
	OUT_MSG("\tinstaller.py install=remotesrc : Install from github(Source code).")
	OUT_MSG("\tinstaller.py install={xxx.zip, xxx.tar.gz} : Install .zip/.tar.gz file.")
	OUT_MSG("")
	
	OUT_MSG("uninstall:")
	OUT_MSG("\tUninstall Ouroboros.")
	OUT_MSG("")
	
	OUT_MSG("update:")
	OUT_MSG("\tUpdate to the latest version(Ouroboros).")
	OUT_MSG("")
		
	OUT_MSG("version:")
	OUT_MSG("\tGet the Ouroboros current version.")
	OUT_MSG("")
	
	OUT_MSG("evn:")
	OUT_MSG("\tThe output of the Ouroboros environment.")
	OUT_MSG("")
	
	OUT_MSG("resetevn:")
	OUT_MSG("\tReset the Ouroboros environment.")
	OUT_MSG("")
	
	OUT_MSG("help:")
	OUT_MSG("\tList all of the command descriptions.")
	OUT_MSG("--------------------------------------------------")

def OUT_MSG(msg):
    try:
        print(msg)
    except UnicodeEncodeError:
        if sys.hexversion >= 0x03000000:
            print(msg.encode('utf8').decode(sys.stdout.encoding))
        else:
            print(msg.encode('utf8'))
    
def INFO_MSG(msg):
	print(msg)

def ERROR_MSG(msg):
	print('ERROR: ' + msg)

def WARING_MSG(msg):
	print('WARING: ' + msg)

def getInput(s):
	if sys.hexversion >= 0x03000000:
		return input(s)
	
	return raw_input(s)
	
def echoOUROEnvironment():
	global OURO_ROOT
	global OURO_RES_PATH
	global OURO_BIN_PATH
	global OURO_UID
	
	OURO_ROOT = getEnvironment('user', 'OURO_ROOT')
	OURO_RES_PATH = getEnvironment('user', 'OURO_RES_PATH')
	OURO_BIN_PATH = getEnvironment('user', 'OURO_BIN_PATH')
	_checkOUROEnvironment(False)
	
	OUT_MSG("OURO_ROOT=" + OURO_ROOT)
	OUT_MSG("OURO_RES_PATH=" + OURO_RES_PATH)
	OUT_MSG("OURO_BIN_PATH=" + OURO_BIN_PATH)
	OUT_MSG("ouro_core_res_path=%s" % ouro_res_path)

def findOuroboros(dir):
	if len(_zip_ouroboros_dirname) > 0:
		return dir + "/" + _zip_ouroboros_dirname + "/"
		
	if dir[-1] != '/' and dir[-1] != '\\':
		dir += '/'
	
	paths = []
	for x in os.listdir(dir):
		if "ouroboros" in x:
			if os.path.isfile(dir + x + "/ouro/res/server/ouroboros_defaults.xml"):
				paths.append(dir + x + "/")
	
	return paths

def find_file_by_pattern(pattern = '.*', base = ".", circle = True):  
    if base == ".":  
        base = os.getcwd()  
    
    final_file_list = []  
    cur_list = os.listdir(base)  
    
    for item in cur_list:
    
        full_path = os.path.join(base, item)

        if os.path.isfile(full_path):  
            if full_path.endswith(pattern):  
                final_file_list.append(full_path)  
        else:
            if (True == circle):  
                final_file_list += find_file_by_pattern(pattern, full_path, circle)

    return final_file_list
    
def resetOUROEnvironment():
	global OURO_ROOT
	global OURO_RES_PATH
	global OURO_BIN_PATH
	global OURO_UID
	
	OURO_ROOT = getEnvironment('user', 'OURO_ROOT')
	OURO_RES_PATH = getEnvironment('user', 'OURO_RES_PATH')
	OURO_BIN_PATH = getEnvironment('user', 'OURO_BIN_PATH')
	OURO_UID = getEnvironment('user', 'UID')
	
	# if root environment configuration is not found, it checks if it is currently in the Ouroboros directory, 
	# if in the Ouroboros directory, then we automatically set the environment
	x_OURO_ROOT = OURO_ROOT
	x_OURO_RES_PATH = OURO_RES_PATH
	x_OURO_BIN_PATH = OURO_BIN_PATH
	x_OURO_UID = OURO_UID
	
	if len(OURO_ROOT) == 0:
		curr = os.getcwd()
		curr = curr.replace("\\", "/")
		
		if "ouro/tools/server/install" in curr:
			curr = curr.replace("ouro/tools/server/install", "").replace("//", "/")
			x_OURO_ROOT = curr

			if x_OURO_ROOT[-1] != "/":
				x_OURO_ROOT += "/"
		else:
			ret = findOuroboros(os.getcwd())
			if len(ret) > 0:
				x_OURO_ROOT = ret[0]
			x_OURO_ROOT = x_OURO_ROOT.replace("\\", "/").replace("//", "/")
		
		if platform.system() == 'Windows':
			x_OURO_RES_PATH = "%OURO_ROOT%/ouro/res/;%OURO_ROOT%/assets/;%OURO_ROOT%/assets/scripts/;%OURO_ROOT%/assets/res/"
		else:
			x_OURO_RES_PATH = "$OURO_ROOT/ouro/res/:$OURO_ROOT/assets/:$OURO_ROOT/assets/scripts/:$OURO_ROOT/assets/res/"
			
	if platform.architecture()[0] == '32bit':
		x_OURO_BIN_PATH = "%OURO_ROOT%/ouro/bin/server/"
	else:
		x_OURO_BIN_PATH = "%OURO_ROOT%/ouro/bin/server/"
		if not os.path.isdir(x_OURO_BIN_PATH):
			x_OURO_BIN_PATH = "%OURO_ROOT%/ouro/bin/server/"
	
	if platform.system() != 'Windows':
		x_OURO_BIN_PATH = x_OURO_BIN_PATH.replace("%OURO_ROOT%", "$OURO_ROOT")
		x_OURO_BIN_PATH = x_OURO_BIN_PATH.replace("\\", "/").replace("//", "/")
		
	if len(OURO_UID) == 0:
		x_OURO_UID = str(random.randint(1, 65535))

	while True:
		INFO_MSG("\nOURO_ROOT current: %s" % (OURO_ROOT))
		OURO_ROOT = getInput('reset OURO_ROOT(No input is [%s]):' % (x_OURO_ROOT)).strip()
		if len(OURO_ROOT) == 0:
			if len(x_OURO_ROOT) == 0:
				INFO_MSG('OURO_ROOT: no change!')
			else:
				OURO_ROOT = x_OURO_ROOT
				
		INFO_MSG("\nOURO_RES_PATH current: %s" % (x_OURO_RES_PATH))
		OURO_RES_PATH = getInput('reset OURO_RES_PATH(No input is [%s]):' % (x_OURO_RES_PATH)).strip()
		if len(OURO_RES_PATH) == 0:
			if len(x_OURO_RES_PATH) == 0:
				INFO_MSG('OURO_RES_PATH: no change!')
			else:
				OURO_RES_PATH = x_OURO_RES_PATH
		
		INFO_MSG("\nOURO_BIN_PATH current: %s" % (x_OURO_BIN_PATH))
		OURO_BIN_PATH = getInput('reset OURO_BIN_PATH(No input is [%s]):' % (x_OURO_BIN_PATH)).strip()
		if len(OURO_BIN_PATH) == 0:
			if len(x_OURO_BIN_PATH) == 0:
				INFO_MSG('OURO_BIN_PATH: no change!')
			else:
				OURO_BIN_PATH = x_OURO_BIN_PATH

		INFO_MSG("\nOURO_UID current: %s" % (x_OURO_UID))
		username = ""
		
		if platform.system() == 'Windows':
			OURO_UID = getInput('reset OURO_UID(No input is [%s]):' % (x_OURO_UID)).strip()
		else:
			# Linux needs to modify the system user ID
			tmp = os_user_name
			if len(tmp) == 0:
				tmp = getpass.getuser()
				
			username = getInput('os system-username(%s):' % tmp).strip()
			if len(username) == 0:
				username = tmp

			OURO_UID = getInput('usermod -u [No input is %s] %s, Enter new uid:' % (OURO_UID, username)).strip()
			
		if len(OURO_UID) == 0:
			if len(x_OURO_UID) == 0:
				INFO_MSG('OURO_UID: no change!')
			else:
				OURO_UID = x_OURO_UID

		if len(OURO_ROOT) > 0:
			setEnvironment('user', 'OURO_ROOT', OURO_ROOT)
			
		if len(OURO_RES_PATH) > 0:
			setEnvironment('user', 'OURO_RES_PATH', OURO_RES_PATH)

		if len(OURO_BIN_PATH) > 0:
			setEnvironment('user', 'OURO_BIN_PATH', OURO_BIN_PATH)

		if len(OURO_UID) > 0:
			if platform.system() == 'Windows':
				setEnvironment('user', 'UID', OURO_UID)
			else:
				setEnvironment('user', 'UID', (OURO_UID, username))
		
		if _checkOUROEnvironment(True):
			break
		
		INFO_MSG("\n---------------------------------------------")
		if getInput('Check to some problems, if you are sure this is not a problem please skip: [yes|no]') == "yes":
			return

	echoOUROEnvironment()

def get_linux_ugid(username):  
	fileobj1 = open('/etc/passwd')  
	fileobj2 = open('/etc/group')  

	uid = None
	gid = None

	for line in fileobj1:  
		if line.startswith(username + ':'):  
			tmp = line.split(':')  
			uid = tmp[2]  

	for line in fileobj2:  
		if line.startswith(username + ':'):  
			tmp = line.split(':')  
			gid = tmp[2]  

	return (uid, gid)  
    
def _checkOUROEnvironment(is_get_error):
	global OURO_ROOT
	global OURO_RES_PATH
	global OURO_BIN_PATH
	global ouro_res_path
	
	OURO_ROOT = getEnvironment('user', 'OURO_ROOT')
	OURO_RES_PATH = getEnvironment('user', 'OURO_RES_PATH')
	OURO_BIN_PATH = getEnvironment('user', 'OURO_BIN_PATH')
	
	ouro_path = OURO_ROOT + "/ouro"
	ouro_path = ouro_path.replace("\\", "/").replace("//", "/")
	if not os.path.isdir(ouro_path):
		if is_get_error:
			ERROR_MSG("OURO_ROOT: is error! The directory or file not found:\nOURO_ROOT/%s\nOURO_ROOT=%s" % (ouro_path, OURO_ROOT)) 
		return False
	
	paths = []

	checkOURORes = [
		"server/ouroboros_defaults.xml",
		"scripts",
	]
	
	checkOUROUserRes = [
		"server/ouroboros.xml",
		"scripts/entities.xml",
	]
	
	OURO_RES_PATH = OURO_RES_PATH.replace("%OURO_ROOT%", OURO_ROOT)
	OURO_RES_PATH = OURO_RES_PATH.replace("$OURO_ROOT", OURO_ROOT)
	
	if ";" in OURO_RES_PATH:
		paths = OURO_RES_PATH.split(";")
	else:
		paths = OURO_RES_PATH.split(":")
	
	paths1 = list(paths)
	paths = []
	for p in paths1:
		paths.append(os.path.expanduser(p))
		
	for path in paths:
		if not os.path.isdir(path):
			if is_get_error:
				ERROR_MSG("OURO_RES_PATH: is error! The directory or file not found:\n%s" % (path)) 
			return False
	
	OURO_BIN_PATH = os.path.expanduser(OURO_BIN_PATH.replace("%OURO_ROOT%", OURO_ROOT).replace("$OURO_ROOT", OURO_ROOT))
	if not os.path.isdir(OURO_BIN_PATH):
		if is_get_error:
			WARING_MSG("OURO_BIN_PATH: is error! The directory or file not found:\n%s" % (OURO_BIN_PATH)) 
	
	ouro_res_path = ""
	for res in checkOURORes:
		found = False
		
		tmp = ""
		for path in paths:
			if path[-1] != '/' and path[-1] != '\\':
				path += '/'
				
			path1 = path + res
			tmp += path1 + "\n"
			if os.path.isdir(path1) or os.path.isfile(path1):
				ouro_res_path = path
				found = True
				break
		
		if not found:
			if is_get_error:
				ERROR_MSG("OURO_RES_PATH: is error! The directory or file not found:\n%s" % (tmp)) 
			return False
	
	for res in checkOUROUserRes:
		found = False
		
		tmp = ""
		for path in paths:
			
			if path[-1] != '/' and path[-1] != '\\':
				path += '/'
				
			path = path + res
			tmp += path + "\n"
			
			if os.path.isdir(path) or os.path.isfile(path):
				found = True
				break
		
		if not found:
			if is_get_error:
				ERROR_MSG("OURO_RES_PATH: is error! The directory or file not found:\n%s" % (tmp)) 
			return False
	
	return True
    
def echoSystemEnvironment():
	OUT_MSG("platform=" + platform.platform())
	OUT_MSG("python_version=" + sys.version)
	OUT_MSG("python_path=" + sys.executable)
	OUT_MSG("currpath=" + os.getcwd())

def findLocalOUROVersion():
	global OURO_ROOT
	
	OURO_ROOT = getEnvironment('user', 'OURO_ROOT')
	
	fpath = "../../../../HISTORY.md"
	if len(OURO_ROOT) > 0:
		fpath = OURO_ROOT + "/HISTORY.md"
	
	try:
		f = open(fpath)
		for line in f.readlines():
			if "#v" in line:
				f.close()
				return line.replace("#", "")

		f.close()
	except:
		pass
		
	return "unknown"
	
def echoOUROVersion():
	INFO_MSG("version=" + findLocalOUROVersion())
	
	if getInput("View the latest version of GitHub? [yes|no]") != "yes":
		return
		
	OUT_MSG("")
	INFO_MSG("Check out the latest version...")
	urls = get_sources_infos()
	src_master_zip_url = urls[0]
	src_zip_url = urls[1]
	src_tgz_url = urls[2]
	release_title = urls[3]
	descrs = urls[4]
	INFO_MSG("-------------------------")
	INFO_MSG(release_title)
	INFO_MSG(descrs)

def removeLinuxEnvironment(scope, name):
	assert scope in ('user', 'system')
	
	files = []
	
	if os.geteuid() == 0:
		if len(os_user_name) > 0:
			files.append("%s/.bashrc" % (pwd.getpwnam(username).pw_dir))
			files.append("%s/.bash_profile" % (pwd.getpwnam(username).pw_dir))
			files.append("%s/.bash_profile" % (pwd.getpwnam(username).pw_dir))
	else:
		files.extend(["~/.bashrc", "~/.bash_profile", "~/.bash_profile"])
	
	for file in files:
		bodys = []
		f = open(os.path.expanduser(file))
		#INFO_MSG("find %s: %s" % (file, name))
		for x in f.readlines():
			if name in x:
				INFO_MSG("remove %s: %s" % (file, x))
				continue
				
			bodys.append(x)
		
		f.close()
		f = open(os.path.expanduser(file), "w")
		f.writelines(bodys)
		f.close()
		
		if os.geteuid() != 0:
			syscommand('bash -c \'source %s\'' % file, False)
	
def setEnvironment(scope, name, value):
	assert scope in ('user', 'system')
	#INFO_MSG('set environment: name=%s, value=%s' % (name, value))

	if platform.system() == 'Windows':
		root, subkey = getWindowsEnvironmentKey(scope)
		# Note: for 'system' scope, you must run this as Administrator
		key = winreg.OpenKey(root, subkey, 0, winreg.KEY_ALL_ACCESS)
		winreg.SetValueEx(key, name, 0, winreg.REG_EXPAND_SZ, value)
		winreg.CloseKey(key)
	else:
		if name.lower() == 'uid':
			uid, username = value
			if uid != str(os.geteuid()):
				ret, cret = syscommand('bash -c \'usermod -d /home/%s/ -u %s %s\'' % (pwd.getpwnam(username).pw_dir, uid, username), True)
				INFO_MSG(ret)
				INFO_MSG(cret)
			return
		
		userhome = "~"
		if len(os_user_name) > 0:
			userhome = pwd.getpwnam(os_user_name).pw_dir 
		
		f = open('%s/.bash_profile' % userhome, 'a')
		f.write("export %s=%s\n\n" % (name, value))
		f.close()
		
		if os.geteuid() > 0:
			syscommand('bash -c \'source %s/.bash_profile\'' % userhome, False)

def getWindowsEnvironmentKey(scope):
	assert scope in ('user', 'system')
	root = winreg.HKEY_CURRENT_USER
	subkey = 'Environment'
	    
	if scope != 'user':
		root = winreg.HKEY_LOCAL_MACHINE
		subkey = r'SYSTEM\CurrentControlSet\Control\Session Manager\Environment'

	return (root, subkey)
	
def remmoveEnvironment(scope, name):
	assert scope in ('user', 'system')

	if platform.system() == 'Windows':
		root, subkey = getWindowsEnvironmentKey(scope)
		key = winreg.OpenKey(root, subkey, 0, winreg.KEY_ALL_ACCESS)

		try:
			winreg.DeleteValue(key, name)
		except WindowsError:
			pass
	else:
		removeLinuxEnvironment(scope, name)

def removeOUROEnvironment():
	INFO_MSG("Remove the Ouroboros-environment variables.")
	
	global OURO_ROOT
	global OURO_RES_PATH
	global OURO_BIN_PATH
	global OURO_UID
	global INSTALLER_EVN_NAME
	
	remmoveEnvironment("user", "OURO_ROOT")
	remmoveEnvironment("user", "OURO_RES_PATH")
	remmoveEnvironment("user", "OURO_BIN_PATH")
	remmoveEnvironment("user", "OURO_UID")
	remmoveEnvironment("user", INSTALLER_EVN_NAME)
	
	OURO_ROOT = ""
	OURO_RES_PATH = ""
	OURO_BIN_PATH = ""
	OURO_UID = ""
	INSTALLER_EVN_NAME = ""
	
def getEnvironment(scope, name):
	assert scope in ('user', 'system')
	
	value = ''
	
	if platform.system() == 'Windows':
		root, subkey = getWindowsEnvironmentKey(scope)
		key = winreg.OpenKey(root, subkey, 0, winreg.KEY_READ)

		try:
			value, _ = winreg.QueryValueEx(key, name)
		except WindowsError:
			value = ''
	else:
		if name.lower() == 'uid':
			return str(os.geteuid())
		
		if len(os_user_name) > 0:
			ret, cret = syscommand('su -l %s -c \'echo ${%s}\'' % (os_user_name, name), True)
			if len(ret) > 0:
				value = ret[0].strip()
		else:
			value = os.environ.get(name, "")
			
	return value

def getMysqlConfig():
	global mysql_root
	cfg = "my.ini"

	if platform.system() != 'Windows':
		cfg = "my.cnf"

	cnf = mysql_root + cfg

	while True:
		if not os.path.isfile(cnf):
			if not os.path.isfile('/etc/' + cfg):
				if not os.path.isfile(mysql_root + "my-default.ini"):
					ERROR_MSG("not found mysqlconfig[%s]." % cnf)
					
					if platform.system() == 'Windows':
						cnf = getInput("Enter the mysqlconfig path(such as [c:/mysql/my.ini or my-default.ini]):")
					else:
						cnf = getInput("Enter the mysqlconfig path(such as [/etc/my.cnf]):")
				else:
					cnf = mysql_root + "my-default.ini"
			else:
				cnf = '/etc/' + cfg
		else:
			break

	config = configparser.ConfigParser()
	config.read(cnf)
	return config, cnf

def installMysql():    
	if platform.system() != 'Windows':
		INFO_MSG("You are Linux, please install MySQL manually!")
		return False
		
	file = 'mysql-win32.msi'

	try:
		os.remove(file)
	except:
		pass
    
	file = download(bin_mysql_url, file)[0]
	INFO_MSG("wait for install:" + file)
	syscommand(file, False)

	
	while True:
		getInput("The MySQL service installation is completed? [yes|no]")
		
		if not findMysqlService():
			ERROR_MSG("-  not found MySQL service.")
			syscommand(file, False)

	os.remove(file)
	return True
	
def restartMsql():
	global mysql_sercive_name
	
	if len(mysql_sercive_name) > 0:
		INFO_MSG('Try to stop %s...' % mysql_sercive_name)
		
		if platform.system() == 'Windows':
			syscommand('net stop ' + mysql_sercive_name, False)
		else:
			syscommand('bash -c \'/etc/init.d/%s stop\'' % mysql_sercive_name, False)
		
	if findMysqlService():
		if platform.system() == 'Windows':
			WARING_MSG('Unable to stop the MySQL, You need administrator privileges.')
		else:
			WARING_MSG('Unable to stop the MySQL, You need root privileges.')

	INFO_MSG('Try to start %s...' % mysql_sercive_name)
	
	if platform.system() == 'Windows':
		syscommand('net start ' + mysql_sercive_name, False)
	else:
		syscommand('bash -c \'/etc/init.d/%s start\'' % mysql_sercive_name, False)
		
	if not findMysqlService():
		if platform.system() == 'Windows':
			WARING_MSG('Unable to start the MySQL, You need administrator privileges.')
		else:
			WARING_MSG('Unable to start the MySQL, You need root privileges.')
	else:
		INFO_MSG('MySQL is ok')

def findMysqlService():
	global mysql_sercive_name
	
	ret = []
	cret = []
	
	if platform.system() == 'Windows':
		ret, cret = syscommand('net start', True)
	else:
		ret, cret = syscommand('bash -c \'service --status-all | grep \"mysql\"\'', True)

	mysql_sercive_name = ""
	
	for s in ret:
		if "mysql" in s.strip().lower():
			if len(mysql_sercive_name) == 0:
				for x in s[s.strip().lower().find("mysql"):]:
					if x.isalnum():
						mysql_sercive_name += x

				INFO_MSG("found mysql service[%s]" % mysql_sercive_name)
				
			return True

	return False

def syscommand(cmdstr, isGetRet):
	filename = os.getcwd() + "/" + str(random.randint(0, 9999999)) + ".log"
	
	if isGetRet:
		cmdstr = "(" + cmdstr + ")"
		cmdstr += " > \"" + filename + "\""
		
	cret = subprocess.Popen(cmdstr, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.readlines()
	#if len(ret) > 0:
	#	os.remove(filename)
	#	return ret
	
	ret = []
	
	if not isGetRet:
		return (ret, [],)

	i = 0
	while True:
		if os.path.isfile(filename):
			break
			
		INFO_MSG("wating(%d) ..." % i)
		i += 1
		
		INFO_MSG(cmdstr)
		
		time.sleep(0.1)
		
	f = open(filename)
	
	while f:
		line = f.readline()
		if not line:
			break
		
		ret.append(line)

	f.close()
	os.remove(filename)

	return (ret, cret,)

def modifyOUROConfig():
	_checkOUROEnvironment(False)
	
	global mysql_ip
	global mysql_port
	global mysql_ouro_name
	global mysql_ouro_password
	global mysql_ouro_db_name
	global ouro_res_path
	
	ouroboros_defs = ouro_res_path + "server/ouroboros_defaults.xml"
	INFO_MSG("Modified: %s" % ouroboros_defs)
	
	if not os.path.isfile(ouroboros_defs):
		ERROR_MSG("not found [%s], Ouroboros is not installed?" % ouroboros_defs)
		ERROR_MSG("Please use the \'python installer.py --install=remotesrc\' or \'python installer.py --install=bin\'")
		return False
		
	if len(mysql_ip) == 0:
		mysql_ip = "localhost"
	
	if len(mysql_port) == 0:
		mysql_port = "0"
	
	if len(mysql_ouro_name) == 0:
		mysql_ouro_name = "ouro"

	if len(mysql_ouro_password) == 0:
		mysql_ouro_password = "pwd123456"
		
	if len(mysql_ouro_db_name) == 0:
		mysql_ouro_db_name = "ouro"
		
	state = 0

	f = None

	try:
		f = open(ouroboros_defs, encoding='UTF-8')
	except:
		f = open(ouroboros_defs)
		
	newxml = []
	for x in f.readlines():
		if "</databaseInterfaces>" in x:
			state = -1

		if state == 0:
			if "<databaseInterfaces>" in x:
				state += 1
				
		if state == 1:
			xx = x.replace(" ", "").replace("	", "")
			if "<host>" in xx:
				xx = xx.split("</")[0].replace("<host>", "")
				x = x.replace(xx, mysql_ip)

			if "<port>" in xx:
				xx = xx.split("</")[0].replace("<port>", "")
				x = x.replace(xx, mysql_port)

			if "<username>" in xx:
				xx = xx.split("</")[0].replace("<username>", "")
				x = x.replace(xx, mysql_ouro_name)

			if "<password>" in xx:
				xx = xx.split("</")[0].replace("<password>", "")
				x = x.replace(xx, mysql_ouro_password)

			if "<databaseName>" in xx:
				xx = xx.split("</")[0].replace("<databaseName>", "")
				x = x.replace(xx, mysql_ouro_db_name)
	                
		newxml.append(x)

	f.close()
	
	try:
		f = open(ouroboros_defs, "w", encoding='UTF-8')
	except:
		f = open(ouroboros_defs, "w")
		
	
	f.writelines(newxml)
	f.close()
	return True
    
def createDatabase():
	global mysql_home
	mysql_home = getEnvironment('user', 'MYSQL_HOME')
	mysql_home = mysql_home.replace("\\", "/")
	
	global mysql_root
	global mysql_verinfo
	global mysql_ip
	global mysql_port
	global mysql_root_password
	global mysql_ouro_name
	global mysql_ouro_password
	global mysql_ouro_db_name
	
	lower_case_table_names = ''
    
	def getRootOpt(rootpasswd):
		if len(rootpasswd) == 0:
			return "-uroot "
		return "-uroot -p" + rootpasswd
    
	rootusePortArgs = ""
	mysql_ip = "localhost"
	mysql_port = "3306"
    
	while True:
		if len(mysql_home) > 0:
			if mysql_home[-1] != '\\' and mysql_home[-1] != '/':
				mysql_home += "/"
		
		ret, cret = syscommand("\"" + mysql_home + "mysql\" --help", True)
		if len(ret) == 0:
			if platform.system() == 'Windows':
				#binpath = find_file_by_pattern("MySQL Command Line Client.lnk", "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\", True)
				#if len(binpath) > 0:
				#	binpath = binpath[0]
				mysql_home = getInput("\Enter mysql.exe path(such as: C:\\MySQL Server 5.1\\bin\\):")
			else:
				tmp = ""
				ret = subprocess.Popen("whereis mysql", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.readlines()[0].split()
				if len(ret) > 0:
					tmp = ret[1].replace("mysql", "")
				
				if len(tmp) == 0:
					mysql_home = getInput("\Enter mysql(The executable file) path(such as: /usr/bin/):")
				
			continue
		else:
			setEnvironment('user', 'MYSQL_HOME', mysql_home)
            
		if len(mysql_root_password) == 0:
			if len(mysql_root_password) == 0:
				mysql_root_password = getInput("- Enter mysql-root password(Don't enter without password):")
				
			sql = "\"select VERSION();show variables like \'port\';show variables like \'lower_case_table_names\';select @@basedir as basePath from dual\""
			cmd = "\"" + mysql_home + ("mysql\" %s%s -hlocalhost -e" % (getRootOpt(mysql_root_password), rootusePortArgs)) + sql
			ret, cret = syscommand(cmd, True)
 
			if len(ret) == 0:
				mysql_root_password = ""
				ERROR_MSG("The password or port error! \n\n\terrorinfos - %s\n\n\tcommand - %s\n\n" % (cret, cmd))
				if str(cret).find("2003") > 0 and str(cret).find("10061") > 0:
					mysql_port = getInput("Please enter the MySQL port number:")
					rootusePortArgs = " -P" + mysql_port

				continue
			else:
				mysql_verinfo = ret[1].strip()
				INFO_MSG("MySQL_Version:" + mysql_verinfo)

				mysql_port = ret[3].replace('port', '').strip()
				INFO_MSG("MySQL_Port:" + mysql_port)

				if platform.system() == 'Windows':
					lower_case_table_names = ret[5].replace('lower_case_table_names', '').strip()

					mysql_root = ret[7].strip()

					if lower_case_table_names != '2':
						ERROR_MSG('mysql lower_case_table_names not is 2')
						config, cnf = getMysqlConfig()
						INFO_MSG('Attempt to modify the [%s]...' % cnf)
						config.set('mysqld', 'lower_case_table_names', '2')
						config.write(open(cnf, "w"))
						restartMsql()
						continue
                
				sql = "\"delete from user where user=\'\';FLUSH PRIVILEGES\""
				cmd = "\"" + mysql_home + ("mysql\" %s%s -hlocalhost -e" % (getRootOpt(mysql_root_password), rootusePortArgs)) + sql + " mysql"
				syscommand(cmd, False)

		if len(mysql_ouro_name) == 0:
			OUT_MSG('')
			INFO_MSG("create ouro mysql-account:")
			mysql_ouro_name = getInput("- username(Do not enter the default is \'ouro\')): ")
			if len(mysql_ouro_name) == 0:
				mysql_ouro_name = "ouro"
                        
			mysql_ouro_password = getInput("- password(Do not enter the default is \'ouro\')): ")
			if len(mysql_ouro_password) == 0:
				mysql_ouro_password = "pwd123456"

			INFO_MSG('Create ouro-account: name=%s, password=%s successfully!' % (mysql_ouro_name, mysql_ouro_password))
            
			if len(mysql_ouro_db_name) == 0:
				OUT_MSG('')
				mysql_ouro_db_name = getInput("Create game database(Do not enter the default is \'ouro\'):")
				if len(mysql_ouro_db_name) == 0:
					mysql_ouro_db_name = "ouro"
                    
			sql = "\"grant all privileges on *.* to %s@\'%%\' identified by \'%s\';grant select,insert,update,delete,create,drop on *.* to %s@\'%%\' identified by \'%s\';FLUSH PRIVILEGES\"" % (mysql_ouro_name, mysql_ouro_password, mysql_ouro_name, mysql_ouro_password)
			cmd = "\"" + mysql_home + ("mysql\" %s%s -hlocalhost -e" % (getRootOpt(mysql_root_password), rootusePortArgs)) + sql + " mysql"
			syscommand(cmd, False)

			sql = "\"delete from user where user=\'\';FLUSH PRIVILEGES\""
			cmd = "\"" + mysql_home + ("mysql\" %s%s -hlocalhost -e" % (getRootOpt(mysql_root_password), rootusePortArgs)) + sql + " mysql"
			syscommand(cmd, False)
                
		# If the table exists, then an error
		has_db_sql = "\"SELECT * FROM information_schema.SCHEMATA where SCHEMA_NAME=\'%s\'\""  % (mysql_ouro_db_name)
		cmd = "\"" + mysql_home + ("mysql\" -u%s -p%s -hlocalhost -P%s -e" % (mysql_ouro_name, mysql_ouro_password, mysql_port)) + has_db_sql
		ret, cret = syscommand(cmd, True)

		if len(ret) > 0:
			ERROR_MSG("database[%s] has exist!" % (mysql_ouro_db_name))
			mysql_ouro_db_name = "";
			mysql_ouro_db_name = "";
			mysql_ouro_name = ''
			continue

		# Create table
		sql = "\"create database %s\"" % (mysql_ouro_db_name)
		cmd = "\"" + mysql_home + ("mysql\" -u%s -p%s -hlocalhost -P%s -e" % (mysql_ouro_name, mysql_ouro_password, mysql_port)) + sql
		syscommand(cmd, False)

		# Once again, check whether the created successfully, return >0, otherwise a new request to create the
		cmd = "\"" + mysql_home + ("mysql\" -u%s -p%s -hlocalhost -P%s -e" % (mysql_ouro_name, mysql_ouro_password, mysql_port)) + has_db_sql
		ret, cret = syscommand(cmd, True)
		if len(ret) == 0:
			ERROR_MSG("database is error! %s" % (cret))
			mysql_ouro_db_name = "";
			mysql_ouro_db_name = "";
			mysql_ouro_name = ''
			continue
		else:
			INFO_MSG("create database(%s) is successfully!" % mysql_ouro_db_name)
			break

		return True

def checkMysql():
	global mysql_ip
	global mysql_port
	global mysql_ouro_name
	global mysql_ouro_password
	global mysql_ouro_db_name
	
	ret = getInput("- MySQL is installed on the remote machine?[yes/no]")
	if ret == 'yes':
		while True:
			if len(mysql_ip) == 0:
				mysql_ip = getInput("- Enter mysql ip-address:")
				continue
				
			if len(mysql_port) == 0:
				mysql_port = getInput("- Enter mysql ip-port:")
				continue
				
			if len(mysql_ouro_name) == 0:
				mysql_ouro_name = getInput("- Enter mysql-account:")
				continue
				
			if len(mysql_ouro_password) == 0:
				mysql_ouro_password = getInput("- Enter mysql-password:")
				continue
				
			if len(mysql_ouro_db_name) == 0:
				mysql_ouro_db_name = getInput("- Enter mysql-databaseName:")
				continue
			
			break
			
		return True
				
	itry = 0
	
	if platform.system() == 'Windows':
		syscommand('net start mysql', False)
	else:
		syscommand('bash -c \'/etc/init.d/mysql start\'', False)
		syscommand('bash -c \'/etc/init.d/mysqld start\'', False)
		
	found = findMysqlService()

	INFO_MSG("MySQL is installed on the local.")
	INFO_MSG("- check mysql service...")
	restartMsql()
	
	manual_installation = False
	
	while True:
		if not found:
			found = findMysqlService()
        
		if not found:
			INFO_MSG("")
			ERROR_MSG("-  not found MySQL service.")
			if itry == 1:
				return False

			ret = getInput("-  Allow automatic installation of MySQL? [yes/no]")
			if ret != 'yes':
				if not manual_installation:
					if getInput("The MySQL service installation is complete? [yes|no]") != "no":
						manual_installation = True
						continue
						
				return False
			else:
				if not installMysql():
					ERROR_MSG("install mysql is failed!")
					return False
				else:
					itry += 1
		else:
			break

	createDatabase()
	return found

def checkGit():
	pass

def checkOUROEnvironment():
	if not _checkOUROEnvironment(True):
		resetOUROEnvironment()
		
	return True
	
def checkDeps():
	setEnvironment('user', INSTALLER_EVN_NAME, os.getcwd())
    
	deps = {
			"ouro_environment": checkOUROEnvironment,
	        "mysql" : checkMysql,
	        # "git" : checkGit
	    }

	OUT_MSG("")
	INFO_MSG("Check the dependences:")
	for dep in deps:
		INFO_MSG("- %s: checking..." % dep)
		ret = deps[dep]()
		if ret:
			INFO_MSG("- %s: yes" % dep)
		else:
			ERROR_MSG("- %s: no" % dep)
			return False
	
	return True
	
def get_sources_infos():
	try:
		response = urllib.request.urlopen(source_url)
	except:
		response = urllib.urlopen(source_url)
        
	html = response.read().decode("utf8")
	ziplist = re.compile("""=\"[a-zA-Z0-9//\/\.?]+.zip""").findall(html)
	tgzlist = re.compile("""=\"[a-zA-Z0-9//\/\.?]+.gz""").findall(html)

	src_master_zip_url = """https://github.com/ouroboros/ouroboros/archive/master.zip"""
	src_zip_url = ziplist[0].replace("=\"", "https://github.com")
	src_tgz_url = tgzlist[0].replace("=\"", "https://github.com")

	# title
	tag_start = """<h1 class="release-title">"""
	tag_end = """</h1>"""

	release_title = html
	release_title = release_title[release_title.find(tag_start) + len(tag_start):]
	release_title = release_title[:release_title.find(tag_end)]
	release_title = re.compile("""\<a(?:\\s+.+?)*?\\s+href=\"(.*?\"\>)(.*?)\<\/a\>""").findall(release_title)
	release_title = release_title[0][1]

	# descriptions
	tag_start = """<div class="markdown-body">"""
	tag_end = """</div>"""

	descrs = html
	descrs = descrs[descrs.find(tag_start) + len(tag_start):]
	descrs = descrs[:descrs.find(tag_end)]
	descrs = descrs.replace("\n", "")
	descrs = descrs.replace("<p>", "\t- ")
	descrs = descrs.replace("</p>", "\n")
	
	descrs = descrs.replace("<ul class=\"task-list\">", "")
	descrs = descrs.replace("<li>", "\t- ")
	descrs = descrs.replace("</li>", "\n")
	descrs = descrs.replace("</ul>", "")
	descrs.strip()
	
	# downloads
	#print("\ndownloads:")
	#print("found:" + src_zip_url)
	#print("found:" + src_tgz_url)
	return (src_master_zip_url, src_zip_url, src_tgz_url, release_title, descrs)
    
def download_hookreport(count, block_size, total_size):
	s = ""
	if total_size <= 0:
		s = '\rdownloading : %.2fMB' % (count * block_size / 1024 / 1024.0)
	else:
		s = '\rdownloading : %d/%d (%02d%%)' % (count * block_size, total_size, 100.0 * count * block_size / total_size)

	sys.stdout.write(s)
	sys.stdout.flush()
	
def download(currurl, fname = None):
	OUT_MSG("")
	INFO_MSG("Downloading from " + currurl)
	
	try:
		if hasattr(urllib, "urlretrieve"):
			return urllib.urlretrieve(currurl, filename = fname, reporthook = download_hookreport)
		else:
			return urllib.request.urlretrieve(currurl, filename = fname, reporthook = download_hookreport)
	except:
		OUT_MSG("")
		ERROR_MSG("timeout!")
		return download(currurl, fname)
		
def getSystemUser():
	global os_user_name
	global os_user_passwd
	
	if len(os_user_name) > 0:
		return
		
	if platform.system() == 'Windows':
		return

	os_user_name = getInput("Install Ouroboros to Linux-account(No input is ouro):")
	if len(os_user_name) == 0:
		os_user_name = "ouro"

	hasuser = ""
	try:
		hasuser = pwd.getpwnam(os_user_name)
	except:
		pass

	if len(hasuser) == 0:
		if getInput("not found system-user[%s], create new user?: [yes|no]" % (os_user_name)) == "yes":
			os_user_passwd = getInput("Please enter the Linux-account passwd(No input is ouro):")

			if len(os_user_passwd) == 0:
				os_user_passwd = "ouro"

			syscommand('bash -c \'useradd %s -p%s\'' % (os_user_name, os_user_passwd), False)
			syscommand('bash -c \'echo \'%s:%s\' | chpasswd\'' % (os_user_name, os_user_passwd), False)

def getInstallPath():
	global OURO_ROOT
	global _install_path
	_install_path = ""
	
	global _zip_ouroboros_dirname
	
	OURO_ROOT = getEnvironment('user', 'OURO_ROOT')
	
	if _checkOUROEnvironment(False):
		INFO_MSG("Already installed Ouroboros, OURO_ROOT=[%s].\n" % (OURO_ROOT))
		if getInput("Want to install to [%s]?[yes|no]" % (OURO_ROOT)) == "yes":
			_install_path = ""
			return

	while True:
		if os.path.isdir(_install_path):
			break
		
		if len(os_user_name) == 0:
			_install_path = getInput("Please enter the installation path:")
		else:
			_install_path = getInput("Please enter the installation path(No inout is %s):" % (pwd.getpwnam(os_user_name).pw_dir))
			if len(_install_path) == 0:
				_install_path = pwd.getpwnam(os_user_name).pw_dir

		_install_path = _install_path + "/" + _zip_ouroboros_dirname + "/"
		if os.path.isdir(_install_path):
			if getInput("Coverage of this directory? [yes|no]") == "yes":
				break
				
			ERROR_MSG("%s has exist!" % _install_path)
			_install_path = ""
			continue
		
		break
	
	if not os.path.isdir(_install_path):
		try:
			os.mkdir(_install_path)
		except:
			ERROR_MSG("path[%s] is error!" % _install_path)
			_install_path = ""
			getInstallPath()
			
def copyFilesTo(root_src_dir, root_dst_dir):
	count = 0
	
	total_count = sum([len(files) for root, dirs, files in os.walk(root_src_dir)])
	
	for src_dir, dirs, files in os.walk(root_src_dir):
		dst_dir = src_dir.replace(root_src_dir, root_dst_dir)
		if not os.path.exists(dst_dir):
			os.mkdir(dst_dir)
	        
		for file_ in files:
			src_file = os.path.join(src_dir, file_)
			dst_file = os.path.join(dst_dir, file_)

			if os.path.exists(dst_file):
				os.remove(dst_file)
			    
			shutil.move(src_file, dst_dir)
			count += 1
	        
			s = "\rmoved: %d/%d (%d%%)" % (count, total_count, (count / float(total_count)) * 100)
			sys.stdout.write(s)
			sys.stdout.flush()
	
	INFO_MSG("")

def copy_new_to_ouroboros_dir(checksources = True):
	global _install_path
	global OURO_ROOT
	global OURO_RES_PATH
	global OURO_BIN_PATH
	global OURO_UID
	global ouro_res_path
	global _zip_ouroboros_path
	global _zip_ouroboros_dirname
	
	currourodir = _zip_ouroboros_path + "/" + _zip_ouroboros_dirname

	if len(_install_path) > 0:
		OURO_ROOT = _install_path
		if platform.system() == 'Windows':
			OURO_RES_PATH = "%OURO_ROOT%ouro/res/;%OURO_ROOT%assets/;%OURO_ROOT%/assets/scripts/;%OURO_ROOT%assets/res/"
			if platform.architecture()[0] == '32bit':
				OURO_BIN_PATH = "%OURO_ROOT%ouro/bin/server/"
			else:
				OURO_BIN_PATH = "%OURO_ROOT%ouro/bin/server/"
		else:
			OURO_RES_PATH = "$OURO_ROOT/ouro/res/:$OURO_ROOT/assets/:$OURO_ROOT/assets/scripts/:$OURO_ROOT/assets/res/"
			if platform.architecture()[0] == '32bit':
				OURO_BIN_PATH = "$OURO_ROOT/ouro/bin/server/"
			else:
				OURO_BIN_PATH = "$OURO_ROOT/ouro/bin/server/"
		
		setEnvironment('user', 'OURO_ROOT', OURO_ROOT)
		setEnvironment('user', 'OURO_RES_PATH', OURO_RES_PATH)
		setEnvironment('user', 'OURO_BIN_PATH', OURO_BIN_PATH)
		
		INFO_MSG("OURO_ROOT = %s" % OURO_ROOT)
		INFO_MSG("OURO_RES_PATH = %s" % OURO_RES_PATH)
		INFO_MSG("OURO_BIN_PATH = %s" % OURO_BIN_PATH)
		
		INFO_MSG("\n\nInstalling Ouroboros...")
		INFO_MSG("moving %s to %s..." % (currourodir, _install_path))
		copyFilesTo(currourodir, _install_path)
		
		if platform.system() != 'Windows':
			syscommand('bash -c \'chmod -R 755 %s\'' % (OURO_ROOT), True)
			
		return
	
	OURO_ROOT = getEnvironment('user', 'OURO_ROOT')
	OURO_RES_PATH = getEnvironment('user', 'OURO_RES_PATH')
	OURO_BIN_PATH = getEnvironment('user', 'OURO_BIN_PATH')
	
	currourodir = currourodir.replace("\\", "/").replace("//", "/")
	OURO_ROOT = OURO_ROOT.replace("\\", "/").replace("//", "/")
	
	if currourodir == OURO_ROOT:
		WARNING_MSG("currourodir[%s] == OURO_ROOT[%s]" % (currourodir, OURO_ROOT))
		return
		
	INFO_MSG("\n\nInstalling Ouroboros[%s]..." % OURO_ROOT)
	ouro_res_path1 = ouro_res_path.replace("%OURO_ROOT%", OURO_ROOT).replace("$OURO_ROOT", OURO_ROOT)
	if len(ouro_res_path1) == 0:
		ouro_res_path1 = OURO_ROOT + "/ouro/res"
		
	if os.path.isdir(OURO_ROOT):
		if os.path.isdir(ouro_res_path1):
			if getInput('Found that the existing directory(%s), whether to replace the: [yes|no]?' % ouro_res_path1) == "yes":
				# shutil.rmtree(ouro_res_path1)
				pass
				
		ouro_tools_path = OURO_ROOT + "/ouro/tools"
		ouro_tools_path = ouro_tools_path.replace("\\", "/").replace("//", "/")
		if os.path.isdir(ouro_tools_path):
			if getInput('Found that the existing directory(%s), whether to replace the: [yes|no]?' % ouro_tools_path) == "yes":
				# shutil.rmtree(ouro_tools_path)
				pass
					
		if checksources:
			srcpath = OURO_ROOT + "/ouro/src"
			srcpath = srcpath.replace("\\", "/").replace("//", "/")
			if os.path.isdir(srcpath):
				if getInput('Found that the existing directory(%s), whether to replace the: [yes|no]?' % (srcpath)) == "yes":
					# shutil.rmtree(srcpath)
					pass
					
			copyFilesTo(currourodir + "/ouro/src", srcpath)
		else:
			binpath = OURO_ROOT + "/ouro/bin"
			binpath = binpath.replace("\\", "/").replace("//", "/")
			if os.path.isdir(binpath):
				if getInput('Found that the existing directory(%s), whether to replace the: [yes|no]?' % (binpath)) == "yes":
					# shutil.rmtree(binpath)
					pass
					
			copyFilesTo(currourodir + "/ouro/bin", binpath)
			
		copyFilesTo(currourodir + "/ouro/tools", ouro_tools_path)
		copyFilesTo(currourodir + "/ouro/res", ouro_res_path1)

	if platform.system() != 'Windows':
		syscommand('bash -c \'chmod -R 755 %s\'' % (OURO_ROOT), True)
		
def download_sources(release = True):
	global _zip_ouroboros_dirname
	_zip_ouroboros_dirname = ""
	global _zip_ouroboros_path
	_zip_ouroboros_path = ""
	
	OUT_MSG("")
	INFO_MSG("Getting the latest source code...")
	urls = get_sources_infos()
	src_master_zip_url = urls[0]
	src_zip_url = urls[1]
	src_tgz_url = urls[2]
	release_title = urls[3]
	descrs = urls[4]

	currurl = src_zip_url

	# If release is False, download the GIT version of master
	if not release:
		currurl = src_master_zip_url

	INFO_MSG("")
	INFO_MSG(release_title)
	INFO_MSG(descrs)

	file = download(currurl)[0]
	_zip_ouroboros_path = tempfile.mkdtemp("_ouroboros")
	namelist = extract_file(file, _zip_ouroboros_path)
	os.remove(file)
	
	for n in namelist[0].replace("\\", "/").split("/"):
		if "ouroboros" in n:
			_zip_ouroboros_dirname = n
			break

def download_binary():
	ERROR_MSG("Has been abandoned, Please use (installer.py install=remotesrc)!")
	return

	global _zip_ouroboros_dirname
	_zip_ouroboros_dirname = ""
	global _zip_ouroboros_path
	_zip_ouroboros_path = ""

	OUT_MSG("")
	INFO_MSG("Getting the latest Ouroboros...")        
	file = download(bin_zip_url)[0]
	_zip_ouroboros_path = tempfile.mkdtemp("_ouroboros")	
	namelist = extract_file(file, _zip_ouroboros_path)
	os.remove(file)
	
	for n in namelist[0].replace("\\", "/").split("/"):
		if "ouroboros" in n:
			_zip_ouroboros_dirname = n
			break

def getRealUrl(url):
	parsedurl = urlparse(url)
	httpConn = http.client.HTTPConnection(parsedurl[1])
	httpConn.request('GET', parsedurl[2])
	response = httpConn.getresponse()
	
	if response.status != 200:
		return getRealUrl(response.getheader('Location'))
	
	return url
	
def getCompressedFileRootDir(src_file):
	f = None
	
	if ".tar" in src_file:
		f = tarfile.open(src_file)
		namelist = f.getnames()
	else:
		f = zipfile.ZipFile(src_file, 'r')
		namelist = f.namelist()
	
	f.close()
	
	return namelist[0]
	
def extract_file(src_file, extractPath = "./"):
	OUT_MSG("")
	
	
	f = None
	total_count = 0
	
	if ".tar" in src_file:
		f = tarfile.open(src_file)
		namelist = f.getnames()
		INFO_MSG("untgz(%s)..." % (src_file))
	else:
		f = zipfile.ZipFile(src_file, 'r')
		namelist = f.namelist()
		INFO_MSG("unzip(%s)..." % (src_file))
	
	total_count = len(namelist)
	count = 0
	
	for file in namelist:
		f.extract(file, extractPath)
		count += 1

		s = "\rextract: %d/%d (%d%%)" % (count, total_count, (count / float(total_count)) * 100)
		sys.stdout.write(s)
		sys.stdout.flush()
	
	f.close()
	OUT_MSG("")
	INFO_MSG("unzip(%s) is completed(%d)!\n\n" % (src_file, len(namelist)))
	return namelist

def normalinstall():
	getSystemUser()
	if checkDeps() and modifyOUROConfig():
		INFO_MSG("Ouroboros has been successfully installed!")
	else:
		ERROR_MSG("Ouroboros installation failed!")

def installclean():
	global _zip_ouroboros_path
	if len(_zip_ouroboros_path) > 0:
		INFO_MSG("Cleanup temporary files...")
		shutil.rmtree(_zip_ouroboros_path)
		
def sourceinstall():
	download_sources()
	getSystemUser()
	getInstallPath()
	copy_new_to_ouroboros_dir(True)
	installclean()
	
	normalinstall()
	
def binaryinstall():
	download_binary()
	getSystemUser()
	getInstallPath()
	copy_new_to_ouroboros_dir(False)
	installclean()
	
	normalinstall()

def localfileinstall(file):
	global _zip_ouroboros_dirname
	_zip_ouroboros_dirname = ""
	global _zip_ouroboros_path
	_zip_ouroboros_path = ""
	
	getSystemUser()
	_zip_ouroboros_dirname = getCompressedFileRootDir(file)
	getInstallPath()
	
	_zip_ouroboros_path = tempfile.mkdtemp("_ouroboros")
	namelist = extract_file(file, _zip_ouroboros_path)
	
	for n in namelist[0].replace("\\", "/").split("/"):
		if "ouroboros" in n:
			_zip_ouroboros_dirname = n
			break
			
	copy_new_to_ouroboros_dir(False)
	installclean()
	normalinstall()
	
def uninstall():
	global OURO_ROOT
	INFO_MSG("Uninstall Ouroboros ...")
	
	if len(OURO_ROOT) > 0:
		if getInput('Waring: Folder[%s] will be deleted: [deleteOuroboros|no]?' % (OURO_ROOT)) == "deleteOuroboros":
			shutil.rmtree(OURO_ROOT)
					
	removeOUROEnvironment()
	INFO_MSG("Uninstall Ouroboros completed!")

def update():
	INFO_MSG("https://github.com/ouroboros/ouroboros/releases/latest")
	sourceinstall()

def processCommand():
	const_args = {
		'update'	: update, 
		'uninstall'	: uninstall, 
		'version'	: echoOUROVersion, 
		'evn'		: echoOUROEnvironment, 
		'resetevn'	: resetOUROEnvironment, 
		'help'		: help
	}
	  
	if len(sys.argv[1:]) == 0:
		hello()
		help() 
		return
		
	argv = sys.argv[1:][0]
	func = const_args.get(argv)
	if func:
		func()
		return
	else:
		if 'install' in argv:
			if platform.system() != 'Windows':
				if os.geteuid() != 0:
					assert False and "You must use the root to installation!"
	
			if argv == 'install':
				normalinstall()
				return
			else:
				if argv.startswith("install="):
					argv = argv.replace("install=", "")
					if argv == "remotesrc":
						sourceinstall()
					elif argv == "remotebin":
						binaryinstall()
					elif os.path.isfile(argv):
						localfileinstall(argv)
					else:
						assert False
					
				return

	help() 
 
if __name__ == "__main__":
	processCommand()
    

    

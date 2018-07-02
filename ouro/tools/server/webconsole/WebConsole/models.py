# -*- coding: utf-8 -*-

from django.db import models

class AuthUser( models.Model ):
	"""
	The user permissions table

CREATE TABLE `User` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(128) NOT NULL COMMENT 'Account',
  `user_name` varchar(128) NOT NULL COMMENT 'Display name',
  `password` varchar(128) NOT NULL COMMENT 'Password',
  `sys_user` varchar(128) NOT NULL COMMENT 'System account',
  `sys_uid` int(10) NOT NULL COMMENT 'System account ID',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8
	"""
	id                      = models.AutoField(primary_key = True)
	name                    = models.CharField(max_length = 128, default = "", help_text = "Account", db_index = True, unique = True)
	show_name               = models.CharField(max_length = 128, default = "", help_text = "Display name")
	password                = models.CharField(max_length = 128, default = "", help_text = "Password")
	sys_user                = models.CharField(max_length = 128, default = "", help_text = "System account")
	sys_uid                 = models.IntegerField(default = 0, help_text = "System account ID")
	ouro_root                = models.CharField(max_length = 256, default = "", help_text = "ouro_root")
	ouro_res_path            = models.CharField(max_length = 256, default = "", help_text = "ouro_res_path")
	ouro_bin_path            = models.CharField(max_length = 256, default = "", help_text = "ouro_bin_path")

class ServerLayout( models.Model ):
	"""
	The server running the configuration table
	"""
	id         = models.AutoField(primary_key = True)
	name       = models.CharField(max_length = 128, default = "", help_text = "Name", db_index = True, unique = True)
	sys_user   = models.CharField(max_length = 128, default = "", help_text = "System account")
	config     = models.TextField(max_length = 32768, default = "", help_text = "Configuration(JSON)")

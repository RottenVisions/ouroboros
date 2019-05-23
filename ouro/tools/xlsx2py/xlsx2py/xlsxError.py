# -*- coding: gb2312 -*-
"""
Error profile
"""

from config import *
import traceback
import sys
import xlsxtool as xt

def except_hook(typ, val, tb):
	"""
	Traceback processing, display Chinese: failed
	"""
	pywinerr_list = []
	sys.__excepthook__(typ, val, tb)
	ex = "\n"

	for e in traceback.format_exception(typ, val, tb):
		ex += e

	pywinerr_pos = ex.find('com_error')

	if pywinerr_pos > -1:
		error_str =  ex[pywinerr_pos+len('com_error')+1:].strip()
		xt.str2List(error_str[1:-1], pywinerr_list)
	return False
	#xt.inputList(pywinerr_list)
			
def error_input(index, args = ""):
		print( "ERROR%d:%s"%(index, EXPORT_ERROR[index],) )
		xt.inputList(args)
		return

def info_input(index, args = ""):
	print( "INFO(%d):%s,"%(index, EXPORT_INFO[index],) )
	xt.inputList(args)

class XlsxException(Exception):
	"""
	Exception handling
	"""
	def __init__(self, index, msg = ""):
		print( "ERROR%d:%s, %s"%(index, EXPORT_ERROR[index], xt.value_to_text(msg)) )
		
		sys.exit(1)

xe  = XlsxException

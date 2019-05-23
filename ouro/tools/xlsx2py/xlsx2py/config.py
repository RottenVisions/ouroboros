# -*- coding: gb2312 -*-
"""
Configuration file
"""


#PY_MODULE_PATH = r'res\entities\datum'
######Generating data header settings##########
EXPORT_DATA_CODING = "utf-8"


EXPORT_DATA_HEAD = "# -*- coding: %s -*-\n\n"%(EXPORT_DATA_CODING,)



###############################
#constant
###############################
#导出sheet prefix
EXPORT_PREFIX_CHAR = '@'
EXPORT_DEFINE_ROW = 1

EXPORT_KEY_NUMS = 1

MAP_DEFINE_ROW = 1
MAP_DATA_ROW = 3

#代对表的名称
EXPORT_MAP_SHEET = 'pair table='

#文件编码, the encoding of the exported file is set to UTF-8
FILE_CODE = "GB2312"

############################
#命令相关#
############################
EXPORT_SIGN_DOT = '.'
EXPORT_SIGN_DOLLAR = '$'
EXPORT_SIGN_GTH = '!'

CHECK_FUN = None

#format: index:checkfunc
EXPORT_SIGN= {
	EXPORT_SIGN_DOT:	CHECK_FUN,
	EXPORT_SIGN_DOLLAR : 	CHECK_FUN,
	EXPORT_SIGN_GTH:	CHECK_FUN,
}

EXPORT_ALL_SIGNS = [e for e in EXPORT_SIGN.keys()]

##############################################################################
EXPORT_ERROR_NOSHEET = 1
EXPORT_ERROR_NOMAP = 2
EXPORT_ERROR_HEADER = 3
EXPORT_ERROR_NOTNULL = 4
EXPORT_ERROR_REPEAT = 5
EXPORT_ERROR_REPKEY = 6
EXPORT_ERROR_NUMKEY = 7
EXPORT_ERROR_NOKEY = 8
EXPORT_ERROR_NOFUNC = 9
#数据检测Error
EXPORT_ERROR_DATAINV  = 20
EXPORT_ERROR_NOSIGN = 21
EXPORT_ERROR_NOTMAP = 22
EXPORT_ERROR_FUNC	= 23
#文件IO操作
EXPORT_ERROR_CPATH = 30
EXPORT_ERROR_FILEOPENED = 31
EXPORT_ERROR_NOEXISTFILE = 32
EXPORT_ERROR_OTHER = 101
EXPORT_ERROR_FILEOPEN = 102
EXPORT_ERROR_IOOP = 103

EXPORT_ERROR = {
	EXPORT_ERROR_NOSHEET: 'No table can be',
	EXPORT_ERROR_NOMAP: 'no generation on the table',
	EXPORT_ERROR_HEADER: 'file header error',
	EXPORT_ERROR_NOTNULL: 'Cannot be empty',
	EXPORT_ERROR_REPEAT: 'Name repeat',
	EXPORT_ERROR_DATAINV: 'The data does not match the definition',
	EXPORT_ERROR_OTHER: 'is an error',
	EXPORT_ERROR_NUMKEY: 'There are too many keys needed',
	EXPORT_ERROR_NOSIGN: 'The symbol that does not exist',
	EXPORT_ERROR_REPKEY: 'The column with the keyword has a duplicate key value',
	EXPORT_ERROR_NOTMAP: 'requires a pair, no pair relationship',
	EXPORT_ERROR_NOKEY: 'There is no primary key',
	EXPORT_ERROR_CPATH: 'Directory creation failed',
	EXPORT_ERROR_FILEOPENED: "The file is open, please close it, it is running",
	EXPORT_ERROR_NOFUNC: "The conversion function does not exist",
	EXPORT_ERROR_NOEXISTFILE: 'excel file does not exist',
	EXPORT_ERROR_FILEOPEN: 'File failed to open',
	EXPORT_ERROR_IOOP: 'File read and write error',
	EXPORT_ERROR_FUNC: 'Function Error',
}

EXPORT_INFO_NULL = 0
EXPORT_INFO_OK = 1
EXPORT_INFO_ING = 2
EXPORT_INFO_CDIR = 3
EXPORT_INFO_YN = 4
EXPORT_INFO_RTEXCEL = 5

EXPORT_INFO = {
	EXPORT_INFO_NULL:"\b",
	EXPORT_INFO_YN: "Do you want to continue Y or N",
	EXPORT_INFO_OK: "The file is configured correctly, do you want to import (Y or N)",
	EXPORT_INFO_ING: "In the guide table",
	EXPORT_INFO_CDIR: "File is open",
	EXPORT_INFO_RTEXCEL: 'Retry the file and try again. You can type O to let the program close for you'.
}

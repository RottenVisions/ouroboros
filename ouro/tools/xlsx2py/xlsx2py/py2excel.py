# -*- coding: utf-8 -*-
"""
*****************************************************************************************
	Usage: python py2excel pyfile(utf-8) readexcelfile, writeexcelfile
	Pyfile please use utf-8, does not support ANSI, py should have dictionary datas, allDatas (py files have at least datas)
	Readexcelfile is the parent table for generating py files
	Writeexcelfile is the exported xlsx file
*****************************************************************************************
"""

import sys
import os

import xlsxtool
from ExcelTool import ExcelTool

from config import *

class Sheet(object):
	"""
	Short form
	"""
	@property
	def sheet(self):
		return self.__parentBook.getSheetByIndex(self.__index)

	def __init__(self, parentBook, sheetIndex):		
		self.__parentBook = parentBook
		self.__index =	sheetIndex

	def getIndex(self):
		return self.__index

	def getParentBook(self):
		return self.__parentBook

	def getColCount(self):
		return self.__parentBook.getColCount(self.__index)	
	
	def getColValues(self, col):
		return self.__parentBook.getColValues(self.sheet, col)

	def getRowValues(self, row):
		return self.__parentBook.getRowValues(self.sheet, row)


class py2excel(object):
	"""
	
	"""
	def __init__(self, pyfile, sourcefile, dstfile):
		"""
		pyfile:py, sourcefile:source excel, excel:dest excel
		"""
		self.pyfile = os.path.abspath(pyfile)
		if sourcefile == '':
			self.sourcefile = sourcefile
		else:
			self.sourcefile = os.path.abspath(sourcefile)

		self.dstfile = os.path.abspath(dstfile)
		self.xlsx = None
		self.xbook = None
		self.sheetCNames =  {}
		self.sheetENames = {}
		self.mapSheet = {}
		

#################### Import py file##############################################
	def importPyModule(self):
		"""
		import self.pyfile as python module
		"""
		self.pyModule = None
		try:
			sys.path.append(PY_MODULE_PATH)
		except NameError:
			pass

		#try:
		pyPath, filename=  os.path.split(self.pyfile)
		pypos = filename.strip().rfind(".py")
		if pypos < 0:
			print( "pypypypypypypypy")
		else:
			filename = filename[:pypos]

		sys.path.append(pyPath)
		#try:
		self.pyModule = __import__(filename)
		#except:
			#print( 'import %s' %(self.pyfile))
			#sys.exit(1)
		sys.path.pop(-1)
		sys.path.pop(-1)

	def getSheetNameFromModule(self):
		if hasattr(self.pyModule, 'allDatas'):	
			return self.pyModule.allDatas
		else:
			return  None

############################################################################################## #######################
	def openXlsx(self):
		if xlsxtool.checkExtName(self.sourcefile, '.xlsx') or xlsxtool.checkExtName(self.sourcefile, ".xls"):
			self.xbook = ExcelTool(self.sourcefile)

			if not self.xbook.getWorkbook():
				Print( "Failed to open file")
				return

			self.xlsx = self.xbook.getXLSX()

	def getSheetCNames(self):
		allDatas = self.getSheetNameFromModule()
		sheetCNames = {}
		for index in range(1, self.xbook.getSheetCount() + 1):
			sheetName = self.xbook.getSheetNameByIndex(index)
			if sheetName.startswith(EXPORT_PREFIX_CHAR):
				if allDatas is  None:
					sheetCNames[index] = sheetName
				Elif sheetName[1:].encode("utf-8") in allDatas: #pyfile encoding is considered to be utf-8
					sheetCNames[index] = sheetName
					
		if len(sheetCNames) == 0:
			print( 'no sheet' )
			self.xbook.close()
			sys.exit(1)
			
		If allDatas is None and len(sheetCNames) > 1: #This is the main processing, when there is no allDatas
			for k,v in sheetCNames.iteritems():
				print( "%d:%s"%(k,v) )

			while True:
				ii = raw_input('input your choice:')
				try:
					ii = int(ii)
				except:
					continue

				if ii > 0 and ii < len(sheetCNames):
					print( sheetCNames[ii] )
					self.sheetCNames[ii] = sheetCNames[ii]
					break
		else:
			self.sheetCNames = sheetCNames


	def readXlsxHeader(self):
		"""
		Read Chinese and English
		"""
		if self.xlsx is None:
			print( "no file opened" )

		Self.names = {} #sn: Chinese name of the table, engName, chnName: English (Chinese) name of the dictionary key,

		for si, sn in self.sheetCNames.iteritems():		#chinese name of  sheetname, sheetindex
			sheet = Sheet(self.xbook, si)
			self.names[sn] = {}
			tmpEInt = 1
			tmpCInt = 1
			for (engStruct, chnName) in zip(sheet.getRowValues(EXPORT_DEFINE_ROW -1), sheet.getRowValues(EXPORT_DEFINE_ROW)):
				if engStruct.find('['):
					engName = engStruct[:engStruct.find('[')]
				else:
					engName = 'undefineE_%d'%(tmpEInt,)
					tmpEInt += 1

				if chnName is  None:
					chnName = 'undefineC_%d'%(tmpCInt,)
					tmpCInt += 1

				self.names[sn][engName] = chnName

		self.sheet = None
		Self.xbook.close() #This is a must when overwriting
		
		self.xbook = None
		return self.names

	def writeNewXlsx(self):
		"""
		Py's dictionary is written to xlsx
		"""
		def getWorkbook():
			dirs, filename = os.path.split(self.dstfile)		
			if not os.path.isdir(dirs):
				os.makedirs(dirs)
			return ExcelTool(self.dstfile) 
		
		if self.xbook is not None:
			self.xbook.close()
			self.xbook = None

		self.xbook = getWorkbook()
		
		if os.path.isfile(self.dstfile):
			self.xbook.getWorkbook(forcedClose = True)

		if self.xbook.getXApp().Workbooks.Count == 0:
			newWB = self.xbook.getXApp().Workbooks.Add()
			newWB.SaveAs(self.dstfile)
			newWB.Close()
		
		if self.xbook.getXLSX() is None:
			if not self.xbook.getWorkbook(forcedClose =True):
				print( "unknow error" )
				return
		if self.sourcefile != '':
			self.writeXlsxWithC()
			
		else:
			self.writeXlsxWithoutC() #没有中文


	Def writeXlsxWithoutC(self): #没有中文
		self.parseWriteSheet('datas')
		data  = None
		if hasattr(self.pyModule, 'datas'):
			data = self.pyModule.datas

		if data is None:
			return

		headerKeys = self.getHeaderKeys(data)
		self.newSheet = self.getWriteSheet('datas')
		self.writeXlsxHeader(headerKeys)
		self.writeData2Cells(data, headerKeys)

		self.xbook.close(saveChanges = True)
		

	Def writeXlsxWithC(self): #Chinese file
		cnames = self.names.keys()
		self.parseWriteSheet(cnames)

		for cname, e2cDict in self.names.iteritems():
			self.newSheet = self.getWriteSheet(cname)

			self.newSheet.UsedRange = None #clear the contents of the table
			data = None

			if self.getSheetNameFromModule()  is not None:
				if  cname[1:].encode("utf-8") not  in self.getSheetNameFromModule():
					continue
				else:
					data = self.getSheetNameFromModule()[cname[1:].encode("utf-8")]

			elif hasattr(self.pyModule, 'datas'):
				data = self.pyModule.datas

			if data is None or not isinstance(data, dict):
				continue

			headerKeys = self.getHeaderKeys(data)
			headerCNames =  []

			for p, he in enumerate(headerKeys):
				cname = e2cDict.get(he, "py_%s"%(str(he),))
				headerCNames.append(cname)

			self.writeXlsxHeader(headerCNames)
			self.writeData2Cells(data, headerKeys)

		self.xbook.close(saveChanges = True)
	
	def writeXlsxHeader(self, headerCNames):
		"""
		Write to the first line of the exported xlsx
		"""
		For pos, cn in enumerate(headerCNames): #ANSI编�?
			self.newSheet.Cells(1, pos+1).Value = cn

	def writeData2Cells(self, data, headerKeys):
		"""
		Dictionary data is written to excel
		"""

		if self.newSheet is None:
			return
		for vp, v in enumerate(data.itervalues()):		#value include key
			for p, he in enumerate(headerKeys):
				text = self.convertType(v.get(he, ''))
				self.newSheet.Cells(vp+2, p+1).Value = text

		return

	def getHeaderKeys(self, data):
		headerKeys =  []
		for v in data.itervalues():	#{1111:{'key':values,,}}
			for vk in v.keys():
				if vk not in headerKeys:
					headerKeys.append(vk)

		return headerKeys

	def getWriteSheet(self, cname):
		"""
		Select the sheet to be written from the workbook
		"""
		if cname in self.repeatUse:
			newSheet = self.xbook.getSheetByIndex(self.repeatUse.pop(cname))

		elif len(self.useless) > 0:
			newSheet = self.xbook.getSheetByIndex(self.useless.pop(-1))
			newSheet.Name = cname
		else:
			newSheet = self.xbook.getXLSX().Sheets.Add()
			newSheet.Name = cname

		return newSheet

	def parseWriteSheet(self, cnames):
		"""
		Do some analysis on the table to be written to ensure that some tables still exist.
		"""
		self.repeatUse = {} #table needs to be overwritten
		Self.useless = [] #These tables are considered useless. When you need a new table, take it from here.

		for index in  range(1, self.xbook.getSheetCount()+1):
			name = self.xbook.getSheetNameByIndex(index)
			if name in cnames:
				self.repeatUse[name] = index
			else:
				self.useless.append(index)
		return

	

	def convertType(self, val):
		"""
		Type conversion
		"""
		if isinstance(val, str):
			return val.decode("utf-8")
		elif isinstance(val, (dict, list, tuple)):
			return xlsxtool.value_to_text(val)		
		return val

		
	def run(self):
		self.importPyModule()
		if self.sourcefile != '':
			self.openXlsx()
			self.getSheetCNames()
			self.readXlsxHeader()

		self.writeNewXlsx()
		
if __name__ == '__main__':
	If len(sys.argv[1:]) == 2: #没有中文表
		pyfile, dstfile = sys.argv[1:]
		a = py2excel(pyfile, '', dstfile)
		a.run()
		
	
	Elif len(sys.argv[1:]) == 3: #有中文表
		pyfile, sourcefile, dstfile  = sys.argv[1:]

		if False in map(lambda x:os.path.isfile(x.decode('gb2312')), sys.argv[1:3]):
			Print( 'File??.decode("utf-8") )
			sys.exit(1)

		a = py2excel(pyfile, sourcefile, dstfile)
		a.run()
		
	else:
		print( __doc__.decode("utf-8") )
		sys.exit(1)
	
	
		


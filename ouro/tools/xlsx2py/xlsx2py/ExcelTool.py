# -*- coding: gb2312 -*-
# written by kebiao, 2010/08/20

from win32com.client import Dispatch
import os
import sys

class ExcelTool:
	"""
	Simple package excel various operations
	System requirements, windows system, install python2.6 and pywin32-214.win32-py2.6.exe, and ms office
	"""
	def __init__(self, fileName):
		#try:
		#	self.close()
		#except:
		#	pass

		self.__xapp = Dispatch("Excel.Application")

		self.__xlsx = None

		self.fileName = os.path.abspath(fileName)

	def getWorkbook(self, forcedClose = False):
		"""
		If the Workbook is already open, you need to close it and open it.
		forcedClose: Whether to force the shutdown, then open the Workbook
		"""
		try:
			wn  = len(self.__xapp.Workbooks)
		except:
			Print('The program exits abnormally. This may be caused by you opening the edited "file" without saving the file. Please save the file')
			sys.exit(1)

		for x in range(0, wn):
			Workbook = self.__xapp.Workbooks[x]

			if self.fileName == os.path.join(Workbook.Path, Workbook.name):
				if forcedClose:
					Workbook.Close(SaveChanges = False)
				return False

		Self.__xlsx = self.__xapp.Workbooks.Open(self.fileName) #Open the file
		return True

	def getXApp(self):
		return self.__xapp

	def getXLSX(self):
		return self.__xlsx

	def close(self, saveChanges = False):
		"""
		Close the excel app
		"""
		if self.__xapp:
			self.__xlsx.Close(SaveChanges = saveChanges)
			if len(self.__xapp.Workbooks) ==0:
				self.__xapp.Quit()
		else:
			return False

	def getSheetCount(self):
		"""
		Get the number of worksheets
		"""
		return self.__xlsx.Sheets.Count

	def getSheetNameByIndex(self, index):
		"""
		Get the table name on the specified index location on excel
		"""
		return self.getSheetByIndex(index).Name

	def getSheetByIndex(self, index):
		"""
		Get the table on the specified index position on excel
		"""
		if index in range(1, len(self.__xlsx.Sheets)+1):
			return self.__xlsx.Sheets(index)

		else:
			return None

	def getRowCount(self, sheetIndex):
		"""
		How many elements are in a row?
		"""
		return self.getSheetByIndex(sheetIndex).Cells(1).CurrentRegion.Columns.Count

	def getColCount(self, sheetIndex):
		"""
		Get a list of how many elements
		"""
		return self.getSheetByIndex(sheetIndex).Cells(1).CurrentRegion.Rows.Count

	def getValue(self, sheet, row, col):
		"""
		Get the value at a location on a worksheet
		"""
		return sheet.Cells(row, col).Value

	def getText(self, sheet, row, col):
		"""
		Get the value at a location on a worksheet
		"""
		return sheet.Cells(row, col).Text

	def getRowValues(self, sheet, row):
		"""
		Whole row
		"""
		return sheet.Cells(1).CurrentRegion.Rows[row].Value[0]

	def getSheetRowIters(self, sheet, row):
		"""
		Row iterator
		"""
		return sheet.Cells(1).CurrentRegion.Rows

	def getSheetColIters(self, sheet, col):
		"""
		Column iterator
		"""
		return sheet.Cells(1).CurrentRegion.Columns

	def getColValues(self, sheet, col):
		"""
				Alignment
		"""
		return sheet.Cells(1).CurrentRegion.Columns[col].Value

#---------------------------------------------------------------------
#使用example
#---------------------------------------------------------------------
def main():
	xbook = ExcelTool("d:\\test1.xlsx")

	print("sheetCount=%i" % xbook.getSheetCount())

	for x in range(1, xbook.getSheetCount() +1 ):
	   print( "      ", xbook.getSheetNameByIndex(x))

	print( "sheet1:rowCount=%i, colCount=%i" % (xbook.getRowCount(1), xbook.getColCount(1)))

	for r in range(1, xbook.getRowCount(1) + 1):
		for c in range(1, xbook.getColCount(1) + 1):
			val = xbook.getValue(xbook.getSheetByIndex(2), r, c)
			if val:
				print( "DATA:", val)

if __name__ == "__main__":
	main()





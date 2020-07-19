# -*- coding: utf-8 -*-
import GlobalConst

from items.base.BaseItem import BaseItem

class NormalItem(BaseItem):

	def __init__(self):
		BaseItem.__init__(self)

	def copy(self):
		return NormalItem()

	def canUse(self, user):
		return GlobalConst.GC_OK

	def use(self, user):
		#self.getID()
		return GlobalConst.GC_OK
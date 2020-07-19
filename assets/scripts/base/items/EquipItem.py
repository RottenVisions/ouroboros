# -*- coding: utf-8 -*-
import GlobalConst

from items.base.BaseItem import BaseItem


class EquipItem(BaseItem):

    def __init__(self):
        BaseItem.__init__(self)

    def copy(self):
        return EquipItem()

    def canUse(self, user):
        return GlobalConst.GC_OK

    def use(self, user):
        # self.getID()
        return GlobalConst.GC_OK

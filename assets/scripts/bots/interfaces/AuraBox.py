# -*- coding: utf-8 -*-
import Ouroboros
import data_auras
import GlobalConst
from OURODebug import *


class AuraBox:
    def __init__(self):
        pass

    def pullauras(self):
        self.cell.requestPull();

    def onAddAura(self, AuraID):
        """
        """
        self.auras.append(AuraID)

    def onRemoveAura(self, AuraID):
        """
        """
        self.auras.remove(AuraID)

    def hasAura(self, AuraID):
        """
        """
        return AuraID in self.auras

    def onAuraStatusUpdate(self, id, icon, description, hp, eg):
        pass
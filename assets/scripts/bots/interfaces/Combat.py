# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *


class Combat:
    def __init__(self):
        pass

    def receiveDamage(self, attackerID, modSource, modID, modIcon, modSchool, hp):
        """
        defined.
        """
        DEBUG_MSG("%s::recvDamage: %i attackerID=%i, modID=%i, modIcon=%i, hp=%i" % \
                  (self.getScriptName(), self.id, attackerID, modID, modIcon, hp))

    def receiveHealing(self, attackerID, modSource, modID, modIcon, modSchool, hp):
        """
        defined.
        """
        DEBUG_MSG("%s::recvDamage: %i attackerID=%i, modID=%i, modIcon=%i, hp=%i" % \
                  (self.getScriptName(), self.id, attackerID, modID, modIcon, hp))

    def onUpdateBaseProperties(self, hp, hpMax, eg, egMax):
        pass
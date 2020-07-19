# -*- coding: utf-8 -*-
import Ouroboros
import data_abilities
import GlobalConst
from OURODebug import *


class AbilityBox:
    def __init__(self):
        pass

    def pullAbilities(self):
        self.cell.requestPull();

    def onAddAbility(self, abilityID):
        """
        """
        self.abilities.append(abilityID)

    def onRemoveAbility(self, abilityID):
        """
        """
        self.abilities.remove(abilityID)

    def hasAbility(self, abilityID):
        """
        """
        return abilityID in self.abilities

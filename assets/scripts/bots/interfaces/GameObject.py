# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *


class GameObject:
    def __init__(self):
        pass

    def getScriptName(self):
        return self.__class__.__name__

    def onEnterWorld(self):
        """
        Ouroboros method.
        This entity has entered the world
        """
        pass

    def onLeaveWorld(self):
        """
        Ouroboros method.
        This entity has exited the world
        """
        pass

    def set_name(self, oldValue):
        """
        Property method.
        The server has set the name attribute
        """
        DEBUG_MSG("%s::set_name: %i changed:%s->%s" % (self.getScriptName(), self.id, oldValue, self.name))

    def set_modelNumber(self, oldValue):
        """
        Property method.
        The server has set the modelNumber attribute
        """
        DEBUG_MSG(
            "%s::set_modelNumber: %i changed:%s->%s" % (self.getScriptName(), self.id, oldValue, self.modelNumber))

    def set_modelScale(self, oldValue):
        """
        Property method.
        The server has set the modelScale attribute
        """
        DEBUG_MSG("%s::set_modelScale: %i changed:%s->%s" % (self.getScriptName(), self.id, oldValue, self.modelScale))

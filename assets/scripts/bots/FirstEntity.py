# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *
from interfaces.GameObject import GameObject


class FirstEntity(Ouroboros.Entity, GameObject):
    def __init__(self):
        Ouroboros.Entity.__init__(self)
        GameObject.__init__(self)

    def onHello(self, entityId, msg):
        pass
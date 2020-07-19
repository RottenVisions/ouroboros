# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *
from interfaces.GameObject import GameObject


class Gate(Ouroboros.Entity, GameObject):
    def __init__(self):
        Ouroboros.Entity.__init__(self)
        GameObject.__init__(self)

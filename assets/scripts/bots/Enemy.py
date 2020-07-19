# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *
from interfaces.GameObject import GameObject
from interfaces.Motion import Motion
from interfaces.State import State
from interfaces.Flags import Flags
from interfaces.Combat import Combat
from interfaces.AnimationState import AnimationState

class Enemy(Ouroboros.Entity,
            GameObject,
            Flags,
            State,
            Motion,
            Combat,
            AnimationState):
    def __init__(self):
        Ouroboros.Entity.__init__(self)
        GameObject.__init__(self)
        Motion.__init__(self)
        Flags.__init__(self)
        State.__init__(self)
        Combat.__init__(self)
        AnimationState.__init__(self)

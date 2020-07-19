# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *

from interfaces.GameObject import GameObject
from interfaces.Motion import Motion
from interfaces.State import State
from interfaces.Flags import Flags
from interfaces.Combat import Combat
from interfaces.AuraBox import AuraBox
from interfaces.AbilityBox import AbilityBox
from interfaces.AnimationState import AnimationState

class TestEntity(Ouroboros.Entity,
            GameObject,
            Flags,
            State,
            Combat,
            AuraBox,
            AbilityBox,
            AnimationState):
    def __init__(self):
        Ouroboros.Entity.__init__(self)
        GameObject.__init__(self)
        Flags.__init__(self)
        State.__init__(self)
        Combat.__init__(self)
        AuraBox.__init__(self)
        AbilityBox.__init__(self)
        AnimationState.__init__(self)

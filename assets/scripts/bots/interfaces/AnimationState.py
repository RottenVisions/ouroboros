# -*- coding: utf-8 -*-
import Ouroboros
import GlobalDefine
from OURODebug import *


class AnimationState:
    def __init__(self):
        pass

    def initEntity(self):
        """
        Virtual method.
        """
        pass

    def onTimer(self, tid, userArg):
        """
        Ouroboros method.
        Engine callback timer trigger
        """

    def onAnimationStateChanged(self, oldValue, newValue):
        pass

    def onMotionChanged(self, isMoving):
        if isMoving:
            self.changeAnimationState(GlobalDefine.ENTITY_ANIMATION_STATE_WALK)
        else:
            self.changeAnimationState(GlobalDefine.ENTITY_ANIMATION_STATE_IDLE)

    def isAnimationState(self, state):
        """
        Virtual method.
        """
        return self.currentState == state

    def changeAnimationState(self, toState):
        self.lastState = self.currentState
        self.currentState = toState
        #self.allClients.onAnimationStateChanged(self._lastState, toState)
        #self.clientapp.allClients.onAnimationStateChanged(self._lastState, toState)
        #self.clientapp.entities[self.id].allClients.onAnimationStateChanged(self.lastState, toState)
        INFO_MSG("%s | state changed %s/%s." % (self.getScriptName(), self.lastState, self.currentState))

    def onStateChanged_(self, oldState, newState):
        """
        virtual method.
        State changed
        """
        INFO_MSG("%i oldState=%i to newState=%i" % (self.id, oldState, newState))
        if self.isState(
                GlobalDefine.ENTITY_STATE_REST or GlobalDefine.ENTITY_STATE_FREE or GlobalDefine.ENTITY_STATE_UNKNOWN):
            self.changeAnimationState(GlobalDefine.ENTITY_ANIMATION_STATE_IDLE)
        elif self.isState(GlobalDefine.ENTITY_STATE_DEAD):
            self.changeAnimationState(GlobalDefine.ENTITY_ANIMATION_STATE_DEATH)

    def onSubStateChanged_(self, oldSubState, newSubState):
        """
        virtual method.
        Substate changed
        """
        if self.isState(
                GlobalDefine.ENTITY_SUB_STATE_GO_BACK or GlobalDefine.ENTITY_SUB_STATE_FLEE or GlobalDefine.ENTITY_SUB_STATE_CHASE_TARGET):
            self.changeAnimationState(GlobalDefine.ENTITY_ANIMATION_STATE_RUN)
        elif self.isState(GlobalDefine.ENTITY_SUB_STATE_NORMAL or GlobalDefine.ENTITY_SUB_STATE_RANDOM_STROLL):
            self.changeAnimationState(GlobalDefine.ENTITY_ANIMATION_STATE_WALK)
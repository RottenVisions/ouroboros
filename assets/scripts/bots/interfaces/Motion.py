# -*- coding: utf-8 -*-
import Ouroboros
from OURODebug import *
import GlobalDefine

class Motion:
    def __init__(self):
        self.set_moveSpeed(10)

    def onMove(self, controllerId, userarg):
        """
        Ouroboros method.
        Use any mobile related interface of the engine, this interface will be called when a move of the entity is completed
        """
        # DEBUG_MSG("%s::onMove: %i controllerId =%i, userarg=%s" % \
        #				(self.getScriptName(), self.id, controllerId, userarg))
        self.isMoving = True
        #self.onMotionStateChanged(GlobalDefine.ENTITY_MOTION_STATE_WALK)
        self.onMotionChanged(self.isMoving)
        pass

    def onMoveFailure(self, controllerId, userarg):
        """
        Ouroboros method.
        Use any mobile related interface of the engine, this interface will be called when a move of the entity has failed
        """
        DEBUG_MSG("%s::onMoveFailure: %i controllerId =%i, userarg=%s" % \
                  (self.getScriptName(), self.id, controllerId, userarg))
        self.isMoving = False
        #self.onMotionStateChanged(GlobalDefine.ENTITY_MOTION_STATE_STATIONARY)
        self.onMotionChanged(self.isMoving)

    def onMoveOver(self, controllerId, userarg):
        """
        Ouroboros method.
        Any mobile related interface using the engine will call this interface at the end of the entity movement
        """
        # DEBUG_MSG("%s::onMoveOver: %i controllerId =%i, userarg=%s" % \
        #				(self.getScriptName(), self.id, controllerId, userarg))
        self.isMoving = False
        #self.onMotionStateChanged(GlobalDefine.ENTITY_MOTION_STATE_STATIONARY)
        self.onMotionChanged(self.isMoving)
        pass

    def set_moveSpeed(self, oldValue):
        """
        Property method.
        Set move speed
        """
        DEBUG_MSG("%s::set_moveSpeed: %i changed:%s->%s" % (self.getScriptName(), self.id, oldValue, self.moveSpeed))

        # Set the engine layer entity movement speed
        #self.velocity = self.moveSpeed * 0.1



    # --------------------------------------------------------------------------------------------
    #                              Callbacks
    # --------------------------------------------------------------------------------------------
    def onMotionStateChanged(self, toState):
        self.motionState = toState
        self.onCalculateAnimationMove(toState)
        #DEBUG_MSG('state changed', toState)

    def onCalculateAnimationMove(self, motionState):
        self.lastState = motionState
        if motionState == GlobalDefine.ENTITY_MOTION_STATE_STATIONARY:
            self.currentState = GlobalDefine.ENTITY_ANIMATION_STATE_IDLE
        if motionState == GlobalDefine.ENTITY_MOTION_STATE_WALK:
            self.currentState = GlobalDefine.ENTITY_ANIMATION_STATE_WALK
        if motionState == GlobalDefine.ENTITY_MOTION_STATE_RUN:
            self.currentState = GlobalDefine.ENTITY_ANIMATION_STATE_RUN
        #DEBUG_MSG('calc anim move', motionState)
        #self.base.allClients.onAnimationStateChanged(self.motionState, motionState)
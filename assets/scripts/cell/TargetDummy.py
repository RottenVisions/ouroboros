import Ouroboros
import ServerConstantsDefine

from interfaces.Ability import Ability
from interfaces.AbilityBox import AbilityBox
from interfaces.Aura import Aura
from interfaces.AuraBox import AuraBox
from interfaces.Combat import Combat
from interfaces.GameObject import GameObject
from interfaces.NPCObject import NPCObject
from interfaces.State import State

from OURODebug import *


class TargetDummy(Ouroboros.Entity,
                  NPCObject,
                  State,
                  Ability,
                  AbilityBox,
                  Aura,
                  AuraBox,
                  Combat,
                  GameObject,
                  ):
    """
    The cell part of the Scene
	A space on the cell represents an abstract space
	The scene is an entity representation of the abstract space for easy control
	"""

    def __init__(self):
        NPCObject.__init__(self)
        State.__init__(self)
        AbilityBox.__init__(self)
        AuraBox.__init__(self)
        Combat.__init__(self)
        GameObject.__init__(self)

        self.onEnable()

    # --------------------------------------------------------------------------------------------
    #                              Callbacks
    # --------------------------------------------------------------------------------------------
    def onTimer(self, tid, userArg):
        """
		Ouroboros method.
		Engine callback timer trigger
		"""
        # DEBUG_MSG('TID: %i %s' % (tid, userArg))

        if ServerConstantsDefine.TIMER_TYPE_HEARTBEAT == userArg:
            self.onHeardTimer()

        if ServerConstantsDefine.TIMER_TYPE_AURA_TICK == userArg:
            AuraBox.onTimer(self, tid, userArg)

        if ServerConstantsDefine.TIMER_TYPE_ABILITY_TICK == userArg:
            AbilityBox.onTimer(self, tid, userArg)

    def onEnter(self, entityCall):
        """
		Entering the Scene
		Called by the base part of Scene, the cell part is notified, and Entity enters this scenario.
		:param entityCall:
		:return:
		"""
        DEBUG_MSG('Scene[%i]::onEnter: entityID = %i.' % (self.id, entityCall.id))

    def onLeave(self, entityId):
        """
		Leaving the Scene
		Called by the base part of Scene, the cell part is notified, and Entity leaves the scene.
		:param entityId:
		:return:
		"""
        DEBUG_MSG('Scene[%i]::onLeave: entityID = %i.' % (self.id, entityId))

    def onDestroy(self):
        """
		Called when an entity is destroyed
		:return:
		"""
        pass

    # --------------------------------------------------------------------------------------------
    #                              Custom
    # --------------------------------------------------------------------------------------------

    def onEnable(self):
        self.heartbeatTimer = self.addTimer(0, ServerConstantsDefine.TICK_TYPE_HEARTBEAT,
                                            ServerConstantsDefine.TIMER_TYPE_HEARTBEAT)
        self.abilityTimer = self.addTimer(0, ServerConstantsDefine.TICK_TYPE_ABILITY,
                                          ServerConstantsDefine.TIMER_TYPE_ABILITY_TICK)
        self.auraTimer = self.addTimer(0, ServerConstantsDefine.TICK_TYPE_AURA,
                                       ServerConstantsDefine.TIMER_TYPE_AURA_TICK)

    def onDisable(self):
        pass

    def onHeardTimer(self):
        """
		Entity heartbeat
		"""
    # DEBUG_MSG('Heard timer')

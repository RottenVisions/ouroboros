# -*- coding: utf-8 -*-
import Account
import random, math
import Math

from OURODebug import *
from interfaces.GameObject import GameObject
from interfaces.AnimationState import AnimationState
#from interfaces.Dialog import Dialog
from interfaces.Teleport import Teleport
from interfaces.State import State
from interfaces.Flags import Flags
from interfaces.Combat import Combat
from interfaces.AuraBox import AuraBox
from interfaces.AbilityBox import AbilityBox
from interfaces.Motion import Motion

# Avatar is only synchronized to other PlayerAvatar entities,
class Avatar(Ouroboros.Entity,
             GameObject,
             AnimationState,
             Flags,
             State,
             Motion,
             Combat,
             AbilityBox,
             AuraBox,
             Teleport):
    def __init__(self):
        Ouroboros.Entity.__init__(self)
        GameObject.__init__(self)
        AnimationState.__init__(self)
        Flags.__init__(self)
        State.__init__(self)
        Motion.__init__(self)
        Combat.__init__(self)
        AbilityBox.__init__(self)
        AuraBox.__init__(self)
        Teleport.__init__(self)

    def onEnterSpace(self):
        """
        Ouroboros method.
        entity enters space
        """
        DEBUG_MSG("%s::onEnterSpace: %i" % (self.__class__.__name__, self.id))

    def onLeaveSpace(self):
        """
        Ouroboros method.
        entity exits space
        """
        DEBUG_MSG("%s::onLeaveSpace: %i" % (self.__class__.__name__, self.id))

    def onBecomePlayer(self):
        """
        Ouroboros method.
        entity becomes player
        """
        DEBUG_MSG("%s::onBecomePlayer: %i" % (self.__class__.__name__, self.id))

    def onJump(self):
        """
        defined method.
        simple jump
        """
        pass

    #######################################
    def receiveChatMessage(self, entityId, entityName, message, type):
        DEBUG_MSG("receiveChatMessage: %s" % (message))

    def onReqItemList(self, itemList, equipList):
        pass

    def onReqAbilityPurchase(self, abilityID, result):
        pass

    def errorInfo(errorCode):
        pass

# PlayerAvatar is the entity that can interact with the server.
class PlayerAvatar(Avatar):
    def __init__(self):
        self.randomWalkRadius = 10.0
        #dir(Ouroboros.bots[xx])

    def onEnterWorld(self):
        pass

    def onLeaveWorld(self):
        pass

    def onEnterSpace(self):
        """
        Ouroboros method.
        entity enters space
        """
        DEBUG_MSG("%s::onEnterSpace: %i" % (self.__class__.__name__, self.id))

        # Note: Since PlayerAvatar is forced to be converted by Avatar at the bottom of the engine, __init__ will not be called again
        # Here is a manual initialization
        self.__init__()

        self.spawnPosition = Math.Vector3(self.position)
        Ouroboros.callback(1, self.updateMove)

    def onLeaveSpace(self):
        """
        Ouroboros method.
        entity exits space
        """
        self.clientApp.disconnect()
        DEBUG_MSG("%s::onLeaveSpace: %i" % (self.__class__.__name__, self.id))

    def calcRandomWalkPosition(self):
        """
        calculate a new random position to move to
        """
        center = self.spawnPosition
        r = random.uniform(1, self.randomWalkRadius)  # Walk at least 1 meter
        b = 360.0 * random.random()
        x = r * math.cos(b)  # Radius * Positive cosin
        z = r * math.sin(b)
        return Math.Vector3(center.x + x, center.y, center.z + z)

    def updateMove(self):
        # DEBUG_MSG("%s::updateMove: %i" % (self.__class__.__name__, self.id))
        Ouroboros.callback(5, self.updateMove)
        self.moveToPoint(self.calcRandomWalkPosition(), self.velocity, 0.0, 0, True, True)




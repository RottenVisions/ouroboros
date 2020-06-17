import Ouroboros
from OURODebug import *

class FirstEntity(Ouroboros.Entity):

	def __init__(self):
		pass

	def hello(self, exposed, content):
		"""
		Accept the client Hello command
		Because the hellomethod can be called to the client, the <Exposed/>tag is used in the declaration,
		and a new parameter is required in the code exposed. This parameter indicates the entity id
		of the caller who called the method. With this id we can get the caller's Entity object
		:param exposed: Caller's id
		:param content: Hello content
		:return:
		"""
		sender_entity = Ouroboros.entities[exposed]

		if sender_entity is None:
			ERROR_MSG("FirstEntity[%i]:: can not find sender with exposedValue=[%i]!" % (self.id, exposed))
			return

		INFO_MSG("FirstEntity[%i]::Hello %s[%i]! I'm in space[%i]. I got your content=%s" % (
			self.id, sender_entity.__class__.__name__, sender_entity.id, self.spaceID, content))

		# Call the sender's client method onFirstEntityHello
		sender_entity.client.onFirstEntityHello("Hi, my master. I'm your first entity!")

	# --------------------------------------------------------------------------------------------
	#                              System Callbacks
	# --------------------------------------------------------------------------------------------

	def onDestroy(self):
		"""
		Ouroboros method.
		Entity destruction
		"""
		DEBUG_MSG("FirstEntity[%i]::onDestroy:" % self.id)
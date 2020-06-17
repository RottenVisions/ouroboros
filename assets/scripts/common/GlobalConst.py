"""
"""
GC_OK								= 0x000

# Ability related
GC_ABILITY_MP_NOT_ENOUGH			= 0x001		# Insufficient mana
GC_ABILITY_ENTITY_DEAD				= 0x002		# Entity is dead

GC_ACCOUNT_AVATAR_LIMIT				= 3

#Inventory
EMPTY_SLOT = -1  # Empty inventory  value
INVENTORY_OPERATION_ERROR = -1	 # Invalid inventory operation value

#Guild Rankings

LEADER = 0
CO_LEADER = 1
MEMBER = 2

#Guild Parameters
GUILD_NAME_LIMIT = 32
GUILD_CAPACITY = 50
GUILD_NOTICE_MAX_LENGTH = 128
GUILD_CREATION_PRICE = 2000

#Party & Raid
PARTY_CAPACITY = 5
MICRO_RAID_CAPACITY = 10
MINI_RAID_CAPACITY = 15
RAID_CAPACITY = 40

#Chat
CHAT_CHANNEL_GLOBAL_CAPACITY = 5

#Maps for different zones
idmo_maps = {
	b'Zone_1' : 6,
	b'Zone_2' : 3,
	b'Zone_3' : 2,
	b'' : 1,
}
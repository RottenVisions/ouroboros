"""
"""

# ------------------------------------------------------------------------------
# Entity State
# ------------------------------------------------------------------------------
ENTITY_STATE_UNKNOWN									= -1
ENTITY_STATE_FREE										= 0
ENTITY_STATE_DEAD										= 1
ENTITY_STATE_REST										= 2
ENTITY_STATE_FIGHT										= 3
ENTITY_STATE_MAX    									= 4

# Sub State
ENTITY_SUB_STATE_NORMAL									= 0
ENTITY_SUB_STATE_RANDOM_STROLL							= 1
ENTITY_SUB_STATE_GO_BACK								= 2
ENTITY_SUB_STATE_CHASE_TARGET							= 3
ENTITY_SUB_STATE_FLEE									= 4

# Some behavioral prohibition signs
FORBID_NO												= 0x00000000
FORBID_MOTION											= 0x00000001
FORBID_CHAT												= 0x00000002
FORBID_SPELL											= 0x00000004
FORBID_TRADE											= 0x00000008
FORBID_EQUIP											= 0x00000010
FORBID_INTONATE											= 0x00000020
FORBID_ATTACK_PHY_NEAR									= 0x00000040
FORBID_ATTACK_PHY_FAR									= 0x00000080
FORBID_ATTACK_MAGIC										= 0x00000080
FORBID_YAW												= 0x00008000

FORBID_ATTACK_PHY = FORBID_ATTACK_PHY_NEAR | FORBID_ATTACK_PHY_FAR
FORBID_ATTACK_MAG = FORBID_ATTACK_MAGIC
FORBID_ATTACK = FORBID_ATTACK_PHY | FORBID_ATTACK_MAG
FORBID_MOTION_YAW = FORBID_MOTION | FORBID_YAW

FORBID_ALL = [
	FORBID_MOTION,
	FORBID_YAW,
	FORBID_CHAT,
	FORBID_ATTACK,
	FORBID_SPELL,
	FORBID_TRADE,
	FORBID_EQUIP,
	FORBID_INTONATE,
	FORBID_ATTACK_PHY_NEAR,
	FORBID_ATTACK_PHY_FAR,
	FORBID_ATTACK_MAGIC,
]

FORBID_ACTIONS = {
	ENTITY_STATE_UNKNOWN  : 0,
	ENTITY_STATE_FREE    : FORBID_NO,
	ENTITY_STATE_DEAD    : FORBID_MOTION_YAW | FORBID_TRADE | FORBID_ATTACK | FORBID_SPELL | FORBID_EQUIP,
	ENTITY_STATE_REST    : FORBID_MOTION_YAW | FORBID_TRADE | FORBID_ATTACK | FORBID_SPELL | FORBID_EQUIP,
	ENTITY_STATE_FIGHT   : FORBID_EQUIP | FORBID_TRADE,
	}

for f in FORBID_ALL: FORBID_ACTIONS[ENTITY_STATE_UNKNOWN] |= f

# ------------------------------------------------------------------------------
# TYPES
# ------------------------------------------------------------------------------

SOURCE_TYPE_AURA = 0
SOURCE_TYPE_ABILITY = 1
SOURCE_TYPE_OTHER = 2

SCHOOL_UNKNOWN = -1
SCHOOL_SHADOW = 0
SCHOOL_HOLY = 1

# ------------------------------------------------------------------------------
# Define conversation related
# ------------------------------------------------------------------------------
DIALOG_TYPE_NORMAL			= 0 # Ordinary conversation
DIALOG_TYPE_QUEST			= 1 # Task dialogue

# ------------------------------------------------------------------------------
# Ability related
# ------------------------------------------------------------------------------
# Ability object category
ABILITY_OBJECT_TYPE_UNKNOWN		= 0
ABILITY_OBJECT_TYPE_ENTITY		= 1
ABILITY_OBJECT_TYPE_POSITION	= 2

# ------------------------------------------------------------------------------
# Aura related
# ------------------------------------------------------------------------------
# Aura object category
AURA_OBJECT_TYPE_UNKNOWN	= 0
AURA_OBJECT_TYPE_ENTITY		= 1

AURA_UPDATE_ADDED			= 0
AURA_UPDATE_REMOVED			= 1
AURA_UPDATE_REFRESHED		= 2
AURA_UPDATE_FINISHED		= 3
AURA_UPDATE_STACKED			= 4

# ------------------------------------------------------------------------------
# Entity Motion State
# ------------------------------------------------------------------------------

ENTITY_MOTION_STATE_STATIONARY					= 0
ENTITY_MOTION_STATE_WALK						= 1
ENTITY_MOTION_STATE_RUN							= 2

# ------------------------------------------------------------------------------
# Entity Animation State
# ------------------------------------------------------------------------------

ENTITY_ANIMATION_STATE_IDLE						= 0
ENTITY_ANIMATION_STATE_WALK						= 1
ENTITY_ANIMATION_STATE_RUN						= 2
ENTITY_ANIMATION_STATE_ATTACK_1					= 3
ENTITY_ANIMATION_STATE_ATTACK_2					= 4
ENTITY_ANIMATION_STATE_ATTACK_3					= 5
ENTITY_ANIMATION_STATE_SPELL_READY_OMNI			= 6
ENTITY_ANIMATION_STATE_SPELL_CAST_OMNI			= 7
ENTITY_ANIMATION_STATE_SPELL_READY_DIRECTED		= 8
ENTITY_ANIMATION_STATE_SPELL_CAST_DIRECTED		= 9
ENTITY_ANIMATION_STATE_DEATH					= 10
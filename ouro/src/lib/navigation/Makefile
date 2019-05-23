LIB =	navigation

SRCS =						\
	DetourAlloc				\
	DetourAssert			\
	DetourCommon			\
	DetourNavMesh			\
	DetourNavMeshBuilder	\
	DetourNavMeshQuery		\
	DetourNode				\
	DetourCrowd				\
	DetourLocalBoundary		\
	DetourObstacleAvoidance	\
	DetourPathCorridor		\
	DetourPathQueue			\
	DetourProximityGrid		\
	DetourTileCache			\
	DetourTileCacheBuilder	\
	navigation				\
	navigation_handle		\
	navigation_tile_handle	\
	navigation_mesh_handle	\
	Recast					\
	RecastAlloc				\
	RecastArea				\
	RecastAssert			\
	RecastContour			\
	RecastFilter			\
	RecastLayers			\
	RecastMesh				\
	RecastMeshDetail		\
	RecastRasterization		\
	RecastRegion

ifndef OURO_ROOT
export OURO_ROOT := $(subst /ouro/src/lib/$(LIB),,$(CURDIR))
endif

include $(OURO_ROOT)/ouro/src/build/common.mak
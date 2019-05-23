// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


#if defined(DEFINE_IN_INTERFACE)
	#undef OURO_PROXY_INTERFACE_MACRO_H
#endif


#ifndef OURO_PROXY_INTERFACE_MACRO_H
#define OURO_PROXY_INTERFACE_MACRO_H

// common include	
#include "network/interface_defs.h"
// windows include	
#if OURO_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace Ouroboros{

/**
	PROXY message macro, the parameter is stream, you need to unlock it yourself.
*/
#if defined(NETWORK_INTERFACE_DECLARE_BEGIN)
	#undef PROXY_MESSAGE_HANDLER_STREAM
#endif

#if defined(DEFINE_IN_INTERFACE)
#if defined(BASEAPP)
#define PROXY_MESSAGE_HANDLER_STREAM(NAME)										\
	void NAME##ProxyMessagehandler_stream::handle(Network::Channel* pChannel,	\
													Ouroboros::MemoryStream& s)	\
	{																			\
			ENTITY_ID eid;														\
			s >> eid;															\
			Ouroboros::Proxy* e =												\
					static_cast<Ouroboros::Proxy*>(								\
				Ouroboros::Baseapp::getSingleton().findEntity(eid));				\
			if(e)																\
			{																	\
				e->NAME(pChannel, s);											\
			}																	\
			else																\
			{																	\
				ERROR_MSG(fmt::format(											\
					"Messagehandler::handle: can't found entityID:{}.\n",		\
					eid));														\
			}																	\
	}																			\

#else
#define PROXY_MESSAGE_HANDLER_STREAM(NAME)										\
	void NAME##ProxyMessagehandler_stream::handle(Network::Channel* pChannel,	\
													Ouroboros::MemoryStream& s)	\
	{																			\
	}																			\
		
#endif
#else
#define PROXY_MESSAGE_HANDLER_STREAM(NAME)										\
	class NAME##ProxyMessagehandler_stream : public Network::MessageHandler		\
	{																			\
	public:																		\
		virtual void handle(Network::Channel* pChannel,							\
							Ouroboros::MemoryStream& s);							\
	};																			\

#endif

#define PROXY_MESSAGE_DECLARE_STREAM(NAME, MSG_LENGTH)							\
	PROXY_MESSAGE_HANDLER_STREAM(NAME)											\
	NETWORK_MESSAGE_DECLARE_STREAM(Proxy, NAME,									\
				NAME##ProxyMessagehandler_stream, MSG_LENGTH)					\
																				\

/**
	Proxy message macro, only zero parameter message
*/
#if defined(NETWORK_INTERFACE_DECLARE_BEGIN)
	#undef PROXY_MESSAGE_HANDLER_ARGS0
#endif

#if defined(DEFINE_IN_INTERFACE)
#if defined(BASEAPP)
#define PROXY_MESSAGE_HANDLER_ARGS0(NAME)										\
	void NAME##ProxyMessagehandler0::handle(Network::Channel* pChannel,			\
											Ouroboros::MemoryStream& s)			\
	{																			\
			ENTITY_ID eid;														\
			s >> eid;															\
			Ouroboros::Proxy* e =												\
					static_cast<Ouroboros::Proxy*>(								\
				Ouroboros::Baseapp::getSingleton().findEntity(eid));				\
			if(e)																\
			{																	\
				e->NAME(pChannel);												\
			}																	\
			else																\
			{																	\
				ERROR_MSG(fmt::format(											\
					"Messagehandler::handle: can't found entityID:{}.\n",		\
					eid));														\
			}																	\
	}																			\

#else
#define PROXY_MESSAGE_HANDLER_ARGS0(NAME)										\
	void NAME##ProxyMessagehandler0::handle(Network::Channel* pChannel,			\
											Ouroboros::MemoryStream& s)			\
	{																			\
	}																			\
		
#endif
#else
#define PROXY_MESSAGE_HANDLER_ARGS0(NAME)										\
	class NAME##ProxyMessagehandler0 : public Network::MessageHandler			\
	{																			\
	public:																		\
		virtual void handle(Network::Channel* pChannel,							\
							Ouroboros::MemoryStream& s);							\
	};																			\

#endif

#define PROXY_MESSAGE_DECLARE_ARGS0(NAME, MSG_LENGTH)							\
	PROXY_MESSAGE_HANDLER_ARGS0(NAME)											\
	NETWORK_MESSAGE_DECLARE_ARGS0(Proxy, NAME,									\
				NAME##ProxyMessagehandler0, MSG_LENGTH)							\
																				\
	
/**
	Proxy message macro, message with only one parameter
*/
#if defined(NETWORK_INTERFACE_DECLARE_BEGIN)
	#undef PROXY_MESSAGE_HANDLER_ARGS1
#endif

#if defined(DEFINE_IN_INTERFACE)
#if defined(BASEAPP)
#define PROXY_MESSAGE_HANDLER_ARGS1(NAME, ARG_TYPE1, ARG_NAME1)					\
	void NAME##ProxyMessagehandler1::handle(Network::Channel* pChannel,			\
											Ouroboros::MemoryStream& s)			\
	{																			\
			ENTITY_ID eid;														\
			s >> eid;															\
			Ouroboros::Proxy* e =												\
					static_cast<Ouroboros::Proxy*>(								\
				Ouroboros::Baseapp::getSingleton().findEntity(eid));				\
			ARG_TYPE1 ARG_NAME1;												\
			s >> ARG_NAME1;														\
			if(e)																\
			{																	\
				e->NAME(pChannel, ARG_NAME1);									\
			}																	\
			else																\
			{																	\
				ERROR_MSG(fmt::format(											\
					"Messagehandler::handle: can't found entityID:{}.\n",		\
					eid));														\
			}																	\
	}																			\

#else
#define PROXY_MESSAGE_HANDLER_ARGS1(NAME, ARG_TYPE1, ARG_NAME1)					\
	void NAME##ProxyMessagehandler1::handle(Network::Channel* pChannel,			\
											Ouroboros::MemoryStream& s)			\
	{																			\
	}																			\
		
#endif
#else
#define PROXY_MESSAGE_HANDLER_ARGS1(NAME, ARG_TYPE1, ARG_NAME1)					\
	class NAME##ProxyMessagehandler1 : public Network::MessageHandler			\
	{																			\
	public:																		\
		virtual void handle(Network::Channel* pChannel,							\
							Ouroboros::MemoryStream& s);							\
	};																			\

#endif

#define PROXY_MESSAGE_DECLARE_ARGS1(NAME, MSG_LENGTH, ARG_TYPE1, ARG_NAME1)		\
	PROXY_MESSAGE_HANDLER_ARGS1(NAME, ARG_TYPE1, ARG_NAME1)						\
	NETWORK_MESSAGE_DECLARE_ARGS1(Proxy, NAME,									\
				NAME##ProxyMessagehandler1, MSG_LENGTH, ARG_TYPE1, ARG_NAME1)	\
																				\


}
#endif

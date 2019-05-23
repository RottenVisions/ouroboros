// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_WEBSOCKET_PROTOCOL_H
#define OURO_WEBSOCKET_PROTOCOL_H

#include "common/common.h"
#include "helper/debug_helper.h"

namespace Ouroboros{

class MemoryStream;

namespace Network
{
	class Channel;
	class Packet;

namespace websocket{


/*	WebSocket FRC6544
*/

class WebSocketProtocol
{
public:
	enum FrameType 
	{
		// next frame and end
		NEXT_FRAME = 0x0,
		END_FRAME = 0x80,

		ERROR_FRAME = 0xFF00,
		INCOMPLETE_FRAME = 0xFE00,

		OPENING_FRAME = 0x3300,
		CLOSING_FRAME = 0x3400,

		// unfinished frame
		INCOMPLETE_TEXT_FRAME = 0x01,
		INCOMPLETE_BINARY_FRAME = 0x02,

		// text frame and binary frame END_FRAME + *_FRAME
		TEXT_FRAME = 0x81,
		BINARY_FRAME = 0x82,

		// END_FRAME + *_FRAME
		PING_FRAME = 0x89,
		PONG_FRAME = 0x8A,

		// close the connection
		CLOSE_FRAME = 0x08
	};

	/**
		Is it a websocket protocol?
	*/
	static bool isWebSocketProtocol(MemoryStream* s);
	
	/**
		Websocket protocol handshake
	*/
	static bool handshake(Network::Channel* pChannel, MemoryStream* s);

	/**
		Frame parsing related
	*/
	static int makeFrame(FrameType frame_type, Packet* pInPacket, Packet* pOutPacket);
	static int getFrame(Packet* pPacket, uint8& msg_opcode, uint8& msg_fin, uint8& msg_masked, uint32& msg_mask, 
		int32& msg_length_field, uint64& msg_payload_length, FrameType& frameType);

	static bool decodingDatas(Packet* pPacket, uint8 msg_masked, uint32 msg_mask);

	static std::string getFrameTypeName(FrameType frame_type);

};

}
}
}

#endif // OURO_WEBSOCKET_PROTOCOL_H


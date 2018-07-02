// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "websocket_protocol.h"
#include "common/memorystream.h"
#include "common/memorystream_converter.h"
#include "network/channel.h"
#include "network/packet.h"
#include "common/base64.h"
#include "common/sha1.h"

#if OURO_PLATFORM == PLATFORM_WIN32
#ifdef _DEBUG
#pragma comment(lib, "libeay32_d.lib")
#pragma comment(lib, "ssleay32_d.lib")
#else
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif
#endif

namespace Ouroboros{
namespace Network{
namespace websocket{

//-------------------------------------------------------------------------------------
bool WebSocketProtocol::isWebSocketProtocol(MemoryStream* s)
{
	OURO_ASSERT(s != NULL);

	// The string plus the terminator must be at least 2 characters long, otherwise it will return to avoid the MemoryStream exception.
	if(s->length() < 2)
		return false;

	std::string data;
	size_t rpos = s->rpos();
	size_t wpos = s->wpos();

	(*s) >> data;

	size_t fi = data.find_first_of("Sec-WebSocket-Key");
	if(fi == std::string::npos)
	{
		s->rpos(rpos);
		s->wpos(wpos);
		return false;
	}

	fi = data.find_first_of("GET");
	if(fi == std::string::npos)
	{
		s->rpos(rpos);
		s->wpos(wpos);
		return false;
	}

	std::vector<std::string> header_and_data;
	header_and_data = Ouroboros::strutil::ouro_splits(data, "\r\n\r\n");

	if(header_and_data.size() != 2)
	{
		s->rpos(rpos);
		s->wpos(wpos);
		return false;
	}

	s->rpos(rpos);
	s->wpos(wpos);
	return true;
}

//-------------------------------------------------------------------------------------
bool WebSocketProtocol::handshake(Network::Channel* pChannel, MemoryStream* s)
{
	OURO_ASSERT(s != NULL);

	// The string plus the terminator must be at least 2 characters long, otherwise it will return to avoid the MemoryStream exception.
	if(s->length() < 2)
		return false;

	std::string data;
	size_t rpos = s->rpos();
	size_t wpos = s->wpos();

	(*s) >> data;

	std::vector<std::string> header_and_data;
	header_and_data = Ouroboros::strutil::ouro_splits(data, "\r\n\r\n");

	if(header_and_data.size() != 2)
	{
		s->rpos(rpos);
		s->wpos(wpos);
		return false;
	}

	OUROUnordered_map<std::string, std::string> headers;
	std::vector<std::string> values;

	values = Ouroboros::strutil::ouro_splits(header_and_data[0], "\r\n");
	std::vector<std::string>::iterator iter = values.begin();

	for (; iter != values.end(); ++iter)
	{
		std::string linedata = (*iter);

		std::string::size_type findex = linedata.find_first_of(':', 0);
		if (findex == std::string::npos)
			continue;

		std::string leftstr = linedata.substr(0, findex);
		std::string rightstr = linedata.substr(findex + 1, linedata.size() - findex);

		headers[Ouroboros::strutil::ouro_trim(leftstr)] = Ouroboros::strutil::ouro_trim(rightstr);
	}

	std::string szKey, szOrigin, szHost;

	OUROUnordered_map<std::string, std::string>::iterator findIter = headers.find("Sec-WebSocket-Origin");
	if(findIter == headers.end())
	{
		findIter = headers.find("Origin");
		if(findIter == headers.end())
		{
			//Some app clients may not have this field
			//s->rpos(rpos);
			//s->wpos(wpos);
			//return false;
		}
	}

	if (findIter != headers.end())
		szOrigin = fmt::format("WebSocket-Origin: {}\r\n", findIter->second);

	findIter = headers.find("Sec-WebSocket-Key");
	if(findIter == headers.end())
	{
		s->rpos(rpos);
		s->wpos(wpos);
		return false;
	}

	szKey = findIter->second;

	findIter = headers.find("Host");
	if(findIter == headers.end())
	{
		s->rpos(rpos);
		s->wpos(wpos);
		return false;
	}

	szHost = findIter->second;


    std::string server_key = szKey;

	//RFC6544_MAGIC_KEY
    server_key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	SHA1 sha;
	unsigned int message_digest[5];

	sha.Reset();
	sha << server_key.c_str();
	sha.Result(message_digest);

	for (int i = 0; i < 5; ++i)
		message_digest[i] = htonl(message_digest[i]);

    server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);

	std::string ackHandshake = fmt::format("HTTP/1.1 101 Switching Protocols\r\n"
								"Upgrade: websocket\r\n"
								"Connection: Upgrade\r\n"
								"Sec-WebSocket-Accept: {}\r\n"
								"{}"
								"WebSocket-Location: ws://{}/WebManagerSocket\r\n"
								"WebSocket-Protocol: WebManagerSocket\r\n\r\n",
								server_key, szOrigin, szHost);

	Network::Bundle* pBundle = Network::Bundle::createPoolObject();
	(*pBundle) << ackHandshake;
	(*pBundle).pCurrPacket()->wpos((*pBundle).pCurrPacket()->wpos() - 1);
	pChannel->send(pBundle);
	return true;
}

//-------------------------------------------------------------------------------------
int WebSocketProtocol::makeFrame(WebSocketProtocol::FrameType frame_type,
	Packet * pInPacket, Packet * pOutPacket)
{
	uint64 size = pInPacket->length();

	// Write frame type
	(*pOutPacket) << ((uint8)frame_type);

	if(size <= 125)
	{
		(*pOutPacket) << ((uint8)size);
	}
	else if (size <= 65535)
	{
		uint8 bytelength = 126;
		(*pOutPacket) << bytelength;

		(*pOutPacket) << ((uint8)(( size >> 8 ) & 0xff));
		(*pOutPacket) << ((uint8)(( size ) & 0xff));
	}
	else
	{
		uint8 bytelength = 127;
		(*pOutPacket) << bytelength;

		MemoryStreamConverter::apply<uint64>(&size);
		(*pOutPacket) << size;
	}

	return pOutPacket->length();
}

//-------------------------------------------------------------------------------------
int WebSocketProtocol::getFrame(Packet * pPacket, uint8& msg_opcode, uint8& msg_fin, uint8& msg_masked, uint32& msg_mask,
		int32& msg_length_field, uint64& msg_payload_length, FrameType& frameType)
{
	/*
	 	0                   1                   2                   3
	 	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		+-+-+-+-+-------+-+-------------+-------------------------------+
		|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
		|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
		|N|V|V|V|       |S|             |   (if payload len==126/127)   |
		| |1|2|3|       |K|             |                               |
		+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
		|     Extended payload length continued, if payload len == 127  |
		+ - - - - - - - - - - - - - - - +-------------------------------+
		|                               |Masking-key, if MASK set to 1  |
		+-------------------------------+-------------------------------+
		| Masking-key (continued)       |          Payload Data         |
		+-------------------------------- - - - - - - - - - - - - - - - +
		:                     Payload Data continued ...                :
		+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
		|                     Payload Data continued ...                |
		+---------------------------------------------------------------+
	*/

	// Less than 3 bytes, need to continue to wait
	int remainSize = 3 - pPacket->length();
	if(remainSize > 0)
	{
		frameType = INCOMPLETE_FRAME;
		return remainSize;
	}

	// The first byte, the most significant bit used to describe the end of the message, the lowest 4 bits used to describe the message type
	uint8 bytedata;
	(*pPacket) >> bytedata;

	msg_opcode = bytedata & 0x0F;
	msg_fin = (bytedata >> 7) & 0x01;

	// The second byte, the second byte of the message is mainly used to describe the mask and message length, the most significant bit is 0 or 1 to describe whether there is a mask
	(*pPacket) >> bytedata;
	msg_masked = (bytedata >> 7) & 0x01;

	// Message decoding
	msg_length_field = bytedata & (~0x80);

// The remaining 7 bits are used to describe the message length. Since 7 bits can only describe 127 at most, this value will represent three cases.
// One is that the message content is less than 126 to store the message length, if the message length is less than UINT16 The value of this case is 126
// this value is 127 when the message length is greater than UINT16;
// the message length in both cases is stored in the byte[] immediately after, which is UINT16 (2 bits) and UINT64, respectively. (4 bits)
	if(msg_length_field <= 125)
	{
		msg_payload_length = msg_length_field;
	}
	else if(msg_length_field == 126)
	{
		// Less than 2 bytes, need to continue waiting
		remainSize = 2 - pPacket->length();
		if(remainSize > 0)
		{
			frameType = INCOMPLETE_FRAME;
			return remainSize;
		}

		uint8 bytedata1, bytedata2;
		(*pPacket) >> bytedata1 >> bytedata2;
		msg_payload_length = (bytedata1 << 8) | bytedata2;
	}
	else if(msg_length_field == 127)
	{
		// Less than 8 bytes, need to continue to wait
		remainSize = 8 - pPacket->length();
		if(remainSize > 0)
		{
			frameType = INCOMPLETE_FRAME;
			return remainSize;
		}

		msg_payload_length = ((uint64)(pPacket->data() + pPacket->rpos() + 0) << 56) |
                         ((uint64)(pPacket->data() + pPacket->rpos() + 1) << 48) |
                         ((uint64)(pPacket->data() + pPacket->rpos() + 2) << 40) |
                         ((uint64)(pPacket->data() + pPacket->rpos() + 3) << 32) |
                         ((uint64)(pPacket->data() + pPacket->rpos() + 4) << 24) |
                         ((uint64)(pPacket->data() + pPacket->rpos() + 5) << 16) |
                         ((uint64)(pPacket->data() + pPacket->rpos() + 6) << 8) |
                         ((uint64)(pPacket->data() + pPacket->rpos() + 7));

		pPacket->read_skip(8);
	}

	// Insufficient buffer read length
	/* Do not check here, only parse the protocol header
	if(pPacket->length() < (size_t)msg_payload_length) {
		frameType = INCOMPLETE_FRAME;
		return (size_t)msg_payload_length - pPacket->length();
	}
	*/

	// Get a 4-byte mask value if there is a mask
	if(msg_masked)
	{
		// Less than 4 bytes, need to continue to wait
		remainSize = 4 - pPacket->length();
		if(remainSize > 0)
		{
			frameType = INCOMPLETE_FRAME;
			return remainSize;
		}

		(*pPacket) >> msg_mask;
	}

	if(NETWORK_MESSAGE_MAX_SIZE < msg_payload_length)
	{
		WARNING_MSG(fmt::format("WebSocketProtocol::getFrame: msglen exceeds the limit! msglen=({}), maxlen={}.\n",
			msg_payload_length, NETWORK_MESSAGE_MAX_SIZE));

		frameType = ERROR_FRAME;
		return 0;
	}

	if(msg_opcode == 0x0) frameType = (msg_fin) ? BINARY_FRAME : INCOMPLETE_BINARY_FRAME; // continuation frame ?
	else if(msg_opcode == 0x1) frameType = (msg_fin) ? TEXT_FRAME : INCOMPLETE_TEXT_FRAME;
	else if(msg_opcode == 0x2) frameType = (msg_fin) ? BINARY_FRAME : INCOMPLETE_BINARY_FRAME;
	else if(msg_opcode == 0x8) frameType = CLOSE_FRAME;
	else if(msg_opcode == 0x9) frameType = PING_FRAME;
	else if(msg_opcode == 0xA) frameType = PONG_FRAME;
	else frameType = ERROR_FRAME;

	return 0;
}

//-------------------------------------------------------------------------------------
bool WebSocketProtocol::decodingDatas(Packet* pPacket, uint8 msg_masked, uint32 msg_mask)
{
	// Decoded content
	if(msg_masked)
	{
		uint8* c = pPacket->data() + pPacket->rpos();
		for(int i=0; i<(int)pPacket->length(); i++) {
			c[i] = c[i] ^ ((uint8*)(&msg_mask))[i % 4];
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------
}
}
}

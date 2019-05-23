// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OUROCommon.h"

namespace Ouroboros
{

class MemoryStream;

/*
	Message reading module
	Analyze all message packets from the packet stream and hand them over to the corresponding message handler
*/
class OUROBOROSPLUGINS_API MessageReader
{
public:
	MessageReader();
	virtual ~MessageReader();

	enum READ_STATE
	{
		// message ID
		READ_STATE_MSGID = 0,

		// The length of the message is less than 65535
		READ_STATE_MSGLEN = 1,

		// Use extended length when the above message length cannot reach the request
		// uint32
		READ_STATE_MSGLEN_EX = 2,

		// the content of the message
		READ_STATE_BODY = 3
	};

public:
	void process(const uint8* datas, MessageLengthEx offset, MessageLengthEx length);

protected:
	MessageID msgid_;
	MessageLength msglen_;
	MessageLengthEx expectSize_;
	READ_STATE state_;
	MemoryStream* pMemoryStream_;

};

}
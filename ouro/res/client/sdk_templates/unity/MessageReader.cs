namespace Ouroboros
{
  	using UnityEngine;
	using System;

	using MessageID = System.UInt16;
	using MessageLength = System.UInt16;
	using MessageLengthEx = System.UInt32;

	/*
		The message read module
		From the data packet stream analysis out all of the message packet and supplies it to the corresponding message processing function
	*/
    public class MessageReader
    {
		enum READ_STATE
		{
			// Message ID
			READ_STATE_MSGID = 0,

			// The message length of 65535 or less
			READ_STATE_MSGLEN = 1,

			// When the upper surface of the length of the message can not reach the requirements when using the extended length
			// uint32
			READ_STATE_MSGLEN_EX = 2,

			// The content of the message
			READ_STATE_BODY = 3
		}

		private MessageID msgid = 0;
		private MessageLength msglen = 0;
		private MessageLengthEx expectSize = 2;
		private READ_STATE state = READ_STATE.READ_STATE_MSGID;
		private MemoryStream stream = new MemoryStream();

		public MessageReader()
		{
		}

		public void process(byte[] datas, MessageLengthEx offset, MessageLengthEx length)
		{
			MessageLengthEx totallen = offset;

			while(length > 0 && expectSize > 0)
			{
				if(state == READ_STATE.READ_STATE_MSGID)
				{
					if(length >= expectSize)
					{
						Array.Copy(datas, totallen, stream.data(), stream.wpos, expectSize);
						totallen += expectSize;
						stream.wpos += (int)expectSize;
						length -= expectSize;
						msgid = stream.readUint16();
						stream.clear();

						Message msg = Messages.clientMessages[msgid];

						if(msg.msglen == -1)
						{
							state = READ_STATE.READ_STATE_MSGLEN;
							expectSize = 2;
						}
						else if(msg.msglen == 0)
						{
							// If it is a 0 parameter of the message, then there is no subsequent contents can be read, the processing of the message and skip to the next message
							#if UNITY_EDITOR
							Dbg.profileStart(msg.name);
							#endif

							msg.handleMessage(stream);

							#if UNITY_EDITOR
							Dbg.profileEnd(msg.name);
							#endif

							state = READ_STATE.READ_STATE_MSGID;
							expectSize = 2;
						}
						else
						{
							expectSize = (MessageLengthEx)msg.msglen;
							state = READ_STATE.READ_STATE_BODY;
						}
					}
					else
					{
						Array.Copy(datas, totallen, stream.data(), stream.wpos, length);
						stream.wpos += (int)length;
						expectSize -= length;
						break;
					}
				}
				else if(state == READ_STATE.READ_STATE_MSGLEN)
				{
					if(length >= expectSize)
					{
						Array.Copy(datas, totallen, stream.data(), stream.wpos, expectSize);
						totallen += expectSize;
						stream.wpos += (int)expectSize;
						length -= expectSize;

						msglen = stream.readUint16();
						stream.clear();

						// The length of the extension
						if(msglen >= 65535)
						{
							state = READ_STATE.READ_STATE_MSGLEN_EX;
							expectSize = 4;
						}
						else
						{
							state = READ_STATE.READ_STATE_BODY;
							expectSize = msglen;
						}
					}
					else
					{
						Array.Copy(datas, totallen, stream.data(), stream.wpos, length);
						stream.wpos += (int)length;
						expectSize -= length;
						break;
					}
				}
				else if(state == READ_STATE.READ_STATE_MSGLEN_EX)
				{
					if(length >= expectSize)
					{
						Array.Copy(datas, totallen, stream.data(), stream.wpos, expectSize);
						totallen += expectSize;
						stream.wpos += (int)expectSize;
						length -= expectSize;

						expectSize = stream.readUint32();
						stream.clear();

						state = READ_STATE.READ_STATE_BODY;
					}
					else
					{
						Array.Copy(datas, totallen, stream.data(), stream.wpos, length);
						stream.wpos += (int)length;
						expectSize -= length;
						break;
					}
				}
				else if(state == READ_STATE.READ_STATE_BODY)
				{
					if(length >= expectSize)
					{
						stream.append (datas, totallen, expectSize);
						totallen += expectSize;
						length -= expectSize;

						Message msg = Messages.clientMessages[msgid];

#if UNITY_EDITOR
						Dbg.profileStart(msg.name);
#endif

						msg.handleMessage(stream);
#if UNITY_EDITOR
						Dbg.profileEnd(msg.name);
#endif

						stream.clear();

						state = READ_STATE.READ_STATE_MSGID;
						expectSize = 2;
					}
					else
					{
						stream.append (datas, totallen, length);
						expectSize -= length;
						break;
					}
				}
			}
		}
    }
}

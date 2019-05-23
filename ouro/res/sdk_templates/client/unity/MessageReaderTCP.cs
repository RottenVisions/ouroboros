namespace Ouroboros
{
  	using UnityEngine; 
	using System; 
	
	using MessageID = System.UInt16;
	using MessageLength = System.UInt16;
	using MessageLengthEx = System.UInt32;
	
	/*
		Message reading module
		Analyze all message packets from the packet stream and hand them over to the corresponding message handler
	*/
    public class MessageReaderTCP : MessageReaderBase
    {
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
		}
		
		private MessageID msgid = 0;
		private MessageLength msglen = 0;
		private MessageLengthEx expectSize = 2;
		private READ_STATE state = READ_STATE.READ_STATE_MSGID;
		private MemoryStream stream = new MemoryStream();
		
		public MessageReaderTCP()
		{
		}
		
		public override void process(byte[] datas, MessageLengthEx offset, MessageLengthEx length)
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
							// If it is a message with 0 parameters, then no subsequent content is readable, process this message and jump directly to the next message
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
						
						// length extension
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

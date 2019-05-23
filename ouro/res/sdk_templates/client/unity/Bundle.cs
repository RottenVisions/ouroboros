namespace Ouroboros
{
  	using UnityEngine; 
	using System; 
	using System.Collections;
	using System.Collections.Generic;
	
	/*
		This module bundles multiple data packets together
		Since each packet has a maximum limit, writing a large amount of data to the Bundle will generate multiple MemoryStreams internally.
		Will be sent out in the send
	*/
	public class Bundle : ObjectPool<Bundle>
    {
		public MemoryStream stream = new MemoryStream();
		public List<MemoryStream> streamList = new List<MemoryStream>();
		public int numMessage = 0;
		public int messageLength = 0;
		public Message msgtype = null;
		private int _curMsgStreamIndex = 0;
		
		public Bundle()
		{
		}

		public void clear()
		{
                        // Put unused MemoryStream back into the buffer pool to reduce garbage collection
            for (int i = 0; i < streamList.Count; ++i)
            {
                if (stream != streamList[i])
                    streamList[i].reclaimObject();
            }

            streamList.Clear();

            if(stream != null)
                stream.clear();
            else
                stream = MemoryStream.createObject();

			numMessage = 0;
			messageLength = 0;
			msgtype = null;
			_curMsgStreamIndex = 0;
		}

		/// <summary>
		/// Put yourself back into the buffer pool
		/// </summary>
		public void reclaimObject()
		{
			clear();
			reclaimObject(this);
		}
		
		public void newMessage(Message mt)
		{
			fini(false);
			
			msgtype = mt;
			numMessage += 1;

			writeUint16(msgtype.id);

			if(msgtype.msglen == -1)
			{
				writeUint16(0);
				messageLength = 0;
			}

			_curMsgStreamIndex = 0;
		}
		
		public void writeMsgLength()
		{
			if(msgtype.msglen != -1)
				return;

			MemoryStream stream = this.stream;
			if(_curMsgStreamIndex > 0)
			{
				stream = streamList[streamList.Count - _curMsgStreamIndex];
			}
			stream.data()[2] = (Byte)(messageLength & 0xff);
			stream.data()[3] = (Byte)(messageLength >> 8 & 0xff);
		}
		
		public void fini(bool issend)
		{
			if(numMessage > 0)
			{
				writeMsgLength();

				streamList.Add(stream);
				stream = MemoryStream.createObject();
			}
			
			if(issend)
			{
				numMessage = 0;
				msgtype = null;
			}

			_curMsgStreamIndex = 0;
		}
		
		public void send(NetworkInterfaceBase networkInterface)
		{
			fini(true);
			
			if(networkInterface.valid())
			{
				for(int i=0; i<streamList.Count; i++)
				{
					MemoryStream tempStream = streamList[i];
					networkInterface.send(tempStream);
				}
			}
			else
			{
				Dbg.ERROR_MSG("Bundle::send: networkInterface invalid!");  
			}

			// We believe that if the send is complete, it is considered that the bundle is no longer used.
			// So we will put it back into the object pool to reduce the consumption of garbage collection.
			// If you need to continue using it, you should restart Bundle.createObject(),
			// If you use it directly without re-creating createObject(), you may have an inexplicable problem.
			// Only use this note to alert the user.
			reclaimObject();
		}
		
		public void checkStream(int v)
		{
			if(v > stream.space())
			{
				streamList.Add(stream);
				stream = MemoryStream.createObject();
				++ _curMsgStreamIndex;
			}
	
			messageLength += v;
		}
		
		//---------------------------------------------------------------------------------
		public void writeInt8(SByte v)
		{
			checkStream(1);
			stream.writeInt8(v);
		}
	
		public void writeInt16(Int16 v)
		{
			checkStream(2);
			stream.writeInt16(v);
		}
			
		public void writeInt32(Int32 v)
		{
			checkStream(4);
			stream.writeInt32(v);
		}
	
		public void writeInt64(Int64 v)
		{
			checkStream(8);
			stream.writeInt64(v);
		}
		
		public void writeUint8(Byte v)
		{
			checkStream(1);
			stream.writeUint8(v);
		}
	
		public void writeUint16(UInt16 v)
		{
			checkStream(2);
			stream.writeUint16(v);
		}
			
		public void writeUint32(UInt32 v)
		{
			checkStream(4);
			stream.writeUint32(v);
		}
	
		public void writeUint64(UInt64 v)
		{
			checkStream(8);
			stream.writeUint64(v);
		}
		
		public void writeFloat(float v)
		{
			checkStream(4);
			stream.writeFloat(v);
		}
	
		public void writeDouble(double v)
		{
			checkStream(8);
			stream.writeDouble(v);
		}
		
		public void writeString(string v)
		{
			checkStream(v.Length + 1);
			stream.writeString(v);
		}

		public void writeUnicode(string v)
		{
			writeBlob(System.Text.Encoding.UTF8.GetBytes((string)v));
		}
		
		public void writeBlob(byte[] v)
		{
			checkStream(v.Length + 4);
			stream.writeBlob(v);
		}

		public void writePython(byte[] v)
		{
			writeBlob(v);
		}

		public void writeVector2(Vector2 v)
		{
			checkStream(8);
			stream.writeVector2(v);
		}

		public void writeVector3(Vector3 v)
		{
			checkStream(12);
			stream.writeVector3(v);
		}

		public void writeVector4(Vector4 v)
		{
			checkStream(16);
			stream.writeVector4(v);
		}

		public void writeEntitycall(byte[] v)
		{
			checkStream(16);

			UInt64 cid = 0;
			Int32 id = 0;
			UInt16 type = 0;
			UInt16 utype = 0;

			stream.writeUint64(cid);
			stream.writeInt32(id);
			stream.writeUint16(type);
			stream.writeUint16(utype);
		}
    }
} 

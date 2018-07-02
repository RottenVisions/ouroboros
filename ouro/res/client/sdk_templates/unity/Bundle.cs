namespace Ouroboros
{
  	using UnityEngine;
	using System;
	using System.Collections;
	using System.Collections.Generic;

/*
  This module of the plurality of data packets bundling together
  Since each packet has a maximum upper limit, to Bundle write large amounts of data will be internally generating a plurality of MemoryStream
  In the send time will be all sent out
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
			stream = MemoryStream.createObject();
			streamList = new List<MemoryStream>();
			numMessage = 0;
			messageLength = 0;
			msgtype = null;
			_curMsgStreamIndex = 0;
		}

		/// <summary>
		/// Put yourself back to the buffer pool
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

		public void send(NetworkInterface networkInterface)
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

			// The unused MemoryStream placed back into the buffer pool to reduce garbage consumption
			for (int i = 0; i < streamList.Count; ++i)
			{
				streamList[i].reclaimObject();
			}

			streamList.Clear();
			stream.clear();

       // We believe that the transmission is complete, it is regarded as this bundle no longer in use,
      // So we will put it back to the object pool to reduce garbage collection is to bring consumption,
      // If need to continue to use, should be to re-Bundle. createObject (), the
      // If the outside does not re-createObject()is used directly, it may appear inexplicable problems,
      // Just as a note, a warning to the user.
			Bundle.reclaimObject(this);
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
    }
}

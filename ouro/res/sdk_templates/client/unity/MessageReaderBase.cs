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
    public abstract class MessageReaderBase
    {
			public abstract void process(byte[] datas, MessageLengthEx offset, MessageLengthEx length);
    }
} 

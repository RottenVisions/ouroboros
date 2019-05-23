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
		Since KCP is an analog TCP stream, a UDP packet may contain one or more packets, and the last packet may be incomplete, so it directly inherits MessageReaderTCP.
	*/
    public class MessageReaderKCP : MessageReaderTCP
    {
    }
} 

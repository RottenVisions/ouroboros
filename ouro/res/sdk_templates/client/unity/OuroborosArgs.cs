namespace Ouroboros
{
	using System; 
	
	using MessageLengthEx = System.UInt32;
	
	/*
		Initialize the parameter class of Ouroboros
	*/
    public class OuroborosArgs 
    {
    	    // login ip and port
		public string ip = "127.0.0.1";
		public int port = @{OURO_LOGIN_PORT};
		
		// client type
		// Reference: http://www.ouroboros.org/docs/programming/clientsdkprogramming.html, client types
		public OuroborosApp.CLIENT_TYPE clientType = OuroborosApp.CLIENT_TYPE.CLIENT_TYPE_MINI;

                // Encrypted communication type
        public OuroborosApp.NETWORK_ENCRYPT_TYPE networkEncryptType = OuroborosApp.NETWORK_ENCRYPT_TYPE.ENCRYPT_TYPE_NONE;

        // Allow synchronization role position information to the server
                // Whether to automatically synchronize player information to the server, the information includes position and direction, milliseconds
                // Non-high real-time games do not need to open this option
        public int syncPlayerMS = 100;

		// Whether to use the alias mechanism
		// The selection of this parameter must be consistent with the parameters of ouroboros_defs.xml::cellapp/aliasEntityID
		public bool useAliasEntityID = @{OURO_USE_ALIAS_ENTITYID};

                // Whether to trigger the set_* event of the property when the Entity is initialized (callPropertysSetMethods)
        public bool isOnInitCallPropertysSetMethods = true;
        
		// send buffer size
		public MessageLengthEx TCP_SEND_BUFFER_MAX = NetworkInterfaceBase.TCP_PACKET_MAX;
		public MessageLengthEx UDP_SEND_BUFFER_MAX = 128;

		// Receive buffer size
		public MessageLengthEx TCP_RECV_BUFFER_MAX = NetworkInterfaceBase.TCP_PACKET_MAX;
		public MessageLengthEx UDP_RECV_BUFFER_MAX = 128;

		// Whether multi-threaded start
		public bool isMultiThreads = false;

		// only enabled in multi-threaded mode
		// thread main loop processing frequency
		public int threadUpdateHZ = @{OURO_UPDATEHZ};

		// Forced to disable UDP communication
		public bool forceDisableUDP = false;

		// heartbeat frequency (tick number)
		public int serverHeartbeatTick = 15;

		public int getTCPRecvBufferSize()
		{
			return (int)TCP_RECV_BUFFER_MAX;
		}

		public int getTCPSendBufferSize()
		{
			return (int)TCP_SEND_BUFFER_MAX;
		}

		public int getUDPRecvBufferSize()
		{
			return (int)UDP_RECV_BUFFER_MAX;
		}

		public int getUDPSendBufferSize()
		{
			return (int)UDP_SEND_BUFFER_MAX;
		}
    }

} 

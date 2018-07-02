namespace Ouroboros
{
	using System;

	using MessageLengthEx = System.UInt32;

	/*
		Initialization Ouroboros the parameters of the class
	*/
    public class OuroborosArgs
    {
    	// Login ip and port
		public string ip = "127.0.0.1";
		public int port = @{OURO_LOGIN_PORT};

		// Client type
		// Reference: http://www.ouroboros.org/docs/programming/clientsdkprogramming.html, client types
		public OuroborosApp.CLIENT_TYPE clientType = OuroborosApp.CLIENT_TYPE.CLIENT_TYPE_MINI;

		// Allow synchronization role position information to the server
		// Whether to open automatically synchronize player information to the server, the information comprising location and direction
		// Non-high-class games do not need to be open to this option
		public bool syncPlayer = true;

		// Whether to use the alias mechanism
		// This parameter must be selected with ouroboros_defs.xml::cellapp/aliasEntityID the parameters remain consistent
		public bool useAliasEntityID = @{OURO_USE_ALIAS_ENTITYID};

        // At Entity initialization time whether to trigger the properties of set_*Event(callPropertysSetMethods)
        public bool isOnInitCallPropertysSetMethods = true;

		// Send buffer size
		public MessageLengthEx SEND_BUFFER_MAX = NetworkInterface.TCP_PACKET_MAX;

		// Receive buffer size
		public MessageLengthEx RECV_BUFFER_MAX = NetworkInterface.TCP_PACKET_MAX;

		// Whether multi-threaded start
		public bool isMultiThreads = false;

		// Only in multi-thread mode is enabled
		// The thread main loop process frequency
		public int threadUpdateHZ = @{OURO_UPDATEHZ};

		// The heartbeat frequency tick number
		public int serverHeartbeatTick = 15;

		public int getRecvBufferSize()
		{
			return (int)RECV_BUFFER_MAX;
		}

		public int getSendBufferSize()
		{
			return (int)SEND_BUFFER_MAX;
		}
    }

}

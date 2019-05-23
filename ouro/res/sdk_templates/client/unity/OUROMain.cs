using UnityEngine;
using System;
using System.Collections;
using Ouroboros;

/*
	Can be understood as the plugin's entry module
	Installed the event (installEvents) that needs to be listened to in this entry, and initializes Ouroboros (initOuroboros).
*/
	
public class KBEMain : MonoBehaviour 
{
	public OuroborosApp gameapp = null;
	
	// visible options in the unity3d interface
	public DEBUGLEVEL debugLevel = DEBUGLEVEL.DEBUG;
	public bool isMultiThreads = false;
	public string ip = "127.0.0.1";
	public int port = @{OURO_LOGIN_PORT};
	public OuroborosApp.CLIENT_TYPE clientType = OuroborosApp.CLIENT_TYPE.CLIENT_TYPE_MINI;
	public OuroborosApp.NETWORK_ENCRYPT_TYPE networkEncryptType = OuroborosApp.NETWORK_ENCRYPT_TYPE.ENCRYPT_TYPE_NONE;
	public int syncPlayerMS = 1000 / @{OURO_UPDATEHZ};

	public int threadUpdateHZ = @{OURO_UPDATEHZ} * 2;
	public int serverHeartbeatTick = @{OURO_SERVER_EXTERNAL_TIMEOUT};
	public int TCP_SEND_BUFFER_MAX = (int)Ouroboros.NetworkInterfaceBase.TCP_PACKET_MAX;
	public int TCP_RECV_BUFFER_MAX = (int)Ouroboros.NetworkInterfaceBase.TCP_PACKET_MAX;
	public int UDP_SEND_BUFFER_MAX = (int)Ouroboros.NetworkInterfaceBase.UDP_PACKET_MAX;
	public int UDP_RECV_BUFFER_MAX = (int)Ouroboros.NetworkInterfaceBase.UDP_PACKET_MAX;
	public bool useAliasEntityID = @{OURO_USE_ALIAS_ENTITYID};
	public bool isOnInitCallPropertysSetMethods = true;
	public bool forceDisableUDP = false;

	public bool automaticallyUpdateSDK = true;

	protected virtual void Awake() 
	 {
		DontDestroyOnLoad(transform.gameObject);
	 }
 
	// Use this for initialization
	protected virtual void Start () 
	{
		MonoBehaviour.print("clientapp::start()");
		installEvents();
		initOuroboros();
	}
	
	public virtual void installEvents()
	{
        Ouroboros.Event.registerOut(EventOutTypes.onVersionNotMatch, this, "onVersionNotMatch");
        Ouroboros.Event.registerOut(EventOutTypes.onScriptVersionNotMatch, this, "onScriptVersionNotMatch");
	}
	
	public void onVersionNotMatch(string verInfo, string serVerInfo)
	{
#if UNITY_EDITOR
		if(automaticallyUpdateSDK)
			gameObject.AddComponent<ClientSDKUpdater>();
#endif
	}

	public void onScriptVersionNotMatch(string verInfo, string serVerInfo)
	{
#if UNITY_EDITOR
		if(automaticallyUpdateSDK)
			gameObject.AddComponent<ClientSDKUpdater>();
#endif
	}

	public virtual void initOuroboros()
	{
		// If an error occurs here, check Assets\Scripts\ouro_scripts\if_Entity_error_use______git_submodule_update_____ouroboros_plugins_______open_this_file_and_I_will_tell_you.cs

		Dbg.debugLevel = debugLevel;

		OuroborosArgs args = new OuroborosArgs();
		
		args.ip = ip;
		args.port = port;
		args.clientType = clientType;
        args.networkEncryptType = networkEncryptType;
        args.syncPlayerMS = syncPlayerMS;
		args.threadUpdateHZ = threadUpdateHZ;
		args.serverHeartbeatTick = serverHeartbeatTick / 2;
		args.useAliasEntityID = useAliasEntityID;
		args.isOnInitCallPropertysSetMethods = isOnInitCallPropertysSetMethods;
		args.forceDisableUDP = forceDisableUDP;

		args.TCP_SEND_BUFFER_MAX = (UInt32)TCP_SEND_BUFFER_MAX;
		args.TCP_RECV_BUFFER_MAX = (UInt32)TCP_RECV_BUFFER_MAX;
		args.UDP_SEND_BUFFER_MAX = (UInt32)UDP_SEND_BUFFER_MAX;
		args.UDP_RECV_BUFFER_MAX = (UInt32)UDP_RECV_BUFFER_MAX;

		args.isMultiThreads = isMultiThreads;
		
		if(isMultiThreads)
			gameapp = new OuroborosAppThread(args);
		else
			gameapp = new OuroborosApp(args);
	}
	
	protected virtual void OnDestroy()
	{
		MonoBehaviour.print("clientapp::OnDestroy(): begin");
        if (OuroborosApp.app != null)
        {
            OuroborosApp.app.destroy();
            OuroborosApp.app = null;
        }
		Ouroboros.Event.clear();
		MonoBehaviour.print("clientapp::OnDestroy(): end");
	}
	
	protected virtual void FixedUpdate () 
	{
		KBEUpdate();
	}

	public virtual void KBEUpdate()
	{
		// Single-threaded mode must be called by itself
		if(!isMultiThreads)
			gameapp.process();
		
		Ouroboros.Event.processOutEvents();
	}
}

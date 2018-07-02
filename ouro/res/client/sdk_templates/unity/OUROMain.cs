using UnityEngine;
using System;
using System.Collections;
using Ouroboros;

/*
	Can be understood as a plug-in inlet module
	In this entry, the installation needs to monitor the event(installEvents)While the initialization Ouroboros(initOuroboros)
*/

public class OUROMain : MonoBehaviour
{
	public OuroborosApp gameapp = null;

	// In unity3d interface visible in the options
	public DEBUGLEVEL debugLevel = DEBUGLEVEL.DEBUG;
	public bool isMultiThreads = false;
	public string ip = "127.0.0.1";
	public int port = @{OURO_LOGIN_PORT};
	public OuroborosApp.CLIENT_TYPE clientType = OuroborosApp.CLIENT_TYPE.CLIENT_TYPE_MINI;
	public bool syncPlayer = true;
	public int threadUpdateHZ = @{OURO_UPDATEHZ};
	public int serverHeartbeatTick = 15;
	public int SEND_BUFFER_MAX = (int)Ouroboros.NetworkInterface.TCP_PACKET_MAX;
	public int RECV_BUFFER_MAX = (int)Ouroboros.NetworkInterface.TCP_PACKET_MAX;
	public bool useAliasEntityID = @{OURO_USE_ALIAS_ENTITYID};
	public bool isOnInitCallPropertysSetMethods = true;

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
	}

	public virtual void initOuroboros()
	{
		// If this error occurs, please review Assets\Scripts\ouro_scripts\if_Entity_error_use______git_submodule_update_____ouroboros_plugins_______open_this_file_and_I_will_tell_you.cs

		Dbg.debugLevel = debugLevel;

		OuroborosArgs args = new OuroborosArgs();

		args.ip = ip;
		args.port = port;
		args.clientType = clientType;
		args.syncPlayer = syncPlayer;
		args.threadUpdateHZ = threadUpdateHZ;
		args.serverHeartbeatTick = serverHeartbeatTick;
		args.useAliasEntityID = useAliasEntityID;
		args.isOnInitCallPropertysSetMethods = isOnInitCallPropertysSetMethods;

		args.SEND_BUFFER_MAX = (UInt32)SEND_BUFFER_MAX;
		args.RECV_BUFFER_MAX = (UInt32)RECV_BUFFER_MAX;

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
		MonoBehaviour.print("clientapp::OnDestroy(): end");
	}

	protected virtual void FixedUpdate ()
	{
		OUROUpdate();
	}

	public virtual void OUROUpdate()
	{
		// The single thread mode must be your call
		if(!isMultiThreads)
			gameapp.process();

		Ouroboros.Event.processOutEvents();
	}
}

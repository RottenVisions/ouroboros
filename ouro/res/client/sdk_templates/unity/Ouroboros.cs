namespace Ouroboros
{
  	using UnityEngine;
	using System;
	using System.Collections;
	using System.Collections.Generic;
	using System.Text;
    using System.Threading;
	using System.Text.RegularExpressions;

	using MessageID = System.UInt16;
	using MessageLength = System.UInt16;

	/*
		This is Ouroboros plug-in core module
		Including network creation, persistence, protocols, entities management, as well as causing the external callable interface.

		Some may refer to places:
		http://www.ouroboros.org/docs/programming/clientsdkprogramming.html
		http://www.ouroboros.org/docs/programming/ouro_message_format.html

		http://www.ouroboros.org/cn/docs/programming/clientsdkprogramming.html
		http://www.ouroboros.org/cn/docs/programming/ouro_message_format.html
	*/
	public class OuroborosApp
	{
		public static OuroborosApp app = null;
		private NetworkInterface _networkInterface = null;

        OuroborosArgs _args = null;

    	// Client categories
    	// http://www.ouroboros.org/docs/programming/clientsdkprogramming.html
    	// http://www.ouroboros.org/cn/docs/programming/clientsdkprogramming.html
		public enum CLIENT_TYPE
		{
			// Mobile(Phone, Pad)
			CLIENT_TYPE_MOBILE				= 1,

			// Windows Application program
			CLIENT_TYPE_WIN					= 2,

			// Linux Application program
			CLIENT_TYPE_LINUX				= 3,

			// Mac Application program
			CLIENT_TYPE_MAC					= 4,

			// Web，HTML5，Flash
			CLIENT_TYPE_BROWSER				= 5,

			// bots
			CLIENT_TYPE_BOTS				= 6,

			// Mini-Client
			CLIENT_TYPE_MINI				= 7,
		};

        public string username = "ouroboros";
        public string password = "123456";

		// The service end of the assigned baseapp address
		public string baseappIP = "";
		public UInt16 baseappPort = 0;

		// The current state of the
		public string currserver = "";
		public string currstate = "";

		// The service end of the downlink and the client uplink is used when logging in the processing of the binding account binary information
		// The information from the user's own extension
		private byte[] _serverdatas = new byte[0];
		private byte[] _clientdatas = new byte[0];

		// Communication Protocol encryption, blowfish Protocol
		private byte[] _encryptedKey = new byte[0];

		// Service side and the client version number, and Protocol MD5
		public string serverVersion = "";
		public string clientVersion = "@{OURO_VERSION}";
		public string serverScriptVersion = "";
		public string clientScriptVersion = "@{OURO_SCRIPT_VERSION}";
		public string serverProtocolMD5 = "@{OURO_SERVER_PROTO_MD5}";
		public string serverEntitydefMD5 = "@{OURO_SERVER_ENTITYDEF_MD5}";

		// The current player's entity id and entity type
		public UInt64 entity_uuid = 0;
		public Int32 entity_id = 0;
		public string entity_type = "";

		private List<Entity> _controlledEntities = new List<Entity>();

		// Current service end of the last synchronization over the players position
		private Vector3 _entityServerPos = new Vector3(0f, 0f, 0f);

		// space data, in particular to see the API Manual on spaceData
		// https://github.com/ouroboros/ouroboros/tree/master/docs/api
		private Dictionary<string, string> _spacedatas = new Dictionary<string, string>();

		// All entities are stored in here, Please refer to the API Manual on the entities part of the
		// https://github.com/ouroboros/ouroboros/tree/master/docs/api
		public Dictionary<Int32, Entity> entities = new Dictionary<Int32, Entity>();

		// In the players View range is less than 256 entities when we can by a one-byte index to find the entity
		private List<Int32> _entityIDAliasIDList = new List<Int32>();
		private Dictionary<Int32, MemoryStream> _bufferedCreateEntityMessages = new Dictionary<Int32, MemoryStream>();

		// All service-side error code corresponding to the error description
		private ServerErrorDescrs _serverErrs = new ServerErrorDescrs();

		private System.DateTime _lastTickTime = System.DateTime.Now;
		private System.DateTime _lastTickCBTime = System.DateTime.Now;
		private System.DateTime _lastUpdateToServerTime = System.DateTime.Now;

		// The player where the current space id, As well as the space corresponding to the resource
		public UInt32 spaceID = 0;
		public string spaceResPath = "";
		public bool isLoadedGeometry = false;

		// According to the standard, each client section should contain this attribute
		public const string component = "client";

        public OuroborosApp(OuroborosArgs args)
        {
			if (app != null)
				throw new Exception("Only one instance of OuroborosApp!");

			app = this;

			initialize(args);
        }

		public virtual bool initialize(OuroborosArgs args)
		{
			_args = args;

			EntityDef.init();

        	initNetwork();

            // Registration event
            installEvents();

         	return true;
		}

		void initNetwork()
		{
			Messages.init();
        	_networkInterface = new NetworkInterface();
		}

		void installEvents()
		{
			Event.registerIn("createAccount", this, "createAccount");
			Event.registerIn("login", this, "login");
			Event.registerIn("reloginBaseapp", this, "reloginBaseapp");
			Event.registerIn("resetPassword", this, "resetPassword");
			Event.registerIn("bindAccountEmail", this, "bindAccountEmail");
			Event.registerIn("newPassword", this, "newPassword");

			// Internal events
			Event.registerIn("_closeNetwork", this, "_closeNetwork");
		}

		public OuroborosArgs getInitArgs()
		{
			return _args;
		}

        public virtual void destroy()
        {
        	Dbg.WARNING_MSG("Ouroboros::destroy()");

        	reset();
        	Ouroboros.Event.deregisterIn(this);
        	resetMessages();

        	OuroborosApp.app = null;
        }

        public NetworkInterface networkInterface()
        {
        	return _networkInterface;
        }

        public byte[] serverdatas()
        {
        	return _serverdatas;
        }

        public void entityServerPos(Vector3 pos)
        {
        	_entityServerPos = pos;
        }

        public void resetMessages()
        {
			_serverErrs.Clear();
			Messages.clear();
			EntityDef.reset();

			Entity.clear();
			Dbg.DEBUG_MSG("Ouroboros::resetMessages()");
        }

		public virtual void reset()
		{
			Ouroboros.Event.clearFiredEvents();

			clearEntities(true);

			currserver = "";
			currstate = "";
			_serverdatas = new byte[0];
			serverVersion = "";
			serverScriptVersion = "";

			entity_uuid = 0;
			entity_id = 0;
			entity_type = "";

			_entityIDAliasIDList.Clear();
			_bufferedCreateEntityMessages.Clear();

			_lastTickTime = System.DateTime.Now;
			_lastTickCBTime = System.DateTime.Now;
			_lastUpdateToServerTime = System.DateTime.Now;

			spaceID = 0;
			spaceResPath = "";
			isLoadedGeometry = false;

			if (_networkInterface != null)
				_networkInterface.reset();

			_networkInterface = new NetworkInterface();

			_spacedatas.Clear();
		}

		public static bool validEmail(string strEmail)
		{
			return Regex.IsMatch(strEmail, @"^([\w-\.]+)@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.)
				|(([\w-]+\.)+))([a-zA-Z]{2,4}|[0-9]{1,3})(\]?)$");
		}

		/*
			插件的主循环处理函数
		*/
		public virtual void process()
		{
			// 处理网络
			if (_networkInterface != null)
				_networkInterface.process();

			// 处理外层抛入的事件
			Event.processInEvents();

			// 向服务端发送心跳以及同步角色信息到服务端
			sendTick();
		}

		/*
			当前玩家entity
		*/
		public Entity player()
		{
			Entity e;
			if(entities.TryGetValue(entity_id, out e))
				return e;

			return null;
		}

		public void _closeNetwork(NetworkInterface networkInterface)
		{
			networkInterface.close();
		}

		/*
			向服务端发送心跳以及同步角色信息到服务端
		*/
		public void sendTick()
		{
			if(_networkInterface == null || _networkInterface.connected == false)
				return;

			TimeSpan span = DateTime.Now - _lastTickTime;

			// Update the player's position and orientation to the service end
			updatePlayerToServer();

			if(span.Seconds > _args.serverHeartbeatTick)
			{
				span = _lastTickCBTime - _lastTickTime;

				// If the heartbeat callback received time is less than the Send heartbeat time, the description did not receive a callback
				// At this time should inform the client dropped.
				if(span.Seconds < 0)
				{
					Dbg.ERROR_MSG("sendTick: Receive appTick timeout!");
					_networkInterface.close();
					return;
				}

				Message Loginapp_onClientActiveTickMsg = null;
				Message Baseapp_onClientActiveTickMsg = null;

				Messages.messages.TryGetValue("Loginapp_onClientActiveTick", out Loginapp_onClientActiveTickMsg);
				Messages.messages.TryGetValue("Baseapp_onClientActiveTick", out Baseapp_onClientActiveTickMsg);

				if(currserver == "loginapp")
				{
					if(Loginapp_onClientActiveTickMsg != null)
					{
						Bundle bundle = Bundle.createObject();
						bundle.newMessage(Messages.messages["Loginapp_onClientActiveTick"]);
						bundle.send(_networkInterface);
					}
				}
				else
				{
					if(Baseapp_onClientActiveTickMsg != null)
					{
						Bundle bundle = Bundle.createObject();
						bundle.newMessage(Messages.messages["Baseapp_onClientActiveTick"]);
						bundle.send(_networkInterface);
					}
				}

				_lastTickTime = System.DateTime.Now;
			}
		}

		/*
			Server heartbeat callback
		*/
		public void Client_onAppActiveTickCB()
		{
			_lastTickCBTime = System.DateTime.Now;
		}

		/*
			With the service end of the handshake, with any one of the processes after the connection should the first be a handshake
		*/
		public void hello()
		{
			Bundle bundle = Bundle.createObject();
			if(currserver == "loginapp")
				bundle.newMessage(Messages.messages["Loginapp_hello"]);
			else
				bundle.newMessage(Messages.messages["Baseapp_hello"]);

			bundle.writeString(clientVersion);
			bundle.writeString(clientScriptVersion);
			bundle.writeBlob(_encryptedKey);
			bundle.send(_networkInterface);
		}

		/*
			The handshake after the service end of the callback
		*/
		public void Client_onHelloCB(MemoryStream stream)
		{
			serverVersion = stream.readString();
			serverScriptVersion = stream.readString();
			serverProtocolMD5 = stream.readString();
			serverEntitydefMD5 = stream.readString();
			Int32 ctype = stream.readInt32();

			Dbg.DEBUG_MSG("Ouroboros::Client_onHelloCB: verInfo(" + serverVersion
				+ "), scriptVersion("+ serverScriptVersion + "), srvProtocolMD5("+ serverProtocolMD5
				+ "), srvEntitydefMD5("+ serverEntitydefMD5 + "), + ctype(" + ctype + ")!");

			onServerDigest();

			if(currserver == "baseapp")
			{
				onLogin_baseapp();
			}
			else
			{
				onLogin_loginapp();
			}
		}

		/*
			Service end Error Description The import.
		*/
		public void Client_onImportServerErrorsDescr(MemoryStream stream)
		{
			// Without achieve, by the plug-in generates static code
		}

		/*
			From the service end returns a binary stream into the client Message Protocol
		*/
		public void Client_onImportClientMessages(MemoryStream stream)
		{
			// Without achieve, by the plug-in generates static code
		}

		/*
			From the service end returns a binary stream into the client Message Protocol
		*/
		public void Client_onImportClientEntityDef(MemoryStream stream)
		{
			// Without achieve, by the plug-in generates static code
		}

		/*
			Engine version does not match
		*/
		public void Client_onVersionNotMatch(MemoryStream stream)
		{
			serverVersion = stream.readString();

			Dbg.ERROR_MSG("Client_onVersionNotMatch: verInfo=" + clientVersion + "(server: " + serverVersion + ")");
			Event.fireAll("onVersionNotMatch", new object[]{clientVersion, serverVersion});
		}

		/*
			The script version does not match
		*/
		public void Client_onScriptVersionNotMatch(MemoryStream stream)
		{
			serverScriptVersion = stream.readString();

			Dbg.ERROR_MSG("Client_onScriptVersionNotMatch: verInfo=" + clientScriptVersion + "(server: " + serverScriptVersion + ")");
			Event.fireAll("onScriptVersionNotMatch", new object[]{clientScriptVersion, serverScriptVersion});
		}

		/*
			Is Service end to kick out
		*/
		public void Client_onKicked(UInt16 failedcode)
		{
			Dbg.DEBUG_MSG("Client_onKicked: failedcode=" + failedcode);
			Event.fireAll("onKicked", new object[]{failedcode});
		}

		/*
			Login to the server, you must log in to complete the loginapp and the gateway(baseapp), The login process is completed
		*/
		public void login(string username, string password, byte[] datas)
		{
			OuroborosApp.app.username = username;
			OuroborosApp.app.password = password;
			OuroborosApp.app._clientdatas = datas;

			OuroborosApp.app.login_loginapp(true);
		}

		/*
			Log in to the service end(loginapp), After a successful login you must also log on to the gateway(baseapp)The login process is completed
		*/
		public void login_loginapp(bool noconnect)
		{
			if(noconnect)
			{
				reset();
				_networkInterface.connectTo(_args.ip, _args.port, onConnectTo_loginapp_callback, null);
			}
			else
			{
				Dbg.DEBUG_MSG("Ouroboros::login_loginapp(): send login! username=" + username);
				Bundle bundle = Bundle.createObject();
				bundle.newMessage(Messages.messages["Loginapp_login"]);
				bundle.writeInt8((sbyte)_args.clientType);
				bundle.writeBlob(OuroborosApp.app._clientdatas);
				bundle.writeString(username);
				bundle.writeString(password);
				bundle.send(_networkInterface);
			}
		}

		private void onConnectTo_loginapp_callback(string ip, int port, bool success, object userData)
		{
			_lastTickCBTime = System.DateTime.Now;

			if(!success)
			{
				Dbg.ERROR_MSG(string.Format("Ouroboros::login_loginapp(): connect {0}:{1} is error!", ip, port));
				return;
			}

			currserver = "loginapp";
			currstate = "login";

			Dbg.DEBUG_MSG(string.Format("Ouroboros::login_loginapp(): connect {0}:{1} is success!", ip, port));

			hello();
		}

		private void onLogin_loginapp()
		{
			_lastTickCBTime = System.DateTime.Now;
			login_loginapp(false);
		}

		/*
			Log on to the service side, login to the gateway(baseapp)
		*/
		public void login_baseapp(bool noconnect)
		{
			if(noconnect)
			{
				Event.fireOut("onLoginBaseapp", new object[]{});

				_networkInterface.reset();
				_networkInterface = new NetworkInterface();
				_networkInterface.connectTo(baseappIP, baseappPort, onConnectTo_baseapp_callback, null);
			}
			else
			{
				Bundle bundle = Bundle.createObject();
				bundle.newMessage(Messages.messages["Baseapp_loginBaseapp"]);
				bundle.writeString(username);
				bundle.writeString(password);
				bundle.send(_networkInterface);
			}
		}

		private void onConnectTo_baseapp_callback(string ip, int port, bool success, object userData)
		{
			_lastTickCBTime = System.DateTime.Now;

			if(!success)
			{
				Dbg.ERROR_MSG(string.Format("Ouroboros::login_baseapp(): connect {0}:{1} is error!", ip, port));
				return;
			}

			currserver = "baseapp";
			currstate = "";

			Dbg.DEBUG_MSG(string.Format("Ouroboros::login_baseapp(): connect {0}:{1} is successfully!", ip, port));

			hello();
		}

		private void onLogin_baseapp()
		{
			_lastTickCBTime = System.DateTime.Now;
			login_baseapp(false);
		}

		/*
			Re-login to the gateway(baseapp)
			Some of the mobile applications easily dropped, you can use this feature to quickly re-with the server to establish a communication
		*/
		public void reloginBaseapp()
		{
			if(_networkInterface.valid())
				return;

			Event.fireAll("onReloginBaseapp", new object[]{});
			_networkInterface.connectTo(baseappIP, baseappPort, onReConnectTo_baseapp_callback, null);
		}

		private void onReConnectTo_baseapp_callback(string ip, int port, bool success, object userData)
		{
			if(!success)
			{
				Dbg.ERROR_MSG(string.Format("Ouroboros::reloginBaseapp(): connect {0}:{1} is error!", ip, port));
				return;
			}


			Dbg.DEBUG_MSG(string.Format("Ouroboros::relogin_baseapp(): connect {0}:{1} is successfully!", ip, port));

			Bundle bundle = Bundle.createObject();
			bundle.newMessage(Messages.messages["Baseapp_reloginBaseapp"]);
			bundle.writeString(username);
			bundle.writeString(password);
			bundle.writeUint64(entity_uuid);
			bundle.writeInt32(entity_id);
			bundle.send(_networkInterface);

			_lastTickCBTime = System.DateTime.Now;
		}

		/*
			By the error id to get a description of the error
		*/
		public string serverErr(UInt16 id)
		{
			return _serverErrs.serverErrStr(id);
		}

		public void onOpenLoginapp_resetpassword()
		{
			Dbg.DEBUG_MSG("Ouroboros::onOpenLoginapp_resetpassword: successfully!");
			currserver = "loginapp";
			currstate = "resetpassword";
			_lastTickCBTime = System.DateTime.Now;

			resetpassword_loginapp(false);
		}

		/*
			Reset the password, by loginapp
		*/
		public void resetPassword(string username)
		{
			OuroborosApp.app.username = username;
			resetpassword_loginapp(true);
		}

		/*
			To reset the password, by loginapp
		*/
		public void resetpassword_loginapp(bool noconnect)
		{
			if(noconnect)
			{
				reset();
				_networkInterface.connectTo(_args.ip, _args.port, onConnectTo_resetpassword_callback, null);
			}
			else
			{
				Bundle bundle = Bundle.createObject();
				bundle.newMessage(Messages.messages["Loginapp_reqAccountResetPassword"]);
				bundle.writeString(username);
				bundle.send(_networkInterface);
			}
		}

		private void onConnectTo_resetpassword_callback(string ip, int port, bool success, object userData)
		{
			_lastTickCBTime = System.DateTime.Now;

			if(!success)
			{
				Dbg.ERROR_MSG(string.Format("Ouroboros::resetpassword_loginapp(): connect {0}:{1} is error!", ip, port));
				return;
			}

			Dbg.DEBUG_MSG(string.Format("Ouroboros::resetpassword_loginapp(): connect {0}:{1} is success!", ip, port));
			onOpenLoginapp_resetpassword();
		}

		public void Client_onReqAccountResetPasswordCB(UInt16 failcode)
		{
			if(failcode != 0)
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onReqAccountResetPasswordCB: " + username + " is failed! code=" + failcode + "!");
				return;
			}

			Dbg.DEBUG_MSG("Ouroboros::Client_onReqAccountResetPasswordCB: " + username + " is successfully!");
		}

		/*
			Binding Email, by the baseapp
		*/
		public void bindAccountEmail(string emailAddress)
		{
			Bundle bundle = Bundle.createObject();
			bundle.newMessage(Messages.messages["Baseapp_reqAccountBindEmail"]);
			bundle.writeInt32(entity_id);
			bundle.writeString(password);
			bundle.writeString(emailAddress);
			bundle.send(_networkInterface);
		}

		public void Client_onReqAccountBindEmailCB(UInt16 failcode)
		{
			if(failcode != 0)
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onReqAccountBindEmailCB: " + username + " is failed! code=" + failcode + "!");
				return;
			}

			Dbg.DEBUG_MSG("Ouroboros::Client_onReqAccountBindEmailCB: " + username + " is successfully!");
		}

		/*
			Set a new password, by baseapp, the Have the players log in online operation it is baseapp it.
		*/
		public void newPassword(string old_password, string new_password)
		{
			Bundle bundle = Bundle.createObject();
			bundle.newMessage(Messages.messages["Baseapp_reqAccountNewPassword"]);
			bundle.writeInt32(entity_id);
			bundle.writeString(old_password);
			bundle.writeString(new_password);
			bundle.send(_networkInterface);
		}

		public void Client_onReqAccountNewPasswordCB(UInt16 failcode)
		{
			if(failcode != 0)
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onReqAccountNewPasswordCB: " + username + " is failed! code=" + failcode + "!");
				return;
			}

			Dbg.DEBUG_MSG("Ouroboros::Client_onReqAccountNewPasswordCB: " + username + " is successfully!");
		}

		public void createAccount(string username, string password, byte[] datas)
		{
			OuroborosApp.app.username = username;
			OuroborosApp.app.password = password;
			OuroborosApp.app._clientdatas = datas;

			OuroborosApp.app.createAccount_loginapp(true);
		}

		/*
			Create an account, by loginapp
		*/
		public void createAccount_loginapp(bool noconnect)
		{
			if(noconnect)
			{
				reset();
				_networkInterface.connectTo(_args.ip, _args.port, onConnectTo_createAccount_callback, null);
			}
			else
			{
				Bundle bundle = Bundle.createObject();
				bundle.newMessage(Messages.messages["Loginapp_reqCreateAccount"]);
				bundle.writeString(username);
				bundle.writeString(password);
				bundle.writeBlob(OuroborosApp.app._clientdatas);
				bundle.send(_networkInterface);
			}
		}

		public void onOpenLoginapp_createAccount()
		{
			Dbg.DEBUG_MSG("Ouroboros::onOpenLoginapp_createAccount: successfully!");
			currserver = "loginapp";
			currstate = "createAccount";
			_lastTickCBTime = System.DateTime.Now;

			createAccount_loginapp(false);
		}

		private void onConnectTo_createAccount_callback(string ip, int port, bool success, object userData)
		{
			_lastTickCBTime = System.DateTime.Now;

			if(!success)
			{
				Dbg.ERROR_MSG(string.Format("Ouroboros::createAccount_loginapp(): connect {0}:{1} is error!", ip, port));
				return;
			}

			Dbg.DEBUG_MSG(string.Format("Ouroboros::createAccount_loginapp(): connect {0}:{1} is success!", ip, port));
			onOpenLoginapp_createAccount();
		}

		/*
			Get the service end of the summary information, A summary including Protocol MD5, entitydefMD5
		*/
		public void onServerDigest()
		{
		}

		/*
			Login loginapp failed
		*/
		public void Client_onLoginFailed(MemoryStream stream)
		{
			UInt16 failedcode = stream.readUint16();
			_serverdatas = stream.readBlob();
			Dbg.ERROR_MSG("Ouroboros::Client_onLoginFailed: failedcode(" + failedcode + "), datas(" + _serverdatas.Length + ")!");
			Event.fireAll("onLoginFailed", new object[]{failedcode});
		}

		/*
			Login loginapp success.
		*/
		public void Client_onLoginSuccessfully(MemoryStream stream)
		{
			var accountName = stream.readString();
			username = accountName;
			baseappIP = stream.readString();
			baseappPort = stream.readUint16();

			Dbg.DEBUG_MSG("Ouroboros::Client_onLoginSuccessfully: accountName(" + accountName + "), addr(" +
					baseappIP + ":" + baseappPort + "), datas(" + _serverdatas.Length + ")!");

			_serverdatas = stream.readBlob();
			login_baseapp(true);
		}

		/*
			Login to baseapp failed.
		*/
		public void Client_onLoginBaseappFailed(UInt16 failedcode)
		{
			Dbg.ERROR_MSG("Ouroboros::Client_onLoginBaseappFailed: failedcode(" + failedcode + ")!");
			Event.fireAll("onLoginBaseappFailed", new object[]{failedcode});
		}

		/*
			Re-login to baseapp failed.
		*/
		public void Client_onReloginBaseappFailed(UInt16 failedcode)
		{
			Dbg.ERROR_MSG("Ouroboros::Client_onReloginBaseappFailed: failedcode(" + failedcode + ")!");
			Event.fireAll("onReloginBaseappFailed", new object[]{failedcode});
		}

		/*
			Login baseapp success.
		*/
		public void Client_onReloginBaseappSuccessfully(MemoryStream stream)
		{
			entity_uuid = stream.readUint64();
			Dbg.DEBUG_MSG("Ouroboros::Client_onReloginBaseappSuccessfully: name(" + username + ")!");
			Event.fireAll("onReloginBaseappSuccessfully", new object[]{});
		}

		/*
			The service end notification to create a character
		*/
		public void Client_onCreatedProxies(UInt64 rndUUID, Int32 eid, string entityType)
		{
			Dbg.DEBUG_MSG("Ouroboros::Client_onCreatedProxies: eid(" + eid + "), entityType(" + entityType + ")!");

			entity_uuid = rndUUID;
			entity_id = eid;
			entity_type = entityType;

			if(!this.entities.ContainsKey(eid))
			{
				ScriptModule module = null;
				if(!EntityDef.moduledefs.TryGetValue(entityType, out module))
				{
					Dbg.ERROR_MSG("Ouroboros::Client_onCreatedProxies: not found module(" + entityType + ")!");
					return;
				}

				Type runclass = module.entityScript;
				if(runclass == null)
					return;

				Entity entity = (Entity)Activator.CreateInstance(runclass);
				entity.id = eid;
				entity.className = entityType;
				entity.onGetBase();

				entities[eid] = entity;

				MemoryStream entityMessage = null;
				_bufferedCreateEntityMessages.TryGetValue(eid, out entityMessage);

				if(entityMessage != null)
				{
					Client_onUpdatePropertys(entityMessage);
					_bufferedCreateEntityMessages.Remove(eid);
					entityMessage.reclaimObject();
				}

				entity.__init__();
				entity.inited = true;

				if(_args.isOnInitCallPropertysSetMethods)
					entity.callPropertysSetMethods();
			}
			else
			{
				MemoryStream entityMessage = null;
				_bufferedCreateEntityMessages.TryGetValue(eid, out entityMessage);

				if(entityMessage != null)
				{
					Client_onUpdatePropertys(entityMessage);
					_bufferedCreateEntityMessages.Remove(eid);
					entityMessage.reclaimObject();
				}
			}
		}

		public Entity findEntity(Int32 entityID)
		{
			Entity entity = null;

			if(!entities.TryGetValue(entityID, out entity))
			{
				return null;
			}

			return entity;
		}

		/*
			Through the flow data to obtain a View of the entity ID
		*/
		public Int32 getViewEntityIDFromStream(MemoryStream stream)
		{
			if (!_args.useAliasEntityID)
				return stream.readInt32();

			Int32 id = 0;
			if(_entityIDAliasIDList.Count > 255)
			{
				id = stream.readInt32();
			}
			else
			{
				byte aliasID = stream.readUint8();

         // If it is 0 and on the client the next step is to re-login or reconnect operation and service end entity in the disconnection period has been in the online state
        // You can ignore this error, because the cellapp may have been in the baseapp sends a synchronization message, when the client re-connect when not, etc.
        // Server initialization step begins with the receipt of the synchronization information, in this case where an error occurs.
				if(_entityIDAliasIDList.Count <= aliasID)
					return 0;

				id = _entityIDAliasIDList[aliasID];
			}

			return id;
		}

		/*
			The service uses an optimized way to update the entity attribute data
		*/
		public void Client_onUpdatePropertysOptimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);
			onUpdatePropertys_(eid, stream);
		}

		/*
			The server update the entity attribute data
		*/
		public void Client_onUpdatePropertys(MemoryStream stream)
		{
			Int32 eid = stream.readInt32();
			onUpdatePropertys_(eid, stream);
		}

		public void onUpdatePropertys_(Int32 eid, MemoryStream stream)
		{
			Entity entity = null;

			if(!entities.TryGetValue(eid, out entity))
			{
				MemoryStream entityMessage = null;
				if(_bufferedCreateEntityMessages.TryGetValue(eid, out entityMessage))
				{
					Dbg.ERROR_MSG("Ouroboros::Client_onUpdatePropertys: entity(" + eid + ") not found!");
					return;
				}

				MemoryStream stream1 = MemoryStream.createObject();
				stream1.wpos = stream.wpos;
				stream1.rpos = stream.rpos - 4;
				Array.Copy(stream.data(), stream1.data(), stream.data().Length);
				_bufferedCreateEntityMessages[eid] = stream1;
				return;
			}

			entity.onUpdatePropertys(stream);
		}

		/*
			The service uses an optimized way to invoke entity method
		*/
		public void Client_onRemoteMethodCallOptimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);
			onRemoteMethodCall_(eid, stream);
		}

		/*
			Service end invoke entity method
		*/
		public void Client_onRemoteMethodCall(MemoryStream stream)
		{
			Int32 eid = stream.readInt32();
			onRemoteMethodCall_(eid, stream);
		}

		public void onRemoteMethodCall_(Int32 eid, MemoryStream stream)
		{
			Entity entity = null;

			if(!entities.TryGetValue(eid, out entity))
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onRemoteMethodCall: entity(" + eid + ") not found!");
				return;
			}

			entity.onRemoteMethodCall(stream);
		}

		/*
			The service end notification of one entity into the world(if the entity is the current player is the player for the first time in a space to create,
      and if the other entity is the other entity enters a player's View)
		*/
		public void Client_onEntityEnterWorld(MemoryStream stream)
		{
			Int32 eid = stream.readInt32();
			if(entity_id > 0 && entity_id != eid)
				_entityIDAliasIDList.Add(eid);

			UInt16 uentityType;
			if(EntityDef.idmoduledefs.Count > 255)
				uentityType = stream.readUint16();
			else
				uentityType = stream.readUint8();

			sbyte isOnGround = 1;

			if(stream.length() > 0)
				isOnGround = stream.readInt8();

			string entityType = EntityDef.idmoduledefs[uentityType].name;
			// Dbg.DEBUG_MSG("Ouroboros::Client_onEntityEnterWorld: " + entityType + "(" + eid + "), spaceID(" + OuroborosApp.app.spaceID + ")!");

			Entity entity = null;

			if(!entities.TryGetValue(eid, out entity))
			{
				MemoryStream entityMessage = null;
				if(!_bufferedCreateEntityMessages.TryGetValue(eid, out entityMessage))
				{
					Dbg.ERROR_MSG("Ouroboros::Client_onEntityEnterWorld: entity(" + eid + ") not found!");
					return;
				}

				ScriptModule module = null;
				if(!EntityDef.moduledefs.TryGetValue(entityType, out module))
				{
					Dbg.ERROR_MSG("Ouroboros::Client_onEntityEnterWorld: not found module(" + entityType + ")!");
				}

				Type runclass = module.entityScript;
				if(runclass == null)
					return;

				entity = (Entity)Activator.CreateInstance(runclass);
				entity.id = eid;
				entity.className = entityType;
				entity.onGetCell();

				entities[eid] = entity;

				Client_onUpdatePropertys(entityMessage);
				_bufferedCreateEntityMessages.Remove(eid);
				entityMessage.reclaimObject();

				entity.isOnGround = isOnGround > 0;
				entity.onDirectionChanged(entity.direction);
				entity.onPositionChanged(entity.position);

				entity.__init__();
				entity.inited = true;
				entity.inWorld = true;
				entity.enterWorld();

				if(_args.isOnInitCallPropertysSetMethods)
					entity.callPropertysSetMethods();
			}
			else
			{
				if(!entity.inWorld)
				{
           // Sake of security, here to empty it out
          // If the service end use giveClientTo switching control
          // Before the entity has entered the world, after the handover of the entities also entered into the world, there may be residual prior to that entity entering the world of information
					_entityIDAliasIDList.Clear();
					clearEntities(false);
					entities[entity.id] = entity;

					entity.onGetCell();

					entity.onDirectionChanged(entity.direction);
					entity.onPositionChanged(entity.position);

					_entityServerPos = entity.position;
					entity.isOnGround = isOnGround > 0;
					entity.inWorld = true;
					entity.enterWorld();

					if(_args.isOnInitCallPropertysSetMethods)
						entity.callPropertysSetMethods();
				}
			}
		}

		/*
			The service uses an optimized way to notify one entity left in the world(if the entity is the current player then the player leaves the space, if the other entity is the other entity to leave the player's View)
		*/
		public void Client_onEntityLeaveWorldOptimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);
			OuroborosApp.app.Client_onEntityLeaveWorld(eid);
		}

		/*
			The service end notification of one entity left in the world(if the entity is the current player then the player leaves the space,
      if the other entity is the other entity to leave the player's View)
		*/
		public void Client_onEntityLeaveWorld(Int32 eid)
		{
			Entity entity = null;

			if(!entities.TryGetValue(eid, out entity))
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onEntityLeaveWorld: entity(" + eid + ") not found!");
				return;
			}

			if(entity.inWorld)
				entity.leaveWorld();

			if(entity_id == eid)
			{
				clearSpace(false);
				entity.onLoseCell();
			}
			else
			{
				if(_controlledEntities.Remove(entity))
					Event.fireOut("onLoseControlledEntity", new object[]{entity});

				entities.Remove(eid);
				entity.onDestroy();
				_entityIDAliasIDList.Remove(eid);
			}
		}

		/*
			The service end notification to the current player into a new space
		*/
		public void Client_onEntityEnterSpace(MemoryStream stream)
		{
			Int32 eid = stream.readInt32();
			spaceID = stream.readUint32();

			sbyte isOnGround = 1;

			if(stream.length() > 0)
				isOnGround = stream.readInt8();

			Entity entity = null;

			if(!entities.TryGetValue(eid, out entity))
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onEntityEnterSpace: entity(" + eid + ") not found!");
				return;
			}

			entity.isOnGround = isOnGround > 0;
			_entityServerPos = entity.position;
			entity.enterSpace();
		}

		/*
			The service end notification to the current player to leave the space
		*/
		public void Client_onEntityLeaveSpace(Int32 eid)
		{
			Entity entity = null;

			if(!entities.TryGetValue(eid, out entity))
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onEntityLeaveSpace: entity(" + eid + ") not found!");
				return;
			}

			entity.leaveSpace();
			clearSpace(false);
		}

		/*
			Account create return result
		*/
		public void Client_onCreateAccountResult(MemoryStream stream)
		{
			UInt16 retcode = stream.readUint16();
			byte[] datas = stream.readBlob();

			Event.fireOut("onCreateAccountResult", new object[]{retcode, datas});

			if(retcode != 0)
			{
				Dbg.WARNING_MSG("Ouroboros::Client_onCreateAccountResult: " + username + " create is failed! code=" + retcode + "!");
				return;
			}

			Dbg.DEBUG_MSG("Ouroboros::Client_onCreateAccountResult: " + username + " create is successfully!");
		}

		/*
			Tell the client: you the current charge or to cancel the control who of the displacement of synchronization
		*/
		public void Client_onControlEntity(Int32 eid, sbyte isControlled)
		{
			Entity entity = null;

			if (!entities.TryGetValue(eid, out entity))
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onControlEntity: entity(" + eid + ") not found!");
				return;
			}

			var isCont = isControlled != 0;
			if (isCont)
			{
				// If controlled by the players themselves, and that means the players themselves are other people control.
				// So the players themselves should not go into this controlled list
				if (player().id != entity.id)
				{
					_controlledEntities.Add(entity);
				}
			}
			else
			{
				_controlledEntities.Remove(entity);
			}

			entity.isControlled = isCont;

			try
			{
				entity.onControlled(isCont);
				Event.fireOut("onControlled", new object[]{entity, isCont});
			}
			catch (Exception e)
			{
				Dbg.ERROR_MSG(string.Format("Ouroboros::Client_onControlEntity: entity id = '{0}', is controlled = '{1}', error = '{1}'", eid, isCont, e));
			}
		}

		/*
			Updates the current player's position and orientation to the service end, you can switch _syncPlayer turn off this mechanism
		*/
		public void updatePlayerToServer()
		{
			if(!_args.syncPlayer || spaceID == 0)
			{
				return;
			}

			var now = DateTime.Now;
			TimeSpan span = now - _lastUpdateToServerTime;

			if (span.Ticks < 1000000)
				return;

			Entity playerEntity = player();
			if (playerEntity == null || playerEntity.inWorld == false || playerEntity.isControlled)
				return;

			_lastUpdateToServerTime = now - (span - TimeSpan.FromTicks(1000000));

			Vector3 position = playerEntity.position;
			Vector3 direction = playerEntity.direction;

			bool posHasChanged = Vector3.Distance(playerEntity._entityLastLocalPos, position) > 0.001f;
			bool dirHasChanged = Vector3.Distance(playerEntity._entityLastLocalDir, direction) > 0.001f;

			if(posHasChanged || dirHasChanged)
			{
				playerEntity._entityLastLocalPos = position;
				playerEntity._entityLastLocalDir = direction;

				Bundle bundle = Bundle.createObject();
				bundle.newMessage(Messages.messages["Baseapp_onUpdateDataFromClient"]);
				bundle.writeFloat(position.x);
				bundle.writeFloat(position.y);
				bundle.writeFloat(position.z);

				double x = ((double)direction.x / 360 * (System.Math.PI * 2));
				double y = ((double)direction.y / 360 * (System.Math.PI * 2));
				double z = ((double)direction.z / 360 * (System.Math.PI * 2));

				// According to the Arc angle of the equation will be negative
				// unity will automatically translate to 0 to 360 degrees, where the need to do a restore
				if(x - System.Math.PI > 0.0)
					x -= System.Math.PI * 2;

				if(y - System.Math.PI > 0.0)
					y -= System.Math.PI * 2;

				if(z - System.Math.PI > 0.0)
					z -= System.Math.PI * 2;

				bundle.writeFloat((float)x);
				bundle.writeFloat((float)y);
				bundle.writeFloat((float)z);
				bundle.writeUint8((Byte)(playerEntity.isOnGround == true ? 1 : 0));
				bundle.writeUint32(spaceID);
				bundle.send(_networkInterface);
			}

			// Start sync of all the controlled entity position
			for (int i = 0; i < _controlledEntities.Count; ++i)
			{
				var entity = _controlledEntities[i];
				position = entity.position;
				direction = entity.direction;

				posHasChanged = Vector3.Distance(entity._entityLastLocalPos, position) > 0.001f;
				dirHasChanged = Vector3.Distance(entity._entityLastLocalDir, direction) > 0.001f;

				if (posHasChanged || dirHasChanged)
				{
					entity._entityLastLocalPos = position;
					entity._entityLastLocalDir = direction;

					Bundle bundle = Bundle.createObject();
					bundle.newMessage(Messages.messages["Baseapp_onUpdateDataFromClientForControlledEntity"]);
					bundle.writeInt32(entity.id);
					bundle.writeFloat(position.x);
					bundle.writeFloat(position.y);
					bundle.writeFloat(position.z);

					double x = ((double)direction.x / 360 * (System.Math.PI * 2));
					double y = ((double)direction.y / 360 * (System.Math.PI * 2));
					double z = ((double)direction.z / 360 * (System.Math.PI * 2));

					// According to the Arc angle of the equation will be negative
					// unity will automatically translate to 0 to 360 degrees, where the need to do a restore
					if(x - System.Math.PI > 0.0)
						x -= System.Math.PI * 2;

					if(y - System.Math.PI > 0.0)
						y -= System.Math.PI * 2;

					if(z - System.Math.PI > 0.0)
						z -= System.Math.PI * 2;

					bundle.writeFloat((float)x);
					bundle.writeFloat((float)y);
					bundle.writeFloat((float)z);
					bundle.writeUint8((Byte)(entity.isOnGround == true ? 1 : 0));
					bundle.writeUint32(spaceID);
					bundle.send(_networkInterface);
				}
			}
		}

		/*
			The current space to add on the geometry and other information of the mapped resource
			The client can use this resource information to load the corresponding scene
		*/
		public void addSpaceGeometryMapping(UInt32 uspaceID, string respath)
		{
			Dbg.DEBUG_MSG("Ouroboros::addSpaceGeometryMapping: spaceID(" + uspaceID + "), respath(" + respath + ")!");

			isLoadedGeometry = true;
			spaceID = uspaceID;
			spaceResPath = respath;
			Event.fireOut("addSpaceGeometryMapping", new object[]{spaceResPath});
		}

		public void clearSpace(bool isall)
		{
			_entityIDAliasIDList.Clear();
			_spacedatas.Clear();
			clearEntities(isall);
			isLoadedGeometry = false;
			spaceID = 0;
		}

		public void clearEntities(bool isall)
		{
			_controlledEntities.Clear();

			if (!isall)
			{
				Entity entity = player();

				foreach (KeyValuePair<Int32, Entity> dic in entities)
				{
					if(dic.Key == entity.id)
						continue;

					if(dic.Value.inWorld)
						dic.Value.leaveWorld();

				    dic.Value.onDestroy();
				}

				entities.Clear();
				entities[entity.id] = entity;
			}
			else
			{
				foreach (KeyValuePair<Int32, Entity> dic in entities)
				{
					if(dic.Value.inWorld)
						dic.Value.leaveWorld();

				    dic.Value.onDestroy();
				}

				entities.Clear();
			}
		}

		/*
			The service end of the initialization of the client spacedata, the spacedata please refer to the API
		*/
		public void Client_initSpaceData(MemoryStream stream)
		{
			clearSpace(false);
			spaceID = stream.readUint32();

			while(stream.length() > 0)
			{
				string key = stream.readString();
				string val = stream.readString();
				Client_setSpaceData(spaceID, key, val);
			}

			Dbg.DEBUG_MSG("Ouroboros::Client_initSpaceData: spaceID(" + spaceID + "), size(" + _spacedatas.Count + ")!");
		}

		/*
			Services end provided the client spacedata, the spacedata please refer to the API
		*/
		public void Client_setSpaceData(UInt32 spaceID, string key, string value)
		{
			Dbg.DEBUG_MSG("Ouroboros::Client_setSpaceData: spaceID(" + spaceID + "), key(" + key + "), value(" + value + ")!");
			_spacedatas[key] = value;

			if(key == "_mapping")
				addSpaceGeometryMapping(spaceID, value);

			Event.fireOut("onSetSpaceData", new object[]{spaceID, key, value});
		}

		/*
			The service side to delete the client spacedata, the spacedata please refer to the API
		*/
		public void Client_delSpaceData(UInt32 spaceID, string key)
		{
			Dbg.DEBUG_MSG("Ouroboros::Client_delSpaceData: spaceID(" + spaceID + "), key(" + key + ")");
			_spacedatas.Remove(key);
			Event.fireOut("onDelSpaceData", new object[]{spaceID, key});
		}

		public string getSpaceData(string key)
		{
			string val = "";

			if(!_spacedatas.TryGetValue(key, out val))
			{
				return "";
			}

			return val;
		}

		/*
			The service end notification of mandatory destruction of an entity
		*/
		public void Client_onEntityDestroyed(Int32 eid)
		{
			Dbg.DEBUG_MSG("Ouroboros::Client_onEntityDestroyed: entity(" + eid + ")");

			Entity entity = null;

			if(!entities.TryGetValue(eid, out entity))
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onEntityDestroyed: entity(" + eid + ") not found!");
				return;
			}

			if(entity.inWorld)
			{
				if(entity_id == eid)
					clearSpace(false);

				entity.leaveWorld();
			}

			if(_controlledEntities.Remove(entity))
				Event.fireOut("onLoseControlledEntity", new object[]{entity});

			entities.Remove(eid);
			entity.onDestroy();
		}

		/*
			Server Update the player's base position, the client in this basis of the position plus the cheap value to calculate the player around the entity coordinates
		*/
		public void Client_onUpdateBasePos(float x, float y, float z)
		{
			_entityServerPos.x = x;
			_entityServerPos.y = y;
			_entityServerPos.z = z;

			var entity = player();
			if (entity != null && entity.isControlled)
			{
				entity.position.Set(_entityServerPos.x, _entityServerPos.y, _entityServerPos.z);
				Event.fireOut("updatePosition", new object[]{entity});
				entity.onUpdateVolatileData();
			}
		}

		public void Client_onUpdateBasePosXZ(float x, float z)
		{
			_entityServerPos.x = x;
			_entityServerPos.z = z;

			var entity = player();
			if (entity != null && entity.isControlled)
			{
				entity.position.x = _entityServerPos.x;
				entity.position.z = _entityServerPos.z;
				Event.fireOut("updatePosition", new object[]{entity});
				entity.onUpdateVolatileData();
			}
		}

		public void Client_onUpdateBaseDir(MemoryStream stream)
		{
			float yaw, pitch, roll;
			yaw = stream.readFloat() * 360 / ((float)System.Math.PI * 2);
			pitch = stream.readFloat() * 360 / ((float)System.Math.PI * 2);
			roll = stream.readFloat() * 360 / ((float)System.Math.PI * 2);

			var entity = player();
			if (entity != null && entity.isControlled)
			{
				entity.direction.Set(roll, pitch, yaw);
				Event.fireOut("set_direction", new object[]{entity});
				entity.onUpdateVolatileData();
			}
		}

		public void Client_onUpdateData(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);
			Entity entity = null;

			if(!entities.TryGetValue(eid, out entity))
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onUpdateData: entity(" + eid + ") not found!");
				return;
			}
		}

		/*
			Service end force setting the player's coordinates
			For example: in the service end of the using avatar. position=(0,0,0),
      or the player position and speed when the abnormality will be forced to pull back to a position
		*/
		public void Client_onSetEntityPosAndDir(MemoryStream stream)
		{
			Int32 eid = stream.readInt32();
			Entity entity = null;

			if(!entities.TryGetValue(eid, out entity))
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onSetEntityPosAndDir: entity(" + eid + ") not found!");
				return;
			}

			Vector3 old_position = new Vector3(entity.position.x, entity.position.y, entity.position.z);
			Vector3 old_direction = new Vector3(entity.direction.x, entity.direction.y, entity.direction.z);

			entity.position.x = stream.readFloat();
			entity.position.y = stream.readFloat();
			entity.position.z = stream.readFloat();

			entity.direction.x = stream.readFloat();
			entity.direction.y = stream.readFloat();
			entity.direction.z = stream.readFloat();

			entity._entityLastLocalPos = entity.position;
			entity._entityLastLocalDir = entity.direction;

			entity.onDirectionChanged(old_direction);
			entity.onPositionChanged(old_position);
		}

		public void Client_onUpdateData_ypr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte y = stream.readInt8();
			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, p, r, -1);
		}

		public void Client_onUpdateData_yp(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte y = stream.readInt8();
			SByte p = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, p, OUROMath.OURO_FLT_MAX, -1);
		}

		public void Client_onUpdateData_yr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte y = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, OUROMath.OURO_FLT_MAX, r, -1);
		}

		public void Client_onUpdateData_pr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, p, r, -1);
		}

		public void Client_onUpdateData_y(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte y = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, -1);
		}

		public void Client_onUpdateData_p(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte p = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, p, OUROMath.OURO_FLT_MAX, -1);
		}

		public void Client_onUpdateData_r(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte r = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, r, -1);
		}

		public void Client_onUpdateData_xz(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 1);
		}

		public void Client_onUpdateData_xz_ypr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte y = stream.readInt8();
			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], y, p, r, 1);
		}

		public void Client_onUpdateData_xz_yp(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte y = stream.readInt8();
			SByte p = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], y, p, OUROMath.OURO_FLT_MAX, 1);
		}

		public void Client_onUpdateData_xz_yr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte y = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], y, OUROMath.OURO_FLT_MAX, r, 1);
		}

		public void Client_onUpdateData_xz_pr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], OUROMath.OURO_FLT_MAX, p, r, 1);
		}

		public void Client_onUpdateData_xz_y(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);
			Vector2 xz = stream.readPackXZ();
			SByte yaw = stream.readInt8();
			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], yaw, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 1);
		}

		public void Client_onUpdateData_xz_p(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte p = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], OUROMath.OURO_FLT_MAX, p, OUROMath.OURO_FLT_MAX, 1);
		}

		public void Client_onUpdateData_xz_r(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, r, 1);
		}

		public void Client_onUpdateData_xyz(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			_updateVolatileData(eid, xz[0], y, xz[1], OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 0);
		}

		public void Client_onUpdateData_xyz_ypr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte yaw = stream.readInt8();
			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], yaw, p, r, 0);
		}

		public void Client_onUpdateData_xyz_yp(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte yaw = stream.readInt8();
			SByte p = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], yaw, p, OUROMath.OURO_FLT_MAX, 0);
		}

		public void Client_onUpdateData_xyz_yr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte yaw = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], yaw, OUROMath.OURO_FLT_MAX, r, 0);
		}

		public void Client_onUpdateData_xyz_pr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], OUROMath.OURO_FLT_MAX, p, r, 0);
		}

		public void Client_onUpdateData_xyz_y(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte yaw = stream.readInt8();
			_updateVolatileData(eid, xz[0], y, xz[1], yaw, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 0);
		}

		public void Client_onUpdateData_xyz_p(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte p = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], OUROMath.OURO_FLT_MAX, p, OUROMath.OURO_FLT_MAX, 0);
		}

		public void Client_onUpdateData_xyz_r(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, r, 0);
		}

		private void _updateVolatileData(Int32 entityID, float x, float y, float z, float yaw, float pitch, float roll, sbyte isOnGround)
		{
			Entity entity = null;

			if(!entities.TryGetValue(entityID, out entity))
			{
				// If it is 0 and on the client the next step is to re-login or reconnect operation and service end entity in the disconnection period has been in the online state
        // You can ignore this error, because the cellapp may have been in the baseapp sends a synchronization message, when the client re-connect when not, etc.
        // Server initialization step begins with the receipt of the synchronization information, in this case where an error occurs.
				Dbg.ERROR_MSG("Ouroboros::_updateVolatileData: entity(" + entityID + ") not found!");
				return;
			}

			// Less than 0 is not set
			if(isOnGround >= 0)
			{
				entity.isOnGround = (isOnGround > 0);
			}

			bool changeDirection = false;

			if(roll != OUROMath.OURO_FLT_MAX)
			{
				changeDirection = true;
				entity.direction.x = OUROMath.int82angle((SByte)roll, false) * 360 / ((float)System.Math.PI * 2);
			}

			if(pitch != OUROMath.OURO_FLT_MAX)
			{
				changeDirection = true;
				entity.direction.y = OUROMath.int82angle((SByte)pitch, false) * 360 / ((float)System.Math.PI * 2);
			}

			if(yaw != OUROMath.OURO_FLT_MAX)
			{
				changeDirection = true;
				entity.direction.z = OUROMath.int82angle((SByte)yaw, false) * 360 / ((float)System.Math.PI * 2);
			}

			bool done = false;
			if(changeDirection == true)
			{
				Event.fireOut("set_direction", new object[]{entity});
				done = true;
			}

	            bool positionChanged = x != OUROMath.OURO_FLT_MAX || y != OUROMath.OURO_FLT_MAX || z != OUROMath.OURO_FLT_MAX;
	            if (x == OUROMath.OURO_FLT_MAX) x = 0.0f;
	            if (y == OUROMath.OURO_FLT_MAX) y = 0.0f;
	            if (z == OUROMath.OURO_FLT_MAX) z = 0.0f;

			if(positionChanged)
			{
				Vector3 pos = new Vector3(x + _entityServerPos.x, y + _entityServerPos.y, z + _entityServerPos.z);

				entity.position = pos;
				done = true;
				Event.fireOut("updatePosition", new object[]{entity});
			}

			if(done)
				entity.onUpdateVolatileData();
		}

		/*
			The service end notification streaming data download start
			Please refer to the API Manual on onStreamDataStarted
		*/
		public void Client_onStreamDataStarted(Int16 id, UInt32 datasize, string descr)
		{
		}

		public void Client_onStreamDataRecv(MemoryStream stream)
		{
			// Int16 resID = stream.readInt16();
			// byte[] datas = stream.readBlob();
		}

		public void Client_onStreamDataCompleted(Int16 id)
		{
		}
	}


	public class OuroborosAppThread : OuroborosApp
	{
		/*
			Ouroboros processing thread
		*/
	    public class OUROThread
	    {

	        OuroborosApp app_;
			public bool over = false;

	        public OUROThread(OuroborosApp app)
	        {
	            this.app_ = app;
	        }

	        public void run()
	        {
				Dbg.INFO_MSG("OUROThread::run()");
				over = false;

	            try
	            {
	                this.app_.process();
	            }
	            catch (Exception e)
	            {
	                Dbg.ERROR_MSG(e.ToString());
	            }

				over = true;
				Dbg.INFO_MSG("OUROThread::end()");
	        }
	    }

		private Thread _t = null;
		public OUROThread ourothread = null;

		// Main loop frequency
		public static int threadUpdateHZ = 10;

		// The main cycle of the ms is optimized to remove the cycles to do the division
		private static float threadUpdatePeriod = 1000f / threadUpdateHZ;

		// Plug-in whether to exit the
		private bool _isbreak = false;

		private System.DateTime _lasttime = System.DateTime.Now;

		public OuroborosAppThread(OuroborosArgs args) :
			base(args)
		{
		}

		public override bool initialize(OuroborosArgs args)
		{
			base.initialize(args);

			OuroborosAppThread.threadUpdateHZ = args.threadUpdateHZ;
			threadUpdatePeriod = 1000f / threadUpdateHZ;

			ourothread = new OUROThread(this);
			_t = new Thread(new ThreadStart(ourothread.run));
			_t.Start();

			return true;
		}

		public override void reset()
		{
			_isbreak = false;
			_lasttime = System.DateTime.Now;

			base.reset();
		}

		/*
			Plug-in exit processing
		*/
		public void breakProcess()
		{
			_isbreak = true;
		}

		public bool isbreak()
		{
			return _isbreak;
		}

		public override void process()
		{
			while(!isbreak())
			{
				base.process();
				_thread_wait();
			}

			Dbg.WARNING_MSG("OuroborosAppThread::process(): break!");
		}

		/*
			Prevent full CPU, Need to make a thread wait for a while
		*/
		void _thread_wait()
		{
			TimeSpan span = DateTime.Now - _lasttime;

			int diff = (int)(threadUpdatePeriod - span.Milliseconds);

			if(diff < 0)
				diff = 0;

			System.Threading.Thread.Sleep(diff);
			_lasttime = DateTime.Now;
		}

		public override void destroy()
		{
			Dbg.WARNING_MSG("OuroborosAppThread::destroy()");
			breakProcess();

			int i = 0;
			while(!ourothread.over && i < 50)
			{
				Thread.Sleep(100);
				i += 1;
			}

			if(_t != null)
				_t.Abort();

			_t = null;

			base.destroy();
		}
	}
}

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
		This is the core module of the Ouroboros plugin.
		This includes network creation, persistence protocols, management of entities, and the creation of externally callable interfaces.
		
		Some places to refer to:
		http://www.ouroboros.org/docs/programming/clientsdkprogramming.html
		http://www.ouroboros.org/docs/programming/ouro_message_format.html
		
		http://www.ouroboros.org/cn/docs/programming/clientsdkprogramming.html
		http://www.ouroboros.org/cn/docs/programming/ouro_message_format.html
	*/
	public class OuroborosApp
	{
		public static OuroborosApp app = null;
		private NetworkInterfaceBase _networkInterface = null;
		
		OuroborosArgs _args = null;
		
		// client category
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

		// Encrypted communication type
		public enum NETWORK_ENCRYPT_TYPE
		{
			//No encryption
			ENCRYPT_TYPE_NONE = 0,

			//Blowfish
			ENCRYPT_TYPE_BLOWFISH = 1,
		};

		public string username = "ouroboros";
		public string password = "123456";
		
		// baseapp address assigned by the server
		public string baseappIP = "";
		public UInt16 baseappTcpPort = 0;
		public UInt16 baseappUdpPort = 0;

		// current state
		public string currserver = "";
		public string currstate = "";
		
		// Binary information of the account binding when the server is running down and the client is used for login.
		// This information is extended by the user himself
		private byte[] _serverdatas = new byte[0];
		private byte[] _clientdatas = new byte[0];
		
		// communication protocol encryption, blowfish protocol
		private byte[] _encryptedKey = new byte[0];
		
		// The version number of the server and client and the protocol MD5
		public string serverVersion = "";
		public string clientVersion = "@{OURO_VERSION}";
		public string serverScriptVersion = "";
		public string clientScriptVersion = "@{OURO_SCRIPT_VERSION}";
		public string serverProtocolMD5 = "@{OURO_SERVER_PROTO_MD5}";
		public string serverEntitydefMD5 = "@{OURO_SERVER_ENTITYDEF_MD5}";
		
		// current player's entity id and entity category
		public UInt64 entity_uuid = 0;
		public Int32 entity_id = 0;
		public string entity_type = "";

		private List<Entity> _controlledEntities = new List<Entity>();
		
		// The player's location that was last synced by the server
		private Vector3 _entityServerPos = new Vector3(0f, 0f, 0f);
		
		// Space data, see the API manual on spaceData
		// https://github.com/ouroboros/ouroboros/tree/master/docs/api
		private Dictionary<string, string> _spacedatas = new Dictionary<string, string>();
		
		// All entities are saved here, please refer to the API manual for the entities section.
		// https://github.com/ouroboros/ouroboros/tree/master/docs/api
		public Dictionary<Int32, Entity> entities = new Dictionary<Int32, Entity>();
		
		// When the player's View range is less than 256 entities, we can find the entity through a one-byte index.
		private List<Int32> _entityIDAliasIDList = new List<Int32>();
		private Dictionary<Int32, MemoryStream> _bufferedCreateEntityMessages = new Dictionary<Int32, MemoryStream>(); 
		
		// error description corresponding to all server error codes
		private ServerErrorDescrs _serverErrs = new ServerErrorDescrs(); 
		
		private System.DateTime _lastTickTime = System.DateTime.Now;
		private System.DateTime _lastTickCBTime = System.DateTime.Now;
		private System.DateTime _lastUpdateToServerTime = System.DateTime.Now;
		
		// Upload player information to the server interval, in milliseconds
		private float _updatePlayerToServerPeroid = 100.0f;
		private const int _1MS_TO_100NS = 10000;

		//Encryption filter
		private EncryptionFilter _filter = null;

		// the id of the space the player is currently in, and the resources corresponding to the space
		public UInt32 spaceID = 0;
		public string spaceResPath = "";
		public bool isLoadedGeometry = false;
		
		// According to the standard, each client part should contain this attribute
		public const string component = "client"; 
		
		public OuroborosApp(OuroborosArgs args)
		{
			if (app != null)
				throw new Exception("Only one instance of OuroborosApp!");
			
			app = this;
			Event.outEventsImmediately = !args.isMultiThreads;

			initialize(args);
		}

		public static OuroborosApp getSingleton() 
		{
			if(OuroborosApp.app == null)
			{
				throw new Exception("Please create OuroborosApp!");
			}

			return OuroborosApp.app;
		}

		public virtual bool initialize(OuroborosArgs args)
		{
			_args = args;
			_updatePlayerToServerPeroid = (float)_args.syncPlayerMS;

			EntityDef.init();

			initNetwork();

						// Registration issue
			installEvents();

			return true;
		}
		
		void initNetwork()
		{
			_filter = null;
			Messages.init();
			_networkInterface = new NetworkInterfaceTCP();
		}
		
		void installEvents()
		{
			Event.registerIn(EventInTypes.createAccount, this, "createAccount");
			Event.registerIn(EventInTypes.login, this, "login");
			Event.registerIn(EventInTypes.logout, this, "logout");
			Event.registerIn(EventInTypes.reloginBaseapp, this, "reloginBaseapp");
			Event.registerIn(EventInTypes.resetPassword, this, "resetPassword");
			Event.registerIn(EventInTypes.bindAccountEmail, this, "bindAccountEmail");
			Event.registerIn(EventInTypes.newPassword, this, "newPassword");
			
			// internal events
			Event.registerIn("_closeNetwork", this, "_closeNetwork");
		}

		public OuroborosArgs getInitArgs()
		{
			return _args;
		}
		
		public virtual void destroy()
		{
			Dbg.WARNING_MSG("Ouroboros::destroy()");
			
			if(currserver == "baseapp")
				logout();
			
			reset();
			Ouroboros.Event.deregisterIn(this);
			resetMessages();
			
			OuroborosApp.app = null;
		}
		
		public NetworkInterfaceBase networkInterface()
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

			_filter = null;
			_networkInterface = new NetworkInterfaceTCP();
			
			_spacedatas.Clear();
		}
		
		public static bool validEmail(string strEmail) 
		{ 
			return Regex.IsMatch(strEmail, @"^([\w-\.]+)@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.)
				|(([\w-]+\.)+))([a-zA-Z]{2,4}|[0-9]{1,3})(\]?)$"); 
		}  
		
		/*
			The main loop processing function of the plugin
		*/
		public virtual void process()
		{
			// Processing network
			if (_networkInterface != null)
				_networkInterface.process();
			
			// handle the event thrown by the outer layer
			Event.processInEvents();
			
			// Send heartbeat to the server and synchronize role information to the server
			sendTick();
		}
		
		/*
			Current player entity
		*/
		public Entity player()
		{
			Entity e;
			if(entities.TryGetValue(entity_id, out e))
				return e;
			
			return null;
		}

		public void _closeNetwork(NetworkInterfaceBase networkInterface)
		{
			networkInterface.close();
		}
		
		/*
			Send heartbeat to the server and synchronize role information to the server
		*/
		public void sendTick()
		{
			if(_networkInterface == null || _networkInterface.connected == false)
				return;

			TimeSpan span = DateTime.Now - _lastTickTime;
			
			// Update the player's position and orientation to the server
			updatePlayerToServer();
			
			if(_args.serverHeartbeatTick > 0 && span.Seconds > _args.serverHeartbeatTick)
			{
				span = _lastTickCBTime - _lastTickTime;
				
				// If the heartbeat callback reception time is less than the heartbeat send time, the callback is not received.
				// At this point, the client should be notified that it is offline.
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
			Handshake with the server, you should handshake the first time after connecting to any process
		*/
		public void hello()
		{
			Bundle bundle = Bundle.createObject();
			if(currserver == "loginapp")
				bundle.newMessage(Messages.messages["Loginapp_hello"]);
			else
				bundle.newMessage(Messages.messages["Baseapp_hello"]);

			_filter = null;

			if (_args.networkEncryptType == NETWORK_ENCRYPT_TYPE.ENCRYPT_TYPE_BLOWFISH)
			{
				_filter = new BlowfishFilter();
				_encryptedKey = ((BlowfishFilter)_filter).key();
				_networkInterface.setFilter(null);
			}

			bundle.writeString(clientVersion);
			bundle.writeString(clientScriptVersion);
			bundle.writeBlob(_encryptedKey);
			bundle.send(_networkInterface);
		}

		/*
			Callback of the server after the handshake
		*/
		public void Client_onHelloCB(MemoryStream stream)
		{
			string str_serverVersion = stream.readString();
			serverScriptVersion = stream.readString();
			string currentServerProtocolMD5 = stream.readString();
			string currentServerEntitydefMD5 = stream.readString();
			Int32 ctype = stream.readInt32();
			
			Dbg.DEBUG_MSG("Ouroboros::Client_onHelloCB: verInfo(" + str_serverVersion 
				+ "), scriptVersion("+ serverScriptVersion + "), srvProtocolMD5("+ serverProtocolMD5 
				+ "), srvEntitydefMD5("+ serverEntitydefMD5 + "), + ctype(" + ctype + ")!");
			
			if(str_serverVersion != "Getting")
			{
				serverVersion = str_serverVersion;

				/* 
				if(serverProtocolMD5 != currentServerProtocolMD5)
				{
					Dbg.ERROR_MSG("Client_onHelloCB: digest not match! serverProtocolMD5=" + serverProtocolMD5 + "(server: " + currentServerProtocolMD5 + ")");
					Event.fireAll(EventOutTypes.onVersionNotMatch, clientVersion, serverVersion);
					return;
				}
				*/
				
				if (serverEntitydefMD5 != currentServerEntitydefMD5)
				{
					Dbg.ERROR_MSG("Client_onHelloCB: digest not match! serverEntitydefMD5=" + serverEntitydefMD5 + "(server: " + currentServerEntitydefMD5 + ")");
					Event.fireAll(EventOutTypes.onVersionNotMatch, clientVersion, serverVersion);
					return;
				}
			}

			if (_args.networkEncryptType == NETWORK_ENCRYPT_TYPE.ENCRYPT_TYPE_BLOWFISH)
			{
				_networkInterface.setFilter(_filter);
				_filter = null;
			}

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
			Server error description imported
		*/
		public void Client_onImportServerErrorsDescr(MemoryStream stream)
		{
			// No need to implement, static code has been generated by the plugin
		}

		/*
			Import the binary message from the server to the client message protocol
		*/
		public void Client_onImportClientMessages(MemoryStream stream)
		{
			// No need to implement, static code has been generated by the plugin
		}

		/*
			Import the binary message from the server to the client message protocol
		*/
		public void Client_onImportClientEntityDef(MemoryStream stream)
		{
			// No need to implement, static code has been generated by the plugin
		}

		public void Client_onImportClientSDK(MemoryStream stream)
		{
			int remainingFiles = 0;
			remainingFiles = stream.readInt32();

			string fileName;
			fileName = stream.readString();

			int fileSize = 0;
			fileSize = stream.readInt32();

			byte[] fileDatas = new byte[0];
			fileDatas = stream.readBlob();

			Event.fireIn("onImportClientSDK", remainingFiles, fileName, fileSize, fileDatas);
		}
		
		/*
			Engine version does not match
		*/
		public void Client_onVersionNotMatch(MemoryStream stream)
		{
			serverVersion = stream.readString();
			
			Dbg.ERROR_MSG("Client_onVersionNotMatch: verInfo=" + clientVersion + "(server: " + serverVersion + ")");
			Event.fireAll(EventOutTypes.onVersionNotMatch, clientVersion, serverVersion);
		}

		/*
			Script version does not match
		*/
		public void Client_onScriptVersionNotMatch(MemoryStream stream)
		{
			serverScriptVersion = stream.readString();
			
			Dbg.ERROR_MSG("Client_onScriptVersionNotMatch: verInfo=" + clientScriptVersion + "(server: " + serverScriptVersion + ")");
			Event.fireAll(EventOutTypes.onScriptVersionNotMatch, clientScriptVersion, serverScriptVersion);
		}
		
		/*
			Kicked out by the server
		*/
		public void Client_onKicked(UInt16 failedcode)
		{
			Dbg.DEBUG_MSG("Client_onKicked: failedcode=" + failedcode + "(" + serverErr(failedcode) + ")");
			Event.fireAll(EventOutTypes.onKicked, failedcode);
		}
		
		/*
			Log in to the server, you must log in to complete the loginapp and gateway (baseapp), the login process is completed.
		*/
		public void login(string username, string password, byte[] datas)
		{
			OuroborosApp.app.username = username;
			OuroborosApp.app.password = password;
			OuroborosApp.app._clientdatas = datas;
			
			OuroborosApp.app.login_loginapp(true);
		}
		
		/*
			Log in to the server (loginapp). After logging in successfully, you must log in to the gateway (baseapp) login process to complete.
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
				Dbg.ERROR_MSG(string.Format("Ouroboros::login_loginapp(): connect {0}:{1} error!", ip, port));  
				return;
			}
			
			currserver = "loginapp";
			currstate = "login";
			
			Dbg.DEBUG_MSG(string.Format("Ouroboros::login_loginapp(): connect {0}:{1} success!", ip, port));

			hello();
		}
		
		private void onLogin_loginapp()
		{
			_lastTickCBTime = System.DateTime.Now;
			login_loginapp(false);
		}
		
		/*
			Log in to the server and log in to the gateway (baseapp)
		*/
		public void login_baseapp(bool noconnect)
		{  
			if(noconnect)
			{
				Event.fireOut(EventOutTypes.onLoginBaseapp);
				
				_networkInterface.reset();

				if(_args.forceDisableUDP || baseappUdpPort == 0)
				{
					_networkInterface = new NetworkInterfaceTCP();
					_networkInterface.connectTo(baseappIP, baseappTcpPort, onConnectTo_baseapp_callback, null);
				}
				else
				{
					_networkInterface = new NetworkInterfaceKCP();
					_networkInterface.connectTo(baseappIP, baseappUdpPort, onConnectTo_baseapp_callback, null);
				}
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
				Dbg.ERROR_MSG(string.Format("Ouroboros::login_baseapp(): connect {0}:{1} error!", ip, port));
				return;
			}
			
			currserver = "baseapp";
			currstate = "";
			
			Dbg.DEBUG_MSG(string.Format("Ouroboros::login_baseapp(): connect {0}:{1} success!", ip, port));

			hello();
		}
		
		private void onLogin_baseapp()
		{
			_lastTickCBTime = System.DateTime.Now;
			login_baseapp(false);
		}
		
		/*
			Re-login to the gateway (baseapp)
			Some mobile applications are easy to drop, and you can use this feature to quickly re-establish communication with the server.
		*/
		public void reloginBaseapp()
		{
			_lastTickTime = System.DateTime.Now;
			_lastTickCBTime = System.DateTime.Now;

			if(_networkInterface.valid())
				return;

			Event.fireAll(EventOutTypes.onReloginBaseapp);

			_networkInterface.reset();

			if(_args.forceDisableUDP || baseappUdpPort == 0)
			{
				_networkInterface = new NetworkInterfaceTCP();
				_networkInterface.connectTo(baseappIP, baseappTcpPort, onReConnectTo_baseapp_callback, null);
			}
			else
			{
				_networkInterface = new NetworkInterfaceKCP();
				_networkInterface.connectTo(baseappIP, baseappUdpPort, onReConnectTo_baseapp_callback, null);
			}
		}

		private void onReConnectTo_baseapp_callback(string ip, int port, bool success, object userData)
		{
			if(!success)
			{
				Dbg.ERROR_MSG(string.Format("Ouroboros::reloginBaseapp(): connect {0}:{1} error!", ip, port));
				return;
			}
			
			Dbg.DEBUG_MSG(string.Format("Ouroboros::relogin_baseapp(): connect {0}:{1} success!", ip, port));

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
			Log out of baseapp
		*/
		public void logout()
		{
			Bundle bundle = Bundle.createObject();
			bundle.newMessage(Messages.messages["Baseapp_logoutBaseapp"]);
			bundle.writeUint64(entity_uuid);
			bundle.writeInt32(entity_id);
			bundle.send(_networkInterface);
		}

		/*
			Get an error description by error id
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
			Reset password via loginapp
		*/
		public void resetPassword(string username)
		{
			OuroborosApp.app.username = username;
			resetpassword_loginapp(true);
		}
		
		/*
			Reset password via loginapp
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
				Dbg.ERROR_MSG(string.Format("Ouroboros::resetpassword_loginapp(): connect {0}:{1} error!", ip, port));
				return;
			}
			
			Dbg.DEBUG_MSG(string.Format("Ouroboros::resetpassword_loginapp(): connect {0}:{1} success!", ip, port)); 
			onOpenLoginapp_resetpassword();
		}
		
		public void Client_onReqAccountResetPasswordCB(UInt16 failcode)
		{
			if(failcode != 0)
			{
				Dbg.ERROR_MSG("Ouroboros::Client_onReqAccountResetPasswordCB: " + username + " failed! code=" + failcode + "(" + serverErr(failcode) + ")!");
				return;
			}
	
			Dbg.DEBUG_MSG("Ouroboros::Client_onReqAccountResetPasswordCB: " + username + " success!");
		}
		
		/*
			Bind email via baseapp
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
				Dbg.ERROR_MSG("Ouroboros::Client_onReqAccountBindEmailCB: " + username + " failed! code=" + failcode + "(" + serverErr(failcode) + ")!");
				return;
			}

			Dbg.DEBUG_MSG("Ouroboros::Client_onReqAccountBindEmailCB: " + username + " success!");
		}
		
		/*
			Set a new password, through baseapp, the player must log in to the online operation so it is baseapp.
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
				Dbg.ERROR_MSG("Ouroboros::Client_onReqAccountNewPasswordCB: " + username + " failed! code=" + failcode + "(" + serverErr(failcode) + ")!");
				return;
			}
	
			Dbg.DEBUG_MSG("Ouroboros::Client_onReqAccountNewPasswordCB: " + username + " success!");
		}

		public void createAccount(string username, string password, byte[] datas)
		{
			OuroborosApp.app.username = username;
			OuroborosApp.app.password = password;
			OuroborosApp.app._clientdatas = datas;
			
			OuroborosApp.app.createAccount_loginapp(true);
		}

		/*
			Create an account via loginapp
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
				Dbg.ERROR_MSG(string.Format("Ouroboros::createAccount_loginapp(): connect {0}:{1} error!", ip, port));
				return;
			}
			
			Dbg.DEBUG_MSG(string.Format("Ouroboros::createAccount_loginapp(): connect {0}:{1} success!", ip, port)); 
			onOpenLoginapp_createAccount();
		}
		
		/*
			Obtained the server summary information, the summary includes the protocol MD5, entitydefMD5
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
			Dbg.ERROR_MSG("Ouroboros::Client_onLoginFailed: failedcode(" + failedcode + ":" + serverErr(failedcode) + "), datas(" + _serverdatas.Length + ")!");
			Event.fireAll(EventOutTypes.onLoginFailed, failedcode, _serverdatas);
		}
		
		/*
			Login loginapp succeeded
		*/
		public void Client_onLoginSuccessfully(MemoryStream stream)
		{
			var accountName = stream.readString();
			username = accountName;
			baseappIP = stream.readString();
			baseappTcpPort = stream.readUint16();
			baseappUdpPort = stream.readUint16();
			_serverdatas = stream.readBlob();

			Dbg.DEBUG_MSG("Ouroboros::Client_onLoginSuccessfully: accountName(" + accountName + "), addr(" + 
					baseappIP + ":" + baseappTcpPort + "|" + baseappUdpPort + "), datas(" + _serverdatas.Length + ")!");

			login_baseapp(true);
		}
		
		/*
			Login to baseapp failed
		*/
		public void Client_onLoginBaseappFailed(UInt16 failedcode)
		{
			Dbg.ERROR_MSG("Ouroboros::Client_onLoginBaseappFailed: failedcode=" + failedcode + "("+ serverErr(failedcode) + ")!");
			Event.fireAll(EventOutTypes.onLoginBaseappFailed, failedcode);
		}

		/*
			Re-login to baseapp failed
		*/
		public void Client_onReloginBaseappFailed(UInt16 failedcode)
		{
			Dbg.ERROR_MSG("Ouroboros::Client_onReloginBaseappFailed: failedcode=" + failedcode + "(" + serverErr(failedcode) + ")!");
			Event.fireAll(EventOutTypes.onReloginBaseappFailed, failedcode);
		}
		
		/*
			Login to baseapp succeeded
		*/
		public void Client_onReloginBaseappSuccessfully(MemoryStream stream)
		{
			entity_uuid = stream.readUint64();
			Dbg.DEBUG_MSG("Ouroboros::Client_onReloginBaseappSuccessfully: name(" + username + ")!");
			Event.fireAll(EventOutTypes.onReloginBaseappSuccessfully);
		}

		/*
			Server notification creates a role
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
				entity.attachComponents();
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
			Get the ID of the View entity by streaming data
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
				
				// If 0 and the client's previous step is to re-login or reconnect and the server entity is online during the disconnection
				// This error can be ignored, because cellapp may always send synchronization messages to baseapp, not waiting when the client reconnects
				// When the server initialization step starts, it receives the synchronization information, and an error occurs here.
				if(_entityIDAliasIDList.Count <= aliasID)
					return 0;
				
				id = _entityIDAliasIDList[aliasID];
			}
			
			return id;
		}
		
		/*
			The server updates the entity attribute data in an optimized manner.
		*/
		public void Client_onUpdatePropertysOptimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);
			onUpdatePropertys_(eid, stream);
		}
		
		/*
			The server updates the entity attribute data.
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
				Array.Copy(stream.data(), stream1.data(), stream.wpos);
				_bufferedCreateEntityMessages[eid] = stream1;
				return;
			}
			
			entity.onUpdatePropertys(stream);
		}

		/*
			The server uses the optimized method to call the entity method.
		*/
		public void Client_onRemoteMethodCallOptimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);
			onRemoteMethodCall_(eid, stream);
		}
		
		/*
			Server call entity method
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
			The server notifies an entity to enter the world (if the entity is the current player, the player is created in a space for the first time, and if it is another entity, the other entity enters the player's View)
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
				entity.attachComponents();
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
					// For security reasons, clear it here.
					// If you use giveClientTo to switch control on the server
					// The previous entity has entered the world, and the switched entity also enters the world. Here, the information of the previous entity entering the world may remain.
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
			The server uses an optimized way to notify an entity to leave the world (the player leaves the space if the entity is the current player, and the other entity leaves the player's View if it is another entity)
		*/
		public void Client_onEntityLeaveWorldOptimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);
			OuroborosApp.app.Client_onEntityLeaveWorld(eid);
		}

		/*
			The server notifies an entity to leave the world (the player leaves the space if the entity is the current player, and the other entity leaves the player's View if it is another entity)
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
					Event.fireOut(EventOutTypes.onLoseControlledEntity, entity);

				entities.Remove(eid);
				entity.destroy();
				_entityIDAliasIDList.Remove(eid);
			}
		}

		/*
			The server notifies the current player that a new space has been entered.
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
			The server notifies the current player that he has left the space.
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
			Account creation returns results
		*/
		public void Client_onCreateAccountResult(MemoryStream stream)
		{
			UInt16 retcode = stream.readUint16();
			byte[] datas = stream.readBlob();
			
			Event.fireOut(EventOutTypes.onCreateAccountResult, retcode, datas);
			
			if(retcode != 0)
			{
				Dbg.WARNING_MSG("Ouroboros::Client_onCreateAccountResult: " + username + " create is failed! code=" + retcode + "(" + serverErr(retcode)+ ")!");
				return;
			}
	
			Dbg.DEBUG_MSG("Ouroboros::Client_onCreateAccountResult: " + username + " create is successfully!");
		}

		/*
			Tell the client: You are currently responsible for (or cancel) controlling who's displacement synchronization
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
				// If the controlled person is the player himself, it means that the player himself is controlled by others.
				// So the player should not enter this controlled list.
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
				Event.fireOut(EventOutTypes.onControlled, entity, isCont);
			}
			catch (Exception e)
			{
				Dbg.ERROR_MSG(string.Format("Ouroboros::Client_onControlEntity: entity id = '{0}', is controlled = '{1}', error = '{1}'", eid, isCont, e));
			}
		}

		/*
			Update the current player's position and orientation to the server, you can turn this mechanism off by switching _syncPlayerMS
		*/
		public void updatePlayerToServer()
		{
			if(_updatePlayerToServerPeroid <= 0.01f || spaceID == 0)
			{
				return;
			}

			var now = DateTime.Now;
			TimeSpan span = now - _lastUpdateToServerTime;

			if (span.Ticks < _updatePlayerToServerPeroid * _1MS_TO_100NS)
				return;
			
			Entity playerEntity = player();
			if (playerEntity == null || playerEntity.inWorld == false || playerEntity.isControlled)
				return;

			_lastUpdateToServerTime = now - (span - TimeSpan.FromTicks(Convert.ToInt64(_updatePlayerToServerPeroid * _1MS_TO_100NS)));
			
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
				
				// According to the arc angle formula will appear negative
				// unity will automatically convert to 0~360 degrees, here you need to do a restore
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

			// Start syncing the location of all controlled entities
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
				
					// According to the arc angle formula will appear negative
					// unity will automatically convert to 0~360 degrees, here you need to do a restore
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
			The current space adds mapping resources for information such as geometry.
			The client can use this resource information to load the corresponding scenario.
		*/
		public void addSpaceGeometryMapping(UInt32 uspaceID, string respath)
		{
			Dbg.DEBUG_MSG("Ouroboros::addSpaceGeometryMapping: spaceID(" + uspaceID + "), respath(" + respath + ")!");
			
			isLoadedGeometry = true;
			spaceID = uspaceID;
			spaceResPath = respath;
			Event.fireOut(EventOutTypes.addSpaceGeometryMapping, spaceResPath);
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
					
					dic.Value.destroy();
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

					dic.Value.destroy();
				}  
		
				entities.Clear();
			}
		}
		
		/*
			The server initializes the client's spacedata. For spacedata, please refer to the API.
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
			The server sets the client's spacedata. For spacedata, please refer to the API.
		*/
		public void Client_setSpaceData(UInt32 spaceID, string key, string value)
		{
			Dbg.DEBUG_MSG("Ouroboros::Client_setSpaceData: spaceID(" + spaceID + "), key(" + key + "), value(" + value + ")!");
			_spacedatas[key] = value;
			
			if(key == "_mapping")
				addSpaceGeometryMapping(spaceID, value);
			
			Event.fireOut(EventOutTypes.onSetSpaceData, spaceID, key, value);
		}

		/*
			The server deletes the client's spacedata. For spacedata, please refer to the API.
		*/
		public void Client_delSpaceData(UInt32 spaceID, string key)
		{
			Dbg.DEBUG_MSG("Ouroboros::Client_delSpaceData: spaceID(" + spaceID + "), key(" + key + ")");
			_spacedatas.Remove(key);
			Event.fireOut(EventOutTypes.onDelSpaceData, spaceID, key);
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
			The server notifies the mandatory destruction of an entity
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
				Event.fireOut(EventOutTypes.onLoseControlledEntity, entity);

			entities.Remove(eid);
			entity.destroy();
		}
		
		/*
			The server updates the base position of the player, and the client calculates the coordinates of the entity around the player by using the base position plus the cheap value.
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
				Event.fireOut(EventOutTypes.updatePosition, entity);
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
				Event.fireOut(EventOutTypes.updatePosition, entity);
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
				Event.fireOut(EventOutTypes.set_direction, entity);
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
			The server forces the player's coordinates
			For example: use avatar.position=(0,0,0) on the server, or force the player to pull back to a position when the player's position and speed are abnormal.
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

			float y = stream.readFloat();
			float p = stream.readFloat();
			float r = stream.readFloat();
			
			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, p, r, -1, false);
		}
		
		public void Client_onUpdateData_yp(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float y = stream.readFloat();
			float p = stream.readFloat();
			
			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, p, OUROMath.OURO_FLT_MAX, -1, false);
		}
		
		public void Client_onUpdateData_yr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float y = stream.readFloat();
			float r = stream.readFloat();
			
			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, OUROMath.OURO_FLT_MAX, r, -1, false);
		}
		
		public void Client_onUpdateData_pr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float p = stream.readFloat();
			float r = stream.readFloat();
			
			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, p, r, -1, false);
		}
		
		public void Client_onUpdateData_y(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float y = stream.readFloat();
			
			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, -1, false);
		}
		
		public void Client_onUpdateData_p(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);
			
			float p = stream.readFloat();
			
			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, p, OUROMath.OURO_FLT_MAX, -1, false);
		}
		
		public void Client_onUpdateData_r(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float r = stream.readFloat();
			
			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, r, -1, false);
		}
		
		public void Client_onUpdateData_xz(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float z = stream.readFloat();

			_updateVolatileData(eid, x, OUROMath.OURO_FLT_MAX, z, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 1, false);
		}
		
		public void Client_onUpdateData_xz_ypr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float z = stream.readFloat();

			float y = stream.readFloat();
			float p = stream.readFloat();
			float r = stream.readFloat();
			
			_updateVolatileData(eid, x, OUROMath.OURO_FLT_MAX, z, y, p, r, 1, false);
		}
		
		public void Client_onUpdateData_xz_yp(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float z = stream.readFloat();

			float y = stream.readFloat();
			float p = stream.readFloat();
			
			_updateVolatileData(eid, x, OUROMath.OURO_FLT_MAX, z, y, p, OUROMath.OURO_FLT_MAX, 1, false);
		}
		
		public void Client_onUpdateData_xz_yr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float z = stream.readFloat();

			float y = stream.readFloat();
			float r = stream.readFloat();
			
			_updateVolatileData(eid, x, OUROMath.OURO_FLT_MAX, z, y, OUROMath.OURO_FLT_MAX, r, 1, false);
		}
		
		public void Client_onUpdateData_xz_pr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float z = stream.readFloat();

			float p = stream.readFloat();
			float r = stream.readFloat();
			
			_updateVolatileData(eid, x, OUROMath.OURO_FLT_MAX, z, OUROMath.OURO_FLT_MAX, p, r, 1, false);
		}
		
		public void Client_onUpdateData_xz_y(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float z = stream.readFloat();

			float yaw = stream.readFloat();

			_updateVolatileData(eid, x, OUROMath.OURO_FLT_MAX, z, yaw, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 1, false);
		}
		
		public void Client_onUpdateData_xz_p(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float z = stream.readFloat();

			float p = stream.readFloat();
			
			_updateVolatileData(eid, x, OUROMath.OURO_FLT_MAX, z, OUROMath.OURO_FLT_MAX, p, OUROMath.OURO_FLT_MAX, 1, false);
		}
		
		public void Client_onUpdateData_xz_r(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float z = stream.readFloat();

			float r = stream.readFloat();
			
			_updateVolatileData(eid, x, OUROMath.OURO_FLT_MAX, z, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, r, 1, false);
		}
		
		public void Client_onUpdateData_xyz(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float y = stream.readFloat();
			float z = stream.readFloat();

			_updateVolatileData(eid, x, y, z, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 0, false);
		}
		
		public void Client_onUpdateData_xyz_ypr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float y = stream.readFloat();
			float z = stream.readFloat();

			float yaw = stream.readFloat();
			float p = stream.readFloat();
			float r = stream.readFloat();
			
			_updateVolatileData(eid, x, y, z, yaw, p, r, 0, false);
		}
		
		public void Client_onUpdateData_xyz_yp(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float y = stream.readFloat();
			float z = stream.readFloat();

			float yaw = stream.readFloat();
			float p = stream.readFloat();

			_updateVolatileData(eid, x, y, z, yaw, p, OUROMath.OURO_FLT_MAX, 0, false);
		}
		
		public void Client_onUpdateData_xyz_yr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float y = stream.readFloat();
			float z = stream.readFloat();

			float yaw = stream.readFloat();
			float r = stream.readFloat();
			
			_updateVolatileData(eid, x, y, z, yaw, OUROMath.OURO_FLT_MAX, r, 0, false);
		}
		
		public void Client_onUpdateData_xyz_pr(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float y = stream.readFloat();
			float z = stream.readFloat();

			float p = stream.readFloat();
			float r = stream.readFloat();
			
			_updateVolatileData(eid, x, y, z, OUROMath.OURO_FLT_MAX, p, r, 0, false);
		}
		
		public void Client_onUpdateData_xyz_y(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float y = stream.readFloat();
			float z = stream.readFloat();

			float yaw = stream.readFloat();

			_updateVolatileData(eid, x, y, z, yaw, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 0, false);
		}
		
		public void Client_onUpdateData_xyz_p(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			float x = stream.readFloat();
			float y = stream.readFloat();
			float z = stream.readFloat();

			float p = stream.readFloat();
			
			_updateVolatileData(eid, x, y, z, OUROMath.OURO_FLT_MAX, p, OUROMath.OURO_FLT_MAX, 0, false);
		}
		
		public void Client_onUpdateData_xyz_r(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);
			
			float x = stream.readFloat();
			float y = stream.readFloat();
			float z = stream.readFloat();

			float r = stream.readFloat();
			
			_updateVolatileData(eid, x, y, z, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, r, 0, false);
		}

		public void Client_onUpdateData_ypr_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte y = stream.readInt8();
			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, p, r, -1, true);
		}

		public void Client_onUpdateData_yp_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte y = stream.readInt8();
			SByte p = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, p, OUROMath.OURO_FLT_MAX, -1, true);
		}

		public void Client_onUpdateData_yr_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte y = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, OUROMath.OURO_FLT_MAX, r, -1, true);
		}

		public void Client_onUpdateData_pr_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, p, r, -1, true);
		}

		public void Client_onUpdateData_y_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte y = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, y, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, -1, true);
		}

		public void Client_onUpdateData_p_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte p = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, p, OUROMath.OURO_FLT_MAX, -1, true);
		}

		public void Client_onUpdateData_r_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			SByte r = stream.readInt8();

			_updateVolatileData(eid, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, r, -1, true);
		}

		public void Client_onUpdateData_xz_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 1, true);
		}

		public void Client_onUpdateData_xz_ypr_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte y = stream.readInt8();
			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], y, p, r, 1, true);
		}

		public void Client_onUpdateData_xz_yp_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte y = stream.readInt8();
			SByte p = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], y, p, OUROMath.OURO_FLT_MAX, 1, true);
		}

		public void Client_onUpdateData_xz_yr_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte y = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], y, OUROMath.OURO_FLT_MAX, r, 1, true);
		}

		public void Client_onUpdateData_xz_pr_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], OUROMath.OURO_FLT_MAX, p, r, 1, true);
		}

		public void Client_onUpdateData_xz_y_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);
			Vector2 xz = stream.readPackXZ();
			SByte yaw = stream.readInt8();
			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], yaw, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 1, true);
		}

		public void Client_onUpdateData_xz_p_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte p = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], OUROMath.OURO_FLT_MAX, p, OUROMath.OURO_FLT_MAX, 1, true);
		}

		public void Client_onUpdateData_xz_r_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();

			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], OUROMath.OURO_FLT_MAX, xz[1], OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, r, 1, true);
		}

		public void Client_onUpdateData_xyz_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			_updateVolatileData(eid, xz[0], y, xz[1], OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 0, true);
		}

		public void Client_onUpdateData_xyz_ypr_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte yaw = stream.readInt8();
			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], yaw, p, r, 0, true);
		}

		public void Client_onUpdateData_xyz_yp_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte yaw = stream.readInt8();
			SByte p = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], yaw, p, OUROMath.OURO_FLT_MAX, 0, true);
		}

		public void Client_onUpdateData_xyz_yr_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte yaw = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], yaw, OUROMath.OURO_FLT_MAX, r, 0, true);
		}

		public void Client_onUpdateData_xyz_pr_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte p = stream.readInt8();
			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], OUROMath.OURO_FLT_MAX, p, r, 0, true);
		}

		public void Client_onUpdateData_xyz_y_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte yaw = stream.readInt8();
			_updateVolatileData(eid, xz[0], y, xz[1], yaw, OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, 0, true);
		}

		public void Client_onUpdateData_xyz_p_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte p = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], OUROMath.OURO_FLT_MAX, p, OUROMath.OURO_FLT_MAX, 0, true);
		}

		public void Client_onUpdateData_xyz_r_optimized(MemoryStream stream)
		{
			Int32 eid = getViewEntityIDFromStream(stream);

			Vector2 xz = stream.readPackXZ();
			float y = stream.readPackY();

			SByte r = stream.readInt8();

			_updateVolatileData(eid, xz[0], y, xz[1], OUROMath.OURO_FLT_MAX, OUROMath.OURO_FLT_MAX, r, 0, true);
		}

		private void _updateVolatileData(Int32 entityID, float x, float y, float z, float yaw, float pitch, float roll, sbyte isOnGround, bool isOptimized)
		{
			Entity entity = null;

			if(!entities.TryGetValue(entityID, out entity))
			{
				// If 0 and the client's previous step is to re-login or reconnect and the server entity is online during the disconnection
				// This error can be ignored, because cellapp may always send synchronization messages to baseapp, not waiting when the client reconnects
				// When the server initialization step starts, it receives the synchronization information, and an error occurs here.
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
				entity.direction.x = isOptimized ? OUROMath.int82angle((SByte)roll, false) * 360 / ((float)System.Math.PI * 2) : roll;
			}

			if(pitch != OUROMath.OURO_FLT_MAX)
			{
				changeDirection = true;
				entity.direction.y = isOptimized ? OUROMath.int82angle((SByte)pitch, false) * 360 / ((float)System.Math.PI * 2) : pitch;
			}
			
			if(yaw != OUROMath.OURO_FLT_MAX)
			{
				changeDirection = true;
				entity.direction.z = isOptimized ? OUROMath.int82angle((SByte)yaw, false) * 360 / ((float)System.Math.PI * 2) : yaw;
			}
			
			bool done = false;
			if(changeDirection == true)
			{
				Event.fireOut(EventOutTypes.set_direction, entity);
				done = true;
			}
			
			bool positionChanged = x != OUROMath.OURO_FLT_MAX || y != OUROMath.OURO_FLT_MAX || z != OUROMath.OURO_FLT_MAX;
			if (x == OUROMath.OURO_FLT_MAX) x = isOptimized ? 0.0f : entity.position.x;
			if (y == OUROMath.OURO_FLT_MAX) y = isOptimized ? 0.0f : entity.position.y;
			if (z == OUROMath.OURO_FLT_MAX) z = isOptimized ? 0.0f : entity.position.z;
			
			if(positionChanged)
			{
				Vector3 pos = isOptimized ? new Vector3(x + _entityServerPos.x, y + _entityServerPos.y, z + _entityServerPos.z) : new Vector3(x, y, z);
				 
				entity.position = pos;
				done = true;
				Event.fireOut(EventOutTypes.updatePosition, entity);
			}
			
			if(done)
				entity.onUpdateVolatileData();
		}
		
		/*
			Server notification stream data download starts
			Please refer to the API manual for onStreamDataStarted
		*/
		public void Client_onStreamDataStarted(Int16 id, UInt32 datasize, string descr)
		{
			Event.fireOut(EventOutTypes.onStreamDataStarted, id, datasize, descr);
		}
		
		public void Client_onStreamDataRecv(MemoryStream stream)
		{
			Int16 resID = stream.readInt16();
			byte[] datas = stream.readBlob();
			Event.fireOut(EventOutTypes.onStreamDataRecv, resID, datas);
		}
		
		public void Client_onStreamDataCompleted(Int16 id)
		{
			Event.fireOut(EventOutTypes.onStreamDataCompleted, id);
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

		// main cycle frequency
		public static int threadUpdateHZ = 10;

		// main loop cycle ms optimization removes the loop to do the division
		private static float threadUpdatePeriod = 1000f / threadUpdateHZ;
		
		// Does the plugin exit?
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
			Plugin exit processing
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
			To prevent the CPU from being filled, you need to let the thread wait for a while.
		*/
		void _thread_wait()
		{
			TimeSpan span = DateTime.Now - _lasttime; 
			
			int diff = (int)(threadUpdatePeriod - span.TotalMilliseconds);

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

ouroboros_js_plugins
========================



Usage
---------------------

	1: Create Ouroboros
		// Initialization
		var args = new Ouroboros.OuroborosArgs();

		args.ip = "127.0.0.1";
		args.port = 20013;
		Ouroboros.create(args);

	2: Implment the OURO defined entity (including the client part)
		See: ouroboros\ouroboros_demos_assets\scripts\entities.xml, hasClient="true" need to implment
			<Account hasClient="true"></Account>
			<Monster hasClient="true"></Monster>
			<Gate hasClient="true"></Gate>
			<Space/>

			Ouroboros.Account = Ouroboros.AccountBase.extend(
			{
				// entity initialization
				__init__ : function()
				{
					this._super();
				}
			}

		Call entity server method
			entity.baseCall("base_func", 1, "arg2", "argN")
			entity.cellCall("cell_func", 1, "arg2", "argN")

		Reference: https://github.com/ouroboros/ouroboros/issues/532

	3: Monitor OURO-plugins event
		For example:
			var StartSceneLayer = Class.extend({
			{
				installEvents : function()
				{
					Ouroboros.Event.register("onConnectionState", this, "onConnectionState");
				}

				onConnectionState : function(success)
				{
					// OURO-plugins event fired
				}
			}

	4: Fire events to the OURO-plugins
		For example:
			Ouroboros.Event.fire("login", this.usernamebox.getString(), this.passwordbox.getString(), "ouroboros_cocos2d_js_demo");  



OURO-Plugin fire-out events(OURO => Unity):
---------------------

	Entity events:
		onEnterWorld
			Description:
				Entity enter the client-world.

			Event-datas:
				Enity


		onLeaveWorld
			Description:
				Entity leave the client-world.

			Event-datas:
				Enity

		onEnterSpace
			Description:
				Player enter the new space.

			Event-datas:
				Enity

		onLeaveSpace
			Description:
				Player enter the space.

			Event-datas:
				Enity

		onCreateAccountResult
			Description:
				Create account feedback results.

			Event-datas:
				uint16: retcode


				bytes: datas
					If you use third-party account system, the system may fill some of the third-party additional datas.

		onControlled
			Description:
				Triggered when the entity is controlled or out of control.

			Event-datas:
				Enity
				bool: isControlled

		onLoseControlledEntity
			Description:
				Lose controlled entity.

			Event-datas:
				Enity

		set_position
			Description:
				Sets the current position of the entity.

			Event-datas:
				Enity

		set_direction
			Description:
				Sets the current direction of the entity.

			Event-datas:
				Enity

		updatePosition
			Description:
				The entity position is updated, you can smooth the moving entity to new location.

			Event-datas:
				Enity

	Protocol events:
		onVersionNotMatch
			Description:
				Engine version mismatch.

			Event-datas:
				string: clientVersion
				string: serverVersion

		onScriptVersionNotMatch
			Description:
				script version mismatch.

			Event-datas:
				string: clientScriptVersion
				string: serverScriptVersion

		Loginapp_importClientMessages
			Description:
				Importing the message protocol for loginapp and client.

			Event-datas:
				No datas.

		Baseapp_importClientMessages
			Description:
				Importing the message protocol for baseapp and client.

			Event-datas:
				No datas.

		Baseapp_importClientEntityDef
			Description:
				Protocol description for importing entities.

			Event-datas:
				No datas.

	Login and Logout status:
		onLoginBaseapp
			Description:
				Login to baseapp.

			Event-datas:
				No datas.

		onReloginBaseapp
			Description:
				Relogin to baseapp.

			Event-datas:
				No datas.

		onKicked
			Description:
				Kicked of the current server.

			Event-datas:
				uint16: retcode


		onLoginFailed
			Description:
				Login failed.

			Event-datas:
				uint16: retcode


		onLoginBaseappFailed
			Description:
				Login baseapp failed.

			Event-datas:
				uint16: retcode


		onReloginBaseappFailed
			Description:
				Relogin baseapp failed.

			Event-datas:
				uint16: retcode
					

		onReloginBaseappSuccessfully
			Description:
				Relogin baseapp success.

			Event-datas:
				No datas.

	Space events:
		addSpaceGeometryMapping
			Description:
				The current space is specified by the geometry mapping.
				Popular said is to load the specified Map Resources.

			Event-datas:
				string: resPath

		onSetSpaceData
			Description:
				Server spaceData set data.

			Event-datas:
				int32: spaceID
				string: key
				string value

		onDelSpaceData
			Description:
				Server spaceData delete data.

			Event-datas:
				int32: spaceID
				string: key

	Network events:
		onConnectionState
			Description:
				Status of connection server.

			Event-datas:
				bool: success or fail

		onDisconnected
			Description:
				Status of connection server.

			Event-datas:
				No datas.

	Download events:
		onStreamDataStarted
			Description:
				Start downloading data.

			Event-datas:
				uint16: resouce id
				uint32: data size
				string: description

		onStreamDataRecv
			Description:
				Receive data.

			Event-datas:
				uint16: resouce id
				bytes: datas

		onStreamDataCompleted
			Description:
				The downloaded data is completed.

			Event-datas:
				uint16: resouce id



OURO-Plugin fire-in events(Unity => OURO):
---------------------

	createAccount
			Description:
				Create new account.

			Event-datas:
				string: accountName
				string: password
				bytes: datas
					Datas by user defined.
					Data will be recorded into the OURO account database, you can access the datas through the script layer.
					If you use third-party account system, datas will be submitted to the third-party system.


	login
			Description:
				Login to server.

			Event-datas:
				string: accountName
				string: password
				bytes: datas
					Datas by user defined.
					Data will be recorded into the OURO account database, you can access the datas through the script layer.
					If you use third-party account system, datas will be submitted to the third-party system.

	reloginBaseapp
			Description:
				Relogin to baseapp.

			Event-datas:
				No datas.

	resetPassword
			Description:
				Reset password.

			Event-datas:
				string: accountName

	newPassword
			Description:
				Request to set up a new password for the account.
				Note: account must be online

			Event-datas:
				string: old_password
				string: new_password

	bindAccountEmail
			Description:
				Request server binding account Email.
				Note: account must be online

			Event-datas:
				string: emailAddress

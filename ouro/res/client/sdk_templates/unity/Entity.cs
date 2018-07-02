namespace Ouroboros
{
  	using UnityEngine;
	using System;
	using System.Collections;
	using System.Collections.Generic;

	/*
		Ouroboros logic layer entity of the base class
		All extensions of out-of-game entity should be inherited from the module
	*/
    public class Entity
    {
		// The current players of the last synchronized to the service end position and toward the
		// The two properties are to the engine Ouroboros.cs, anywhere else do not modify
		public Vector3 _entityLastLocalPos = new Vector3(0f, 0f, 0f);
		public Vector3 _entityLastLocalDir = new Vector3(0f, 0f, 0f);

    	public Int32 id = 0;
		public string className = "";
		public Vector3 position = new Vector3(0.0f, 0.0f, 0.0f);
		public Vector3 direction = new Vector3(0.0f, 0.0f, 0.0f);
		public float velocity = 0.0f;

		public bool isOnGround = true;

		public object renderObj = null;

		//public EntityCall baseEntityCall = null;
		//public EntityCall cellEntityCall = null;

		// enterworld之后设置为true
		public bool inWorld = false;

		/// <summary>
		/// For the player itself, it indicates whether their Is other player control;
		/// For the other entity, represents my present machine is the control of this entity
		/// </summary>
		public bool isControlled = false;

		// __init__after the call is set to true
		public bool inited = false;

		public static void clear()
		{
		}

		public Entity()
		{
		}

		public virtual void onDestroy ()
		{
		}

		public bool isPlayer()
		{
			return id == OuroborosApp.app.entity_id;
		}

		public virtual void onRemoteMethodCall(MemoryStream stream)
		{
			// Dynamically generated
		}

		public virtual void onUpdatePropertys(MemoryStream stream)
		{
			// Dynamically generated
		}

		public virtual void onGetBase()
		{
			// Dynamically generated
		}

		public virtual void onGetCell()
		{
			// Dynamically generated
		}

		public virtual void onLoseCell()
		{
			// Dynamically generated
		}

		public virtual EntityCall getBaseEntityCall()
		{
			// Dynamically generated
			return null;
		}

		public virtual EntityCall getCellEntityCall()
		{
			// Dynamically generated
			return null;
		}

		/*
			Ouroboros the entity constructor, with the server script.
			The presence of such a constructor is because the OURO need to create entities and attributes and other data filled in order to tell the script layer initialized
		*/
		public virtual void __init__()
		{
		}

		public virtual void callPropertysSetMethods()
		{
			// Dynamically generated
		}

		public void baseCall(string methodname, params object[] arguments)
		{
			if(OuroborosApp.app.currserver == "loginapp")
			{
				Dbg.ERROR_MSG(className + "::baseCall(" + methodname + "), currserver=!" + OuroborosApp.app.currserver);
				return;
			}

			ScriptModule module = null;
			if(!EntityDef.moduledefs.TryGetValue(className, out module))
			{
				Dbg.ERROR_MSG("entity::baseCall:  entity-module(" + className + ") error, can not find from EntityDef.moduledefs");
				return;
			}

			Method method = null;
			if(!module.base_methods.TryGetValue(methodname, out method))
			{
				Dbg.ERROR_MSG(className + "::baseCall(" + methodname + "), not found method!");
				return;
			}

			UInt16 methodID = method.methodUtype;

			if(arguments.Length != method.args.Count)
			{
				Dbg.ERROR_MSG(className + "::baseCall(" + methodname + "): args(" + (arguments.Length) + "!= " + method.args.Count + ") size is error!");
				return;
			}

			EntityCall baseEntityCall = getBaseEntityCall();

			baseEntityCall.newCall();
			baseEntityCall.bundle.writeUint16(0);
			baseEntityCall.bundle.writeUint16(methodID);

			try
			{
				for(var i=0; i<method.args.Count; i++)
				{
					if(method.args[i].isSameType(arguments[i]))
					{
						method.args[i].addToStream(baseEntityCall.bundle, arguments[i]);
					}
					else
					{
						throw new Exception("arg" + i + ": " + method.args[i].ToString());
					}
				}
			}
			catch(Exception e)
			{
				Dbg.ERROR_MSG(className + "::baseCall(method=" + methodname + "): args is error(" + e.Message + ")!");
				baseEntityCall.bundle = null;
				return;
			}

			baseEntityCall.sendCall(null);
		}

		public void cellCall(string methodname, params object[] arguments)
		{
			if(OuroborosApp.app.currserver == "loginapp")
			{
				Dbg.ERROR_MSG(className + "::cellCall(" + methodname + "), currserver=!" + OuroborosApp.app.currserver);
				return;
			}

			ScriptModule module = null;
			if(!EntityDef.moduledefs.TryGetValue(className, out module))
			{
				Dbg.ERROR_MSG("entity::cellCall:  entity-module(" + className + ") error, can not find from EntityDef.moduledefs!");
				return;
			}

			Method method = null;
			if(!module.cell_methods.TryGetValue(methodname, out method))
			{
				Dbg.ERROR_MSG(className + "::cellCall(" + methodname + "), not found method!");
				return;
			}

			UInt16 methodID = method.methodUtype;

			if(arguments.Length != method.args.Count)
			{
				Dbg.ERROR_MSG(className + "::cellCall(" + methodname + "): args(" + (arguments.Length) + "!= " + method.args.Count + ") size is error!");
				return;
			}

			EntityCall cellEntityCall = getCellEntityCall();

			if(cellEntityCall == null)
			{
				Dbg.ERROR_MSG(className + "::cellCall(" + methodname + "): no cell!");
				return;
			}

			cellEntityCall.newCall();
			cellEntityCall.bundle.writeUint16(0);
			cellEntityCall.bundle.writeUint16(methodID);

			try
			{
				for(var i=0; i<method.args.Count; i++)
				{
					if(method.args[i].isSameType(arguments[i]))
					{
						method.args[i].addToStream(cellEntityCall.bundle, arguments[i]);
					}
					else
					{
						throw new Exception("arg" + i + ": " + method.args[i].ToString());
					}
				}
			}
			catch(Exception e)
			{
				Dbg.ERROR_MSG(className + "::cellCall(" + methodname + "): args is error(" + e.Message + ")!");
				cellEntityCall.bundle = null;
				return;
			}

			cellEntityCall.sendCall(null);
		}

		public void enterWorld()
		{
			// Dbg.DEBUG_MSG(className + "::enterWorld(" + getDefinedProperty("uid") + "): " + id);
			inWorld = true;

			try{
				onEnterWorld();
			}
			catch (Exception e)
			{
				Dbg.ERROR_MSG(className + "::onEnterWorld: error=" + e.ToString());
			}

			Event.fireOut("onEnterWorld", new object[]{this});
		}

		public virtual void onEnterWorld()
		{
		}

		public void leaveWorld()
		{
			// Dbg.DEBUG_MSG(className + "::leaveWorld: " + id);
			inWorld = false;

			try{
				onLeaveWorld();
			}
			catch (Exception e)
			{
				Dbg.ERROR_MSG(className + "::onLeaveWorld: error=" + e.ToString());
			}

			Event.fireOut("onLeaveWorld", new object[]{this});
		}

		public virtual void onLeaveWorld()
		{
		}

		public virtual void enterSpace()
		{
			// Dbg.DEBUG_MSG(className + "::enterSpace(" + getDefinedProperty("uid") + "): " + id);
			inWorld = true;

			try{
				onEnterSpace();
			}
			catch (Exception e)
			{
				Dbg.ERROR_MSG(className + "::onEnterSpace: error=" + e.ToString());
			}

			Event.fireOut("onEnterSpace", new object[]{this});

			// You want to immediately refresh the presentation layer the position of the object
			Event.fireOut("set_position", new object[]{this});
			Event.fireOut("set_direction", new object[]{this});
		}

		public virtual void onEnterSpace()
		{
		}

		public virtual void leaveSpace()
		{
			// Dbg.DEBUG_MSG(className + "::leaveSpace: " + id);
			inWorld = false;

			try{
				onLeaveSpace();
			}
			catch (Exception e)
			{
				Dbg.ERROR_MSG(className + "::onLeaveSpace: error=" + e.ToString());
			}

			Event.fireOut("onLeaveSpace", new object[]{this});
		}

		public virtual void onLeaveSpace()
		{
		}

		public virtual void onPositionChanged(Vector3 oldValue)
		{
			//Dbg.DEBUG_MSG(className + "::set_position: " + oldValue + " => " + v);

			if(isPlayer())
				OuroborosApp.app.entityServerPos(position);

			if(inWorld)
				Event.fireOut("set_position", new object[]{this});
		}

		public virtual void onUpdateVolatileData()
		{
		}

		public virtual void onDirectionChanged(Vector3 oldValue)
		{
			//Dbg.DEBUG_MSG(className + "::set_direction: " + oldValue + " => " + v);

			if(inWorld)
			{
				direction.x = direction.x * 360 / ((float)System.Math.PI * 2);
				direction.y = direction.y * 360 / ((float)System.Math.PI * 2);
				direction.z = direction.z * 360 / ((float)System.Math.PI * 2);
				Event.fireOut("set_direction", new object[]{this});
			}
			else
			{
				direction = oldValue;
			}
		}

		/// <summary>
		/// This callback method is called when the local entity control by the client has been enabled or disabled.
		/// See the Entity.controlledBy() method in the CellApp server code for more infomation.
		/// </summary>
		/// <param name="isControlled">
		/// For the player itself, it indicates whether their Is other player control;
		/// For the other entity, represents my present machine is the control of this entity
		/// </param>
		public virtual void onControlled(bool isControlled_)
		{

		}
    }

}

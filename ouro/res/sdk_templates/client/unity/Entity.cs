namespace Ouroboros
{
  	using UnityEngine; 
	using System; 
	using System.Collections; 
	using System.Collections.Generic;
	
	/*
		Entity base class of the Ouroboros logical layer
		All extended game entities should inherit from this module
	*/
    public class Entity 
    {
		// The position and orientation of the current player's last sync to the server
		// These two properties are for the engine Ouroboros.cs, don't modify it elsewhere.
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
		
		// set to true after enterworld
		public bool inWorld = false;

		/// <summary>
		/// For the player itself, it indicates whether it is controlled by other players;
		/// For other entities, it means that my machine controls this entity.
		/// </summary>
		public bool isControlled = false;
		
		// set to true after __init__ call
		public bool inited = false;

		public static void clear()
		{
		}

		public Entity()
		{
		}
		
		public void destroy()
		{
			detachComponents();
			onDestroy();
		}

		public virtual void onDestroy()
		{
		}
		
		public bool isPlayer()
		{
			return id == OuroborosApp.app.entity_id;
		}
		
		public virtual void onRemoteMethodCall(MemoryStream stream)
		{
			// Dynamic generation
		}

		public virtual void onUpdatePropertys(MemoryStream stream)
		{
			// Dynamic generation
		}

		public virtual void onGetBase()
		{
			// Dynamic generation
		}

		public virtual void onGetCell()
		{
			// Dynamic generation
		}

		public virtual void onLoseCell()
		{
			// Dynamic generation
		}

		public virtual void onComponentsEnterworld()
		{
			// dynamically generated, notification component onEnterworld
		}

		public virtual void onComponentsLeaveworld()
		{
			// dynamically generated, notification component onLeaveworld
		}

		public virtual EntityCall getBaseEntityCall()
		{
			// Dynamic generation
			return null;
		}

		public virtual EntityCall getCellEntityCall()
		{
			// Dynamic generation
			return null;
		}

		/*
			Ouroboros's entity constructor, corresponding to the server script.
			Exist in such a constructor because KBE needs to create a good entity and fill in the data such as attributes to tell the script layer to initialize
		*/
		public virtual void __init__()
		{
		}
		
		public virtual void callPropertysSetMethods()
		{
			// Dynamic generation
		}
		
		public virtual void attachComponents()
		{
			// Dynamic generation
		}

		public virtual void detachComponents()
		{
			// Dynamic generation
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
				onComponentsEnterworld();
			}
			catch (Exception e)
			{
				Dbg.ERROR_MSG(className + "::onEnterWorld: error=" + e.ToString());
			}

			Event.fireOut(EventOutTypes.onEnterWorld, this);
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
				onComponentsLeaveworld();
			}
			catch (Exception e)
			{
				Dbg.ERROR_MSG(className + "::onLeaveWorld: error=" + e.ToString());
			}

			Event.fireOut(EventOutTypes.onLeaveWorld, this);
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
			
			Event.fireOut(EventOutTypes.onEnterSpace, this);
			
			// To refresh the position of the presentation layer object immediately
			Event.fireOut(EventOutTypes.set_position, this);
			Event.fireOut(EventOutTypes.set_direction, this);
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
			
			Event.fireOut(EventOutTypes.onLeaveSpace, this);
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
				Event.fireOut(EventOutTypes.set_position, this);
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
				Event.fireOut(EventOutTypes.set_direction, this);
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
		/// For the player itself, it indicates whether it is controlled by other players;
		/// For other entities, it means that my machine controls this entity.
		/// </param>
		public virtual void onControlled(bool isControlled_)
		{
		
		}

		public virtual List<EntityComponent> getComponents(string componentName, bool all)
		{
			List<EntityComponent> founds = new List<EntityComponent>();
			return founds;
		}
    }
    
}

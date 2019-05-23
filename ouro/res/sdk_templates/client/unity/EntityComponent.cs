namespace Ouroboros
{
	using UnityEngine; 
	using System; 
	using System.Collections; 
	using System.Collections.Generic;
	using System.Threading;

	/*
		Entity component module base class
	*/
	public class EntityComponent
	{
		public UInt16 entityComponentPropertyID = 0;
		public UInt16 componentType = 0;
		public Int32 ownerID = 0;
		public Entity owner = null;
		public string name_ = "";

		public virtual void onAttached(Entity ownerEntity)
		{

		}

		public virtual void onDetached(Entity ownerEntity)
		{

		}

		public virtual void onEnterworld()
		{
		}

		public virtual void onLeaveworld()
		{
		}

        public virtual ScriptModule getScriptModule()
        {
			// Dynamic generation
            return null;
        }

		public virtual void onRemoteMethodCall(UInt16 methodUtype, MemoryStream stream)
		{
			// Dynamic generation
		}

		public virtual void onUpdatePropertys(UInt16 propUtype, MemoryStream stream, int maxCount)
		{
			// Dynamic generation
		}

		public virtual void callPropertysSetMethods()
		{
			// Dynamic generation
		}

		public virtual void createFromStream(MemoryStream stream)
		{
			componentType = (UInt16)stream.readInt32();
			ownerID = stream.readInt32();

			//UInt16 ComponentDescrsType;
			stream.readUint16();

			UInt16 count = stream.readUint16();

			if(count > 0)
				onUpdatePropertys(0, stream, count);
		}
	}
}
namespace Ouroboros
{
  	using UnityEngine;
	using System;

	/*
    Abstract an entitydef attributes defined
    This module describes the attributes of the id and the data type and other information
	*/
    public class Property
    {
		public enum EntityDataFlags
		{
			ED_FLAG_UNKOWN													= 0x00000000, // Undefined
			ED_FLAG_CELL_PUBLIC												= 0x00000001, // Related to all cell broadcast
			ED_FLAG_CELL_PRIVATE											= 0x00000002, // The current cell
			ED_FLAG_ALL_CLIENTS												= 0x00000004, // cell broadcast with all the client
			ED_FLAG_CELL_PUBLIC_AND_OWN										= 0x00000008, // cell broadcast with your own client
			ED_FLAG_OWN_CLIENT												= 0x00000010, // The current cell and the client
			ED_FLAG_BASE_AND_CLIENT											= 0x00000020, // base and client
			ED_FLAG_BASE													= 0x00000040, // The current base
			ED_FLAG_OTHER_CLIENTS											= 0x00000080, // cell broadcast and other clients
		};

		public string name = "";
		public UInt16 properUtype = 0;
		public UInt32 properFlags = 0;
		public Int16 aliasID = -1;
		public object defaultVal = null;

		public Property()
		{

		}

		public bool isBase()
		{
			return properFlags == (UInt32)EntityDataFlags.ED_FLAG_BASE_AND_CLIENT ||
				properFlags == (UInt32)EntityDataFlags.ED_FLAG_BASE;
		}

		public bool isOwnerOnly()
		{
			return properFlags == (UInt32)EntityDataFlags.ED_FLAG_CELL_PUBLIC_AND_OWN ||
				properFlags == (UInt32)EntityDataFlags.ED_FLAG_OWN_CLIENT;
		}

		public bool isOtherOnly()
		{
			return properFlags == (UInt32)EntityDataFlags.ED_FLAG_OTHER_CLIENTS ||
				properFlags == (UInt32)EntityDataFlags.ED_FLAG_OTHER_CLIENTS;
		}
    }

}

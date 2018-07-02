﻿namespace Ouroboros
{
  	using UnityEngine;
	using System;
	using System.Collections;
	using System.Collections.Generic;
	using System.Reflection;

	/*
    An entitydef defined in the script module description of the class
    Contains an entity definition of the properties and methods as well as the entity script the name of the module with the module ID
	*/
    public class ScriptModule
    {
		public string name;
		public bool usePropertyDescrAlias;
		public bool useMethodDescrAlias;

		public Dictionary<string, Property> propertys = new Dictionary<string, Property>();
		public Dictionary<UInt16, Property> idpropertys = new Dictionary<UInt16, Property>();

		public Dictionary<string, Method> methods = new Dictionary<string, Method>();
		public Dictionary<string, Method> base_methods = new Dictionary<string, Method>();
		public Dictionary<string, Method> cell_methods = new Dictionary<string, Method>();

		public Dictionary<UInt16, Method> idmethods = new Dictionary<UInt16, Method>();
		public Dictionary<UInt16, Method> idbase_methods = new Dictionary<UInt16, Method>();
		public Dictionary<UInt16, Method> idcell_methods = new Dictionary<UInt16, Method>();

		public Type entityScript = null;

		public ScriptModule(string modulename)
		{
			name = modulename;

			foreach (System.Reflection.Assembly ass in AppDomain.CurrentDomain.GetAssemblies())
			{
				entityScript = ass.GetType("Ouroboros." + modulename);
				if(entityScript == null)
				{
					entityScript = ass.GetType(modulename);
				}

				if(entityScript != null)
					break;
			}

			usePropertyDescrAlias = false;
			useMethodDescrAlias = false;

			if(entityScript == null)
				Dbg.ERROR_MSG("can't load(Ouroboros." + modulename + ")!");
		}
    }

}

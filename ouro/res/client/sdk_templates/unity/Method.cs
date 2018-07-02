namespace Ouroboros
{
  	using UnityEngine;
	using System;
	using System.Collections;
	using System.Collections.Generic;

	/*
    Entity the method defined in module
    Abstract a def file to define the method, the modified module class provides the method of the related description information
    For example: method parameters, method id, the method corresponding to the script handler
	*/
    public class Method
    {
		public string name = "";
    	public UInt16 methodUtype = 0;
    	public Int16 aliasID = -1;
		public List<DATATYPE_BASE> args = null;

		public Method()
		{

		}
    }

}

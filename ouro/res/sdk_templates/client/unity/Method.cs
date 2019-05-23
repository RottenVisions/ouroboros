namespace Ouroboros
{
  	using UnityEngine; 
	using System; 
	using System.Collections; 
	using System.Collections.Generic;
	
	/*
		Entity defined method module
		Abstract a method defined in a def file, and the modified module class provides related description information about the method.
		For example: the parameters of the method, the id of the method, and the handler of the method corresponding to the script
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

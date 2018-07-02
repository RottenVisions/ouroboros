using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;


namespace Ouroboros
{
	/// <summary>
	/// A Simple Object pool
	/// </summary>
	/// <typeparam name="T">Object type</typeparam>
	public class ObjectPool<T> where T : new()
	{
		private static Stack<T> _objects = new Stack<T>();
		private static T v;

		public static T createObject()
		{
			lock (_objects)
			{
				if (_objects.Count > 0)
				{
					v = _objects.Pop();
					return v;
				}
				else
				{
					return new T();
				}
			}
		}

		public static void reclaimObject(T item)
		{
			lock (_objects)
			{
				_objects.Push(item);
			}
		}
	}

}

namespace Ouroboros
{
	using System; 
	using System.Net.Sockets; 
	using System.Net; 
	using System.Collections; 
	using System.Collections.Generic;
	using System.Text;
	using System.Text.RegularExpressions;
	using System.Threading;

	using MessageID = System.UInt16;
	using MessageLength = System.UInt16;
	
	/*
		Packet receiving module (corresponding to the name of the server network part)
		Handling the reception of network data
	*/
	public abstract class PacketReceiverBase
	{
		public delegate void AsyncReceiveMethod(); 
		protected MessageReaderBase _messageReader = null;
		protected NetworkInterfaceBase _networkInterface = null;

		public PacketReceiverBase(NetworkInterfaceBase networkInterface)
		{
			_networkInterface = networkInterface;
		}

		~PacketReceiverBase()
		{
		}

		public NetworkInterfaceBase networkInterface()
		{
			return _networkInterface;
		}

		public abstract void process();

		public virtual void startRecv()
		{
			AsyncReceiveMethod asyncReceiveMethod = new AsyncReceiveMethod(this._asyncReceive);
			asyncReceiveMethod.BeginInvoke(new AsyncCallback(_onRecv), asyncReceiveMethod);
		}

		protected abstract void _asyncReceive();

		private void _onRecv(IAsyncResult ar)
		{
			AsyncReceiveMethod caller = (AsyncReceiveMethod)ar.AsyncState; ;
			caller.EndInvoke(ar);
		}
	}
} 

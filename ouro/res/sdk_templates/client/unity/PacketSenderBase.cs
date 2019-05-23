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
		Packet sending module (corresponding to the name of the server network part)
		Handling the transmission of network data
	*/
    public abstract class PacketSenderBase 
    {
    	public delegate void AsyncSendMethod();

		protected NetworkInterfaceBase _networkInterface = null;
		AsyncCallback _asyncCallback = null;
		AsyncSendMethod _asyncSendMethod;
		
		public PacketSenderBase(NetworkInterfaceBase networkInterface)
		{
			_networkInterface = networkInterface;
			_asyncSendMethod = new AsyncSendMethod(this._asyncSend);
			_asyncCallback = new AsyncCallback(_onSent);
		}

		~PacketSenderBase()
		{
		}

		public NetworkInterfaceBase networkInterface()
		{
			return _networkInterface;
		}

		public abstract bool send(MemoryStream stream);

		protected void _startSend()
		{
			// Since the socket is non-blocking, you cannot use the socket.send() method directly here.
			// must be placed in another thread to do

			_asyncSendMethod.BeginInvoke(_asyncCallback, _asyncSendMethod);
		}

		protected abstract void _asyncSend();
		
		protected static void _onSent(IAsyncResult ar)
		{
			AsyncSendMethod caller = (AsyncSendMethod)ar.AsyncState;
			caller.EndInvoke(ar);
		}
	}
} 

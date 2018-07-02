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
	using System.Runtime.Remoting.Messaging;

	using MessageID = System.UInt16;
	using MessageLength = System.UInt16;

	/*
		The packet receiving module(with the server part of the network name of the corresponding)
		Processing network data received
	*/
	public class PacketReceiver
	{
		public delegate void AsyncReceiveMethod();

		private MessageReader messageReader = null;
		private NetworkInterface _networkInterface = null;

		private byte[] _buffer;

		// socket to the buffer to write the starting position
		int _wpos = 0;

		// The main thread reads the data of the starting position
		int _rpos = 0;

		public PacketReceiver(NetworkInterface networkInterface)
		{
			_init(networkInterface);
		}

		~PacketReceiver()
		{
			Dbg.DEBUG_MSG("PacketReceiver::~PacketReceiver(), destroyed!");
		}

		void _init(NetworkInterface networkInterface)
		{
			_networkInterface = networkInterface;
			_buffer = new byte[OuroborosApp.app.getInitArgs().RECV_BUFFER_MAX];

			messageReader = new MessageReader();
		}

		public NetworkInterface networkInterface()
		{
			return _networkInterface;
		}

		public void process()
		{
			int t_wpos = Interlocked.Add(ref _wpos, 0);

			if (_rpos < t_wpos)
			{
				messageReader.process(_buffer, (UInt32)_rpos, (UInt32)(t_wpos - _rpos));
				Interlocked.Exchange(ref _rpos, t_wpos);
			}
			else if (t_wpos < _rpos)
			{
				messageReader.process(_buffer, (UInt32)_rpos, (UInt32)(_buffer.Length - _rpos));
				messageReader.process(_buffer, (UInt32)0, (UInt32)t_wpos);
				Interlocked.Exchange(ref _rpos, t_wpos);
			}
			else
			{
				// There is no readable data
			}
		}

		int _free()
		{
			int t_rpos = Interlocked.Add(ref _rpos, 0);

			if (_wpos == _buffer.Length)
			{
				if (t_rpos == 0)
				{
					return 0;
				}

				Interlocked.Exchange(ref _wpos, 0);
			}

			if (t_rpos <= _wpos)
			{
				return _buffer.Length - _wpos;
			}

			return t_rpos - _wpos - 1;
		}

		public void startRecv()
		{

			var v = new AsyncReceiveMethod(this._asyncReceive);
			v.BeginInvoke(new AsyncCallback(_onRecv), null);
		}

		private void _asyncReceive()
		{
			if (_networkInterface == null || !_networkInterface.valid())
			{
				Dbg.WARNING_MSG("PacketReceiver::_asyncReceive(): network interface invalid!");
				return;
			}

			var socket = _networkInterface.sock();

			while (true)
			{
				// Must have space to write, otherwise we are blocking the thread until there is room so far
				int first = 0;
				int space = _free();

				while (space == 0)
				{
					if (first > 0)
					{
						if (first > 1000)
						{
							Dbg.ERROR_MSG("PacketReceiver::_asyncReceive(): no space!");
							Event.fireIn("_closeNetwork", new object[] { _networkInterface });
							return;
						}

						Dbg.WARNING_MSG("PacketReceiver::_asyncReceive(): waiting for space, Please adjust 'RECV_BUFFER_MAX'! retries=" + first);
						System.Threading.Thread.Sleep(5);
					}

					first += 1;
					space = _free();
				}

				int bytesRead = 0;
				try
				{
					bytesRead = socket.Receive(_buffer, _wpos, space, 0);
				}
				catch (SocketException se)
				{
					Dbg.ERROR_MSG(string.Format("PacketReceiver::_asyncReceive(): receive error, disconnect from '{0}'! error = '{1}'", socket.RemoteEndPoint, se));
					Event.fireIn("_closeNetwork", new object[] { _networkInterface });
					return;
				}

				if (bytesRead > 0)
				{
					// Update the write position
					Interlocked.Add(ref _wpos, bytesRead);
				}
				else
				{
					Dbg.WARNING_MSG(string.Format("PacketReceiver::_asyncReceive(): receive 0 bytes, disconnect from '{0}'!", socket.RemoteEndPoint));
					Event.fireIn("_closeNetwork", new object[] { _networkInterface });
					return;
				}
			}
		}

		private void _onRecv(IAsyncResult ar)
		{
			AsyncResult result = (AsyncResult)ar;
			AsyncReceiveMethod caller = (AsyncReceiveMethod)result.AsyncDelegate;
			caller.EndInvoke(ar);
		}
	}
}

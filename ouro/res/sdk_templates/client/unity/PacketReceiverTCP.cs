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
	public class PacketReceiverTCP : PacketReceiverBase
	{
		private byte[] _buffer;

		// The starting position of the socket to write to the buffer
		int _wpos = 0;

		// The starting position of the main thread to read data
		int _rpos = 0;

		public PacketReceiverTCP(NetworkInterfaceBase networkInterface) : base(networkInterface) 
		{
			_buffer = new byte[OuroborosApp.app.getInitArgs().TCP_RECV_BUFFER_MAX];
			_messageReader = new MessageReaderTCP();
		}

		~PacketReceiverTCP()
		{
			Dbg.DEBUG_MSG("PacketReceiverTCP::~PacketReceiverTCP(), destroyed!");
		}

		public override void process()
		{
			int t_wpos = Interlocked.Add(ref _wpos, 0);

			if (_rpos < t_wpos)
			{
                if (_networkInterface.fileter() != null)
                {
                    _networkInterface.fileter().recv(_messageReader, _buffer, (UInt32)_rpos, (UInt32)(t_wpos - _rpos));
                }
                else
                {
                    _messageReader.process(_buffer, (UInt32)_rpos, (UInt32)(t_wpos - _rpos));
                }
                    
				Interlocked.Exchange(ref _rpos, t_wpos);
			}
			else if (t_wpos < _rpos)
			{
                if (_networkInterface.fileter() != null)
                {
                    _networkInterface.fileter().recv(_messageReader, _buffer, (UInt32)_rpos, (UInt32)(_buffer.Length - _rpos));
                    _networkInterface.fileter().recv(_messageReader, _buffer, (UInt32)0, (UInt32)t_wpos);
                }
                else
                {
                    _messageReader.process(_buffer, (UInt32)_rpos, (UInt32)(_buffer.Length - _rpos));
                    _messageReader.process(_buffer, (UInt32)0, (UInt32)t_wpos);
                }
                
				Interlocked.Exchange(ref _rpos, t_wpos);
			}
			else
			{
				// no readable data
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

		protected override void _asyncReceive()
		{
			if (_networkInterface == null || !_networkInterface.valid())
			{
				Dbg.WARNING_MSG("PacketReceiverTCP::_asyncReceive(): network interface invalid!");
				return;
			}

			var socket = _networkInterface.sock();

			while (true)
			{
				// There must be space to write, otherwise we block in the thread until there is space
				int first = 0;
				int space = _free();

				while (space == 0)
				{
					if (first > 0)
					{
						if (first > 1000)
						{
							Dbg.ERROR_MSG("PacketReceiverTCP::_asyncReceive(): no space!");
							Event.fireIn("_closeNetwork", new object[] { _networkInterface });
							return;
						}

						Dbg.WARNING_MSG("PacketReceiverTCP::_asyncReceive(): waiting for space, Please adjust 'RECV_BUFFER_MAX'! retries=" + first);
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
					Dbg.ERROR_MSG(string.Format("PacketReceiverTCP::_asyncReceive(): receive error, disconnect from '{0}'! error = '{1}'", socket.RemoteEndPoint, se));
					Event.fireIn("_closeNetwork", new object[] { _networkInterface });
					return;
				}

				if (bytesRead > 0)
				{
					// update the write location
					Interlocked.Add(ref _wpos, bytesRead);
				}
				else
				{
					Dbg.WARNING_MSG(string.Format("PacketReceiverTCP::_asyncReceive(): receive 0 bytes, disconnect from '{0}'!", socket.RemoteEndPoint));
					Event.fireIn("_closeNetwork", new object[] { _networkInterface });
					return;
				}
			}
		}
	}
} 

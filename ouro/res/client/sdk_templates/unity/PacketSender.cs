﻿namespace Ouroboros
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
		Packet transmission module(with the server part of the network name of the corresponding)
		Processing the network data transmission
	*/
    public class PacketSender
    {
    		public delegate void AsyncSendMethod();

		private byte[] _buffer;

		int _wpos = 0;				// The write position of the data
		int _spos = 0;				// Send completed data location
		int _sending = 0;

		private NetworkInterface _networkInterface = null;
		AsyncCallback _asyncCallback = null;
		AsyncSendMethod _asyncSendMethod;

        public PacketSender(NetworkInterface networkInterface)
        {
        	_init(networkInterface);
        }

		~PacketSender()
		{
			Dbg.DEBUG_MSG("PacketSender::~PacketSender(), destroyed!");
		}

		void _init(NetworkInterface networkInterface)
		{
			_networkInterface = networkInterface;

			_buffer = new byte[OuroborosApp.app.getInitArgs().SEND_BUFFER_MAX];
			_asyncSendMethod = new AsyncSendMethod(this._asyncSend);
			_asyncCallback = new AsyncCallback(_onSent);

			_wpos = 0;
			_spos = 0;
			_sending = 0;
		}

		public NetworkInterface networkInterface()
		{
			return _networkInterface;
		}

		public bool send(MemoryStream stream)
		{
			int dataLength = (int)stream.length();
			if (dataLength <= 0)
				return true;

			if (0 == Interlocked.Add(ref _sending, 0))
			{
				if (_wpos == _spos)
				{
					_wpos = 0;
					_spos = 0;
				}
			}

			int t_spos = Interlocked.Add(ref _spos, 0);
			int space = 0;
			int tt_wpos = _wpos % _buffer.Length;
			int tt_spos = t_spos % _buffer.Length;

			if(tt_wpos >= tt_spos)
				space = _buffer.Length - tt_wpos + tt_spos - 1;
			else
				space = tt_spos - tt_wpos - 1;

			if (dataLength > space)
			{
				Dbg.ERROR_MSG("PacketSender::send(): no space, Please adjust 'SEND_BUFFER_MAX'! data(" + dataLength
					+ ") > space(" + space + "), wpos=" + _wpos + ", spos=" + t_spos);

				return false;
			}

			int expect_total = tt_wpos + dataLength;
			if(expect_total <= _buffer.Length)
			{
				Array.Copy(stream.data(), stream.rpos, _buffer, tt_wpos, dataLength);
			}
			else
			{
				int remain = _buffer.Length - tt_wpos;
				Array.Copy(stream.data(), stream.rpos, _buffer, tt_wpos, remain);
				Array.Copy(stream.data(), stream.rpos + remain, _buffer, 0, expect_total - _buffer.Length);
			}

			Interlocked.Add(ref _wpos, dataLength);

			if (Interlocked.CompareExchange(ref _sending, 1, 0) == 0)
			{
				_startSend();
			}

			return true;
		}

		void _startSend()
		{
			// Due to the socket with non-blocking, so here can not directly use the socket.send()Method
			// Must be placed into another thread to do it
			_asyncSendMethod.BeginInvoke(_asyncCallback, null);
		}

		void _asyncSend()
		{
			if (_networkInterface == null || !_networkInterface.valid())
			{
				Dbg.WARNING_MSG("PacketSender::_asyncSend(): network interface invalid!");
				return;
			}

			var socket = _networkInterface.sock();

			while (true)
			{
				int sendSize = Interlocked.Add(ref _wpos, 0) - _spos;
				int t_spos = _spos % _buffer.Length;
				if (t_spos == 0)
					t_spos = sendSize;

				if (sendSize > _buffer.Length - t_spos)
					sendSize = _buffer.Length - t_spos;

				int bytesSent = 0;
				try
				{
					bytesSent = socket.Send(_buffer, _spos % _buffer.Length, sendSize, 0);
				}
				catch (SocketException se)
				{
					Dbg.ERROR_MSG(string.Format("PacketSender::_asyncSend(): send data error, disconnect from '{0}'! error = '{1}'", socket.RemoteEndPoint, se));
					Event.fireIn("_closeNetwork", new object[] { _networkInterface });
					return;
				}

				int spos = Interlocked.Add(ref _spos, bytesSent);

				// All data is sent.
				if (spos == Interlocked.Add(ref _wpos, 0))
				{
					Interlocked.Exchange(ref _sending, 0);
					return;
				}
			}
		}

		private static void _onSent(IAsyncResult ar)
		{
			AsyncResult result = (AsyncResult)ar;
			AsyncSendMethod caller = (AsyncSendMethod)result.AsyncDelegate;
			caller.EndInvoke(ar);
		}
	}
}

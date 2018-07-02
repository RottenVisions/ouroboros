// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#include "telnet_handler.h"
#include "telnet_server.h"
#include "network/bundle.h"
#include "network/endpoint.h"
#include "network/network_interface.h"
#include "pyscript/script.h"

#ifndef CODE_INLINE
#include "telnet_handler.inl"
#endif

namespace Ouroboros {

char _g_state_str[][256] = {
	"password",
	"root",
	"python",
	"readonly"
};

/*
	Format: echo &quot;\033[word background color; font color m string\033[0m&quot;

	E.g:
	echo "\033[41;36m something here \033[0m"

	The position of 41 represents the background color, and the position of 36 represents the color of the word.


	Those ascii code is the beginning and end of the call to the color.
	\033[ ; m .... \033[0m



	Word background color range:40----49
	40:black
	41:Dark red
	42:green
	43:yellow
	44:blue
	45:purple
	46:Dark green
	47:white

	Word color:30-----------39
	30:black
	31:red
	32:green
	33:yellow
	34:blue
	35:purple
	36:Dark green
	37:white

	\33[0m Close all properties
	\33[1m Set high brightness
	\33[4m Underline
	\33[5m Blinking
	\33[7m Reverse display
	\33[8m Blanking
	\33[30m -- \33[37m Set the foreground
	\33[40m -- \33[47m Set the background color
	\33[nA Move cursor up n rows
	\33[nB Cursor down n rows
	\33[nC Cursor moves right by n lines
	\33[nD Cursor left shift n lines
	\33[y;xH sets the cursor position
	\33[2J Clear screen
	\33[K Clear the contents from the cursor to the end of the line
	\33[s Save cursor position
	\33[u Restore cursor position
	\33[?25l Hide the cursor
	\33[?25h Display cursor

	Using the format can be more complicated:
	^[[..m;..m;..m;..m
	E.g: \033[2;7;1m highlight\033[2;7;0m
*/

#define TELNET_CMD_LEFT							"\033[D"			// left
#define TELNET_CMD_RIGHT						"\033[C"			// right
#define TELNET_CMD_UP							"\033[A"			// on
#define TELNET_CMD_DOWN							"\033[B"			// under
#define TELNET_CMD_HOME							"\033[1~"			// Move to the beginning of the line
#define TELNET_CMD_END							"\033[4~"			// Move to the end of the line

#define TELNET_CMD_DEL							"\033[K"			// Delete character
#define TELNET_CMD_NEWLINE						"\r\n"				// New line
#define TELNET_CMD_MOVE_FOCUS_LEFT_MAX			"\33[9999999999D"	// Move the cursor left to the front
#define TELNET_CMD_MOVE_FOCUS_RIGHT_MAX			"\33[9999999999C"	// Move cursor right to the end

//-------------------------------------------------------------------------------------
TelnetHandler::TelnetHandler(Network::EndPoint* pEndPoint, TelnetServer* pTelnetServer, Network::NetworkInterface* pNetworkInterface, TELNET_STATE defstate):
historyCommand_(),
historyCommandIndex_(0),
command_(),
pEndPoint_(pEndPoint),
pTelnetServer_(pTelnetServer),
state_(defstate),
currPos_(0),
pProfileHandler_(NULL),
pNetworkInterface_(pNetworkInterface),
getingHistroyCmd_(false)
{
}

//-------------------------------------------------------------------------------------
TelnetHandler::~TelnetHandler(void)
{
	if(pProfileHandler_) {
		pProfileHandler_->destroy();
		pProfileHandler_ = NULL;
	}
}

//-------------------------------------------------------------------------------------
std::string TelnetHandler::getInputStartString()
{
	return fmt::format("[{}@{} ~]{} ", COMPONENT_NAME_EX(g_componentType),
		_g_state_str[(int)state_], (state_ == TELNET_STATE_PYTHON ? " >>>" : "#"));
}

//-------------------------------------------------------------------------------------
std::string TelnetHandler::getWelcome()
{
	return fmt::format("\033[1;32mwelcome to {} \r\n"
			"Version: {}. "
			"ScriptVersion: {}. "
			"Config: {}. "
			"Built: {} {}. "
			"AppID: {}. "
			"UID: {}. "
			"PID: {}"
			"\r\n---------------------------------------------"
			"{}"
			"\r\n---------------------------------------------"
			" \033[0m\r\n{}",
		COMPONENT_NAME_EX(g_componentType), OUROVersion::versionString(), OUROVersion::scriptVersionString(),
		OURO_CONFIG, __TIME__, __DATE__,
		g_componentID, getUserUID(), getProcessPID(), help(), getInputStartString());
}

//-------------------------------------------------------------------------------------
std::string TelnetHandler::help()
{
	return 	"\033[1;32m\r\nCommand List:"
		"\r\n[:help          ]: list commands."
		"\r\n[:quit          ]: quit the server."
		"\r\n[:python        ]: python console."
		"\r\n[:root          ]: return to the root layer."
		"\r\n[:cprofile      ]: collects and reports the internal c++ profiles \r\n\t\tof a server process over a period of time."
		"\r\n\t\t usage: \":cprofile 30\""
		"\r\n[:pyprofile     ]: collects and reports the python profiles \r\n\t\tof a server process over a period of time."
		"\r\n\t\t usage: \":pyprofile 30\""
		"\r\n[:eventprofile  ]: a server process over a period of time, \r\n\t\tcollects and reports the all non-volatile cummunication \r\n\t\tdown to the client."
		"\r\n\t\t usage: \":eventprofile 30\""
		"\r\n[:networkprofile]: collects and reports the network profiles \r\n\t\tof a server process over a period of time."
		"\r\n\t\t usage: \":networkprofile 30\""
		"\r\n\r\n\033[0m";
};

//-------------------------------------------------------------------------------------
void TelnetHandler::historyCommandCheck()
{
	if(historyCommand_.size() > 50)
		historyCommand_.pop_front();

	if(historyCommandIndex_ < 0)
		historyCommandIndex_ = historyCommand_.size() - 1;

	if(historyCommandIndex_ > (int)historyCommand_.size() - 1)
		historyCommandIndex_ = 0;
}

//-------------------------------------------------------------------------------------
std::string TelnetHandler::getHistoryCommand(bool isNextCommand)
{
	if(isNextCommand)
		historyCommandIndex_++;
	else
		historyCommandIndex_--;

	historyCommandCheck();

	if(historyCommand_.size() == 0)
		return "";

	return historyCommand_[historyCommandIndex_];
}

//-------------------------------------------------------------------------------------
Network::Reason TelnetHandler::checkLastErrors()
{
	int err;
	Network::Reason reason;

#ifdef unix
	err = errno;

	switch (err)
	{
	case ECONNREFUSED:	reason = Network::REASON_NO_SUCH_PORT; break;
	case EAGAIN:		reason = Network::REASON_RESOURCE_UNAVAILABLE; break;
	case EPIPE:			reason = Network::REASON_CLIENT_DISCONNECTED; break;
	case ECONNRESET:	reason = Network::REASON_CLIENT_DISCONNECTED; break;
	case ENOBUFS:		reason = Network::REASON_TRANSMIT_QUEUE_FULL; break;
	default:			reason = Network::REASON_GENERAL_NETWORK; break;
	}
#else
	err = WSAGetLastError();

	if (err == WSAEWOULDBLOCK || err == WSAEINTR)
	{
		reason = Network::REASON_RESOURCE_UNAVAILABLE;
	}
	else
	{
		switch (err)
		{
		case WSAECONNREFUSED:   reason = Network::REASON_NO_SUCH_PORT; break;
		case WSAECONNRESET:     reason = Network::REASON_CLIENT_DISCONNECTED; break;
		case WSAECONNABORTED:   reason = Network::REASON_CLIENT_DISCONNECTED; break;
		default:                reason = Network::REASON_GENERAL_NETWORK; break;
		}
	}
#endif

	return reason;
}

//-------------------------------------------------------------------------------------
int	TelnetHandler::handleInputNotification(int fd)
{
	OURO_ASSERT((*pEndPoint_) == fd);

	char data[1024] = {0};
	int recvsize = pEndPoint_->recv(data, sizeof(data));

	if(recvsize == -1)
	{
		Network::Reason err = checkLastErrors();
		if (err != Network::REASON_RESOURCE_UNAVAILABLE)
			pTelnetServer_->onTelnetHandlerClosed(fd, this);
		return 0;

	}
	else if(recvsize == 0)
	{
		pTelnetServer_->onTelnetHandlerClosed(fd, this);
		return 0;
	}

	if(state_ == TELNET_STATE_READONLY)
		return 0;

	onRecvInput(data, recvsize);
	return 0;
}

//-------------------------------------------------------------------------------------
void TelnetHandler::onRecvInput(const char *buffer, int size)
{
	int idx = 0;
	while (idx < size)
	{
		char c = buffer[idx++];

		switch(c)
		{
		case '\r':
			getingHistroyCmd_ = false;
			if (idx < size)
			{
				int cc = buffer[idx++];
				if(cc == '\n')
				{
					if(!processCommand())
						return;
				}
			}
			break;
		case 8:		// backspace
			if (currPos_ == 0)
			{
				resetStartPosition();
			}
			else
			{
				sendDelChar();
				checkAfterStr();
			}
			break;
		case 27:	// Vt100 command code: 0x1b
		{
			std::string s = "";
			std::string vt100cmd(s + c);
			bool shouldBeContinue = true;

			while (shouldBeContinue && idx < size)
			{
				if (buffer[idx] == '\r')
					break;

				c = buffer[idx++];
				vt100cmd.append(s + c);
				switch (c)
				{
				case 'A': // Move cursor up n rows
				case 'B': // Cursor down n rows
				case 'C': // Cursor right shift n column
				case 'D': // Cursor left shift n column
				case '~': // home, end wait
					shouldBeContinue = false;
					break;

				case 'm': // Colors and other attributes or commands
				case 'J': // Clear screen
				case 'K': // Clear the contents from the cursor to the end of the line
				case 's': // Save cursor position
				case 'u': // Restore cursor position
				case 'l': // Hide the cursor
				case 'h': // Display cursor
				case 'H': // Set the cursor position
				default:
					break;
				}
			}

			if (!checkUDLR(vt100cmd))
			{
				// Output the unrecognized command as it is but change the command to &quot;^&quot;
				// To avoid client trigger command operations
				vt100cmd[0] = '^';
				command_.insert(currPos_, vt100cmd);
				currPos_ += vt100cmd.length();
				if (currPos_ == (int32)command_.length())
				{
					pEndPoint_->send(vt100cmd.c_str(), vt100cmd.size());
				}
				else
				{
					std::string s = command_.substr(currPos_ - vt100cmd.length(), command_.size() - currPos_ + vt100cmd.length());
					s += fmt::format("\33[{}D", command_.size() - currPos_);
					pEndPoint_->send(s.c_str(), s.size());
				}
			}
			break;
		}
		case 0x7f: // delete
			if (currPos_ < (int32)command_.length())
			{
				command_.erase(currPos_, 1);
				pEndPoint_->send(TELNET_CMD_DEL, strlen(TELNET_CMD_DEL));
				checkAfterStr();
			}
			break;
		default:
			{
				std::string s = "";
				s += c;
				command_.insert(currPos_, s);
				currPos_++;
				checkAfterStr();
				break;
			}
		};
	}
}

//-------------------------------------------------------------------------------------
void TelnetHandler::checkAfterStr()
{
	if(currPos_ != (int32)command_.size())
	{
		std::string s = "";
		s = command_.substr(currPos_, command_.size() - currPos_);
		s += fmt::format("\33[{}D", s.size());
		pEndPoint_->send(s.c_str(), s.size());
	}
}

//-------------------------------------------------------------------------------------
bool TelnetHandler::checkUDLR(const std::string &cmd)
{
	if (cmd.find(TELNET_CMD_UP) != std::string::npos)		// on
	{
		pEndPoint_->send(TELNET_CMD_MOVE_FOCUS_LEFT_MAX, strlen(TELNET_CMD_MOVE_FOCUS_LEFT_MAX));
		sendDelChar();
		std::string startstr = getInputStartString();
		pEndPoint_->send(startstr.c_str(), startstr.size());
		resetStartPosition();

		if(!getingHistroyCmd_)
		{
			++historyCommandIndex_;
			getingHistroyCmd_ = true;
		}

		std::string s = getHistoryCommand(false);
		pEndPoint_->send(s.c_str(), s.size());
		command_ = s;
		currPos_ = s.size();
		return true;
	}
	else if (cmd.find(TELNET_CMD_DOWN) != std::string::npos)	// under
	{
		pEndPoint_->send(TELNET_CMD_MOVE_FOCUS_LEFT_MAX, strlen(TELNET_CMD_MOVE_FOCUS_LEFT_MAX));
		sendDelChar();
		std::string startstr = getInputStartString();
		pEndPoint_->send(startstr.c_str(), startstr.size());
		resetStartPosition();

		if(!getingHistroyCmd_)
		{
			--historyCommandIndex_;
			getingHistroyCmd_ = true;
		}

		std::string s = getHistoryCommand(true);
		pEndPoint_->send(s.c_str(), s.size());
		command_ = s;
		currPos_ = s.size();
		return true;
	}
	else if (cmd.find(TELNET_CMD_RIGHT) != std::string::npos)	// right
	{
		int cmdlen = strlen(TELNET_CMD_RIGHT);
		if(currPos_ < (int)command_.size())
		{
			currPos_++;
			pEndPoint_->send(TELNET_CMD_RIGHT, cmdlen);
		}
		return true;
	}
	else if (cmd.find(TELNET_CMD_LEFT) != std::string::npos)	// left
	{
		int cmdlen = strlen(TELNET_CMD_LEFT);
		if(currPos_ > 0)
		{
			currPos_--;
			pEndPoint_->send(TELNET_CMD_LEFT, cmdlen);
		}
		return true;
	}
	else if (cmd.find(TELNET_CMD_HOME) != std::string::npos)	// Move to the beginning of the line
	{
		if (currPos_ > 0)
		{
			std::string cmdstr = fmt::format("\033[{}D", currPos_);
			pEndPoint_->send(cmdstr.c_str(), cmdstr.length());
			currPos_ = 0;
		}
		return true;
	}
	else if (cmd.find(TELNET_CMD_END) != std::string::npos)	    // Move to the end of the line
	{
		if (currPos_ != (int32)command_.length())
		{
			std::string cmdstr = fmt::format("\033[{}C", command_.length() - currPos_);
			pEndPoint_->send(cmdstr.c_str(), cmdstr.length());
			currPos_ = command_.length();
		}
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------
bool TelnetHandler::processCommand()
{
	if(command_.size() == 0)
	{
		sendNewLine();
		return true;
	}

	if(state_ == TELNET_STATE_PASSWD)
	{
		if(command_ == pTelnetServer_->passwd())
		{
			state_ = (TELNET_STATE)pTelnetServer_->deflayer();
			std::string s = getWelcome();
			pEndPoint_->send(s.c_str(), s.size());
			command_ = "";
			sendEnter();
			sendNewLine();
			return true;
		}
		else
		{
			command_ = "";
			sendNewLine();
			return true;
		}
	}


	bool logcmd = true;
	//for(int i=0; i<(int)historyCommand_.size(); ++i)
	{
		//if(historyCommand_[i] == command_)
		//{
		//	logcmd = false;
		//	break;
		//}
	}

	if(logcmd)
	{
		historyCommand_.push_back(command_);
		historyCommandCheck();
		historyCommandIndex_ = historyCommand_.size() - 1;
	}

	std::string cmd = command_;
	command_ = "";

	if(cmd == ":python")
	{
		if(pTelnetServer_->pScript() == NULL)
			return true;

		state_ = TELNET_STATE_PYTHON;
		sendNewLine();
		return true;
	}
	else if(cmd == ":help")
	{
		std::string str = help();
		pEndPoint_->send(str.c_str(), str.size());
		sendNewLine();
		return true;
	}
	else if(cmd == ":root")
	{
		sendNewLine();
		state_ = TELNET_STATE_ROOT;
		return true;
	}
	else if(cmd == ":quit")
	{
		state_ = TELNET_STATE_QUIT;

		pTelnetServer_->closeHandler((*pEndPoint_), this);
		return false;
	}
	else if(cmd.find(":cprofile") == 0)
	{
		uint32 timelen = 10;

		cmd.erase(cmd.find(":cprofile"), strlen(":cprofile"));
		if(cmd.size() > 0)
		{
			try
			{
				Ouroboros::StringConv::str2value(timelen, cmd.c_str());
			}
			catch(...)
			{
				timelen = 10;
			}

			if(timelen < 1 || timelen > 999999999)
				timelen = 10;
		}

		std::string str = fmt::format("Waiting for {} secs.\r\n", timelen);
		pEndPoint_->send(str.c_str(), str.size());

		std::string profileName = Ouroboros::StringConv::val2str(Ouroboros::genUUID64());

		if(pProfileHandler_) pProfileHandler_->destroy();
		pProfileHandler_ = new TelnetCProfileHandler(this, *pTelnetServer_->pNetworkInterface(),
			timelen, profileName, pEndPoint_->addr());

		readonly();
		return false;
	}
	else if(cmd.find(":pyprofile") == 0)
	{
		uint32 timelen = 10;

		cmd.erase(cmd.find(":pyprofile"), strlen(":pyprofile"));
		if(cmd.size() > 0)
		{
			try
			{
				Ouroboros::StringConv::str2value(timelen, cmd.c_str());
			}
			catch(...)
			{
				timelen = 10;
			}

			if(timelen < 1 || timelen > 999999999)
				timelen = 10;
		}

		std::string str = fmt::format("Waiting for {} secs.\r\n", timelen);
		pEndPoint_->send(str.c_str(), str.size());

		std::string profileName = Ouroboros::StringConv::val2str(Ouroboros::genUUID64());

		if(pProfileHandler_) pProfileHandler_->destroy();
		pProfileHandler_ = new TelnetPyProfileHandler(this, *pTelnetServer_->pNetworkInterface(),
			timelen, profileName, pEndPoint_->addr());

		readonly();
		return false;
	}
	else if (cmd.find(":pytickprofile") == 0)
	{
		uint32 timelen = 10;

		cmd.erase(cmd.find(":pytickprofile"), strlen(":pytickprofile"));
		if (cmd.size() > 0)
		{
			try
			{
				Ouroboros::StringConv::str2value(timelen, cmd.c_str());
			}
			catch (...)
			{
				timelen = 10;
			}

			if (timelen < 1 || timelen > 999999999)
				timelen = 10;
		}

		std::string profileName = Ouroboros::StringConv::val2str(Ouroboros::genUUID64());

		if (pProfileHandler_) pProfileHandler_->destroy();
		pProfileHandler_ = new TelnetPyTickProfileHandler(this, *pTelnetServer_->pNetworkInterface(),
			timelen, profileName, pEndPoint_->addr());

		readonly();
		return false;
	}
	else if (cmd.find(":eventprofile") == 0)
	{
		uint32 timelen = 10;

		cmd.erase(cmd.find(":eventprofile"), strlen(":eventprofile"));
		if(cmd.size() > 0)
		{
			try
			{
				Ouroboros::StringConv::str2value(timelen, cmd.c_str());
			}
			catch(...)
			{
				timelen = 10;
			}

			if(timelen < 1 || timelen > 999999999)
				timelen = 10;
		}

		std::string str = fmt::format("Waiting for {} secs.\r\n", timelen);
		pEndPoint_->send(str.c_str(), str.size());

		std::string profileName = Ouroboros::StringConv::val2str(Ouroboros::genUUID64());

		if(pProfileHandler_) pProfileHandler_->destroy();
		pProfileHandler_ = new TelnetEventProfileHandler(this, *pTelnetServer_->pNetworkInterface(),
			timelen, profileName, pEndPoint_->addr());

		readonly();
		return false;
	}
	else if(cmd.find(":networkprofile") == 0)
	{
		uint32 timelen = 10;

		cmd.erase(cmd.find(":networkprofile"), strlen(":networkprofile"));
		if(cmd.size() > 0)
		{
			try
			{
				Ouroboros::StringConv::str2value(timelen, cmd.c_str());
			}
			catch(...)
			{
				timelen = 10;
			}

			if(timelen < 1 || timelen > 999999999)
				timelen = 10;
		}

		std::string str = fmt::format("Waiting for {} secs.\r\n", timelen);
		pEndPoint_->send(str.c_str(), str.size());

		std::string profileName = Ouroboros::StringConv::val2str(Ouroboros::genUUID64());

		if(pProfileHandler_) pProfileHandler_->destroy();
		pProfileHandler_ = new TelnetNetworkProfileHandler(this, *pTelnetServer_->pNetworkInterface(),
			timelen, profileName, pEndPoint_->addr());

		readonly();
		return false;
	}

	if(state_ == TELNET_STATE_PYTHON)
	{
		processPythonCommand(cmd);
	}

	sendNewLine();
	return true;
}

//-------------------------------------------------------------------------------------
void TelnetHandler::processPythonCommand(std::string command)
{
	if(pTelnetServer_->pScript() == NULL || command.size() == 0)
		return;

	command += "\n";
	PyObject* pycmd = PyUnicode_DecodeUTF8(command.data(), command.size(), NULL);
	if(pycmd == NULL)
	{
		SCRIPT_ERROR_CHECK();
		return;
	}

	DEBUG_MSG(fmt::format("TelnetHandler::processPythonCommand: size({}), command={}.\n",
		command.size(), command));

	std::string retbuf = "";
	PyObject* pycmd1 = PyUnicode_AsEncodedString(pycmd, "utf-8", NULL);

	pTelnetServer_->pScript()->run_simpleString(PyBytes_AsString(pycmd1), &retbuf);

	// Replace '\' in return value with '\r\n' to resolve incorrect display in vt100 terminal
	std::string::size_type pos = 0;
	while ((pos = retbuf.find('\n', pos)) != std::string::npos)
	{
		if (retbuf[pos - 1] != '\r')
		{
			retbuf.insert(pos, "\r");
			pos++;
		}
		pos++;
	}

	if(retbuf.size() > 0)
	{
		// Return the result to the client
		Network::Bundle* pBundle = Network::Bundle::createPoolObject();
		(*pBundle) << retbuf;
		pEndPoint_->send(pBundle);
		Network::Bundle::reclaimPoolObject(pBundle);
		sendEnter();
	}

	Py_DECREF(pycmd);
	Py_DECREF(pycmd1);
}

//-------------------------------------------------------------------------------------
void TelnetHandler::sendEnter()
{
	pEndPoint_->send(TELNET_CMD_NEWLINE, strlen(TELNET_CMD_NEWLINE));
}

//-------------------------------------------------------------------------------------
void TelnetHandler::sendDelChar()
{
	if(command_.size() > 0)
	{
		if(currPos_ > 0)
		{
			command_.erase(currPos_ - 1, 1);
			currPos_--;
			pEndPoint_->send(TELNET_CMD_DEL, strlen(TELNET_CMD_DEL));
		}
	}
}

//-------------------------------------------------------------------------------------
void TelnetHandler::sendNewLine()
{
	std::string startstr = getInputStartString();
	pEndPoint_->send(startstr.c_str(), startstr.size());
	resetStartPosition();
	currPos_ = 0;
}

//-------------------------------------------------------------------------------------
void TelnetHandler::resetStartPosition()
{
	pEndPoint_->send(TELNET_CMD_MOVE_FOCUS_LEFT_MAX, strlen(TELNET_CMD_MOVE_FOCUS_LEFT_MAX));
	std::string startstr = getInputStartString();
	std::string backcmd = fmt::format("\33[{}C", startstr.size());
	pEndPoint_->send(backcmd.c_str(), backcmd.size());
}

//-------------------------------------------------------------------------------------
void TelnetHandler::setReadWrite()
{
	state_ = TELNET_STATE_ROOT;
}

//-------------------------------------------------------------------------------------
void TelnetHandler::readonly()
{
	state_ = TELNET_STATE_READONLY;
}

//-------------------------------------------------------------------------------------
void TelnetHandler::onProfileEnd(const std::string& datas)
{
	if (datas.size() > 0)
	{
		sendEnter();
		pEndPoint()->send(datas.c_str(), datas.size());
	}
	setReadWrite();
	sendEnter();
	sendNewLine();
	pProfileHandler_ = NULL;
}

//-------------------------------------------------------------------------------------
void TelnetPyProfileHandler::sendStream(MemoryStream* s)
{
	if(isDestroyed_) return;

	std::string datas;
	(*s) >> datas;

	// Replace '\n' in return value with '\r' to resolve incorrect display in vt100 terminal
	std::string::size_type pos = 0;
	while ((pos = datas.find('\n', pos)) != std::string::npos)
	{
		if (datas[pos - 1] != '\r')
		{
			datas.insert(pos, "\r");
			pos++;
		}
		pos++;
	}

	pTelnetHandler_->onProfileEnd(datas);
}

//-------------------------------------------------------------------------------------
void TelnetPyTickProfileHandler::sendStream(MemoryStream* s)
{
	if (isDestroyed_ || !pTelnetHandler_)
		return;


	std::string datas;
	(*s) >> datas;

	// Replace '\n' in return value with '\r' to resolve incorrect display in vt100 terminal
	std::string::size_type pos = 0;
	while ((pos = datas.find('\n', pos)) != std::string::npos)
	{
		if (datas[pos - 1] != '\r')
		{
			datas.insert(pos, "\r");
			pos++;
		}
		pos++;
	}

	pTelnetHandler_->sendEnter();
	pTelnetHandler_->pEndPoint()->send(datas.c_str(), datas.size());
	//pTelnetHandler_->onProfileEnd(datas);
}

//-------------------------------------------------------------------------------------
void TelnetPyTickProfileHandler::timeout()
{
	PyTickProfileHandler::timeout();

	if (isDestroyed_ || !pTelnetHandler_)
		return;

	pTelnetHandler_->onProfileEnd("");
}

//-------------------------------------------------------------------------------------
void TelnetPyTickProfileHandler::destroy()
{
	TelnetProfileHandler::destroy();
	pTelnetHandler_ = NULL;
}

//-------------------------------------------------------------------------------------
void TelnetCProfileHandler::sendStream(MemoryStream* s)
{
	if(isDestroyed_) return;

	std::string datas;
	uint32 timinglen;
	ArraySize size;

	(*s) >> timinglen >> size;

	datas = "ncalls\ttottime\tpercall\tcumtime\tpercall\tfilename:lineno(function)\r\n";

	while(size-- > 0)
	{
		uint32 count;
		float lastTime;
		float sumTime;
		float lastIntTime;
		float sumIntTime;
		std::string name;

		(*s) >> name >> count >> lastTime >> sumTime >> lastIntTime >> sumIntTime;

		char buf[256];
		ouro_snprintf(buf, 256, "%u", count);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%.3f", sumTime);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%.3f", lastTime);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%.3f", sumIntTime);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%.3f", lastIntTime);
		datas += buf;
		datas += "\t";

		datas += name;
		datas += "\r\n";
	};

	pTelnetHandler_->onProfileEnd(datas);
}

//-------------------------------------------------------------------------------------
void TelnetEventProfileHandler::sendStream(MemoryStream* s)
{
	if(isDestroyed_) return;

	std::string datas;
	uint32 timinglen;
	ArraySize size;

	(*s) >> timinglen >> size;

	if(size == 0)
		datas += "results is empty!";

	while(size-- > 0)
	{
		std::string type_name;
		(*s) >> type_name;

		datas += fmt::format("Event Type:{}\r\n\r\n(name|count|size)\r\n---------------------\r\n\r\n", type_name);

		Ouroboros::ArraySize size1;
		(*s) >> size1;

		while(size1-- > 0)
		{
			uint32 count;
			uint32 eventSize;
			std::string name;

			(*s) >> name >> count >> eventSize;

			if(count == 0)
				continue;

			datas += fmt::format("{}\t\t\t\t\t{}\t{}\r\n", name, count, eventSize);
		}

		datas += "\r\n\r\n";
	};

	pTelnetHandler_->onProfileEnd(datas);
}

//-------------------------------------------------------------------------------------
void TelnetNetworkProfileHandler::sendStream(MemoryStream* s)
{
	if(isDestroyed_) return;

	std::string datas;
	uint32 timinglen;
	ArraySize size;

	(*s) >> timinglen >> size;

	datas = "name\tsent#\tsize\tavg\ttotal#\ttotalsize\trecv#\tsize\tavg\ttotal#\ttotalsize\r\n";

	while(size-- > 0)
	{
		std::string name;

		uint32			send_size;
		uint32			send_avgsize;
		uint32			send_count;

		uint32			total_send_size;
		uint32			total_send_count;

		uint32			recv_size;
		uint32			recv_count;
		uint32			recv_avgsize;

		uint32			total_recv_size;
		uint32			total_recv_count;

		(*s) >> name >> send_count >> send_size >> send_avgsize >> total_send_size >> total_send_count;
		(*s)  >> recv_count >> recv_size >> recv_avgsize >> total_recv_size >> total_recv_count;

		char buf[256];

		datas += name;
		datas += "\t";

		ouro_snprintf(buf, 256, "%u", send_count);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%u", send_size);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%u", send_avgsize);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%u", total_send_count);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%u", total_send_size);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%u", recv_count);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%u", recv_size);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%u", recv_avgsize);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%u", total_recv_count);
		datas += buf;
		datas += "\t";

		ouro_snprintf(buf, 256, "%u", total_recv_size);
		datas += buf;

		datas += "\r\n";
	};

	pTelnetHandler_->onProfileEnd(datas);
}

//-------------------------------------------------------------------------------------
}

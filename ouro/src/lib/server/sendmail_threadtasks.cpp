// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "jwsmtp.h"
#include "sendmail_threadtasks.h"
#include "server/serverconfig.h"
#include "common/deadline.h"

namespace Ouroboros{

//-------------------------------------------------------------------------------------
bool SendEMailTask::process()
{
	jwsmtp::mailer m(emailaddr_.c_str(), g_ouroSrvConfig.emailServerInfo_.username.c_str(), subject(),
		g_ouroSrvConfig.emailAtivationInfo_.subject.c_str(), g_ouroSrvConfig.emailServerInfo_.smtp_server.c_str(),
		g_ouroSrvConfig.emailServerInfo_.smtp_port, false);

	m.authtype(g_ouroSrvConfig.emailServerInfo_.smtp_auth == 1 ? jwsmtp::mailer::LOGIN : jwsmtp::mailer::PLAIN);  

	std::string mailmessage = message();

	Ouroboros::strutil::ouro_replace(mailmessage, "${backlink}", fmt::format("http://{}:{}/{}_{}", 
		cbaddr_,
		cbport_,
		getopkey(),
		code_));

	Ouroboros::strutil::ouro_replace(mailmessage, "${username}", emailaddr_);
	Ouroboros::strutil::ouro_replace(mailmessage, "${code}", code_);
	mailmessage = Ouroboros::strutil::ouro_trim(mailmessage);

	m.setmessageHTML(mailmessage);

	m.username(g_ouroSrvConfig.emailServerInfo_.username.c_str());
	m.password(g_ouroSrvConfig.emailServerInfo_.password.c_str());
	m.send(); // send the mail

	INFO_MSG(fmt::format("SendEMailTask::process: sendmail[{}]: {}\n", getopkey(), m.response()));
	return false;
}

//-------------------------------------------------------------------------------------
thread::TPTask::TPTaskState SendEMailTask::presentMainThread()
{
	return thread::TPTask::TPTASK_STATE_COMPLETED; 
}

//-------------------------------------------------------------------------------------
const char* SendActivateEMailTask::subject()
{
	return g_ouroSrvConfig.emailAtivationInfo_.subject.c_str();
}

//-------------------------------------------------------------------------------------
const char* SendActivateEMailTask::message()
{
	return g_ouroSrvConfig.emailAtivationInfo_.message.c_str();
}

//-------------------------------------------------------------------------------------
const char* SendResetPasswordEMailTask::subject()
{
	return g_ouroSrvConfig.emailResetPasswordInfo_.subject.c_str();
}

//-------------------------------------------------------------------------------------
const char* SendResetPasswordEMailTask::message()
{
	return g_ouroSrvConfig.emailResetPasswordInfo_.message.c_str();
}

//-------------------------------------------------------------------------------------
const char* SendBindEMailTask::subject()
{
	return g_ouroSrvConfig.emailBindInfo_.subject.c_str();
}

//-------------------------------------------------------------------------------------
const char* SendBindEMailTask::message()
{
	return g_ouroSrvConfig.emailBindInfo_.message.c_str();
}

//-------------------------------------------------------------------------------------
}

This is a Ouroboros server asset library
========

##Starting the server

Start with fixed parameters: (meaning of the parameter: http://www.ouroboros.org/cn/docs/startup_shutdown.html)
	
	Linux:
		start_server.sh

	Windows:
		start_server.bat


##Close the server

Quickly kill the server process:

	Linux:
		kill_server.sh

	Windows:
		kill_server.bat


If it is a formal operating environment, you should use a secure shutdown method, which ensures the safe archiving of data, safely tells users to go offline, and so on.

	Linux:
		safe_kill.sh

	Windows:
		safe_kill.bat
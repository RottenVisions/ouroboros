Instructions:
1.Please install the matching django module for the python you are using.
  1.1.If you don't want to install django, there is another option:
  1.1.1.If you are using python2.6.6: you can choose to go to the "ouro/tools/server/django_packages" directory and extract the Django-1.6.11.tar.gz file from that directory (unzip to the current directory)
  1.1.2.If you are using python2.7 or above: you can choose to go to the "ouro/tools/server/django_packages" directory and extract the Django-1.8.9.tar.gz file from that directory (unzip to the current directory)
2.Modify sync_db.bat, sync_db.sh, and run_server.bat, run_server.sh to point them to the path of the python you are using.
3.First use, need to initialize data
  3.1.Windows python3.3 django 1.8.9, run the "sync_db.bat" command
  3.2.Linux python3.3 django 1.8.9, run the "sync_db.sh" command
  3.3.Linux python2.6 django 1.6.11, run the "sync_db_dj-1.6.sh" command
  3.4.Windows python2.6 django 1.6.11, please refer to "sync_db_dj-1.6.sh" to create a .bat file
4.Running the server
  4.1.Under Windows, run the "run_server.bat" command
  4.2.Under Linux, run the "run_server.sh" command
5.Enter "http://xxx.xxx.xxx.xxx:8000/wc/" in the browser to access, where "xxx.xxx.xxx.xxx" is the machine IP running webconsole

Description:
1.The first time you use this console, the default login account is ¡°Admin¡± and the default password is ¡°123456¡±. This account is also the only management account in the background. Please change the password in time after login.
2.The first time you use Admin to enter the background, you need to create a new control user based on the user account name and user UID of the server you started. After the creation is complete, you need to log out of Admin and use the new user to log in to perform the actual background operation.
3.Since the stage is a beta version, there are still many features that are not perfect and are not recommended for use in a production environment.
4.The development environment of this platform is python3.3 django 1.8.9, which is tested under Linux using python 2.6.6 django-1.6.11.
5.All of the tools in this tool are derived from Machine, so to use the functionality of this tool, you must ensure that the machine process is running correctly.
6.Any use problems, please put forward on the official Ouroboros platform.

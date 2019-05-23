# -*- coding: utf-8 -*-
import sys
import Ouroboros

def printMsg(args, isPrintPath):
	for m in args:print (m)

def TRACE_MSG(*args): 
	Ouroboros.scriptLogType(Ouroboros.LOG_TYPE_NORMAL)
	printMsg(args, False)
	
def DEBUG_MSG(*args): 
	if Ouroboros.publish() == 0:
		Ouroboros.scriptLogType(Ouroboros.LOG_TYPE_DBG)
		printMsg(args, True)
	
def INFO_MSG(*args): 
	if Ouroboros.publish() <= 1:
		Ouroboros.scriptLogType(Ouroboros.LOG_TYPE_INFO)
		printMsg(args, False)
	
def WARNING_MSG(*args): 
	Ouroboros.scriptLogType(Ouroboros.LOG_TYPE_WAR)
	printMsg(args, True)

def ERROR_MSG(*args): 
	Ouroboros.scriptLogType(Ouroboros.LOG_TYPE_ERR)
	printMsg(args, True)

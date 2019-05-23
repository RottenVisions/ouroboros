// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SERVER_ERRORS_H
#define OURO_SERVER_ERRORS_H

#include "common/common.h"

namespace Ouroboros { 

/**
	Server error, mainly used by the server to return to the client.
*/
	
typedef uint16 SERVER_ERROR_CODE; // error code category


#define SERVER_SUCCESS 0 // Success.
#define SERVER_ERR_SRV_NO_READY 1 // The server is not ready.
#define SERVER_ERR_SRV_OVERLOAD 2 // The server is overloaded.
#define SERVER_ERR_ILLEGAL_LOGIN 3 // Illegal login.
#define SERVER_ERR_NAME_PASSWORD 4 // The username or password is incorrect.
#define SERVER_ERR_NAME 5 // The username is incorrect.
#define SERVER_ERR_PASSWORD 6 // The password is incorrect.
#define SERVER_ERR_ACCOUNT_CREATE_FAILED 7 // Failed to create an account.
#define SERVER_ERR_BUSY 8 // The operation is too busy (for example: create an account N consecutive times if the server did not execute the previous request).
#define SERVER_ERR_ACCOUNT_LOGIN_ANOTHER 9 // The current account is logged in at another location.
#define SERVER_ERR_ACCOUNT_IS_ONLINE 10 // You are logged in and the server refuses to log in again.
#define SERVER_ERR_PROXY_DESTROYED 11 // The proxy associated with the client has been destroyed on the server.
#define SERVER_ERR_ENTITYDEFS_NOT_MATCH						12			// entityDefs≤ª∆•≈‰°£
#define SERVER_ERR_IN_SHUTTINGDOWN 13 // The server is shutting down
#define SERVER_ERR_NAME_MAIL 14 // The email address is incorrect.
#define SERVER_ERR_ACCOUNT_LOCK 15 // The account is frozen.
#define SERVER_ERR_ACCOUNT_DEADLINE 16 // The account has expired.
#define SERVER_ERR_ACCOUNT_NOT_ACTIVATED 17 // The account is not activated.
#define SERVER_ERR_VERSION_NOT_MATCH 18 // Does not match the version of the server.
#define SERVER_ERR_OP_FAILED 19 // The operation failed.
#define SERVER_ERR_SRV_STARTING 20 // The server is starting up.
#define SERVER_ERR_ACCOUNT_REGISTER_NOT_AVAILABLE 21 // The account registration function is not open.
#define SERVER_ERR_CANNOT_USE_MAIL 22 // You cannot use an email address.
#define SERVER_ERR_NOT_FOUND_ACCOUNT 23 // This account cannot be found.
#define SERVER_ERR_DB 24 // Database error (check dbmgr log and DB).
#define SERVER_ERR_USER1 25 // User-defined error code 1
#define SERVER_ERR_USER2 26 // User-defined error code 2
#define SERVER_ERR_USER3 27 // User-defined error code 3
#define SERVER_ERR_USER4 28 // User-defined error code 4
#define SERVER_ERR_USER5 29 // User-defined error code 5
#define SERVER_ERR_USER6 30 // User-defined error code 6
#define SERVER_ERR_USER7 31 // User-defined error code 7
#define SERVER_ERR_USER8 32 // User-defined error code 8
#define SERVER_ERR_USER9 33 // User-defined error code 9
#define SERVER_ERR_USER10 34 // User-defined error code 10
#define SERVER_ERR_LOCAL_PROCESSING 35 // Local processing, usually for a thing not handled by a third party but by a KBE server
#define SERVER_ERR_ACCOUNT_RESET_PASSWORD_NOT_AVAILABLE 36 // The account reset password function is not open.
#define SERVER_ERR_ACCOUNT_LOGIN_ANOTHER_SERVER 37 // The current account is logged in on another server
#define SERVER_ERR_MAX 38 // Please put this at the end of all errors. This is not an error flag. It only indicates how many error definitions are there.

const char SERVER_ERR_STR[][256] = {
	"SERVER_SUCCESS",
	"SERVER_ERR_SRV_NO_READY",
	"SERVER_ERR_SRV_OVERLOAD",
	"SERVER_ERR_ILLEGAL_LOGIN",
	"SERVER_ERR_NAME_PASSWORD",
	"SERVER_ERR_NAME",
	"SERVER_ERR_PASSWORD",
	"SERVER_ERR_ACCOUNT_CREATE_FAILED",
	"SERVER_ERR_BUSY",
	"SERVER_ERR_ACCOUNT_LOGIN_ANOTHER",
	"SERVER_ERR_ACCOUNT_IS_ONLINE",
	"SERVER_ERR_PROXY_DESTROYED",
	"SERVER_ERR_ENTITYDEFS_NOT_MATCH",
	"SERVER_ERR_IN_SHUTTINGDOWN",
	"SERVER_ERR_NAME_MAIL",
	"SERVER_ERR_ACCOUNT_LOCK",
	"SERVER_ERR_ACCOUNT_DEADLINE",
	"SERVER_ERR_ACCOUNT_NOT_ACTIVATED",
	"SERVER_ERR_VERSION_NOT_MATCH",
	"SERVER_ERR_OP_FAILED",
	"SERVER_ERR_SRV_STARTING",
	"SERVER_ERR_ACCOUNT_REGISTER_NOT_AVAILABLE",
	"SERVER_ERR_CANNOT_USE_MAIL",
	"SERVER_ERR_NOT_FOUND_ACCOUNT",
	"SERVER_ERR_DB",
	"SERVER_ERR_USER1",
	"SERVER_ERR_USER2",
	"SERVER_ERR_USER3",
	"SERVER_ERR_USER4",
	"SERVER_ERR_USER5",
	"SERVER_ERR_USER6",
	"SERVER_ERR_USER7",
	"SERVER_ERR_USER8",
	"SERVER_ERR_USER9",
	"SERVER_ERR_USER10",
	"SERVER_ERR_LOCAL_PROCESSING",
	"SERVER_ERR_ACCOUNT_RESET_PASSWORD_NOT_AVAILABLE",
	"SERVER_ERR_ACCOUNT_LOGIN_ANOTHER_SERVER"
};

}

#endif // OURO_SERVER_ERRORS_H

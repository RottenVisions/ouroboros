#include "OBEventTypes.h"

namespace Ouroboros
{

const FString OBEventTypes::createAccount = "createAccount";
const FString OBEventTypes::login = "login";
const FString OBEventTypes::logout = "logout";
const FString OBEventTypes::reloginBaseapp = "reloginBaseapp";
const FString OBEventTypes::bindAccountEmail = "bindAccountEmail";
const FString OBEventTypes::newPassword = "newPassword";
const FString OBEventTypes::resetPassword = "resetPassword";

// ------------------------------------ Connection related ---------- --------------------------
const FString OBEventTypes::onKicked = "onKicked";
const FString OBEventTypes::onDisconnected = "onDisconnected";
const FString OBEventTypes::onConnectionState = "onConnectionState";

// ------------------------------------logon related---------- --------------------------
const FString OBEventTypes::onCreateAccountResult = "onCreateAccountResult";
const FString OBEventTypes::onVersionNotMatch = "onVersionNotMatch";
const FString OBEventTypes::onScriptVersionNotMatch = "onScriptVersionNotMatch";
const FString OBEventTypes::onLoginFailed = "onLoginFailed";
const FString OBEventTypes::onLoginBaseapp = "onLoginBaseapp";
const FString OBEventTypes::onLoginBaseappFailed = "onLoginBaseappFailed";
const FString OBEventTypes::onReloginBaseapp = "onReloginBaseapp";
const FString OBEventTypes::onReloginBaseappSuccessfully = "onReloginBaseappSuccessfully";
const FString OBEventTypes::onReloginBaseappFailed = "onReloginBaseappFailed";

// ------------------------------------ Entity cell related events -------- ----------------------------
const FString OBEventTypes::onEnterWorld = "onEnterWorld";
const FString OBEventTypes::onLeaveWorld = "onLeaveWorld";
const FString OBEventTypes::onEnterSpace = "onEnterSpace";
const FString OBEventTypes::onLeaveSpace = "onLeaveSpace";
const FString OBEventTypes::set_position = "set_position";
const FString OBEventTypes::set_direction = "set_direction";
const FString OBEventTypes::updatePosition = "updatePosition";
const FString OBEventTypes::addSpaceGeometryMapping = "addSpaceGeometryMapping";
const FString OBEventTypes::onSetSpaceData = "onSetSpaceData";
const FString OBEventTypes::onDelSpaceData = "onDelSpaceData";
const FString OBEventTypes::onControlled = "onControlled";
const FString OBEventTypes::onLoseControlledEntity = "onLoseControlledEntity";

// ------------------------------------ Data download related --------- ---------------------------
const FString OBEventTypes::onStreamDataStarted = "onStreamDataStarted";
const FString OBEventTypes::onStreamDataRecv = "onStreamDataRecv";
const FString OBEventTypes::onStreamDataCompleted = "onStreamDataCompleted";

// ------------------------------------SDK update related --------- ----------------------------
const FString OBEventTypes::onImportClientSDKSuccessfully = "onImportClientSDKSuccessfully";
const FString OBEventTypes::onDownloadSDK = "onDownloadSDK";

}
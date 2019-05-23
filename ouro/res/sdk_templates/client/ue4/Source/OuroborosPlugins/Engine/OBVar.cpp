
#include "KBVar.h"
#include "KBDebug.h"
#include "OUROCommon.h"

namespace Ouroboros
{

void KBVar::ErrorLog(const FString& errstr) const
{
	(const_cast<KBVar*>(this))->Error = 1;

	SET_WARN_COLOR(COLOR_RED); 
	const FString NewMsg = FString::Printf(TEXT("**ERROR** %s"), *errstr);
	UE_LOG(LogOuroboros, Log, TEXT("%s"), *NewMsg); 
	CLEAR_WARN_COLOR(); 
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, NewMsg); 
}

}


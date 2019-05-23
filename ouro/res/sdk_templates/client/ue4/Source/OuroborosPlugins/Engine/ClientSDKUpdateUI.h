#pragma once

#include "OuroborosPluginsPrivatePCH.h"


class OUROBOROSPLUGINS_API SClientSDKUpdateUI : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SClientSDKUpdateUI){}
	
	SLATE_END_ARGS()

	void Construct(const FArguments& args);

	FReply UpdateSDKClicked();


};

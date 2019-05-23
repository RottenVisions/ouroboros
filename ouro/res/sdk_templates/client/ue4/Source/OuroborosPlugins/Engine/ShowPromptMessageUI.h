#pragma once

#include "OuroborosPluginsPrivatePCH.h"


class OUROBOROSPLUGINS_API SShowPromptMessageUI : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SShowPromptMessageUI){}
	
	SLATE_END_ARGS()

	void Construct(const FArguments& args);

};

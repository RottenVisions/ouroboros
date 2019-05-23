#include "ClientSDKUpdateUI.h"
#include "Engine.h"
#include "OBEvent.h"
#include "Ouroboros.h"
#include "OBEventTypes.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SClientSDKUpdateUI::Construct(const FArguments& args)
{
	FTextBlockStyle textStyle = FCoreStyle::Get().GetWidgetStyle< FTextBlockStyle >("NormalText");
	textStyle.SetFont(FSlateFontInfo("Veranda", 26));
	textStyle.SetColorAndOpacity(FLinearColor::White);

	ChildSlot
		[
			SNew(SOverlay)
			/*+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::White)
		.ShadowColorAndOpacity(FLinearColor::Black)
		.ShadowOffset(FIntPoint(-1, 1))
		.Font(FSlateFontInfo("Arial", 26))
		.Text(FText::FromString("Main Menu"))
		]*/
	+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(20)
			[
				SNew(SButton)
				.Text(FText::FromString("Update SDK"))
				.TextStyle(&textStyle)
				.ContentPadding(FMargin(30.0, 15.0))
				.ButtonColorAndOpacity(FLinearColor::Green)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Top)
				.OnClicked(this, &SClientSDKUpdateUI::UpdateSDKClicked)
			]
		]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SClientSDKUpdateUI::UpdateSDKClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Update SDK!"));
	}

	UOBEventData_onDownloadSDK* pEventData = NewObject<UOBEventData_onDownloadSDK>();
	pEventData->isDownload = true;
	OUROBOROS_EVENT_FIRE(Ouroboros::OBEventTypes::onDownloadSDK, pEventData);
	return FReply::Handled();
}

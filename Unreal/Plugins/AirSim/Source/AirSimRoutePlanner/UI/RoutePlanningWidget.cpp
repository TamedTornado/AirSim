#include "RoutePlanningWidget.h"



void URoutePlanningWidget::CallZoomInClicked()
{
	OnZoomInClicked.Broadcast();
}

void URoutePlanningWidget::CallZoomOutClicked()
{
	OnZoomOutClicked.Broadcast();
}

void URoutePlanningWidget::CallZoomValueChanged(float NewZoomValue)
{
	OnZoomValueChanged.Broadcast(NewZoomValue);
}

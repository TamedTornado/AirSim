#include "MapSelectableActor.h"



void AMapSelectableActor::SetHoveredOver_Implementation(bool bNewHoveredOver)
{
	bHoveredOver = bNewHoveredOver;
	UpdateVisuals();
}

void AMapSelectableActor::SetSelected_Implementation(bool bNewSelected)
{
	bSelected = bNewSelected;
	UpdateVisuals();
}

void AMapSelectableActor::UpdateVisuals()
{
	HandleVisualsUpdate();
}

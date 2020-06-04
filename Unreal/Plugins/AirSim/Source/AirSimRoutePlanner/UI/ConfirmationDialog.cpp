// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfirmationDialog.h"

void UConfirmationDialog::CallOkButtonClicked()
{
	OnOkButtonClicked.Broadcast();
}

void UConfirmationDialog::CallCancelButtonClicked()
{
	OnCancelButtonClicked.Broadcast();
}

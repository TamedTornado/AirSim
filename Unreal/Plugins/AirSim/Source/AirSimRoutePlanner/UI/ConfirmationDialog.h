// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmationDialog.generated.h"

DECLARE_MULTICAST_DELEGATE(FConfirmationDialogEventSig);

/**
 * A modal ok/cancel dialog
 */
UCLASS()
class AIRSIMROUTEPLANNER_API UConfirmationDialog : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Confirmation Dialog")
	FText				HeaderText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Confirmation Dialog")
	FText				BodyText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Confirmation Dialog")
	FText				OkButtonText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Confirmation Dialog")
	FText				CancelButtonText;

	UFUNCTION(BlueprintCallable, Category = "Confirmation Dialog")
	void								CallOkButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Confirmation Dialog")
	void								CallCancelButtonClicked();

	FConfirmationDialogEventSig			OnOkButtonClicked;

	FConfirmationDialogEventSig			OnCancelButtonClicked;
};

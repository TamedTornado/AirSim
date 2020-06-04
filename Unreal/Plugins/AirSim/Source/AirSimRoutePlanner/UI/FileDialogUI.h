// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FileDialogUI.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FFileDialogFileEvent, FString);
DECLARE_MULTICAST_DELEGATE(FFileDialogEvent);

UCLASS(BlueprintType)
class UDialogFileEntry : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "File Entry")
	FString			FullPath;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "File Entry")
	FString			FilenameWOExt;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "File Entry")
	FString			LastModified;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "File Entry")
	FString			Size;
};

/**
 * A widget that shows a list of files of a certain spec in the specified directory. Lets the user select one, or alternately both select AND/OR choose a new name
 */
UCLASS(BlueprintType, Blueprintable)
class AIRSIMROUTEPLANNER_API UFileDialogUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "File Dialog")
	FString						FileExtension;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "File Dialog")
	FString						Directory;

	// If true, this will allow the user to enter a new name rather than just selecting a file
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "File Dialog")
	bool						bIsSaveDialog = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "File Dialog")
	TArray<UDialogFileEntry*>	Files;

	UFUNCTION(BlueprintCallable, Category = "File Dialog")
	void						UpdateFilesList();

	UFUNCTION(BlueprintImplementableEvent, Category = "File Dialog")
	void						FilesListUpdated();

	UFUNCTION(BlueprintCallable, Category = "File Dialog")
	void						CallOnFileEntryChosen(UDialogFileEntry* fileEntry);

	// Called by the "save" version of the dialog, just gives a filename without extension
	UFUNCTION(BlueprintCallable, Category = "File Dialog")
	void						CallOnFilenameChosen(FString filename);


	FFileDialogFileEvent		OnFilenameChosen;

	UFUNCTION(BlueprintCallable, Category = "File Dialog")
	void						CallOnCanceled();

	FFileDialogEvent			OnCanceled;

	
};

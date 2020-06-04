// Fill out your copyright notice in the Description page of Project Settings.


#include "FileDialogUI.h"
#include "HAL/FileManager.h"
#include "GenericPlatform/GenericPlatformFile.h"



void UFileDialogUI::UpdateFilesList()
{
	// Get all of the files matching the file spec in the specified directory and add them to the Files array

	Files.Empty();

	class FFileMatch : public IPlatformFile::FDirectoryStatVisitor
	{
	public:
		TArray<UDialogFileEntry*>& Result;
		FString WildCard;
		UFileDialogUI* Owner;

		FFileMatch(TArray<UDialogFileEntry*>& InResult, const FString& InWildCard, UFileDialogUI* InOwner):
			Result(InResult), 
			WildCard(InWildCard),
			Owner(InOwner)
		{
		}

		bool Visit(const TCHAR* FilenameOrDirectory, const FFileStatData& StatData) override
		{
			if (!StatData.bIsDirectory && StatData.bIsValid)
			{
				const FString Filename = FPaths::GetCleanFilename(FilenameOrDirectory);
				if (Filename.MatchesWildcard(WildCard))
				{
					UDialogFileEntry* newEntry = NewObject<UDialogFileEntry>(Owner, NAME_None);
					newEntry->FullPath = FilenameOrDirectory;
					newEntry->FilenameWOExt = FPaths::GetBaseFilename(Filename);
					newEntry->LastModified = StatData.ModificationTime.ToString();
					
					// Format to kb, until we find a better function or make one

					int32 fileSize = (int32)StatData.FileSize;
					if (fileSize > 1024)
					{
						newEntry->Size = FString::Format(TEXT("{0} kb"), { FString::FormatAsNumber(fileSize/1024) }); // Truncates the decimal, probably fine
					} else
						newEntry->Size = FString::Format(TEXT("{0} bytes"), { FString::FormatAsNumber(fileSize) });;

					Result.Add(newEntry);
				}
			}

			return true;
		}

	};
	FString Filename(Directory+"*"+FileExtension);
	FPaths::NormalizeFilename(Filename);
	const FString CleanFilename = FPaths::GetCleanFilename(Filename);
	FFileMatch FileMatch(Files, CleanFilename, this);
	IFileManager::Get().IterateDirectoryStat(*FPaths::GetPath(Filename), FileMatch);

	FilesListUpdated();
}

void UFileDialogUI::CallOnFileEntryChosen(UDialogFileEntry* fileEntry)
{
	if (fileEntry)
	{
		OnFilenameChosen.Broadcast(fileEntry->FullPath);
	}
}

void UFileDialogUI::CallOnFilenameChosen(FString filename)
{
	FString fullPath = Directory + filename	+ FileExtension;

	OnFilenameChosen.Broadcast(fullPath);
}

void UFileDialogUI::CallOnCanceled()
{
	OnCanceled.Broadcast();
}

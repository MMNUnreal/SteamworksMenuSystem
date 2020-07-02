// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class MULTI_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void SetServerName(const FString& ServerNameIn);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerName;

	//** Calls select index on parent and tell what index row is **//
	void Setup(class UMainMenu* ParentIn, uint32 IndexIn);

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* RowButton;

	UFUNCTION()
	void OnRowClicked();

	UPROPERTY()
	class UMainMenu* Parent;

	UPROPERTY()
	uint32 Index;

	
};

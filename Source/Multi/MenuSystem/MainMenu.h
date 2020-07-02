// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class MULTI_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

public:
	UMainMenu(const FObjectInitializer& ObjectInitializer);

	//**Populate Server **//
	UFUNCTION()
	void PopulateServerList(TArray<FString> ServerNames);

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void ExitApplication();

	UFUNCTION()
	void SwitchMenu(UWidgetSwitcher* WidgetSwitcher, UWidget* MenuToSwitchTo);

private:

	//** Main Menu **//
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ExitApplicationButton;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	//** Join Menu **//
	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinGameButton;

	/*
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* IPAddressField;
	*/

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;

	//** Server Row **//
	//class UServerRow* ServerRow;

	TSubclassOf<UUserWidget> ServerRowClass;

};

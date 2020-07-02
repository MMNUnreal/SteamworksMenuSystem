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
	//** Set up server row bp class **//
	UMainMenu(const FObjectInitializer& ObjectInitializer);

	//**Populate Server **//
	UFUNCTION()
	void PopulateServerList(TArray<FString> ServerNames);

protected:
	//** Set up bindings on widgets **//
	virtual bool Initialize() override;

	//** Host the game **//
	UFUNCTION()
	void HostServer();

	//** Opens join server menu and refresh server list **//
	UFUNCTION()
	void OpenJoinMenu();

	//** Opens main menu **//
	UFUNCTION()
	void OpenMainMenu();

	//** Calls join server on interface **//
	UFUNCTION()
	void JoinServer();

	//** Quits game **//
	UFUNCTION()
	void ExitApplication();

	//** Switches between widgets in main menu widget **//
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

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;

	TSubclassOf<UUserWidget> ServerRowClass;

};

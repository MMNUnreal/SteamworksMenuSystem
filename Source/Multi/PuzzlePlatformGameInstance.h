// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include "OnlineSubsystem.h"

#include "PuzzlePlatformGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MULTI_API UPuzzlePlatformGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

private:
	UPuzzlePlatformGameInstance(const FObjectInitializer& ObjectInitializer);

	//** Initialise **//
	virtual void Init();

	//- - - - SESSION - - - - //

	//** Host the session **//
	UFUNCTION(Exec)
	virtual void Host() override;

	//** Join Server on IP Address **//
	UFUNCTION(Exec)
	virtual void Join(const FString& Address) override;

	//** Create an online session **//
	void CreateSession();

	//** Fired when session created **//
	void OnCreateSessionComplete(FName SessionName, bool bSuccess);

	//** Fired when session destroyed **//
	void OnDestroySessionComplete(FName SessionName, bool bSuccess);

	IOnlineSessionPtr SessionInterface;

	// - - - - - WIDGET MENUS - - - - - - //

	//** Load the Main Menu **//
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void LoadMenu();

	//** Load the in game menu **//
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void LoadInGameMenu();

	//** Loads the main menu from the in game menu **//
	virtual void LoadMainMenu() override;

	//** Debugger **//
	void DebugMessage(const FString& Message);

	/** WIDGET MENU **/
	TSubclassOf<UUserWidget> MenuClass;

	//** Main Menu widget **//
	class UMainMenu* Menu;

	/** WIDGET IN GAME MENU **/
	TSubclassOf<UUserWidget> InGameMenuClass;

	//** In game menu widget **//
	class UMenuWidget* InGameMenu;
	
};

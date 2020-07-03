// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

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

	//** Server name **//
	FString DesiredServerName;

	//** Host the session **//
	UFUNCTION(Exec)
	virtual void Host(FString ServerNameIn) override;

	//** Join Server on IP Address **//
	UFUNCTION(Exec)
	virtual void Join(uint32 IndexIn) override;

	//** Create an online session **//
	void CreateSession();

	//** Callback when session created **//
	void OnCreateSessionComplete(FName SessionName, bool bSuccess);

	//** Callback when session destroyed **//
	void OnDestroySessionComplete(FName SessionName, bool bSuccess);

	//** Find all current live sessions to populate server list **//
	virtual void FindSessions() override;

	//** Callback when find session is comleted **//
	void OnFindSessionComplete(bool bSuccess);

	//** Callback on successful join of session, do the client travel on successful connection **//
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

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

// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformGameInstance.h"

#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MenuSystem/MainMenu.h"
#include "MenuSystem/InGameMenu.h"

#include "MenuSystem/MenuWidget.h"

#include "PlatformTrigger.h"

const static FName SESSION_NAME = TEXT("SessionGame");

UPuzzlePlatformGameInstance::UPuzzlePlatformGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuClassBP(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (!ensure(MenuClassBP.Class != NULL)) return;
	MenuClass = MenuClassBP.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuClassBP(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
	if (!ensure(InGameMenuClassBP.Class != NULL)) return;
	InGameMenuClass = InGameMenuClassBP.Class;
}

//** Set up online subsystem **//
void UPuzzlePlatformGameInstance::Init()
{
	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();
	if (SubSystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Subsystem: %s"), *SubSystem->GetSubsystemName().ToString());
		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformGameInstance::OnDestroySessionComplete);
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL Subsystem: %s"), *SubSystem->GetSubsystemName().ToString());
	}
}

//** Host server **//
void UPuzzlePlatformGameInstance::Host()
{
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}

	}	
}

//** Create online session **//
void UPuzzlePlatformGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

//** On session completed, travel to main map **//
void UPuzzlePlatformGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (!bSuccess)
	{	
		UE_LOG(LogTemp, Warning, TEXT("Failed to complete session creation"));
		return;
	}

	if (Menu != nullptr)
	{
		Menu->Teardown();
	}

	DebugMessage(TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
}

//** Destroy session - once completed create a new session **//
void UPuzzlePlatformGameInstance::OnDestroySessionComplete(FName SessionName, bool bSuccess)
{
	if(!bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to destroy session"));
		return;
	}
	else
	{
		CreateSession();
	}
}

//** Join server **//
void UPuzzlePlatformGameInstance::Join(const FString& Address)
{
	if (Menu != nullptr)
	{
		Menu->Teardown();
	}

	FString JoinMessage = "Joining: " + Address;
	DebugMessage(JoinMessage);

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(World);
	if(!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

//** Add Menu Widget to viewport **//
void UPuzzlePlatformGameInstance::LoadMenu()
{
	if(!ensure(MenuClass != nullptr)) return;
	Menu = CreateWidget<UMainMenu>(this, MenuClass); 

	if(!ensure(Menu != nullptr)) return;
	Menu->Setup();
	Menu->SetMenuInterface(this);
}

//** Add In Game Menu Widget to viewport **//
void UPuzzlePlatformGameInstance::LoadInGameMenu()
{
	if (!ensure(InGameMenuClass != nullptr)) return;
	InGameMenu = CreateWidget<UMenuWidget>(this, InGameMenuClass);

	if (!ensure(InGameMenu != nullptr)) return;
	InGameMenu->Setup();
	InGameMenu->SetMenuInterface(this);
}

//** When loading main menu after exiting in game menu
void UPuzzlePlatformGameInstance::LoadMainMenu()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(World);
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel("/Game/Maps/L_MainMenu", ETravelType::TRAVEL_Absolute);
}

//** Debugging message to log to screen **//
void UPuzzlePlatformGameInstance::DebugMessage(const FString& Message)
{
	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(-1,
		2,
		FColor::Green,
		Message);
}

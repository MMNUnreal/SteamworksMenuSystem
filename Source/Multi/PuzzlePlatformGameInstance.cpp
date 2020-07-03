// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformGameInstance.h"

#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"

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
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL Subsystem: %s"), *SubSystem->GetSubsystemName().ToString());
	}
}

//** Find all online sessions **//
void UPuzzlePlatformGameInstance::FindSessions()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		//** disable for steam **//
		//SessionSearch->bIsLanQuery = true;

		//* For steam **//
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Starting find sessions"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

//** On finding sessions completed **//
void UPuzzlePlatformGameInstance::OnFindSessionComplete(bool bSuccess)
{
	if (bSuccess && SessionSearch.IsValid() && Menu != nullptr)
	{
		TArray<FString> ServerNames;
		UE_LOG(LogTemp, Warning, TEXT("Finished find session"));
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session: %s"), *SearchResult.GetSessionIdStr());
			ServerNames.Add(SearchResult.GetSessionIdStr());
		}

		Menu->PopulateServerList(ServerNames);
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
		SessionSettings.bIsLANMatch = false;
		SessionSettings.NumPublicConnections = 2;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;

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
	else
	{
		if (Menu != nullptr)
		{
			Menu->Teardown();
		}

		DebugMessage(TEXT("Hosting"));

		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return;

		World->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
	}
	
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
void UPuzzlePlatformGameInstance::Join(uint32 IndexIn)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;

	if (Menu != nullptr)
	{
		Menu->Teardown();

	}

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[IndexIn]);
}

//** Do the client travel on successful connection **//
void UPuzzlePlatformGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString JoinMessage = "Joining: " + SessionName.ToString();
	DebugMessage(JoinMessage);

	FString ConnectionInfo;

	bool bConnectSuccess = SessionInterface->GetResolvedConnectString(SessionName, ConnectionInfo);

	if (bConnectSuccess)
	{

		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return;

		APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(World);
		if (!ensure(PlayerController != nullptr)) return;

		PlayerController->ClientTravel(ConnectionInfo, ETravelType::TRAVEL_Absolute);
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get ConnectString"));
	}

	
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

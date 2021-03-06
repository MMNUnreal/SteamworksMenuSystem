// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiMenuGameInstance.h"

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

const static FName SESSION_NAME = TEXT("Game");
const static FName SETTING_GAMENAME = TEXT("SETTING_GAMENAME");
const static FString LOBBY_MAP = "/Game/Maps/L_Lobby?listen";
const static FString MAINMENU_MAP = "/Game/Maps/L_MainMenu";

UMultiMenuGameInstance::UMultiMenuGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuClassBP(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (!ensure(MenuClassBP.Class != nullptr)) return;
	MenuClass = MenuClassBP.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuClassBP(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
	if (!ensure(InGameMenuClassBP.Class != nullptr)) return;
	InGameMenuClass = InGameMenuClassBP.Class;
}

//** Set up online subsystem **//
void UMultiMenuGameInstance::Init()
{
	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();
	if (SubSystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Subsystem: %s"), *SubSystem->GetSubsystemName().ToString());
		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiMenuGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiMenuGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiMenuGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiMenuGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL Subsystem: %s"), *SubSystem->GetSubsystemName().ToString());
	}
}

//** Find all online sessions **//
void UMultiMenuGameInstance::FindSessions()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		//** disable for steam **//
		//SessionSearch->bIsLanQuery = true;

		//* For steam **//
		// As not using private app ID, but shared app ID so need to have high search results
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		UE_LOG(LogTemp, Warning, TEXT("Starting find sessions"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

//** On finding sessions completed **//
void UMultiMenuGameInstance::OnFindSessionComplete(bool bSuccess)
{
	if (bSuccess && SessionSearch.IsValid() && Menu != nullptr)
	{
		TArray<FServerData> ServerNames;
		UE_LOG(LogTemp, Warning, TEXT("Finished find session"));
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session: %s"), *SearchResult.GetSessionIdStr());
			
			FServerData Data;

			Data.HostUserName = SearchResult.Session.OwningUserName;
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			FString ServerName;
			if (SearchResult.Session.SessionSettings.Get(SETTING_GAMENAME, ServerName))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Data found in settings: %s"), *ServerName);
				Data.Name = ServerName;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No data found in settings"));
				Data.Name = "Could not find name";
			}

			ServerNames.Add(Data);
		}

		Menu->PopulateServerList(ServerNames);
	}
}

//** Host server **//
void UMultiMenuGameInstance::Host(FString ServerNameIn)
{
	DesiredServerName = ServerNameIn;

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
void UMultiMenuGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		// If using NULL subsystem allow for LAN use, else false for steam
		if(IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			SessionSettings.bIsLANMatch = false;
		} 

		SessionSettings.NumPublicConnections = 5;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(SETTING_GAMENAME, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);


		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

//** On session completed, travel to main map **//
void UMultiMenuGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
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

		World->ServerTravel(LOBBY_MAP);
	}
	
}

//** Destroy session - once completed create a new session **//
void UMultiMenuGameInstance::OnDestroySessionComplete(FName SessionName, bool bSuccess)
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
void UMultiMenuGameInstance::Join(uint32 IndexIn)
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
void UMultiMenuGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
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

// ** Start session, called from game mode **//
void UMultiMenuGameInstance::StartSession()
{
	if(SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);
	}
}

//** Add Menu Widget to viewport **//
void UMultiMenuGameInstance::LoadMenu()
{
	if(!ensure(MenuClass != nullptr)) return;
	Menu = CreateWidget<UMainMenu>(this, MenuClass);

	if(!ensure(Menu != nullptr)) return;
	Menu->Setup();
	Menu->SetMenuInterface(this);
}

//** Add In Game Menu Widget to viewport **//
void UMultiMenuGameInstance::LoadInGameMenu()
{
	if (!ensure(InGameMenuClass != nullptr)) return;
	InGameMenu = CreateWidget<UMenuWidget>(this, InGameMenuClass);

	if (!ensure(InGameMenu != nullptr)) return;
	InGameMenu->Setup();
	InGameMenu->SetMenuInterface(this);
}

//** When loading main menu after exiting in game menu
void UMultiMenuGameInstance::LoadMainMenu()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(World);
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(MAINMENU_MAP, ETravelType::TRAVEL_Absolute);
}

//** Debugging message to log to screen **//
void UMultiMenuGameInstance::DebugMessage(const FString& Message)
{
	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(-1,
		2,
		FColor::Green,
		Message);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "TimerManager.h"
#include "PuzzlePlatformGameInstance.h"

const static FString MAIN_MAP = "/Game/Maps/L_GameMap";

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//PlayerControllerList.Add(NewPlayer);
	++NumberOfPlayers;

	if(NumberOfPlayers >= ThresholdPlayers)
	{
		// ** Start timer **//
		UE_LOG(LogTemp, Warning, TEXT("Reached 2 players, starting session in 4 seconds"));
		UWorld* World = GetWorld();

		if (!ensure(World != nullptr)) return;
		GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ALobbyGameMode::LoadMainGame, MaxWaitTime, false);
	}
}

void ALobbyGameMode::LoadMainGame()
{
	auto GameInstance = Cast<UPuzzlePlatformGameInstance>(GetGameInstance());

	if(GameInstance == nullptr) return;
	GameInstance->StartSession();

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	bUseSeamlessTravel = true;
	World->ServerTravel(MAIN_MAP);
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	//PlayerControllerList.Remove(Exiting);
	--NumberOfPlayers;
}

void ALobbyGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Ensure the game timer is cleared by using the timer handle
	GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);

	// Alternatively you can clear ALL timers that belong to this (Actor) instance.
	// GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

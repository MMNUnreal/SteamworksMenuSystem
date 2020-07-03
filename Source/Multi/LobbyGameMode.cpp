// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

const static FString MAIN_MAP = "/Game/Maps/L_GameMap";

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//PlayerControllerList.Add(NewPlayer);
	++NumberOfPlayers;

	if(NumberOfPlayers >= 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reached 2 players"));
		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return;

		bUseSeamlessTravel = true;
		World->ServerTravel(MAIN_MAP);
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	//PlayerControllerList.Remove(Exiting);
	--NumberOfPlayers;
}

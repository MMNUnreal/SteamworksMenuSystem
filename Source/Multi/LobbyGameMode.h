// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTI_API ALobbyGameMode : public AMultiGameMode
{
	GENERATED_BODY()

public:
	//virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//** Count player number, start timer when threshold reached **//
	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController* Exiting) override;

private:
	// ** Current number of player controllers list **//
	TArray<class APlayerController*> PlayerControllerList;

	// ** Current number of players **//
	uint32 NumberOfPlayers = 0;

	// ** Current number of players
	uint32 ThresholdPlayers = 2;

	// ** Load game timer handle **//
	FTimerHandle GameTimerHandle;

	// ** Max seconds to wait once threshold players reached **//
	uint32 MaxWaitTime = 4;

	//** Load game map after max wait time when threshold players reached **//
	UFUNCTION()
	void LoadMainGame();

	
};

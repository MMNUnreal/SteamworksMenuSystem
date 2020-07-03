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


	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController* Exiting) override;

private:
	TArray<class APlayerController*> PlayerControllerList;

	uint32 NumberOfPlayers = 0;
	
};

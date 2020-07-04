// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "MainMenu.h"

void UServerRow::SetServerName(const FString& ServerNameIn)
{
	ServerName->SetText(FText::FromString(ServerNameIn));
}

void UServerRow::SetHostUserName(const FString& HostNameIn)
{
	HostUserName->SetText(FText::FromString(HostNameIn));
}

void UServerRow::SetNumberPlayers(const FString& NumberPlayersIn)
{
	NumberPlayers->SetText(FText::FromString(NumberPlayersIn));
}

void UServerRow::Setup(UMainMenu* ParentIn, uint32 IndexIn)
{
	Parent = ParentIn;
	Index = IndexIn;
	if (!ensure(RowButton != nullptr)) return;
	RowButton->OnClicked.AddDynamic(this, &UServerRow::OnRowClicked);

	if(!ensure(HostUserName != nullptr)) return;
	//HostUserName = Parent-

	if (!ensure(NumberPlayers != nullptr)) return;
}

void UServerRow::OnRowClicked()
{
	Parent->SelectIndex(Index);
}

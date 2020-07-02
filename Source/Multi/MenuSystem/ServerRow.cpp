// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "MainMenu.h"

void UServerRow::SetServerName(const FString& ServerNameIn)
{
	ServerName->SetText(FText::FromString(ServerNameIn));
}

void UServerRow::Setup(UMainMenu* ParentIn, uint32 IndexIn)
{
	Parent = ParentIn;
	Index = IndexIn;
	if (!ensure(RowButton != nullptr)) return;
	RowButton->OnClicked.AddDynamic(this, &UServerRow::OnRowClicked);
}

void UServerRow::OnRowClicked()
{
	
}

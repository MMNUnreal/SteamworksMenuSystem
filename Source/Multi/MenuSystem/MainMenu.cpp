// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/PanelWidget.h"
#include "ServerRow.h"
#include "UObject/ConstructorHelpers.h"
#include "GenericPlatform/GenericPlatformMisc.h"

//** Set up server row bp class **//
UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	ServerRowClass = ServerRowBPClass.Class;
}

//** Set up bindings on widgets **//
bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(HostButton != nullptr)) return false;
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(JoinButton != nullptr)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(BackButton != nullptr)) return false;
	BackButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(JoinGameButton != nullptr)) return false;
	JoinGameButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if (!ensure(ExitApplicationButton != nullptr)) return false;
	ExitApplicationButton->OnClicked.AddDynamic(this, &UMainMenu::ExitApplication);

	return true;
}

//** Host the game **//
void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		MenuInterface->Host();
	}
}

//** Opens join server menu and refresh server list **//
void UMainMenu::OpenJoinMenu()
{
	SwitchMenu(MenuSwitcher, JoinMenu);
	if(MenuInterface != nullptr)
	{
		MenuInterface->FindSessions();
	}
}

//** Opens main menu **//
void UMainMenu::OpenMainMenu()
{
	SwitchMenu(MenuSwitcher, MainMenu);
}

//** Switches between widgets in main menu widget **//
void UMainMenu::SwitchMenu(UWidgetSwitcher* WidgetSwitcher, UWidget* MenuToSwitchTo)
{
	if (!ensure(WidgetSwitcher != nullptr)) return;
	if (!ensure(MenuToSwitchTo != nullptr)) return;
	WidgetSwitcher->SetActiveWidget(MenuToSwitchTo);
}

//**Populate Server **//
void UMainMenu::PopulateServerList(TArray<FString> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	ServerList->ClearChildren();

	uint32 i =0;
	for (const FString& servername : ServerNames)
	{
		if (!ensure(ServerRowClass != nullptr)) return;
		UServerRow* ServerRow = CreateWidget<UServerRow>(World, ServerRowClass);
		

		if (!ensure(ServerRow != nullptr)) return;
		ServerRow->SetServerName(servername);
		ServerRow->Setup(this, i);
		++i;

		ServerList->AddChild(ServerRow);
	}
}

//** Set the selected server index **//
void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
}

//** Calls join server on interface **//
void UMainMenu::JoinServer()
{
	if(SelectedIndex.IsSet())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index: %d"), SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index not set"));
	}

	if (MenuInterface != nullptr)
	{
		/*if (!ensure(IPAddressField != nullptr)) return;
		const FString& IPAddress = IPAddressField->GetText().ToString();*/
		MenuInterface->Join("");
	}
}

//** Quits the game **//
void UMainMenu::ExitApplication()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ConsoleCommand("quit");
}
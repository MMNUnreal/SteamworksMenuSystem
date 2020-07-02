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

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	ServerRowClass = ServerRowBPClass.Class;
}

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

void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		MenuInterface->Host();
	}
}

void UMainMenu::OpenJoinMenu()
{
	SwitchMenu(MenuSwitcher, JoinMenu);
	if(MenuInterface != nullptr)
	{
		MenuInterface->FindSessions();
	}
}

void UMainMenu::OpenMainMenu()
{
	SwitchMenu(MenuSwitcher, MainMenu);
}

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

	for (const FString& servername : ServerNames)
	{
		if (!ensure(ServerRowClass != nullptr)) return;
		UServerRow* ServerRow = CreateWidget<UServerRow>(World, ServerRowClass);

		if (!ensure(ServerRow != nullptr)) return;
		ServerList->AddChild(ServerRow);
		ServerRow->SetServerName(servername);
	}
}


void UMainMenu::JoinServer()
{
	if (MenuInterface != nullptr)
	{
		/*if (!ensure(IPAddressField != nullptr)) return;
		const FString& IPAddress = IPAddressField->GetText().ToString();*/
		MenuInterface->Join("");
	}
}

void UMainMenu::ExitApplication()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ConsoleCommand("quit");
}
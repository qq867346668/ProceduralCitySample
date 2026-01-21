// Fill out your copyright notice in the Description page of Project Settings.

#include "PasswordLockComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

// Sets default values for this component's properties
UPasswordLockComponent::UPasswordLockComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UPasswordLockComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UPasswordLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (LockUI)
	{
		auto textWidget = Cast<UTextBlock>(LockUI->GetWidgetFromName(TEXT("InputPassword")));
		if (textWidget && textWidget->GetVisibility() == ESlateVisibility::Visible)
		{
			auto &textContent = textWidget->GetText().ToString();
			if (textContent.Len() < Password.Len())
			{
				return;
			}

			if (textContent == Password)
			{
				OnPasswordCorrect();
			}

			TurnOnOrOff(false);
		}
	}
}

void UPasswordLockComponent::TurnOnOrOff(bool turnOn)
{
	auto PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (turnOn)
	{
		LockUI = CreateWidget(PlayerController, UIClass);
		LockUI->AddToViewport();
		FInputModeUIOnly inputMode;
		inputMode.SetWidgetToFocus(LockUI->TakeWidget());
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		PlayerController->SetInputMode(inputMode);
	}
	else
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		LockUI->RemoveFromParent();
		LockUI = nullptr;
	}

	PlayerController->SetShowMouseCursor(turnOn);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PasswordLockComponent.generated.h"


UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCEDURALCITYSAMPLE_API UPasswordLockComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPasswordLockComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
	void TurnOnOrOff(bool turnOn);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPasswordCorrect();

private:
	UPROPERTY(EditAnywhere)
	FString Password;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> UIClass;

	UUserWidget* LockUI = nullptr;
};

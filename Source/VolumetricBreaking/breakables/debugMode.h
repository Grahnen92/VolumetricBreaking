// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "debugMode.generated.h"

/**
 * 
 */
UCLASS()
class VOLUMETRICBREAKING_API AdebugMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AdebugMode(const FObjectInitializer& ObjectInitializer);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitGame(const FString &MapName, const FString &Options, FString &ErrorMessage) override;

protected:
	virtual void Tick(float DeltaSeconds) override;
};

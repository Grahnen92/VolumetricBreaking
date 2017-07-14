// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "debugController.generated.h"

/**
 * 
 */
UCLASS()
class VOLUMETRICBREAKING_API AdebugController : public APlayerController
{
	GENERATED_BODY()
	
	
public:
	//virtual void Tick(float DeltaTime) override;
	AdebugController();
	AdebugController(const class FObjectInitializer& ObjectInitializer);

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
};

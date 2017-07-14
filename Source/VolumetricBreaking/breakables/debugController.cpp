// Fill out your copyright notice in the Description page of Project Settings.

#include "debugController.h"
#include "VolumetricBreaking.h"



AdebugController::AdebugController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AdebugController::BeginPlay()
{
	Super::BeginPlay();

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AdebugController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	//if (IsPaused()) {
	//	this->pause
	//	SetPause(false);
	//}
		
}



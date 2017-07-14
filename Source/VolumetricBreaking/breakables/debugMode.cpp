// Fill out your copyright notice in the Description page of Project Settings.

#include "debugMode.h"
#include "VolumetricBreaking.h"

#include "breakables/debugController.h"


AdebugMode::AdebugMode(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


void AdebugMode::BeginPlay()
{
	Super::BeginPlay();
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


void AdebugMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	//if(MyPlayer->IsPaused())
	//	MyPlayer->SetPause(false);
}

void AdebugMode::InitGame(const FString &MapName, const FString &Options, FString &ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	bPauseable = true;
	PlayerControllerClass = AdebugController::StaticClass();
}
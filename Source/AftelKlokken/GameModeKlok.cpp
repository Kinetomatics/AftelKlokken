// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeKlok.h"
#include "Kismet/GameplayStatics.h"

AGameModeKlok::AGameModeKlok()
{
}

void AGameModeKlok::BeginPlay()
{
	//Player controller opvragen en manipuleren (muis voor controle)
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	//Server check niet nodig mits game mode enkel op de server loopt (extra controle)
	if (IsValid(PC) && PC->HasAuthority())
	{
		PC->bEnableClickEvents = true;
		PC->bShowMouseCursor = true;
	}
}

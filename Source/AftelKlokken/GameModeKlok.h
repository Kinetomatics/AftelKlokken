// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeKlok.generated.h"

/**
 * 
 */
UCLASS()
class AFTELKLOKKEN_API AGameModeKlok : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGameModeKlok();

protected:

	virtual void BeginPlay() override;
};

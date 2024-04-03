// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "AftelKlok.generated.h"

UCLASS()
class AFTELKLOKKEN_API AAftelKlok : public AActor
{
	GENERATED_BODY()
	
public:	

	AAftelKlok();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void OnConstruction(const FTransform& transform) override;
	virtual void BeginPlay() override;
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;


	UFUNCTION(BlueprintCallable)
	void SetTime(bool isTimeUp);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateTimer();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ROS_HandleTimer();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multi_HandleTimer();

	//Klok text component met garbage collection
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTextRenderComponent> T_CountDown;

	//Klok achtergrond component met garbage collection
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> SM_Background;

	//Klok count down rate
	UPROPERTY(BlueprintReadWrite)
	FTimespan mMinus;

	//Klok data timer
	UPROPERTY(BlueprintReadWrite, Replicated)
	FTimespan mCountDown;

	//Klok op 0 variable
	UPROPERTY(BlueprintReadWrite, Replicated)
	bool mTimeIsUp;

	//Klok start tijd
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int32 mTimerStartSeconds;

private:
	//Klok component root met garbage collection
	UPROPERTY()
	TObjectPtr<USceneComponent> uSceneComponent;

	//Timer handle met garbage collection
	UPROPERTY()
	FTimerHandle hKlokTimerHandle;

	//Klok start/stop variable
	bool bStartTimer;
};


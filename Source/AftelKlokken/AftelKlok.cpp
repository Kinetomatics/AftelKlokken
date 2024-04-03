// Fill out your copyright notice in the Description page of Project Settings.


#include "AftelKlok.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetTextLibrary.h"

// Sets default values
AAftelKlok::AAftelKlok() : mCountDown(), mMinus(), mTimeIsUp(false), mTimerStartSeconds(0), bStartTimer(false)
{
    //Basis settings
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

    //Root compoment voor blueprint
    uSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    SetRootComponent(uSceneComponent);

    //Achtergrond compoment van klok koppelen aan root component voor blueprint
    SM_Background = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneComponent"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
    if (PlaneMesh.Succeeded())
    {
        SM_Background->SetStaticMesh(PlaneMesh.Object);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find plane mesh"));
    }
    SM_Background->SetupAttachment(uSceneComponent);

    //text compoment van klok koppelen aan root component voor blueprint
    T_CountDown = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
    T_CountDown->SetupAttachment(uSceneComponent);
}

void AAftelKlok::BeginPlay()
{
    //Default gedrag
	Super::BeginPlay();

    //Init
    mCountDown = FTimespan::FTimespan(0,0, mTimerStartSeconds);

    //Koppel timer voor klok aan server event per seconde
    GetWorld()->GetTimerManager().SetTimer(hKlokTimerHandle,this,&AAftelKlok::UpdateTimer_Implementation,1,true,0); 
}

void AAftelKlok::OnConstruction(const FTransform& transform)
{
    //Zorgt ervoor dat het construct script kan worden aangepast in de blueprint
    Super::OnConstruction(transform);
}

void AAftelKlok::Tick(float DeltaTime)
{
    //Default gedrag
	Super::Tick(DeltaTime);
}

//Nodig voor de variable die replicated zijn
void AAftelKlok::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAftelKlok, mCountDown);
	DOREPLIFETIME(AAftelKlok, mTimeIsUp);
}

//Server event call
void AAftelKlok::UpdateTimer_Implementation()
{
    this->ROS_HandleTimer_Implementation();
}

//Timer aanpassingen enkel via server mogelijk->meer controle
void AAftelKlok::ROS_HandleTimer_Implementation()
{
    //Tel af
    mCountDown = mCountDown - mMinus;
    //GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::White, mCountDown);
    this->Multi_HandleTimer_Implementation();

    //Check om te controleren wanneer de timer op 0 staat en kleur aan te passen
    if (mCountDown.IsZero())
    {
        mTimeIsUp = true;
        T_CountDown->SetTextRenderColor(FColor::Red);

        //Syntactic sugar om snel een functie door te geven aan de timer
        FTimerDelegate TimerDelegateToggleText;
        TimerDelegateToggleText.BindLambda([&]
        {
            T_CountDown->ToggleVisibility();
        });
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegateToggleText, 0.66, true, 0);
    }
}

//Pas de klok timer aan op het relevante display
void AAftelKlok::Multi_HandleTimer_Implementation()
{
    SetTime(mTimeIsUp);
}

//Pas de klok tijd aan met de nodige checks
void AAftelKlok::SetTime(bool isTimeUp)
{
    //Extra check om geen negatieve timing te krijgen
    int32 startIndex = 0;
    if(isTimeUp)
        startIndex = 1;

    //Formateer met leading zeros zonder milliseconden
    float totalseconds = UKismetMathLibrary::GetTotalSeconds(mCountDown);
    FString totalsecondsstring = UKismetStringLibrary::TimeSecondsToString(totalseconds);

    //Lengte moet altijd 5 zijn = '00:00' (min:sec)
    FString totalsecondsstringformatted = UKismetStringLibrary::GetSubstring(totalsecondsstring, startIndex, 5);

    //Pas de klok display aan
    FText totalsecondstext = UKismetTextLibrary::Conv_StringToText(totalsecondsstringformatted);
    T_CountDown->SetText(totalsecondstext);
}

//Start/stop timer bij click event via mMinus 
void AAftelKlok::NotifyActorOnClicked(FKey ButtonPressed)
{
    //Default gedrag
    Super::NotifyActorOnClicked(ButtonPressed);

    //Geen click mogelijk als de tijd op heeft gestaan
    if (mTimeIsUp)
        return;

    //flip/flop
    bStartTimer = !bStartTimer;
    mMinus = bStartTimer ? UKismetMathLibrary::MakeTimespan(0, 0, 0, 1, 0) : UKismetMathLibrary::MakeTimespan(0, 0, 0, 0, 0);
}

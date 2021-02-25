// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // set the collsiondetection to overlapevents only
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	TriggerBox->SetBoxExtent(FVector(64.0f, 64.0f, 32.0f));

	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));
	FloorSwitch->SetupAttachment(GetRootComponent());

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(GetRootComponent());

	bIsPlayerOnSwitch = false;

	SwitchTime = 2.0f;
}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::BeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::EndOverlap);

	InitialDoorLocation = Door->GetComponentLocation();
	InitialFloorSwitchLocation = FloorSwitch->GetComponentLocation();
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("BeginOverlap works fine !"));
	if (!bIsPlayerOnSwitch)
	{
		bIsPlayerOnSwitch = true;
	}
	RaiseDoor();
	LowerFloorSwitch();
}

void AFloorSwitch::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("EndOverlap works fine !"));
	if (bIsPlayerOnSwitch)
	{
		bIsPlayerOnSwitch = false;
	}
	GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFloorSwitch::CloseDoor, SwitchTime);
}

void AFloorSwitch::UpdateDoorMovement(float Z)
{
	FVector NewLocation = InitialDoorLocation;
	NewLocation.Z += Z;
	Door->SetWorldLocation(NewLocation);
}

void AFloorSwitch::UpdateFloorSwitchMovement(float Z)
{
	FVector NewLocation = InitialFloorSwitchLocation;
	NewLocation.Z += Z;
	FloorSwitch->SetWorldLocation(NewLocation);
}

void AFloorSwitch::CloseDoor()
{
	if (!bIsPlayerOnSwitch)
	{
		LowerDoor();
		RaiseFloorSwitch();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainChar.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Agro = CreateDefaultSubobject<USphereComponent>(TEXT("Agro"));
	Agro->SetupAttachment(GetRootComponent());
	Agro->InitSphereRadius(600.0f);

	Combat = CreateDefaultSubobject<USphereComponent>(TEXT("Combat"));
	Combat->SetupAttachment(GetRootComponent());
	Combat->InitSphereRadius(120.0f);

	Chasing = CreateDefaultSubobject<USphereComponent>(TEXT("Chasing"));
	Chasing->SetupAttachment(GetRootComponent());
	Chasing->InitSphereRadius(1000.0f);

	bOverlappingCombatSphere = false;

	Health = 60.0f;
	MaxHealth = 100.0f;
	Damage = 10.0f;
}

void AEnemy::AgroOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainChar* MainChar = Cast<AMainChar>(OtherActor);
		if (MainChar)
		{
			MoveToTarget(MainChar);
		}
	}
}

void AEnemy::AgroOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainChar* MainChar = Cast<AMainChar>(OtherActor);
		if (MainChar)
		{
			CombatTarget = MainChar; // setting the target for the animmationsblueprint
			bOverlappingCombatSphere = true;
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainChar* MainChar = Cast<AMainChar>(OtherActor);
		if (MainChar)
		{
			bOverlappingCombatSphere = false;
			if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
			{
				MoveToTarget(MainChar);
				CombatTarget = nullptr;
			} 
		}
	}
}

void AEnemy::ChasingOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainChar* MainChar = Cast<AMainChar>(OtherActor);
		if (MainChar)
		{
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController)
			{
				AIController->StopMovement();
			}
		}
	}
}

void AEnemy::MoveToTarget(AMainChar* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(5.0f);

		FNavPathSharedPtr NavPath;
		AIController->MoveTo(MoveRequest, &NavPath); //MoveTo function needs FAIMoveRequest and FNavPathSharedPtr

		/*
		auto PathPoints = NavPath->GetPathPoints();
		for (auto point : PathPoints)
		{
			FVector Location = point.Location;
			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.0f, 8, FLinearColor::Red, 10.0f, 5.0f); // DebugSphere
		}
		*/
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	AIController = Cast<AAIController>(GetController()); // derives from controller

	// Delegates 
	Agro->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroOnOverlapBegin);
	Agro->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroOnOverlapEnd);

	Combat->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	Combat->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	Chasing->OnComponentEndOverlap.AddDynamic(this, &AEnemy::ChasingOnOverlapEnd);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


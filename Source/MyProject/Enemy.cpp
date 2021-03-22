// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainChar.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Agro"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.0f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Combat"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(120.0f);

	Chasing = CreateDefaultSubobject<USphereComponent>(TEXT("Chasing"));
	Chasing->SetupAttachment(GetRootComponent());
	Chasing->InitSphereRadius(1000.0f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // WorldDynamics can be moved and create Overlapevents
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // First ignore all Channels...
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // ... then set the needed Channel

	bOverlappingCombatSphere = false;

	Health = 60.0f;
	MaxHealth = 100.0f;
	Damage = 10.0f;

	AttackMinTime = 0.5f;
	AttackMaxTime = 2.5f;
}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainChar* MainChar = Cast<AMainChar>(OtherActor); // set otheractor as mainchar to get all its functionalities
		if (MainChar)
		{
			MainChar->SetCombatTarget(this); 

			CombatTarget = MainChar; // setting the target for the animmationsblueprint
			bOverlappingCombatSphere = true;
			Attack();
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainChar* MainChar = Cast<AMainChar>(OtherActor);
		if (MainChar)
		{
			MainChar->SetCombatTarget(nullptr);
			bOverlappingCombatSphere = false;
			if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
			{
				MoveToTarget(MainChar);
				CombatTarget = nullptr;
			} 
			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) // checks the actor we collided with
	{
		AMainChar* MainChar = Cast<AMainChar>(OtherActor); // cast the actor to an enemy type so he will be declared as enemy and get its specific values and functions
		if (MainChar)
		{
			if (MainChar->HitParticles)
			{
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				if (TipSocket)
				{
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MainChar->HitParticles, SocketLocation, FRotator(0.0f), false);
				}
			}
			if (MainChar->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, MainChar->HitSound);
			}
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

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
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

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

void AEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}
}

void AEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::AttackEnd()
{
	bAttacking = false;
	if (bOverlappingCombatSphere)
	{
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	}
}

void AEnemy::Attack()
{
	if (AIController)
	{
		AIController->StopMovement();
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
	}
	if (!bAttacking)
	{
		bAttacking = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.5f);
			AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
		}
	}
}


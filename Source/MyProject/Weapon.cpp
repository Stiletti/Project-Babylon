// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainChar.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemy.h"

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // WorldDynamics can be moved and create Overlapevents
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // First ignore all Channels...
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // ... then set the needed Channel

	bWeaponParticles = false;

	bIsRightHand = true;

	Damage = 20.0f;

	WeaponState = EWeaponState::EWS_PickUp;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if ((WeaponState == EWeaponState::EWS_PickUp) && OtherActor)
	{
		AMainChar* MainChar = Cast<AMainChar>(OtherActor);
		if (MainChar)
		{
			MainChar->SetActiveOverlappingItem(this); // this means the actual overlapping item the player is facing
		}
	}
	
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor)
	{
		AMainChar* MainChar = Cast<AMainChar>(OtherActor);
		if (MainChar)
		{
			MainChar->SetActiveOverlappingItem(nullptr); // clear the overlappingitem slot for next overlapping action
		}
	}
}

void AWeapon::Equip(AMainChar* MainChar)
{
	if (MainChar)
	{
		// need to set collisionchannels for pawn and camera to ignore to avoid cameraissues
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		// if the character is simulating physics we have to disable it or the weapon is kinda glitching
		SkeletalMesh->SetSimulatePhysics(false);

		// find the socket to attach the weapon to it
		const USkeletalMeshSocket* RightHandSocket = MainChar->GetMesh()->GetSocketByName("RightHandSocket");
		const USkeletalMeshSocket* LeftHandSocket = MainChar->GetMesh()->GetSocketByName("LeftHandSocket");
		if (bIsRightHand)
		{
			if (RightHandSocket)
			{
				RightHandSocket->AttachActor(this, MainChar->GetMesh());
				bSetRotation = false;

				MainChar->SetEquippedWeapon(this); // sets particular weapon instance for each weapon
				MainChar->SetActiveOverlappingItem(nullptr);
			}
		}
		if (!bIsRightHand)
		{
			if (LeftHandSocket)
			{
				LeftHandSocket->AttachActor(this, MainChar->GetMesh());
				bSetRotation = false;

				MainChar->SetEquippedWeapon(this); // sets particular weapon instance for each weapon
				MainChar->SetActiveOverlappingItem(nullptr);
			}
		}
		if (EquipSound)
		{
			UGameplayStatics::PlaySound2D(this, EquipSound);
		}
		if (!bWeaponParticles)
		{
			IdleParticleSystem->Deactivate();
		}
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) // checks the actor we collided with
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor); // cast the actor to an enemy type so he will be declared as enemy and get its specific values and functions
		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket"); 
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.0f), false);
				}
			}
		}
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

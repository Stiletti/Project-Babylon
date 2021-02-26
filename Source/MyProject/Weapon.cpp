// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainChar.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	bWeaponParticles = false;

	bIsRightHand = true;

	WeaponState = EWeaponState::EWS_PickUp;
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

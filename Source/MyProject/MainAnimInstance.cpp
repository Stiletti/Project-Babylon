// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainChar.h"

void UMainAnimInstance::NativeInitializeAnimation() // equivalent to BeginPlay for Animation Blueprints
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();

		if (Pawn)
		{
			MainChar = Cast<AMainChar>(Pawn);

		}
	}
}

void UMainAnimInstance::UpdateAnimProps()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f); // LateralSpeed just take care for the Yawvalues
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if (MainChar == nullptr)
		{
			MainChar = Cast<AMainChar>(Pawn);
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderMovementComponent.h"

void UColliderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	FVector ForceMovementPerFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * /*Speed*/ 150.0f;

	if (!ForceMovementPerFrame.IsNearlyZero())
	{
		FHitResult HitResult;
		SafeMoveUpdatedComponent(ForceMovementPerFrame, UpdatedComponent->GetComponentRotation(), true, HitResult);

		// if we collide with something, slide along the side of it
		if (HitResult.IsValidBlockingHit())
		{
			SlideAlongSurface(ForceMovementPerFrame, 1.0f - HitResult.Time, HitResult.Normal, HitResult);
		}
	}
}
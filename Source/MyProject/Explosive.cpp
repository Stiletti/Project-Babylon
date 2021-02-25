// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "MainChar.h"

AExplosive::AExplosive()
{
	Damage = 10.0f;
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	UE_LOG(LogTemp, Warning, TEXT("Explosive::OverlapBegin fired !"));

	if (OtherActor)
	{
		AMainChar* MainChar = Cast<AMainChar>(OtherActor); // if OtherActor isnt type AMainChar its 0
		if (MainChar)
		{
			MainChar->DecrementHealth(Damage);
			Destroy();
		}
	}
}

void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	UE_LOG(LogTemp, Warning, TEXT("Explosive::OverlapEnd fired !"));
}

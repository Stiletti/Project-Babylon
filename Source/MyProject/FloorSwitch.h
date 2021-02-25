// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class MYPROJECT_API AFloorSwitch : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFloorSwitch();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
		class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
		class UStaticMeshComponent* FloorSwitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
		UStaticMeshComponent* Door;

	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
		FVector InitialDoorLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
		FVector InitialFloorSwitchLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Switch")
		float SwitchTime;

	FTimerHandle SwitchHandle;

	bool bIsPlayerOnSwitch;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
		void RaiseDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
		void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
		void RaiseFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
		void LowerFloorSwitch();

	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
		void UpdateDoorMovement(float Z);

	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
		void UpdateFloorSwitchMovement(float Z);

	void CloseDoor();
};

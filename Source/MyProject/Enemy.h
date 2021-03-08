// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking UMETA(DisplayName = "Attacking"),

	EMS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class MYPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// EnemyMovementStatus Function Setup
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
		EEnemyMovementStatus EnemyMovementStatus;

	// Enemy HitSound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class USoundCue* HitSound;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }

	// creating collisionzones for agro and combatfunctions
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		class USphereComponent* Agro;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		USphereComponent* Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		USphereComponent* Chasing;

	// creating AIController
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		class AAIController* AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		bool bOverlappingCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		AMainChar* CombatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float Damage;

	// OverlapEvents
	UFUNCTION()
		virtual void AgroOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void AgroOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void ChasingOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void MoveToTarget(class AMainChar* Target);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

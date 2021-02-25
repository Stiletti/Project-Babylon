// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainChar.generated.h"

UENUM(BlueprintType) // get access to the blueprint system for enums
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"), // if UENUM() is used u need to declare the enumsnames with UMETA()
	EMS_Sprint UMETA(DisplayName = "Sprinting"),
	
	EMS_MAX UMETA(Displayname = "DefaultMAX") // often used as last enum
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "Minimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(Displayname = "DefaultMAX")
};

UCLASS()
class MYPROJECT_API AMainChar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainChar();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate;

	bool bShiftKeyDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Items")
	class AItem* ActiveOverlappingItem;

	bool bPickUpKeyDown;

	// Player Stats

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
		float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
		float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
		float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
		float CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coins")
		int32 Coins;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coins")
		int32 MaxCoins;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float NormalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handles Character movement Forwards/Backwards
	void MoveForward(float Value);

	// Handles Character movement Left/Right
	void MoveRight(float Value);

	// calles via input at given rate
	// - param range 1.0 means 100%
	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	void DecrementHealth(float Amount);

	void Die();

	void IncrementCoinCount(int32 Amount);

	// Set Movementstatus and Runningspeed
	void SetMovementStatus(EMovementStatus Status);

	// for sprinting
	void ShiftKeyDown();

	void ShiftKeyUp();

	// Set Pickupfunctions
	void PickUpKeyDown();
	void PickUpKeyUp();

	// Cameraattachment
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	void SetEquippedWeapon(AWeapon* WeaponToSet);

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }
};

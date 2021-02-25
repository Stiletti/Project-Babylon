// Fill out your copyright notice in the Description page of Project Settings.


#include "MainChar.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"

// Sets default values
AMainChar::AMainChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create CameraBoom 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 500.0f; // Armlength 
	CameraBoom->bUsePawnControlRotation = true; // rotate Arm based on Controller

	// Create FollowCamera attached to CameraBoom
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

	// Attach the FollowCamera at the End of the CameraBoom and let the CameraBoom handle the Rotation
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	BaseTurnRate = 3.0f;
	BaseLookUpRate = 3.0f;

	// Dont Rotate when the PlayerController rotates
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure Charactermovement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the Direction of Input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f); // ... at this Rotationrate
	GetCharacterMovement()->JumpZVelocity = 400.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Player Stats

	MaxHealth = 100.0f;
	CurrentHealth = 65.0f;

	MaxStamina = 250.0f;
	CurrentStamina = 90.0f;

	Coins = 0;

	bShiftKeyDown = false;
	bPickUpKeyDown = false;

	// Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 15.0f;
	MinSprintStamina = 50.0f;

	NormalSpeed = 400.0f;
	SprintSpeed = 650.0f;
}

// Called when the game starts or when spawned
void AMainChar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Stamina updating each Frame
	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown) // if Shiftkey is pressed
		{
			if (CurrentStamina - DeltaStamina <= MinSprintStamina) // if Stamina while draining goes into Minimumzone 
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				CurrentStamina -= DeltaStamina; // draining Stamina
			}
			else
			{
				CurrentStamina -= DeltaStamina;	// while we arent going into Minimumzone just drain Stamina in Normal State
			}
			SetMovementStatus(EMovementStatus::EMS_Sprint);
		}
		else // if Shiftkey isnt pressed
		{
			if (CurrentStamina + DeltaStamina >= MaxStamina)
			{
				CurrentStamina = MaxStamina;
			}
			else
			{
				CurrentStamina += DeltaStamina; // recovering Stamina
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown) // if Shiftkey is pressed
		{
			if (CurrentStamina - DeltaStamina <= 0.0f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				CurrentStamina = 0.0f;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				CurrentStamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprint);
			}
		}
		else // if Shiftkey isnt pressed
		{
			if (CurrentStamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				CurrentStamina += DeltaStamina;
			}
			else
			{
				CurrentStamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			CurrentStamina = 0.0f;
		}
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			CurrentStamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (CurrentStamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			CurrentStamina += DeltaStamina;
		}
		else
		{
			CurrentStamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	default:
		; // nothing to do here
	}
}

// Called to bind functionality to input
void AMainChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainChar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainChar::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMainChar::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainChar::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainChar::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainChar::ShiftKeyUp);

	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AMainChar::PickUpKeyDown);
	PlayerInputComponent->BindAction("PickUp", IE_Released, this, &AMainChar::PickUpKeyUp);
}

void AMainChar::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMainChar::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMainChar::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainChar::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainChar::DecrementHealth(float Amount)
{
	if (CurrentHealth - Amount <= 0.0f)
	{
		CurrentHealth -= Amount;
		Die();
	}
	else
	{
		CurrentHealth -= Amount;
	}
}

void AMainChar::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Player died !"));
}

void AMainChar::IncrementCoinCount(int32 Amount)
{
	Coins += Amount;
}

void AMainChar::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void AMainChar::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMainChar::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMainChar::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy(); // if a weapon is already attached, destroy it to get the new one
	}

	EquippedWeapon = WeaponToSet;
}

void AMainChar::PickUpKeyDown()
{
	bPickUpKeyDown = true;
	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this); // sends the weapon to SetOverlappingItem() in Weapon.h and attach the actual instance to the player
			SetActiveOverlappingItem(nullptr); // setting to nullptr after equip the weapon so the overlapping mechanic works without trbl
		}
	}
}

void AMainChar::PickUpKeyUp()
{
	bPickUpKeyDown = false;
}

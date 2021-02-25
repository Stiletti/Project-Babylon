// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ColliderMovementComponent.h"

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// CollisionObject
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArmComponent->TargetArmLength = 500.0f;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 3.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	OurMovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("OurMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	CameraInput = FVector2D(0.0f, 0.0f);
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Left / Right
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += CameraInput.X;
	SetActorRotation(NewRotation);

	// UP / Down
	FRotator NewSpringArmRotation = SpringArmComponent->GetComponentRotation();
	NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch += CameraInput.Y, -80.0f, -15.0f);
	SpringArmComponent->SetWorldRotation(NewSpringArmRotation);
}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACollider::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACollider::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &ACollider::PitchCamera);
	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &ACollider::YawCamera);
}

void ACollider::MoveForward(float Value)
{
	FVector Forward = GetActorForwardVector();
	if (OurMovementComponent)
	{
		OurMovementComponent->AddInputVector(Value * Forward, true);
	}
}

void ACollider::MoveRight(float Value)
{
	FVector Right = GetActorRightVector();
	if (OurMovementComponent)
	{
		OurMovementComponent->AddInputVector(Value * Right, true);
	}
}

void ACollider::PitchCamera(float Value)
{
	CameraInput.Y = Value;
}

void ACollider::YawCamera(float Value)
{
	CameraInput.X = Value;
}

UPawnMovementComponent* ACollider::GetMovementComponent() const
{
	return OurMovementComponent;
}
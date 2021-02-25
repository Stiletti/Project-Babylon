// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	// Reference to the UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadwrite,Category = "Widget")
		TSubclassOf<class UUserWidget> HUDOverlayAsset;

	// Variable to hold the Widget after creating it
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Widget")
		UUserWidget* HUDOverlay;

protected:
	virtual void BeginPlay() override;
};

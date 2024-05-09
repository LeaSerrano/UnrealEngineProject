// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "InteractableLevitating.generated.h"

UCLASS(config = Game)
class UNREALENGINEPROJECT_API UInteractableLevitating : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", meta = (AllowPrivateAccess = "true"))
	UClass* firefliesBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* levitatedMaterial;

public:	
	// Sets default values for this component's properties
	UInteractableLevitating();

	void setLevitatingActorMaterial();
	void getBackActorMaterial();

	void EnableMove();
	void DisableMove();

	bool isLevitatingModeEnable = false;
	bool isLevitatingModeDisable = false;
	bool isObjectOnTheGround = false;
	bool shouldAddImpulse = false;
	float levitationStartTime;
	bool isAnimationMovingUp = true;

	bool areFirefliesSpawned = false;
	AActor* firefliesActor;
	//UClass* firefliesBlueprintClass;

	AActor* ownerActor;
	TArray<UMaterialInterface*> materialSave;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};

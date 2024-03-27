// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableLevitating.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALENGINEPROJECT_API UInteractableLevitating : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractableLevitating();

	void setLevitatingActorMaterial(UMaterialInterface* newMaterial, TArray<UMaterialInterface*>& materialSave);
	void getBackActorMaterial(TArray<UMaterialInterface*> materialSave);

	bool isOnBottomCollision();
	void EnableMove();
	void DisableMove();

	bool isLevitatingModeDisable = false;
	bool isObjectOnTheGround = false;
	bool shouldAddImpulse = false;
	float levitationStartTime;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

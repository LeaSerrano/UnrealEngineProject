// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableLevitating.h"

// Sets default values for this component's properties
UInteractableLevitating::UInteractableLevitating()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractableLevitating::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInteractableLevitating::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractableLevitating::setLevitatingActorMaterial(UMaterialInterface* newMaterial, TArray<UMaterialInterface*>& materialSave)
{
	UStaticMeshComponent* meshComponent = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (meshComponent)
	{
		const TArray<UMaterialInterface*>& materials = meshComponent->GetMaterials();
		materialSave.Reserve(materials.Num());

		for (int32 i = 0; i < materials.Num(); ++i)
		{
			materialSave.Push(materials[i]);

			meshComponent->SetMaterial(i, newMaterial);
		}
	}
}

void UInteractableLevitating::getBackActorMaterial(TArray<UMaterialInterface*> materialSave) {

	UStaticMeshComponent* meshComponent = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (meshComponent)
	{
		for (int32 i = 0; i < materialSave.Num(); ++i)
		{
			meshComponent->SetMaterial(i, materialSave[i]);
		}
	}
}


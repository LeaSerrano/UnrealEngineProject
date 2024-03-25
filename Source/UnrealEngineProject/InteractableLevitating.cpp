// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableLevitating.h"

// Sets default values for this component's properties
UInteractableLevitating::UInteractableLevitating()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UInteractableLevitating::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UInteractableLevitating::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (isLevitatingModeDisable) {

		EnableMove();
		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
		PrimitiveComponent->SetSimulatePhysics(true);

		bool isCollision = isOnBottomCollision();

		if (isCollision) {
			PrimitiveComponent->SetSimulatePhysics(false);
			DisableMove();
			isLevitatingModeDisable = false;
		}
	}
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

bool UInteractableLevitating::isOnBottomCollision() {

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return false;
	}

	FVector ActorLocation = OwnerActor->GetActorLocation();

	FVector Start = ActorLocation;
	FVector End = ActorLocation - FVector(0, 0, 10);

	FHitResult HitResult;

	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;
	CollisionParams.bReturnPhysicalMaterial = false;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			return true;
		}
	}

	return false;
}

void UInteractableLevitating::EnableMove()
{
	AActor* actor = GetOwner();

	if (actor)
	{
		TArray<UStaticMeshComponent*> staticMeshComponents;
		actor->GetComponents<UStaticMeshComponent>(staticMeshComponents);

		for (UStaticMeshComponent* staticMeshComponent : staticMeshComponents)
		{
			if (staticMeshComponent)
			{
				staticMeshComponent->SetMobility(EComponentMobility::Movable);
			}
		}
	}
}

void UInteractableLevitating::DisableMove()
{
	AActor* actor = GetOwner();

	if (actor)
	{
		TArray<UStaticMeshComponent*> staticMeshComponents;
		actor->GetComponents<UStaticMeshComponent>(staticMeshComponents);

		for (UStaticMeshComponent* staticMeshComponent : staticMeshComponents)
		{
			if (staticMeshComponent)
			{
				staticMeshComponent->SetMobility(EComponentMobility::Static);
			}
		}
	}
}


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

	firefliesBlueprintClass = LoadClass<AActor>(nullptr, TEXT("/Script/Engine.Blueprint'/Game/Particles_Wind_Control_System/Blueprints/BP_Particle_Fireflies_2.BP_Particle_Fireflies_2_C'"));
	ownerActor = GetOwner();
	USceneComponent* ComponentToAttach = ownerActor->GetRootComponent();
}

// Called every frame
void UInteractableLevitating::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (isLevitatingModeDisable) 
	{
		isLevitatingModeEnable = false;

		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ownerActor->GetRootComponent());

		if (!shouldAddImpulse) 
		{
			EnableMove();

			PrimitiveComponent->SetSimulatePhysics(true);

			FVector DownwardImpulse = FVector(0.0f, 0.0f, -1.0f) * 20;
			PrimitiveComponent->AddImpulse(DownwardImpulse, NAME_None, true);

			shouldAddImpulse = true;

			levitationStartTime = GetWorld()->GetTimeSeconds();
		}

		float elapsedTime = GetWorld()->GetTimeSeconds() - levitationStartTime;
		const float levitationDurationThreshold = 1.0f;
		const float epsilon = 0.1f;

		if (elapsedTime > levitationDurationThreshold) 
		{
			float speed = ownerActor->GetVelocity().Size();

			if (speed < epsilon) 
			{
				PrimitiveComponent->SetSimulatePhysics(false);
				DisableMove();
				isLevitatingModeDisable = false;

				if (firefliesActor)
				{
					areFirefliesSpawned = false;
					firefliesActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
					firefliesActor->Destroy();
				}
			}
		}
	}
	
	if (isLevitatingModeEnable) 
	{
		EnableMove();

		FVector newLocation = ownerActor->GetActorLocation();
		FRotator newRotation = ownerActor->GetActorRotation();
		float runningTime = ownerActor->GetGameTimeSinceCreation();

		float deltaHeight = (FMath::Sin(runningTime + DeltaTime) - FMath::Sin(runningTime));
		newLocation.Z += deltaHeight * 10.0f;
		
		float deltaRotationRoll = FMath::Cos(runningTime + DeltaTime/4.0) - FMath::Cos(runningTime + DeltaTime);
		newRotation.Roll += deltaRotationRoll * 10.0f;

		float deltaRotationYaw = (FMath::Sin(runningTime + DeltaTime) - FMath::Sin(runningTime - DeltaTime));
		newRotation.Yaw += deltaRotationYaw * 5.0f;
		
		ownerActor->SetActorLocationAndRotation(newLocation, newRotation, true);


		if (!areFirefliesSpawned) 
		{
			if (firefliesBlueprintClass)
			{
				firefliesActor = GetWorld()->SpawnActor<AActor>(firefliesBlueprintClass);
			}

			if (firefliesActor)
			{
				FVector ActorLocation = ownerActor->GetActorLocation();
				firefliesActor->SetActorLocation(ActorLocation);

				FAttachmentTransformRules AttachRules = FAttachmentTransformRules(
					EAttachmentRule::KeepWorld,
					EAttachmentRule::KeepWorld,
					EAttachmentRule::KeepWorld,
					true
				);

				firefliesActor->AttachToActor(ownerActor, AttachRules);
			}
			areFirefliesSpawned = true;
		}
	}
}

void UInteractableLevitating::setLevitatingActorMaterial()
{
	UMaterialInterface* levitatedMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Fantastic_Village_Pack/materials/MI_CLR_emission_purple.MI_CLR_emission_purple'"));
	UStaticMeshComponent* meshComponent = Cast<UStaticMeshComponent>(ownerActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (meshComponent)
	{
		const TArray<UMaterialInterface*>& materials = meshComponent->GetMaterials();
		materialSave.Reserve(materials.Num());

		for (int32 i = 0; i < materials.Num(); ++i)
		{
			materialSave.Push(materials[i]);

			meshComponent->SetMaterial(i, levitatedMaterial);
		}
	}
}

void UInteractableLevitating::getBackActorMaterial() {

	UStaticMeshComponent* meshComponent = Cast<UStaticMeshComponent>(ownerActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (meshComponent)
	{
		for (int32 i = 0; i < materialSave.Num(); ++i)
		{
			meshComponent->SetMaterial(i, materialSave[i]);
		}
	}
}

void UInteractableLevitating::EnableMove()
{
	if (ownerActor)
	{
		TArray<UStaticMeshComponent*> staticMeshComponents;
		ownerActor->GetComponents<UStaticMeshComponent>(staticMeshComponents);

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
	if (ownerActor)
	{
		TArray<UStaticMeshComponent*> staticMeshComponents;
		ownerActor->GetComponents<UStaticMeshComponent>(staticMeshComponents);

		for (UStaticMeshComponent* staticMeshComponent : staticMeshComponents)
		{
			if (staticMeshComponent)
			{
				staticMeshComponent->SetMobility(EComponentMobility::Static);
			}
		}
	}
}


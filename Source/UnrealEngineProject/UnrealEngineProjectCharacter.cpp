// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealEngineProjectCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include <iostream>
#include <Kismet/GameplayStatics.h>
#include "Engine/StaticMeshActor.h"


//////////////////////////////////////////////////////////////////////////
// AUnrealEngineProjectCharacter

AUnrealEngineProjectCharacter::AUnrealEngineProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AUnrealEngineProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUnrealEngineProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUnrealEngineProjectCharacter::Look);

		PlayerInputComponent->BindAxis("MoveForward", this, &AUnrealEngineProjectCharacter::MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &AUnrealEngineProjectCharacter::MoveRight);
		PlayerInputComponent->BindAxis("MoveUp", this, &AUnrealEngineProjectCharacter::MoveUp);

		PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AUnrealEngineProjectCharacter::Interact);
		PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AUnrealEngineProjectCharacter::Shoot);
	}

}

void AUnrealEngineProjectCharacter::MoveForward(float value)
{
	if (!isCharacterMoveLocked) {
		if (value != 0.0f && Controller != nullptr)
		{
			const FRotator rotation = Controller->GetControlRotation();
			const FRotator yawRotation(0, rotation.Yaw, 0);

			const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);

			AddMovementInput(forwardDirection, value);
		}
	}
	else {
		if (nearestObject)
		{
			FVector newLocation = nearestObject->GetActorLocation() + FVector(value * 10.f, 0.f, 0.f);

			FRotator newRotation = nearestObject->GetActorRotation();
			newRotation.Pitch = FMath::Sin(value * -5.0f);

			nearestObject->SetActorLocationAndRotation(newLocation, newRotation, true);
		}
	}
}

void AUnrealEngineProjectCharacter::MoveRight(float value)
{
	if (!isCharacterMoveLocked) {
		if (value != 0.0f && Controller != nullptr)
		{
			const FRotator rotation = Controller->GetControlRotation();
			const FRotator yawRotation(0, rotation.Yaw, 0);

			const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

			AddMovementInput(rightDirection, value);
		}
	}
	else {
		if (nearestObject)
		{
			FVector newLocation = nearestObject->GetActorLocation() + FVector(0.f, value * 10.f, 0.f);

			FRotator newRotation = nearestObject->GetActorRotation();
			newRotation.Roll = FMath::Sin(value * -5.0f);

			nearestObject->SetActorLocationAndRotation(newLocation, newRotation, true);
		}
	}
}

void AUnrealEngineProjectCharacter::MoveUp(float value)
{
	if (isCharacterMoveLocked && nearestObject)
	{
		FVector newLocation = nearestObject->GetActorLocation() + FVector(0.f, 0.f, value * 10.f);
		nearestObject->SetActorLocation(newLocation, true);
	}
}

void AUnrealEngineProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AUnrealEngineProjectCharacter::Interact()
{
	nearestObjectComponent = FindNearestLevitatingObject(levitatingObjectTag);

	if (selectedNearestObject != nullptr) {
		selectedInteractableComponent->isLevitatingModeDisable = true;
		selectedInteractableComponent->shouldAddImpulse = false;

		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		selectedInteractableComponent->DisableMove();
		isCharacterMoveLocked = false;

		selectedInteractableComponent->getBackActorMaterial();

		selectedInteractableComponent = nullptr;
		selectedNearestObject = nullptr;
	}
	else if (nearestObjectComponent != nullptr) {
		if (selectedInteractableComponent == nearestObjectComponent) {
			selectedInteractableComponent->isLevitatingModeDisable = true;
			selectedInteractableComponent->shouldAddImpulse = false;

			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			nearestObjectComponent->DisableMove();
			isCharacterMoveLocked = false;

			nearestObjectComponent->getBackActorMaterial();

			selectedInteractableComponent = nullptr;
			selectedNearestObject = nullptr;
			nearestObject = nullptr;
			nearestObjectComponent = nullptr;
		}
		else {
			GetCharacterMovement()->DisableMovement();
			isCharacterMoveLocked = true;

			FString objectName = nearestObjectComponent->GetName();
			nearestObjectComponent->setLevitatingActorMaterial();

			selectedInteractableComponent = nearestObjectComponent;
			selectedInteractableComponent->isLevitatingModeEnable = true;
			selectedNearestObject = nearestObject;

			UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(nearestObjectComponent->GetOwner()->GetRootComponent());
			PrimitiveComponent->SetSimulatePhysics(false);

			nearestObjectComponent->EnableMove();

			FVector newLocation = nearestObject->GetActorLocation() + FVector(0.f, 0.f, 50.f);
			nearestObject->SetActorLocation(newLocation, true);
		}
	}
}

UInteractableLevitating* AUnrealEngineProjectCharacter::FindNearestLevitatingObject(FName tag)
{
	FVector characterPosition = GetActorLocation();
	float minDistance = FLT_MAX;

	TArray<AActor*> actorsInRadius;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), characterPosition, detectionRadius, TArray<TEnumAsByte<EObjectTypeQuery>>(), nullptr, TArray<AActor*>(), actorsInRadius);

	for (AActor* actor : actorsInRadius)
	{
		if (actor->ActorHasTag(tag))
		{
			TArray<UActorComponent*> components;
			actor->GetComponents(components);

			for (UActorComponent* component : components)
			{
				UInteractableLevitating* levitatedComponent = actor->FindComponentByClass<UInteractableLevitating>();
				if (levitatedComponent)
				{
					float distance = FVector::Distance(characterPosition, actor->GetActorLocation());

					if (distance < minDistance)
					{
						minDistance = distance;
						nearestObjectComponent = levitatedComponent;
						nearestObject = actor;
					}
				}
			}
		}
	}

	if (nearestObjectComponent)
	{
		isObjectSelectedForLevitation = !isObjectSelectedForLevitation;
	}

	if (nearestObject)
	{
		nearestObject->SetActorEnableCollision(true);
	}

	return nearestObjectComponent;
}

void AUnrealEngineProjectCharacter::Shoot()
{
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	FVector ForwardVector = GetActorForwardVector();

	float Distance = 50.0f;
	float HeightOffset = 50.0f;

	FVector ObjectPosition = SpawnLocation + ForwardVector * Distance + FVector(0.0f, 0.0f, HeightOffset);

	AShootingProjectile* MyProjectile = GetWorld()->SpawnActor< AShootingProjectile>(shootClass, ObjectPosition, SpawnRotation);

	AActor* nearestShootableObject = FindNearestShootableObject(shootableObjectTag);

	FVector ImpulseDirection;

	ImpulseDirection = GetActorForwardVector();
	MyProjectile->FireInDirection(ImpulseDirection);
}

AActor* AUnrealEngineProjectCharacter::FindNearestShootableObject(FName tag)
{
	FVector characterPosition = GetActorLocation();
	float minDistance = FLT_MAX;
	AActor* nearestShootableObject = nullptr;

	TArray<AActor*> actorsInRadius;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), characterPosition, detectionRadius, TArray<TEnumAsByte<EObjectTypeQuery>>(), nullptr, TArray<AActor*>(), actorsInRadius);

	for (AActor* actor : actorsInRadius)
	{
		if (actor->ActorHasTag(tag))
		{
			float distance = FVector::Distance(characterPosition, actor->GetActorLocation());

			if (distance < minDistance)
			{
				minDistance = distance;
				nearestShootableObject = actor;
			}
		}
	}

	return nearestShootableObject;
}
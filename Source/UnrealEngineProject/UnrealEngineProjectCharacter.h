// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InteractableLevitating.h"
#include "ShootingProjectile.h"
#include "UnrealEngineProjectCharacter.generated.h"


UCLASS(config=Game)
class AUnrealEngineProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	FName levitatingObjectTag = "LevitatingObject";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	FName shootableObjectTag = "ShootableObject";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	float detectionRadius = 200.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot", meta = (AllowPrivateAccess = "true"))
	UClass* shootClass;

public:
	AUnrealEngineProjectCharacter();
	
	bool isObjectSelectedForLevitation = false;
	bool isCharacterMoveLocked = false;

	UInteractableLevitating* selectedInteractableComponent = nullptr;
	UInteractableLevitating* nearestObjectComponent = nullptr;
	AActor* nearestObject = nullptr;
	AActor* selectedNearestObject = nullptr;
	UAnimMontage* interactAnim;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			
	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveUp(float Value);
	void Interact();
	void Shoot();

	UInteractableLevitating* FindNearestLevitatingObject(FName tag);
	AActor* FindNearestShootableObject(FName tag);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


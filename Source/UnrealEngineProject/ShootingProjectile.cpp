// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingProjectile.h"

// Sets default values
AShootingProjectile::AShootingProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
    }

    if (!CollisionComponent)
    {
        CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
        CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
        CollisionComponent->OnComponentHit.AddDynamic(this, &AShootingProjectile::OnHit);
        CollisionComponent->InitSphereRadius(15.0f);
        RootComponent = CollisionComponent;
    }

    if (!ProjectileMovementComponent)
    {
        ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
        ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
        ProjectileMovementComponent->InitialSpeed = 200.0f;
        ProjectileMovementComponent->MaxSpeed = 200.0f;
        ProjectileMovementComponent->bRotationFollowsVelocity = true;
        ProjectileMovementComponent->bShouldBounce = true;
        ProjectileMovementComponent->Bounciness = 0.2f;
        ProjectileMovementComponent->ProjectileGravityScale = 0.1f;
    }

    if (!ProjectileMeshComponent)
    {
        ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));

        ProjectileMeshComponent->SetStaticMesh(shootingProjectileMesh);

        if (sphereMaterial)
        {
            ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(sphereMaterial, ProjectileMeshComponent);
        }
        ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);

        ProjectileMeshComponent->SetupAttachment(RootComponent);
    }
}

// Called when the game starts or when spawned
void AShootingProjectile::BeginPlay()
{
    Super::BeginPlay();

    SetLifeSpan(1.5f);

    if (firefliesBlueprintClass)
    {
        firefliesActor = GetWorld()->SpawnActor<AActor>(firefliesBlueprintClass);

        FVector ActorLocation = GetActorLocation();
        firefliesActor->SetActorLocation(ActorLocation);

        FAttachmentTransformRules AttachRules = FAttachmentTransformRules(
            EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld,
            true
        );

        firefliesActor->AttachToActor(this, AttachRules);
        firefliesActor->SetLifeSpan(1.5f);
    }
}

// Called every frame
void AShootingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShootingProjectile::FireInDirection(const FVector& ShootDirection)
{
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AShootingProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor->ActorHasTag("ShootableObject")) {
        OtherActor->Destroy();

        firefliesActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        firefliesActor->Destroy();

        Destroy();
    }
}
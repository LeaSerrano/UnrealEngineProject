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
        UStaticMesh* shootingProjectileMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/LevelPrototyping/Meshes/Sphere.Sphere"));
        ProjectileMeshComponent->SetStaticMesh(shootingProjectileMesh);

        UMaterialInterface* sphereMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Fantastic_Village_Pack/materials/MI_CLR_emission_purple.MI_CLR_emission_purple'"));
        if (sphereMaterial)
        {
            ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(sphereMaterial, ProjectileMeshComponent);
        }
        ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);

        ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
        ProjectileMeshComponent->SetupAttachment(RootComponent);
    }
}

// Called when the game starts or when spawned
void AShootingProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(1.5f);
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

        Destroy();
    }
}
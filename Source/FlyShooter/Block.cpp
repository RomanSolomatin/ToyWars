// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyShooter.h"
#include "Block.h"


// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BlockMeshAsset(TEXT("/Game/Flying/Meshes/Block/Block.Block"));

	// Create mesh component for the projectile sphere
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	Mesh->SetStaticMesh(BlockMeshAsset.Object);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetNotifyRigidBodyCollision(true);
	RootComponent = Mesh;

	health = 100;

}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlock::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ABlock::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Blockhit from %s"), *Other->GetName());

	if (Other->GetName().Contains("RocketProjectile"))
	{
		health -= 20;
	}

	if (health <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Block destroyed"));
		Destroy();
	}
}

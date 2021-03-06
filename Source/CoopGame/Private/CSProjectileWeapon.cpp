// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\CSProjectileWeapon.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "..\Public\ACSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "..\Public\CSCharacter.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

ACSProjectileWeapon::ACSProjectileWeapon()
{
	BaseDamage = 4000.0f;
	RateOfFire = 100;

	MagazineCapacity = 6;
	MaxBullets = 6;

	CountOfBulletsInMagazine = MagazineCapacity;
	CountOfBulletsOnCharacter = MaxBullets;

	SetReplicates(true);
}

void ACSProjectileWeapon::Fire()
{
	if (Role < ROLE_Authority)
	{
		ServerFireProjectile();
	}

	if (CanShoot())
	{
		if (Projectile)
		{
			FVector StartLocation = SkeletalMeshComponent->GetSocketLocation("Muzzle Socket");
			FRotator StartRotation = SkeletalMeshComponent->GetSocketRotation("Muzzle Socket");

			FActorSpawnParameters ActorSpawn;
			ActorSpawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AActor* Owner = GetOwner();

			CountOfBulletsInMagazine--;

			if (Owner)
			{
				FVector EyeLocation;
				FRotator EyeRotation;
				Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

				FVector ShotDirection = EyeRotation.Vector();

				AACSProjectile* ProjectileObj = GetWorld()->SpawnActor<AACSProjectile>(Projectile, StartLocation, EyeRotation, ActorSpawn);
				ProjectileObj->MoveProjectile(ShotDirection);
			}
		}
	}
}

void ACSProjectileWeapon::ServerFireProjectile_Implementation()
{
	Fire();
}

bool ACSProjectileWeapon::ServerFireProjectile_Validate()
{
	return true;
}

void ACSProjectileWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ACSProjectileWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ACSProjectileWeapon::StopFire()
{
	Super::StopFire();
}



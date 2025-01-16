// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability.h"
UAbility::UAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}
void UAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("Habilidad Activada"))
	if(ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor info valida"))
		AActor* AvatarActor = ActorInfo->AvatarActor.Get(); // El actor que está usando la habilidad
		ACharacter* Character = Cast<ACharacter>(AvatarActor);
		// Verificar si el AvatarActor es un ACharacter (ya que los personajes tienen un CharacterMovementComponent)
		AController* Controller = Character->GetController();
		AGasPlayerController* CustomController = Cast<AGasPlayerController>(Controller);
		if(Character)
		{
			UE_LOG(LogTemp, Warning, TEXT("char valido"));
			// Obtener el CharacterMovementComponent del personaje
			if (Controller && CustomController)
			{
			
				bCanActivate = CustomController->bIsClimbing;

				if(!bCanActivate)
				{
					UE_LOG(LogTemp, Warning, TEXT("No Esta Cayendo"))
						if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
						{
							EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
						}

					UT_playMontageAndWaitForEvent* montageTask = UT_playMontageAndWaitForEvent::playMontageAndWaitForEvent(
						this,
						NAME_None,
						CastMontage,
						FGameplayTagContainer(),
						1.f,
						NAME_None,
						false,
						1.f
					);

					montageTask->OnBlendOut.AddDynamic(this, &UAbility::OnMontageCompleted);
					montageTask->OnComplete.AddDynamic(this, &UAbility::OnMontageCompleted);
					montageTask->OnInterrupted.AddDynamic(this, &UAbility::OnMontageCancelled);
					montageTask->OnCancelled.AddDynamic(this, &UAbility::OnMontageCancelled);
					montageTask->OnEventReceived.AddDynamic(this, &UAbility::EventReceived);

					montageTask->ReadyForActivation();

				}
				else
				{
					EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);  // Cancelar habilidad
					return;  // Salir de la función
					
				}
			}

		}
	}
}



void UAbility::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

}

void UAbility::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);



}

void UAbility::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == ProjectileSpawnTag && bCanActivate)
	{
		UE_LOG(LogTemp, Warning, TEXT("EventoRecibido"));
		AGasCharacter* character = Cast<AGasCharacter>(GetAvatarActorFromActorInfo());
		if (!IsValid(character))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		FVector startLoc = character->GetMesh()->GetSocketLocation(AbilitySocketName);
		FVector endLoc = character->GetCameraBoom()->GetComponentLocation() + (character->GetFollowCamera()->GetForwardVector() * ProjectileRange);
		FRotator projectileRotation = UKismetMathLibrary::FindLookAtRotation(startLoc, endLoc);

		FTransform spawnTransform = character->GetMesh()->GetSocketTransform(AbilitySocketName);
		spawnTransform.SetRotation(projectileRotation.Quaternion());

		FActorSpawnParameters spawnparams;
		spawnparams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AProjectile* firebolt = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, spawnTransform, GetOwningActorFromActorInfo(), character,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (IsValid(firebolt))
			firebolt->Multicast_IgnoreActor(character);
		firebolt->Range = ProjectileRange;

		if (IsValid(firebolt->SphereComponent))
		{
			firebolt->SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		firebolt->FinishSpawning(spawnTransform);

	}
}

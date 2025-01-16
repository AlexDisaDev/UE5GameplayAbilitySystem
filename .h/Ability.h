// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplpayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "EnhancedInputComponent.h"
#include "T_PlayMontageAndWaitForEvent.h"
#include "T_Projectile.h"
#include "GasCharacter.h"
#include "PlayerControllerParkour.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Ability.generated.h"

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class /*YourProject*/_API Ability : public UGameAbility
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool bCancelAbilityOnInputReleased;

public:


	//UAbility(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* CastMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AT_Projectile> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag ProjectileSpawnTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName AbilitySocketName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float ProjectileRange;

	bool bCanActivate;

protected:
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent;

	TArray<uint32> TriggeredEventHandles;

	//~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	


protected:

	UFUNCTION()
	void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};


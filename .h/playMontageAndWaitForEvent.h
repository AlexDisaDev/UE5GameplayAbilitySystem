// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "PcEre.h"
#include "CharacterEre.h"
#include "playMontageAndWaitForEvent.generated.h"


class APcEre;

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FACMplayMontageAndWaitForEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

/**
 *
 */
UCLASS()
class TOKIOGAME_API UplayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()

public:
	UplayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer);

	virtual void Activate();

	virtual void ExternalCancell();

	virtual FString GetDebugString() const;

	virtual void OnDestroy(bool bInOwnerFinished);

	UPROPERTY(BlueprintAssignable)
	FACMplayMontageAndWaitForEventDelegate OnComplete;

	UPROPERTY(BlueprintAssignable)
	FACMplayMontageAndWaitForEventDelegate OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FACMplayMontageAndWaitForEventDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FACMplayMontageAndWaitForEventDelegate OnCancelled;

	UPROPERTY(BlueprintAssignable)
	FACMplayMontageAndWaitForEventDelegate OnEventReceived;

	


	UFUNCTION(BlueprintCallable, Category = "Ability|Task", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwingAbility", BlueprintInternalUseOnly = "TRUE"))
	static UplayMontageAndWaitForEvent* playMontageAndWaitForEvent(UGameplayAbility* OwingAbility, FName taskInstanceName, UAnimMontage* Montageplay,
		FGameplayTagContainer EventTags, float Rate = 1.f, FName StartSection = NAME_None, bool bstopWhenAbilityEnds = true, float AnimRootMotionTranslationScale = 1.f);

private:
	UPROPERTY(EditAnywhere)
	UAnimMontage* MontageToPlay;

	UPROPERTY()
	FGameplayTagContainer EventTags;

	UPROPERTY()
	float Rate;

	UPROPERTY()
	FName StartSection;

	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	float AnimRootMotionTranslationScale;

	UPROPERTY()
	bool bStopWhenAbilityEnds;

	bool StopPlayingMontage();

	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	void OnAbiltiyCancelled();

	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void OnGamePlayEvent(FGameplayTag EventTag, const FGameplayEventData* PayLoad) const;

	

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
};

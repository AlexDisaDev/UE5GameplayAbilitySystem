// Fill out your copyright notice in the Description page of Project Settings.


#include "playMontageAndWaitForEvent.h"

UplayMontageAndWaitForEvent::UplayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
	
}

void UplayMontageAndWaitForEvent::Activate()
{
	if (Ability == nullptr)
	{
		return;

	}
	bool bPlayerMontage = false;
	AActor* OwningActor = GetAvatarActor();
	if (OwningActor)
	{
		
		if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OwningActor))
		{
			AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();

			if (AbilitySystemComponent)
			{

				
				const FGameplayAbilityActorInfo* actorInfo = Ability->GetCurrentActorInfo();
				UAnimInstance* animInstance = actorInfo->GetAnimInstance();

				if (animInstance != nullptr)
				{
					EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this,
						&UplayMontageAndWaitForEvent::OnGamePlayEvent));
					if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
					{
						
						//float ability = AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection);
						
						if (ShouldBroadcastAbilityTaskDelegates() == false)
						{
							return;
						}

						CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UplayMontageAndWaitForEvent::OnAbiltiyCancelled);

						BlendingOutDelegate.BindUObject(this, &UplayMontageAndWaitForEvent::OnMontageBlendingOut);
						animInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

						MontageEndedDelegate.BindUObject(this, &UplayMontageAndWaitForEvent::OnMontageEnded);
						animInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

						ACharacter* character = Cast<ACharacter>(GetAvatarActor());
						if (character && (character->GetLocalRole() == ROLE_Authority ||
							(character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
						{
							character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
							UE_LOG(LogTemp, Warning, TEXT("playanim reproducida correctamente"))
						}

						bPlayerMontage = true;
						
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ASC NO VALIDO "));
			}
			if (!bPlayerMontage)
			{
				if (ShouldBroadcastAbilityTaskDelegates())
				{
					OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
				}
			}
		}
	}
	

	

	
}

//=====================================================================================================
void UplayMontageAndWaitForEvent::ExternalCancell()
{
	check(AbilitySystemComponent);

	OnAbiltiyCancelled();

	Super::ExternalCancel();

	
}
//=====================================================================================================
FString UplayMontageAndWaitForEvent::GetDebugString() const
{
	UAnimMontage* playingMontage = nullptr;

	if (Ability)
	{
		const FGameplayAbilityActorInfo* actorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* animInstance = actorInfo->GetAnimInstance();

		if (animInstance != nullptr)
		{
			playingMontage = animInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : animInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s (CurrentPlaying): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(playingMontage));
}
//=====================================================================================================
void UplayMontageAndWaitForEvent::OnDestroy(bool bInOwnerFinished)
{

	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (bInOwnerFinished && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	if (AbilitySystemComponent && EventHandle.IsValid())
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(bInOwnerFinished);
}
//=====================================================================================================
UplayMontageAndWaitForEvent* UplayMontageAndWaitForEvent::playMontageAndWaitForEvent(UGameplayAbility* OwingAbility, FName taskInstanceName,
	UAnimMontage* Montageplay, FGameplayTagContainer EventTags, float Rate, FName StartSection, bool BstopWhenAbilityEnds, float AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UplayMontageAndWaitForEvent* myObj = NewAbilityTask<UplayMontageAndWaitForEvent>(OwingAbility, taskInstanceName);
	
	myObj->MontageToPlay = Montageplay;
	myObj->EventTags = EventTags;
	myObj->Rate = Rate;
	myObj->StartSection = StartSection;
	myObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	myObj->bStopWhenAbilityEnds = BstopWhenAbilityEnds;

	//UE_LOG(LogTemp, Warning, TEXT("EVENTO RECIBIDO: %s"), *EventTag.GetTagName().ToString());

	// Imprimir las variables
	//UE_LOG(LogTemp, Warning, TEXT("MontageToPlay: %s"), *MontageToPlay->GetName());
	UE_LOG(LogTemp, Warning, TEXT("EventTags: %s"), *EventTags.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Rate: %f"), Rate);
	UE_LOG(LogTemp, Warning, TEXT("StartSection: %s"), *StartSection.ToString());
	UE_LOG(LogTemp, Warning, TEXT("AnimRootMotionTranslationScale: %f"), AnimRootMotionTranslationScale);
	//UE_LOG(LogTemp, Warning, TEXT("bStopWhenAbilityEnds: %s"), bStopWhenAbilityEnds ? TEXT("true") : TEXT("false"));

	return myObj;
	
}
//=====================================================================================================
bool UplayMontageAndWaitForEvent::StopPlayingMontage()
{
	const FGameplayAbilityActorInfo* actorinfo = Ability->GetCurrentActorInfo();
	if (!actorinfo)
	{
		EndTask();
		return false;
	}

	UAnimInstance* animInstance = actorinfo->GetAnimInstance();
	if (animInstance == nullptr)
	{
		EndTask();
		return false;
		
	}

	if (AbilitySystemComponent && Ability)
	{
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability && AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			FAnimMontageInstance* montageInstance = animInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (montageInstance)
			{
				montageInstance->OnMontageBlendingOutStarted.Unbind();
				montageInstance->OnMontageEnded.Unbind();
			}

			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}
	EndTask();
	return false;
	
}
//=====================================================================================================
void UplayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			ACharacter* character = Cast<ACharacter>(GetAvatarActor());
			if (character && (character->GetLocalRole() == ROLE_Authority ||
				(character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				character->SetAnimRootMotionTranslationScale(1.f);
			}
		}
	}

	if (bInterrupted && ShouldBroadcastAbilityTaskDelegates())
	{
		OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
	}
	else if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
	}
	EndTask(); // <-- Llamado en ambos casos
}
//=====================================================================================================

void UplayMontageAndWaitForEvent::OnAbiltiyCancelled()
{
	if (StopPlayingMontage())
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	EndTask();
}

//=====================================================================================================
void UplayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnComplete.Broadcast(FGameplayTag(), FGameplayEventData());
		}

	}
	EndTask();
}
//=====================================================================================================

void UplayMontageAndWaitForEvent::OnGamePlayEvent(FGameplayTag EventTag, const FGameplayEventData* PayLoad) const
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData tempData = *PayLoad;
		tempData.EventTag = EventTag;

		OnEventReceived.Broadcast(EventTag, tempData);
	}
}






//=====================================================================================================
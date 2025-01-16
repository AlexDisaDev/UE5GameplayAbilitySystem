// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet.h"
#include "GasCharacter.h"

UAttributeSet::UAttributeSet()
{
	MaxHealth = 100.f;
	Health = MaxHealth;
	HealthRegen = 0.5f;

	MaxFreeze = 10.f;
	Freeze = 0.f;
	FreezeRegen = 1.f;

	MaxInput = 1.f;
	Input = MaxInput;
	
}

void UAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxFreezeAttribute())
	{
		AdjustAttributeForMaxChange(Freeze, MaxFreeze, NewValue, GetFreezeAttribute());
	}
	else if (Attribute == GetMaxMovSpeedAttribute())
	{
		AdjustAttributeForMaxChange(MovSpeed, MaxMovSpeed, NewValue,GetMovSpeedAttribute());
		AActor* OwningActor = GetOwningActor();

		if (!OwningActor)
		{
			UE_LOG(LogTemp, Error, TEXT("Owning Actor no es de tipo ACharacter"));
			return;
		}
		if (OwningActor)
		{
			APlayerState* PlayerState = Cast<APlayerState>(OwningActor);
			UE_LOG(LogTemp, Error, TEXT("ownining actor check"));

			if (PlayerState)
			{
				UE_LOG(LogTemp, Error, TEXT("PS check"));

				APlayerController* PlayerController = PlayerState->GetPlayerController();
				if (PlayerController)
				{
					PlayerController->InputEnabled();
					UE_LOG(LogTemp, Error, TEXT("PC check"));

					APawn* ControlledPawn = PlayerController->GetPawn();
					ControlledCharacter = Cast<ACharacter>(ControlledPawn);
					if (ControlledCharacter)
					{						
						ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = NewValue;
						//UE_LOG(LogTemp, Warning, TEXT("El nuevo valor de MaxMovSpeed es: %f"), NewValue);				

					}
				}
			}
			
			
		}
		
	}
	else if (Attribute == GetMaxInputAttribute())
	{
		AdjustAttributeForMaxChange(Input, MaxInput, NewValue, GetInputAttribute());
		AActor* OwningActor = GetOwningActor();

		if (!OwningActor)
		{
			UE_LOG(LogTemp, Error, TEXT("Owning Actor no es de tipo ACharacter"));
			return;
		}
		if (OwningActor)
		{
			APlayerState* PlayerState = Cast<APlayerState>(OwningActor);
			UE_LOG(LogTemp, Error, TEXT("ownining actor check"));

			if (PlayerState)
			{
				UE_LOG(LogTemp, Error, TEXT("PS check"));

				APlayerController* PlayerController = PlayerState->GetPlayerController();
				if (PlayerController)
				{
					if (NewValue == 1)
					{
						PlayerController->EnableInput(PlayerController);
						UE_LOG(LogTemp, Error, TEXT("Input enabled"));
					}
					else if( NewValue == 0 )
					{
						PlayerController->DisableInput(PlayerController);
						UE_LOG(LogTemp, Error, TEXT("Input disabled"));
					}
					
				}
			}
		}
	}
}

void UAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSet, Health)))
	{
		Health.SetCurrentValue(FMath::Clamp(Health.GetCurrentValue(), 0.f, MaxHealth.GetCurrentValue()));
		Health.SetBaseValue(FMath::Clamp(Health.GetBaseValue(), 0.f, MaxHealth.GetCurrentValue()));
		OnhealthChangedelegate.Broadcast(Health.GetCurrentValue(), MaxHealth.GetCurrentValue());
	}
	else if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSet, Freeze)))
	{
		Freeze.SetCurrentValue(FMath::Clamp(Freeze.GetCurrentValue(), 0.f, MaxFreeze.GetCurrentValue()));
		Freeze.SetBaseValue(FMath::Clamp(Freeze.GetBaseValue(), 0.f, MaxFreeze.GetCurrentValue()));
		OnFreezeChangeDelegate.Broadcast(Freeze.GetCurrentValue(), MaxFreeze.GetCurrentValue());
	}
	else if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSet, MovSpeed)))
	{
		MovSpeed.SetCurrentValue(FMath::Clamp(MovSpeed.GetCurrentValue(), 0.f, MaxMovSpeed.GetCurrentValue()));
		MovSpeed.SetBaseValue(FMath::Clamp(MovSpeed.GetBaseValue(), 0.f, MaxMovSpeed.GetCurrentValue()));
		OnMovSpeedChangeDelegate.Broadcast(MovSpeed.GetCurrentValue(), MaxMovSpeed.GetCurrentValue());
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = MovSpeed.GetCurrentValue();
		UE_LOG(LogTemp, Warning, TEXT("Velocidad es : %f"), MovSpeed.GetCurrentValue());
	}
	else if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSet, Input)))
	{
		Input.SetCurrentValue(FMath::Clamp(Input.GetCurrentValue(), 0.f, MaxInput.GetCurrentValue()));
		Input.SetBaseValue(FMath::Clamp(Input.GetBaseValue(), 0.f, MaxInput.GetCurrentValue()));
		OnInputChangeDelegate.Broadcast(Input.GetCurrentValue(), MaxInput.GetCurrentValue());
	}
}

void UAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* AbilityComponent = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && IsValid(AbilityComponent))
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = CurrentMaxValue > 0.0f ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComponent->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet, Health, OldHealth);
}

void UAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet, MaxHealth, OldMaxHealth);
}

void UAttributeSet::OnRep_HealthRegen(const FGameplayAttributeData& OldHealthRegen)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet, HealthRegen, OldHealthRegen);
}

void UAttributeSet::OnRep_Freeze(const FGameplayAttributeData& OldFreeze)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet,Freeze, OldFreeze);
}

void UAttributeSet::OnRep_MaxFreeze(const FGameplayAttributeData& OldMaxFreeze)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet, MaxFreeze, OldMaxFreeze);
}

void UAttributeSet::OnRep_FreezeRegen(const FGameplayAttributeData& OldFreezeRegen)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet, FreezeRegen, OldFreezeRegen);
}

void UAttributeSet::OnRep_MovSpeed(const FGameplayAttributeData& OldMovSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet, MovSpeed, OldMovSpeed);
	
}

void UAttributeSet::OnRep_MaxMovSpeed(const FGameplayAttributeData& OldMaxMovSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet, MaxMovSpeed, OldMaxMovSpeed);
}

void UAttributeSet::OnRep_Input(const FGameplayAttributeData& OldInput)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet, Input, OldInput);
}

void UAttributeSet::OnRep_MaxInput(const FGameplayAttributeData& OldMaxInput)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet, MaxMovSpeed, OldMaxInput);
}



void UAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet, HealthRegen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet, Freeze, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet, MaxFreeze, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet, FreezeRegen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet, MovSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet, MaxMovSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet, Input, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet, MaxInput, COND_None, REPNOTIFY_Always);
}
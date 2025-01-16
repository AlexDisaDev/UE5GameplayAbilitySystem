// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "GameplayEffectExtension.h"
#include "AS_Ere.generated.h"

class AGasCharacter;

/**
 * 
 */ //AtributeSet
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceChange, float, currentValue, float, maxValue);


UCLASS()
class /*YourProjectName*/_API UAS_Ere : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UAS_Ere();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	void  AdjustAttributeForMaxChange(
		FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttribute,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty
	) const;

	ACharacter* ControlledCharacter;

	//Attributos
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = Onrep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAS_Ere, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = Onrep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAS_Ere, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = Onrep_HealthRegen)
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UAS_Ere, HealthRegen);

	UPROPERTY(BlueprintReadOnly, Category = "freeze", ReplicatedUsing = Onrep_Freeze)
	FGameplayAttributeData Freeze;
	ATTRIBUTE_ACCESSORS(UAS_Ere, Freeze);

	UPROPERTY(BlueprintReadOnly, Category = "Freeze", ReplicatedUsing = Onrep_MaxFreeze)
	FGameplayAttributeData MaxFreeze;
	ATTRIBUTE_ACCESSORS(UAS_Ere, MaxFreeze);

	UPROPERTY(BlueprintReadOnly, Category = "Freeze", ReplicatedUsing = Onrep_FreezeRegen)
	FGameplayAttributeData FreezeRegen;
	ATTRIBUTE_ACCESSORS(UAS_Ere, FreezeRegen);

	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = Onrep_MovSpeed)
	FGameplayAttributeData MovSpeed;
	ATTRIBUTE_ACCESSORS(UAS_Ere, MovSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = Onrep_MaxMovSpeed)
	FGameplayAttributeData MaxMovSpeed;
	ATTRIBUTE_ACCESSORS(UAS_Ere, MaxMovSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Input", ReplicatedUsing = Onrep_Input)
	FGameplayAttributeData Input;
	ATTRIBUTE_ACCESSORS(UAS_Ere, Input);

	UPROPERTY(BlueprintReadOnly, Category = "Input", ReplicatedUsing = Onrep_MaxInput)
	FGameplayAttributeData MaxInput;
	ATTRIBUTE_ACCESSORS(UAS_Ere, MaxInput);

	FOnResourceChange OnhealthChangedelegate;

	FOnResourceChange OnFreezeChangeDelegate;

	FOnResourceChange OnMovSpeedChangeDelegate;
	
	FOnResourceChange OnInputChangeDelegate;

	

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_HealthRegen(const FGameplayAttributeData& OldHealthRegen);

	UFUNCTION()
	virtual void OnRep_Freeze(const FGameplayAttributeData& OldFreeze);

	UFUNCTION()
	virtual void OnRep_MaxFreeze(const FGameplayAttributeData& OldMaxFreeze);

	UFUNCTION()
	virtual void OnRep_FreezeRegen(const FGameplayAttributeData& OldFreezeRegen);

	UFUNCTION()
	virtual void OnRep_MovSpeed(const FGameplayAttributeData& OldMovSpeed);
	UFUNCTION()
	virtual void OnRep_MaxMovSpeed(const FGameplayAttributeData& OldMaxMovSpeed);
	
	UFUNCTION()
	virtual void OnRep_Input(const FGameplayAttributeData& OldInput);
	UFUNCTION()
	virtual void OnRep_MaxInput(const FGameplayAttributeData& OldMaxInput);
};

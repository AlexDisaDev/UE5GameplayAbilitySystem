// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h" // Interfaz para el Ability System
#include "AbilitySystemComponent.h" // Incluye tu set de atributos
#include "AttributeSet.h" // attribute set creado como mi clase en c++
#include "GasPlayerState.generated.h"


class CharacterEre;
/**
 * 
 */
UCLASS()
class /*YourProjectName*/ GasPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:

	GasPlayerState();

	//void InitializeAttributes();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UAttributeSet* AttributeSet;

	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	UAbilitySystemComponent* GetPSAbilitySystemComponent() const { return AbilitySystemComponent; }

	UAttributeSet* GetAttributeSet() const;

	bool IsAlive() const;

	bool IsDead() const;

	float GetHealth() const;

	float GetMaxHealth() const;

	float  GetHealthRegen() const;

	float GetInput() const;

	
	//float GetInput() const;

	virtual void BeginPlay() override;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle InputChangedDelegateHandle;

	virtual void OnHealthChanged(const FOnAttributeChangeData& Data);
	virtual void OnInputChanged(const FOnAttributeChangeData& Data);

	UFUNCTION(BlueprintCallable)
	void ScoreKill();

	int32 killCount;

	APlayerController* PlayerController;


private:

	void EndPlay(const EEndPlayReason::Type EndPlayReason);
};

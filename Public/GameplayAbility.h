// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include <InputAction.h>
#include "GameplayAbility.generated.h"




/**
 * 
 */
UCLASS()
class  /*YourProjectName*/_API GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	GameplayAbility(const FObjectInitializer& ObjectInitializer);

	/*UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gameplay Ability")
	EGA_AbilityInputID AbilityID;*/

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gameplay Ability")
	EGA_AbilityInputID AbilityInputID;

	

};

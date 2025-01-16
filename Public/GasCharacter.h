// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayAbility.h"
#include "GasPlayerState.h"
#include <AbilitySystemGlobals.h>
#include <AbilitySystemBlueprintLibrary.h>
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GasCharacter.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
class APlayerControllerParkour;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceCharacterChange, float, PercentValue);

UCLASS()
class /*YourProjectName*/_API AGasCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	AGasCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

	virtual void PossessedBy(AController* NewController) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/*--- Gameplay abilities System Start*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Ability System")
	UAbilitySystemComponent* AbilitySystemComponenet;

	UFUNCTION(BlueprintCallable, Category = "Gameplay Ability System")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Ability System")
	UAttributeSet* AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Ability System")
	TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Ability System")
	TArray<TSubclassOf<UGameplayEffect>> StartingEffects;

	void SetupGasinputs(APlayerControllerParkour* PlayerController);

	void SetupAbilities();

	void SetupEffects();

	bool AbilitiesGiven;

	bool EffectsGiven;

	bool IsInputbound;

	AGasCharacter* Character;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	APlayerControllerParkour* PC;

	/*---- Gameplay Ability System End ---*/
	bool IsDying;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DeathEffectClass;

	UFUNCTION(Server, Reliable)
	virtual void serverDie(AGasCharacter* killer);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath();

	virtual void OnRep_PlayerState() override;

protected:

	void JumpDistance();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "trace")
	float MaxDistanceToGround;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "trace")
	float FrontDiagonalDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "JumpAction")
	bool bCanjump;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* SlideMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* EndSlideMontage;

protected:
	//Health
	UPROPERTY(BlueprintAssignable)
	FOnResourceCharacterChange OnCharacterhealthDelegate;

	UFUNCTION()
	void OnCharacterhealthChanged(float health, float maxHealth);

	UFUNCTION(Client, Reliable)
	void Client_healthChanged(float health, float maxhealth);

	

	//Freeze
	UPROPERTY(BlueprintAssignable)
	FOnResourceCharacterChange OnCharacterFreezeDelegate;

	UFUNCTION()
	void OnCharacterFreezeChanged(float Freeze, float maxFreeze);

	UFUNCTION(Client, Reliable)
	void Client_FreezeChanged(float Freeze, float maxFreeze);
	
	
	//SpeedMovmente

	UPROPERTY(BlueprintAssignable)
	FOnResourceCharacterChange OnCharacterMovSpeedDelegate;

	UFUNCTION()
	void OnCharacterMovSpeedChanged(float Speed, float maxSpeed);

	UFUNCTION(Client, Reliable)
	void Client_MovSpeedChanged(float Speed, float maxSpeed);

	//Iniputbool
	FOnResourceCharacterChange OnCharacterInputDelegate;

	UFUNCTION()
	void OnCharacterInputChanged(float Input, float maxinput);

	UFUNCTION(Client, Reliable)
	void Client_InputChanged(float Input, float maxinput);
};

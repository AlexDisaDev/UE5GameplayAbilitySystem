// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemInterface.h"
#include "Engine/EngineTypes.h"
#include "GasCharacter.h"
#include <GameplayAbility.h>
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"
#include "Net/UnrealNetwork.h"
#include "GameplayAbilities/Public/GameplayAbilitySpec.h" // Para FGameplayAbilitySpec
#include "GameplayAbilities/Public/AbilitySystemComponent.h" // Para UAbilitySystemComponent
#include "GasPlayerController.generated.h"



class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
class playMontageAndWaitForEvent;
USTRUCT(BlueprintType)
struct FAngleResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool bIsSlopeUp;

	UPROPERTY(BlueprintReadWrite)
	float Angle;
};
/**
 * 
 */
UCLASS()
class /*YourProjectName*/_API GasPlayerController : public APlayerController

{
	GENERATED_BODY()

public:

	GasPlayerController();
	//APS_Ere* MyPlayerState;


protected:

	virtual void SetupInputComponent() override;
	
	
protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* aPawn) override;

public:
	

	//UPROPERTY(EditAnywhere, Category = "IAInputs")
	//UInputMappingContext* InputMappingcontext;

	// Declarar las InputActions para movimiento y otras acciones
	UPROPERTY(EditAnywhere, Category = "IAInput")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "IAInput"


	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, Category = "IAInput")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "IAInput")
	UInputAction* SlideAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "JumpAction")
	bool bCantJump;

	UPROPERTY(EditAnywhere, Category = "IAInput")
	UInputAction* Ability1;
	
	UPROPERTY(EditAnywhere, Category = "IAInput")
	UInputAction* Ability2;

	UPROPERTY(EditAnywhere, Category = "IAInput")
	UInputAction* Ability3;

	UPROPERTY(EditAnywhere, Category = "IAInput")
	UInputAction* Ability4;

	UPROPERTY(EditAnywhere, Category = "IAInput")
	UInputAction* Ability5;

	UEnhancedInputComponent* EInputComponent;

	APawn* ControlledPawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PawnClass")
	AGasCharacter* ControlledCharacter;
	APlayerState* PlayerState;
	
	UPROPERTY(Replicated)
	FVector AimDirection;

	UInputAction* GetInputActionFromAbilityInputID(EGA_AbilityInputID InputID);
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void Slide(const FInputActionValue& Value);
	void Ability1Action1();
	void Ability1Action2();
	void Ability1Action3();
	void Ability1Action4();
	void Ability1Action5();
	void PermissionAbilityInputs(AGasCharacter* character);
	virtual void SendAbilityLocalInput(const FInputActionValue& Value, int32 InputID);

	//void GetPlayerAbilitySystemComponent();
	bool moveActive;
	bool bAiming;
	//===============Variables Replicadas========================================
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAimDirection(const FVector& NewAimDIrection);

	bool ServerSetAimDirection_Validate(const FVector& NewAimDirection);

	
	
	//=========================================================================================

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AInput", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UPlayerGameplayAbilitiesDataAsset> PlayerGameplayAbilitiesDataAsset;
	
	/*void StartVoiceChat();
	UFUNCTION(BlueprintCallable, Category = "VoiceChat")
	void StopVoiceChat();
	
	UFUNCTION(BlueprintCallable, Category = "VoiceChat")
	void MutePlayer(APlayerController* MutePlayer, const FUniqueNetIdRepl& PlayerID);*/

	//TSharedPtr<const FUniqueNetId> FriendLobbyId;



protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Joystick")
	bool bIsJoystick;
	// Aquí declaras el AbilitySystemComponent
	UPROPERTY(BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

	/*UFUNCTION(server, Reliable)
	void InitAbilitySystem();

	UFUNCTION(BlueprintCallable,client, Reliable)
	void RequestAbilities();*/
	void OnAbilityInputPressed(int32 InputID);
	void OnAbilityInputReleased(int32 InputID);
	void ActForward();
	// funciones para sliding

	// Variables para el sistema de deslizamiento
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	float SlidingAcceleration; // Aceleración al deslizar hacia abajo

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	float SlidingDeceleration; // Desaceleración en terreno plano o ascendente

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	float MinSlidingAngle; // Ángulo mínimo para deslizarse

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	float MaxSlidingAngle; // Ángulo máximo para acelerar más

	UAnimMontage* CharSlideMontage;
	UAnimMontage* ChareEndSlideMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	bool bIsSliding;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	bool bCanSlide;

	void tracefloorWhileSliding();

	void allignPlayerToFloor();

	void FootFrwContact();

	//collisions
	bool CapsuleTrace(FHitResult HitResult,FVector startPosition, FVector EndPosition, float capsuleRadius, float capsuleHalfHeight, ECollisionChannel collisionChannel, FCollisionQueryParams collisionparams);
	bool SphereTrace(FHitResult HitResult,FVector startPosition, FVector EndPosition, float capsuleRadius, ECollisionChannel collisionChannel, FCollisionQueryParams collisionparams);


	UFUNCTION(BlueprintCallable, Category = "Sliding")
	void FindCurrentFloorAngleAndDirection();

	FTimerHandle ActSlidePosition;
	FTimerHandle JumpDistances;

	FVector NewVel;
	float changeVelCrouch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sliding")
	FVector currentSliidingVelocity;

	FVector CameraBoomInit;
	FVector CameraBoomSliding;

	void CrouchUnCrouchPosition(FVector CameraBoomPositionObj, FVector ActualCameraBoomPosition, float interpSeconds );
	
	void JumpDistance();

	void JumpZVelocity();
	
	float JumpVelocityZ;

	void JumpFallCalculateParabolic();

	bool bJumpinput;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "climb")
	bool bClimb;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "trace")
	float MaxDistanceToGround;

	float DistanceToFloorRecieved;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "trace")
	float FrontDiagonalDistance;

	UFUNCTION(BlueprintCallable, Category = "Sliding")
	void ResetMovements(FRotator RotationToFrwPosition, bool crouch,FVector charMeshSlidePosition ,FVector cameraPosition, float crouchSpeed, bool IsSliding, UAnimMontage * AnimStop , UAnimMontage* AnimPlay);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "trace")
	FVector ImpactPointRightHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "trace")
	FVector ImpactPointLeftHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "test")
	bool TestVariable; 

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Climb")
	bool bIsClimbing;

};
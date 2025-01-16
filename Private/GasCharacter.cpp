// Fill out your copyright notice in the Description page of Project Settings.

#include "GasCharacter.h"
#include <PlayerControllerParkour.h>

// Sets default values
AGasCharacter::AGasCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	IsInputbound = false;
	AbilitiesGiven = false;
	EffectsGiven = false;
	IsDying = false;
	
}

// Called when the game starts or when spawned
void AGasCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponenet)
	{
		if (PC)
		{
			//PC->PermissionAbilityInputs(this);
			UE_LOG(LogTemp, Warning, TEXT("PC VALIDO"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PC no valido"));
			return;
		}
	}
}

void AGasCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APS_Ere* playerState = GetPlayerState<APS_Ere>();
	PC = Cast<APlayerControllerParkour>(NewController);
	if (IsValid(playerState))
	{
		AbilitySystemComponenet = playerState->GetAbilitySystemComponent();
		playerState->GetAbilitySystemComponent()->InitAbilityActorInfo(playerState, this);
		AttributeSet = playerState->GetAttributeSet();
		float CurrentMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		AbilitySystemComponenet->SetNumericAttributeBase(AttributeSet->GetMaxMovSpeedAttribute(), CurrentMaxWalkSpeed);
		AbilitySystemComponenet->SetNumericAttributeBase(AttributeSet->GetMovSpeedAttribute(), CurrentMaxWalkSpeed);
		AttributeSet->OnhealthChangedelegate.AddDynamic(this, &AGasCharacter::OnCharacterhealthChanged);
		AttributeSet->OnFreezeChangeDelegate.AddDynamic(this, &AGasCharacter::OnCharacterFreezeChanged);
		AttributeSet->OnMovSpeedChangeDelegate.AddDynamic(this, &AGasCharacter::OnCharacterMovSpeedChanged);
		AttributeSet->OnInputChangeDelegate.AddDynamic(this, &AGasCharacter::OnCharacterInputChanged);
		

		SetupAbilities();
		SetupEffects();
		SetupGasinputs(PC);
		
	}
	
	
}

// Called every frame
void AGasCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

UAbilitySystemComponent* AGasCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponenet;
}

void AGasCharacter::SetupGasinputs(APlayerControllerParkour* PlayerController)
{
	//if (GetLocalRole() != ROLE_Authority)
	//{
	//	
	//	constexpr int32 AbilityLevel = 1;
	//	for (const auto& It : InputAbilities)
	//	{
	//		if (It.IsValid() && It.GameplayAbilityClass)
	//		{
	//			// Corrigiendo el acceso para que use el objeto PlayerController
	//			UE_LOG(LogTemp, Warning, TEXT("IT es valido %s"), *PlayerController->GetPlayerGameplayAbilitiesDataAsset()->GetName());

	//			const FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(It.GameplayAbilityClass, AbilityLevel, It.InputID);
	//			if (AbilitySystemComponenet)
	//			{
	//				AbilitySystemComponenet->GiveAbility(AbilitySpec);
	//			}
	//			else
	//			{
	//				UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponenet es nulo en SetupGasinputs"));
	//				return;
	//			}
	//		}
	//	}
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("PlayerController o PlayerGameplayAbilitiesDataAsset son nulos en SetupGasinputs"));
	//}
}


void AGasCharacter::SetupAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponenet) || AbilitiesGiven)
	{
		return;
	}
	for (TSubclassOf<UGA_Ere>& currentAbility : StartingAbilities)
	{
		if (IsValid(currentAbility))
		{
			UGA_Ere* defaultObj = currentAbility->GetDefaultObject<UGA_Ere>();
			FGameplayAbilitySpec abilitySpec(defaultObj, 1, static_cast<int32>(defaultObj->AbilityInputID), this);
			AbilitySystemComponenet->GiveAbility(abilitySpec);
		}
	}

	AbilitiesGiven = true;
}

void AGasCharacter::SetupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponenet) || EffectsGiven)
	{
		return;
	}

	FGameplayEffectContextHandle effectContext = AbilitySystemComponenet->MakeEffectContext();
	effectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect>& currentEffect : StartingEffects)
	{
		if (IsValid(currentEffect))
		{
			FGameplayEffectSpecHandle newHandle = AbilitySystemComponenet->MakeOutgoingSpec(currentEffect, 1.f, effectContext);
			if (newHandle.IsValid())
			{
				FActiveGameplayEffectHandle activeGEHandle = AbilitySystemComponenet->ApplyGameplayEffectSpecToTarget(*newHandle.Data.Get(), GetAbilitySystemComponent());
			}
		}
	}

	EffectsGiven = true;
}

void AGasCharacter::serverDie_Implementation(AGasCharacter* killer)
{
	if (IsDying)
	{
		return;
	}
	IsDying = true;

	if (IsValid(DeathEffectClass))
	{
		FGameplayEffectContextHandle EffectContext;
		AbilitySystemComponenet->ApplyGameplayEffectToSelf(DeathEffectClass->GetDefaultObject<UGameplayEffect>(), 1.f, EffectContext);
	}


	APS_Ere* KillerPlayerstate = Cast<APS_Ere>(killer->GetPlayerState());
	if (IsValid(KillerPlayerstate))
	{
		KillerPlayerstate->ScoreKill();
	}
	Multicast_OnDeath();
}

void AGasCharacter::Multicast_OnDeath_Implementation()
{
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);

	}
	else
	{
		Destroy();
	}
}

void AGasCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	APS_Ere* playerState = GetPlayerState<APS_Ere>();
	if (IsValid(playerState))
	{
		AbilitySystemComponenet = playerState->GetAbilitySystemComponent();
		AbilitySystemComponenet->InitAbilityActorInfo(playerState, this);
		AttributeSet = playerState->GetAttributeSet();
		float CurrentMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		AbilitySystemComponenet->SetNumericAttributeBase(AttributeSet->GetMaxMovSpeedAttribute(), CurrentMaxWalkSpeed);
		AbilitySystemComponenet->SetNumericAttributeBase(AttributeSet->GetMovSpeedAttribute(), CurrentMaxWalkSpeed);
		AttributeSet->OnhealthChangedelegate.AddDynamic(this, &AGasCharacter::OnCharacterhealthChanged);
		AttributeSet->OnFreezeChangeDelegate.AddDynamic(this, &AGasCharacter::OnCharacterFreezeChanged);
		AttributeSet->OnMovSpeedChangeDelegate.AddDynamic(this, &AGasCharacter::OnCharacterMovSpeedChanged);	
		AttributeSet->OnInputChangeDelegate.AddDynamic(this, &AGasCharacter::OnCharacterInputChanged);
	}
}

void AGasCharacter::OnCharacterhealthChanged(float health, float maxHealth)
{
	Client_healthChanged(health, maxHealth);
}

void AGasCharacter::Client_healthChanged_Implementation(float health, float maxhealth)
{
	OnCharacterhealthDelegate.Broadcast(health / maxhealth); 
}

void AGasCharacter::OnCharacterFreezeChanged(float Freeze, float maxFreeze)
{
	Client_FreezeChanged(Freeze, maxFreeze);
}

void AGasCharacter::OnCharacterMovSpeedChanged(float Speed, float maxSpeed)
{
	Client_MovSpeedChanged(Speed, maxSpeed);
}

void AGasCharacter::OnCharacterInputChanged(float Input, float maxinput)
{
	Client_InputChanged(Input, maxinput);
}

void AGasCharacter::Client_InputChanged_Implementation(float Input, float maxinput)
{
	OnCharacterInputDelegate.Broadcast(Input / maxinput);
}

void AGasCharacter::Client_MovSpeedChanged_Implementation(float Speed, float maxSpeed)
{
	OnCharacterMovSpeedDelegate.Broadcast(Speed / maxSpeed);
}

void AGasCharacter::Client_FreezeChanged_Implementation(float Freeze, float maxFreeze)
{
	OnCharacterFreezeDelegate.Broadcast(Freeze / Freeze);
}

void AGasCharacter::JumpDistance()
{
	
	FVector StartLocation = GetActorLocation();

	// Configuración de trazados
	float TraceDistanceDown = 1000.0f; // Distancia máxima hacia abajo

	FCollisionQueryParams TraceParams(FName(TEXT("GroundAndForwardTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	// ======= Trazado hacia abajo =======
	FVector EndLocationDown = StartLocation - FVector(0, 0, TraceDistanceDown);

	FHitResult HitResultDown;
	bool bHitDown = GetWorld()->LineTraceSingleByChannel(HitResultDown, StartLocation, EndLocationDown, ECC_Visibility, TraceParams);

	if (bHitDown)
	{
		float CurrentDistance = (StartLocation - HitResultDown.ImpactPoint).Size();

		// Solo actualiza la distancia máxima si es válida
		if (CurrentDistance > MaxDistanceToGround && FrontDiagonalDistance > 600.f)
		{
			MaxDistanceToGround = CurrentDistance;

			// Muestra la nueva distancia máxima en pantalla
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Nueva distancia máxima al suelo: %f"), MaxDistanceToGround));
		}

		// Dibuja el trazado hacia abajo para depuración
		//DrawDebugLine(GetWorld(), StartLocation, HitResultDown.ImpactPoint, FColor::Blue, false, 2.0f, 0, 1.0f);
	}


	FVector ForwardVector = GetActorForwardVector(); // Dirección hacia adelante
	FVector UpVector = FVector(0, 0, -1); // Dirección hacia arriba (Z)

	float ForwardDistance = 1500.f; // Distancia máxima hacia adelante
	float DiagonalAngle = 0.5f; // Ajuste de inclinación (0.5 = 45 grados)

	// Dirección diagonal combinando adelante y arriba
	FVector DiagonalDirection = ForwardVector + (UpVector * DiagonalAngle);
	DiagonalDirection.Normalize(); // Normalizar para mantener la magnitud uniforme

	// Posición final del trazado
	FVector EndLocationDiagonal = StartLocation + (DiagonalDirection * ForwardDistance);

	// Configuración del Line Trace
	FHitResult HitResultDiagonal;

	// Ejecutar el trazado
	bool bHitDiagonal = GetWorld()->LineTraceSingleByChannel(HitResultDiagonal, StartLocation, EndLocationDiagonal, ECC_Visibility, TraceParams);

	if (bHitDiagonal)
	{
		// Medir la distancia del impacto
		float DiagonalDistance = (StartLocation - HitResultDiagonal.ImpactPoint).Size();

		FrontDiagonalDistance = DiagonalDistance;
		// Mostrar la distancia en pantalla
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Distancia diagonal: %f"), DiagonalDistance));

		// Dibuja el trazado para depuración
		//DrawDebugLine(GetWorld(), StartLocation, HitResultDiagonal.ImpactPoint, FColor::Purple, false, 2.0f, 0, 1.0f);
	}

}






// Fill out your copyright notice in the Description page of Project Settings.


#include "GasPlayerController.h"
#include "Ability.h"

AGasPlayerController::AGasPlayerController()
{
		
	// en el character de clase GasCharacter debe configurarse 
	// las animaciones de deslizamiento y colocarse de pie despues del deslizamiento
	PrimaryActorTick.bCanEverTick = true;
	bAiming = false;
	bIsJoystick = false;
	bCantJump = true;
	SlidingAcceleration = 1500.0f;
	SlidingDeceleration = 800.0f;
	MinSlidingAngle = 15.0f; // Grados
	MaxSlidingAngle = 45.0f; // Grados
	bIsSliding = false;
	bCanSlide = true;
	CameraBoomInit = FVector(0.f, 0.f, 0.f);
	CameraBoomSliding = FVector(0.f, 0.f, 65.f);

}
//Configuracion de enhanced input system.
//init del sistema EIS
void AGasPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EInputComponent = EnhancedInputComponent;

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem <UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);

			if (DefaultMappingContext)
			{
					EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGasPlayerController::Move);
					EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGasPlayerController::Look);
					EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGasPlayerController::Jump);
					EnhancedInputComponent->BindAction(SlideAction,ETriggerEvent::Triggered, this, &AGasPlayerController::Slide);
					EnhancedInputComponent->BindAction(Ability1, ETriggerEvent::Triggered, this, &AGasPlayerController::Ability1Action1);
					EnhancedInputComponent->BindAction(Ability2, ETriggerEvent::Triggered, this, &AGasPlayerController::Ability1Action2);
					EnhancedInputComponent->BindAction(Ability3, ETriggerEvent::Triggered, this, &AGasPlayerController::Ability1Action3);
					EnhancedInputComponent->BindAction(Ability4, ETriggerEvent::Triggered, this, &AGasPlayerController::Ability1Action4);
					EnhancedInputComponent->BindAction(Ability5, ETriggerEvent::Triggered, this, &AGasPlayerController::Ability1Action5);


					
			}
		}
	}
}
//===========================================================================================
void AGasPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ControlledPawn = GetPawn();
	//ControlledCharacter = Cast<AGasCharacter>(ControlledPawn);
	PlayerState = GetPlayerState<GasPlayerState>();
	//StartVoiceChat();
}
//===========================================================================================
void AGasPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//test para controlar con sliding, puede ser mejorado con set timer by function.
	if(bIsSliding)
	{
		FindCurrentFloorAngleAndDirection();
		FVector Velocity = ControlledCharacter->GetCharacterMovement()->Velocity;
		float velf = Velocity.Size();
	}
}
//===========================================================================================
//control del pawn que se obtiene con sus habilidades listadas en la clase GasCharacter.
void AGasPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (aPawn)
	{
		ControlledCharacter = Cast<AGasCharacter>(aPawn);
		if (ControlledCharacter)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PAWN VALIDO PARA PERMISO INPUT"))

			PermissionAbilityInputs(ControlledCharacter);
			CharSlideMontage = ControlledCharacter->SlideMontage;
			ChareEndSlideMontage = ControlledCharacter->EndSlideMontage;

			//UE_LOG(LogTemp, Warning, TEXT("El anim montage es : %s"), *CharSlideMontage->GetName());
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("PAWN NO  VALIDO PARA PERMISO INPUT"))

			return;
		}
		
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("PC NO VALIDO"))

		return;
	}


	
}
//===========================================================================================
//movimiento basico para 3era persona.
void AGasPlayerController::Move(const FInputActionValue& Value)
{
	if ( ControlledPawn != nullptr)
	{
		
		const FVector2D MovementVector = Value.Get<FVector2D>();
		const FRotator Rotation = ControlledPawn->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);

		
	}
}
//===========================================================================================
//movimiento de la camara basica para 3era persona
void AGasPlayerController::Look(const FInputActionValue& Value)
{
	FVector MouseLocation, MouseDirection;
	bAiming = true;

	if (ControlledPawn != nullptr)
	{
		FVector2D AimInput = Value.Get<FVector2D>();
		if (AimInput.IsZero())
		{
			bAiming = false;
			//UE_LOG(LogTemp, Warning, TEXT("el valor es %s"), *AimInput.ToString());
		}

		ControlledPawn-> AddControllerYawInput(AimInput.X);
		ControlledPawn-> AddControllerPitchInput(-AimInput.Y);
	}
		
}			
//===========================================================================================

void AGasPlayerController::Jump(const FInputActionValue& Value)
{
	//control de salto parkour para obtener diferentes alturas de superficies.
	//AnimMontage controladas desde el GasCharacter.
	if (ControlledCharacter != nullptr)
	{
		bool bIsJumping = Value.Get<bool>();
		bJumpinput = bIsJumping;
		FVector meshNormalPosition = FVector(0.f, 0.f, -97.f);
		// rotacion Vector hacia adelante
		FRotator ForwardRotation = ControlledCharacter->GetActorForwardVector().Rotation();
		FRotator rotationToNormalPos = FRotator(0.0f, ForwardRotation.Yaw, 0.0f);
		if (bIsJumping && bCantJump && !bIsSliding)
		{
			UE_LOG(LogTemp, Warning, TEXT("Saltando01"));
			ControlledCharacter->Jump();
			float JumpZVelocity = ControlledCharacter->GetCharacterMovement()->JumpZVelocity; // Velocidad vertical inicial
			float Gravity = FMath::Abs(GetWorld()->GetGravityZ()) * ControlledCharacter->GetCharacterMovement()->GravityScale; // Gravedad efectiva

			// Calcular el tiempo total del salto (subida + bajada)
			float TimeToApex = JumpZVelocity / Gravity; // Tiempo hasta el punto m�s alto
			float TotalTime = TimeToApex * 2.0f;       // Tiempo total del salto

			// Velocidad horizontal (ejemplo de velocidad constante)
			FVector HorizontalVelocity = ControlledCharacter->GetVelocity();
			HorizontalVelocity.Z = 0; // Ignorar la componente vertical

			// Calcular puntos de la par�bola
			FVector StartPosition = ControlledCharacter->GetActorLocation();
			const int32 NumPoints = 30; // N�mero de puntos para dibujar la trayectoria
		
			for (int32 i = 0; i <= NumPoints; ++i)
			{
				// Calcular el tiempo para este punto
				float Time = (i / (float)NumPoints) * TotalTime;

				// Posici�n en X/Y (movimiento horizontal)
				
				FVector Position = StartPosition + HorizontalVelocity * Time;

				// Posici�n en Z (movimiento vertical)
				Position.Z = StartPosition.Z + (JumpZVelocity * Time) - (0.5f * Gravity * FMath::Square(Time));

				FHitResult HitResult;
				
				FVector TraceStart = Position ;
				FVector TraceEnd = Position + FVector(0, 0, -60); // Trace hacia abajo (ajustar distancia seg�n necesidad)
				FCollisionShape Sphere = FCollisionShape::MakeSphere(20.0f); // Radio de la esfera para el trace

				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(ControlledCharacter); // Ignorar al personaje controlado

				// Realizar el trace
				bool bHit = GetWorld()->SweepSingleByChannel(HitResult, TraceStart , TraceEnd , FQuat::Identity, ECC_Visibility, Sphere, QueryParams);

				// Dibujar un punto en la trayectoria
				if (bHit)
				{
					// Si el trace colisiona con algo, dibujar el punto donde ocurri� la colisi�n
					DrawDebugSphere(GetWorld(), HitResult.Location, 10.0f, 12, FColor::Red, false, 2.0f);
					int32 lastDrawIndex = i;

					break; // Salir del ciclo si se detecta una colisi�n
				}
				else
				{
					// Si no colisiona, dibujar el punto del trace
					DrawDebugSphere(GetWorld(), Position, 5.0f, 12, FColor::Blue, false, 2.0f);

					if (i == NumPoints)
					{
						bool hit2 = GetWorld()->SweepSingleByChannel(HitResult, Position, Position - FVector(0.f, 0.f, 2000.f), FQuat::Identity, ECC_Visibility, Sphere, QueryParams);
						DrawDebugSphere(GetWorld(), HitResult.Location, 10.0f, 12, FColor::Purple, false, 2.0f);
						float distanceToFall = HitResult.Distance;
						GetWorld()->GetTimerManager().SetTimer(JumpDistances, this, &AGasPlayerController::JumpZVelocity, 0.1f, true);
						DistanceToFloorRecieved = distanceToFall;
					}
				}
			}

			//GetWorld()->GetTimerManager().SetTimer(JumpDistances, this, &AGasPlayerController::JumpDistance, 0.1f, true);
		}
		else if (bIsSliding && bCantJump)
		{
			ResetMovements(rotationToNormalPos, false, meshNormalPosition,CameraBoomInit, 300.f ,false, CharSlideMontage ,ChareEndSlideMontage);//funcion creada para detener AnimMontage Slide y colocarse de pie
			ControlledCharacter->Jump();
			bCanSlide = true;
			GetWorld()->GetTimerManager().SetTimer(JumpDistances, this, &AGasPlayerController::JumpDistance, 0.1f, true);
			GetWorld()->GetTimerManager().ClearTimer(ActSlidePosition);	
		}
		else
		{
			// Si el valor es falso, detener el salto
			ControlledCharacter->StopJumping();
			bJumpinput = false;
			ResetMovements(rotationToNormalPos, false, meshNormalPosition, CameraBoomInit, 300.f, false, CharSlideMontage, nullptr);
			GetWorld()->GetTimerManager().ClearTimer(JumpDistances);
			GetWorld()->GetTimerManager().ClearTimer(ActSlidePosition);
		}
	}	
}
//===========================================================================================
//slide input
void AGasPlayerController::Slide(const FInputActionValue& Value)
{
	
	FVector Velocity = ControlledCharacter->GetCharacterMovement()->Velocity;
	
	FVector meshSlidePosition = FVector(0.f, 0.f, -35.f); //ubicacion relativa de la posicion del mesh para que se acople mejor al suelo. puede modificarse.
	UE_LOG(LogTemp, Warning, TEXT("el input value es: %s"), bCanSlide ? TEXT("true") : TEXT("False"));
	if (bCanSlide && CharSlideMontage && Velocity.Size() > 200.f && !ControlledCharacter->GetCharacterMovement()->IsFalling())
	{
		bIsSliding = true;
		ControlledCharacter->GetMesh()->GetAnimInstance()->Montage_Play(CharSlideMontage);
		ControlledCharacter->GetMesh()->SetRelativeLocation(meshSlidePosition);
		ControlledCharacter->Crouch();
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
		ControlledCharacter->GetCharacterMovement()->SetCrouchedHalfHeight(30.f);
		ControlledCharacter->GetCameraBoom()->SetRelativeLocation(CameraBoomSliding);
	
		GetWorld()->GetTimerManager().SetTimer(ActSlidePosition, this, &AGasPlayerController::tracefloorWhileSliding, 0.01f, true);
	}
	else if (!bCanSlide)
	{
		//ControlledCharacter->GetCapsuleComponent()->SetCapsuleHalfHeight(90.f);
	}
}

//===========================================================================================
// Esta funci�n devuelve una acci�n de entrada (UInputAction) asociada con un ID de entrada de habilidad (EGA_AbilityInputID).
UInputAction* AGasPlayerController::GetInputActionFromAbilityInputID(EGA_AbilityInputID InputID)
{
	static TMap<EGA_AbilityInputID, UInputAction*> InputActionMap = {
		{EGA_AbilityInputID::Ability1, Ability1}, // Asocia Ability1 con su acci�n de entrada correspondiente.
		{EGA_AbilityInputID::Ability2, Ability2},
		{EGA_AbilityInputID::Ability3, Ability3},
		{EGA_AbilityInputID::Ability4, Ability4},
		{EGA_AbilityInputID::Ability5, Ability5},
		// Agregar m�s mapeos seg�n sea necesario
	};

	if (InputActionMap.Contains(InputID))
	{
		return InputActionMap[InputID];
	}

	return nullptr; // Acci�n no encontrada
}
//===========================================================================================
//Ability actions inicializadas sin funcionalidad ya que se activa en el GAS Gameplay ability
void AGasPlayerController::Ability1Action1(){}

void AGasPlayerController::Ability1Action2(){}

void AGasPlayerController::Ability1Action3(){}

void AGasPlayerController::Ability1Action4(){}

void AGasPlayerController::Ability1Action5(){}

//===========================================================================================
//funcion llamada en el OnPosses para obtener unicamente la habilidades del pawn obtenido (GasCharacter) con su listado de habilidades asociados a los inputs 
void AGasPlayerController::PermissionAbilityInputs(AGasCharacter* character)
{
	
	if (character && character->AbilitySystemComponenet)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ASC"));
		for (TSubclassOf<UGA_Ere> AbilityClass : character->StartingAbilities)//Habilidades asociadas en el array del GasCharacter.
		{
			if (AbilityClass)
			{
		
				const UGA_Ere* DefaultAbility = AbilityClass.GetDefaultObject();

				if (DefaultAbility && DefaultAbility->AbilityInputID != EGA_AbilityInputID::None)
				{
					// Obtener el InputAction asociada
					UInputAction* InputAction = GetInputActionFromAbilityInputID(DefaultAbility->AbilityInputID);
					if (InputAction)
					{
						const int32 InputID = static_cast<int32>(DefaultAbility->AbilityInputID);

						// Vincular la acci�n con los eventos
						EInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &AGasGasPlayerController::OnAbilityInputPressed, InputID);
						EInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &AGasGasPlayerController::OnAbilityInputReleased, InputID);

						//UE_LOG(LogTemp, Warning, TEXT("Acci�n vinculada para Habilidad: %s con InputID: %d"),
							*AbilityClass->GetName(),
							InputID);
					}
				}
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("no abilities"));
				return;

			}
			
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("no ability system component"));

	}
}

//===========================================================================================
//DEPRECATED
void AGasGasPlayerController::SendAbilityLocalInput(const FInputActionValue& Value, int32 InputID)
{
	if (!ControlledCharacter->AbilitySystemComponenet)
	{
		return;
	}
	if(Value.Get<bool>())
	{
		AbilitySystemComponent->AbilityLocalInputPressed(InputID);
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputPressed(InputID);
	}
}
//===========================================================================================
//funcion de replicacion de variables.
//Replicacion cliente servidor, multicast. para obtener la direccion de apuntado del GasCharacter.
void AGasGasPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGasGasPlayerController, AimDirection);
}
//===========================================================================================
bool AGasGasPlayerController::ServerSetAimDirection_Validate(const FVector& NewAimDirection)
{
	return true;
}

void AGasGasPlayerController::ServerSetAimDirection_Implementation(const FVector& NewAimDIrection)
{
	AimDirection = NewAimDIrection;
}

//===========================================================================================
//Deprecated
void AGasGasPlayerController::OnAbilityInputPressed(int32 InputID)
{
	if (ControlledCharacter->AbilitySystemComponenet)
	{
		ControlledCharacter->AbilitySystemComponenet->AbilityLocalInputPressed(InputID);
	}
}
//===========================================================================================
//Deprecated
void AGasGasPlayerController::OnAbilityInputReleased(int32 InputID)
{
	if (ControlledCharacter->AbilitySystemComponenet)
	{
		//AbilitySystemComponent->AbilityLocalInputReleased(InputID);
	}

}
//===========================================================================================
void AGasGasPlayerController::ActForward()
{
	AimDirection =  ControlledPawn->GetActorForwardVector();
}
//===========================================================================================
//obtener la colision con el suelo cuando activamos el Slide para llamar funciones de alineacion con la superficie y sus normales.
void AGasGasPlayerController::tracefloorWhileSliding()
{
	FVector StartPos = ControlledCharacter->GetActorLocation();
	FVector EndPos = ControlledCharacter->GetActorLocation();
	float CapsuleRadius = 4.f;
	float CapsuleHalfHeight = 90.f;
	ECollisionChannel CollisionChannel = ECC_Visibility;
	FCollisionQueryParams CollisionParams;
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(ControlledCharacter);
	CollisionParams.AddIgnoredActors(IgnoredActors);
	FHitResult HitResult;

	bool bHit = CapsuleTrace(HitResult ,StartPos, EndPos, CapsuleRadius, CapsuleHalfHeight, CollisionChannel, CollisionParams);
	

	if (bHit)
	{
		//DrawDebugCapsule(GetWorld(), StartPos, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, FColor::Green, false, 1.0f, 0, 1.0f);
		allignPlayerToFloor();
		FootFrwContact();
		//FindCurrentFloorAngleAndDirection();
		
	}
	else
	{
		
		//DrawDebugCapsule(GetWorld(), StartPos, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, FColor::Red, false, 1.0f, 0, 1.0f);
		bIsSliding = false;
		ControlledCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, CharSlideMontage);
		GetWorld()->GetTimerManager().ClearTimer(ActSlidePosition);

		
	}

	

}
//===========================================================================================
//obtencion del suelo colisionado para obtener su inclinacion.
void AGasGasPlayerController::allignPlayerToFloor()
{
	const FFindFloorResult& CurrentFloor = ControlledCharacter->GetCharacterMovement()->CurrentFloor;

	FVector ImpactNorm = CurrentFloor.HitResult.ImpactNormal.GetSafeNormal();
	FRotator ActorRot = ControlledCharacter->GetActorRotation();
	FVector ActorFrwVector = ControlledCharacter->GetActorForwardVector();
	FMatrix RotMatrix = FRotationMatrix::MakeFromZX(ImpactNorm, ActorFrwVector);
	FRotator interPolResult = FMath::RInterpTo(ActorRot,RotMatrix.Rotator(), GetWorld()->GetDeltaSeconds(), 2.f);
	ControlledCharacter->SetActorRotation(interPolResult);
}
//===========================================================================================
//obtener colisiones frontales si el personale choca cuando se esta deslizando.
void AGasGasPlayerController::FootFrwContact()
{
	const FFindFloorResult& CurrentFloor = ControlledCharacter->GetCharacterMovement()->CurrentFloor;
	AActor* hitActor = CurrentFloor.HitResult.GetActor();
	 USkeletalMeshComponent* mesh = ControlledCharacter->GetMesh();
	 FVector BoneNameLocation = mesh->GetSocketLocation(TEXT("foot_l"));
	 FVector TraceZoffset = FVector(0.f, 0.f, -10.f);
	 float capsuleRadius =30.f;
	 ECollisionChannel collsionChannel = ECC_Visibility;
	 FCollisionQueryParams CollisionParams;
	 TArray<AActor*> IgnoredActors;
	//IgnoredActors.Add(hitActor);
	IgnoredActors.Add(ControlledCharacter);
	 CollisionParams.AddIgnoredActors(IgnoredActors);
	 FVector VectorVelocity = ControlledCharacter->GetVelocity();
	 float Velocity = VectorVelocity.Size();

	
	  
	 FHitResult HitResult;
	 bool bHitSphere = SphereTrace(HitResult, BoneNameLocation + TraceZoffset, BoneNameLocation + TraceZoffset, capsuleRadius, collsionChannel, CollisionParams);
	 bool bhitFall = GetWorld()->LineTraceSingleByChannel(HitResult, BoneNameLocation, BoneNameLocation + FVector(0.f, 0.f, -50.f), collsionChannel, CollisionParams);
	 // crear un drawdebugline para que contecte hace abajo y si hay demasiada distancia que oueda caer en su animacion y volver al uncrouch
	 DrawDebugSphere(GetWorld(), BoneNameLocation + TraceZoffset,capsuleRadius, 12, bHitSphere ? FColor::Blue : FColor::Red, false, 1.0f);
	 DrawDebugLine(GetWorld(), BoneNameLocation, BoneNameLocation + FVector(0.f, 0.f, -100.f), bhitFall ? FColor::Green : FColor::Red, false, 1.f);



	 if (bHitSphere)
	 {
		 FVector ImpactNormal = HitResult.ImpactNormal;  // Obt�n la normal del impacto
		 FVector ImpactPoint = HitResult.ImpactPoint;    // Obt�n el punto de impacto
		
		 if (!ImpactNormal.IsZero())
		 {
			 FVector UpVector = FVector(0.f, 0.f, 1.f);  // Direcci�n "arriba"
			 float DotProduct = FVector::DotProduct(ImpactNormal, UpVector);  // Producto punto entre las dos direcciones
			 float AngleRadians = FMath::Acos(FMath::Abs( DotProduct));  // Convierte el producto punto en un �ngulo (en radianes)
			 float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);  // Convierte a grados

			 // Imprime el �ngulo y otros detalles
			/* UE_LOG(LogTemp, Warning, TEXT("Impact Normal: %s"), *ImpactNormal.ToString());
			 UE_LOG(LogTemp, Warning, TEXT("Impact Point: %s"), *ImpactPoint.ToString());
			 UE_LOG(LogTemp, Warning, TEXT("El �ngulo de la pendiente es: %f grados"), AngleDegrees);*/

			 if (AngleDegrees > 60.f)
			 {
				 FVector meshNormalPosition = FVector(0.f, 0.f, 0.f);
				 // rotacion Vector hacia adelante
				 FRotator ForwardRotation = ControlledCharacter->GetActorForwardVector().Rotation();
				 FRotator rotationToNormalPos = FRotator(0.0f, ForwardRotation.Yaw, 0.0f);
				 ResetMovements(rotationToNormalPos, false, meshNormalPosition, CameraBoomInit, 300.f, false, nullptr ,ChareEndSlideMontage);
				/* ControlledCharacter->SetActorRotation(rotationToNormalPos);
				 ControlledCharacter->UnCrouch();
				 bIsSliding = false;
				 ControlledCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;*/
				 GetWorld()->GetTimerManager().ClearTimer(ActSlidePosition);
				 DrawDebugSphere(GetWorld(), BoneNameLocation + TraceZoffset, capsuleRadius, 12, FColor::Red, false, 1.0f);
				 UE_LOG(LogTemp, Warning, TEXT(" stop por angulos"))
			 }
			
		 } 
	 }
	 //control de velocidad para que no se pueda deslizar si la velocidad es menor a.....
	 if (Velocity <=150.f)
	 {
		
		 FVector meshNormalPosition = FVector(0.f, 0.f, -97.f);
		 CameraBoomInit = FVector(0.f, 0.f, 0.f);
		 ControlledCharacter->GetMesh()->GetAnimInstance()->Montage_Play(ChareEndSlideMontage, 1.f);
		 // rotacion Vector hacia adelante
		 FRotator ForwardRotation = ControlledCharacter->GetActorForwardVector().Rotation();
		 FRotator rotationToNormalPos = FRotator(0.0f, ForwardRotation.Yaw, 0.0f);
		 ControlledCharacter->SetActorRotation(rotationToNormalPos);
		 ControlledCharacter->UnCrouch();
		 ControlledCharacter->GetCameraBoom()->SetRelativeLocation(CameraBoomInit);
		 ControlledCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
		 bIsSliding = false;
		 GetWorld()->GetTimerManager().ClearTimer(ActSlidePosition);
		 UE_LOG(LogTemp, Warning, TEXT(" stop por velocidad"))
	 }
	 // si no esta colisionando con nada detener el slide (ejem. deslizarse en una superficie con altura y caer fuera de ella)
	 else if(!bHitSphere && !bhitFall)
	 {
		 FRotator ForwardRotation = ControlledCharacter->GetActorForwardVector().Rotation();
		 FRotator rotationToNormalPos = FRotator(0.0f, ForwardRotation.Yaw, 0.0f);
		 ControlledCharacter->SetActorRotation(rotationToNormalPos);
		 ControlledCharacter->UnCrouch();
		 ControlledCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
		  ControlledCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.4f, CharSlideMontage);
		  bIsSliding = false;
		  GetWorld()->GetTimerManager().ClearTimer(ActSlidePosition);
		  UE_LOG(LogTemp, Warning, TEXT(" stop por falta de colision"))
	 }
}
//===========================================================================================
bool AGasGasPlayerController::CapsuleTrace(FHitResult HitResult, FVector startPosition, FVector EndPosition, float capsuleRadius, float capsuleHalfHeight, ECollisionChannel collisionChannel, FCollisionQueryParams collisionparams)
{

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,         // Resultado del hit
		startPosition,             // Posici�n de inicio
		EndPosition,               // Posici�n final
		FQuat::Identity,   // Rotaci�n (sin rotaci�n en este caso)
		collisionChannel,  // Canal de colisi�n
		FCollisionShape::MakeCapsule(capsuleRadius, capsuleHalfHeight), // Forma de la c�psula
		collisionparams);    // Par�metros de colisi�n

	return bHit;
}
//===========================================================================================
bool AGasGasPlayerController::SphereTrace(FHitResult HitResult, FVector startPosition, FVector EndPosition, float capsuleRadius, ECollisionChannel collisionChannel, FCollisionQueryParams collisionparams)
{
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,         // Resultado del hit
		startPosition,             // Posici�n de inicio
		EndPosition,               // Posici�n final
		FQuat::Identity,   // Rotaci�n (sin rotaci�n en este caso)
		collisionChannel,  // Canal de colisi�n
		FCollisionShape::MakeSphere(capsuleRadius), // Forma de la c�psula
		collisionparams);    // Par�metros de colisi�n

	return bHit;
}

//===========================================================================================
//Obtener el angulo de la normal de la superficie.
void AGasGasPlayerController::FindCurrentFloorAngleAndDirection()
{
	const FFindFloorResult& CurrentFloor = ControlledCharacter->GetCharacterMovement()->CurrentFloor;
	FVector ImpactNormal=  CurrentFloor.HitResult.ImpactNormal;
	FVector Velocity = ControlledCharacter->GetVelocity();
	float velocityChar = Velocity.Size();
	Velocity.Normalize();
	ImpactNormal.Normalize();
	float CurrentSpeed;
	//UE_LOG(LogTemp, Warning, TEXT("dotproduct es: %f"), DotProduct);
	if (bIsSliding)
	{
		float DotProduct = FVector::DotProduct(Velocity, ImpactNormal);
		float slopeAngle = FMath::Acos(FMath::Abs(FVector::DotProduct(ImpactNormal, FVector(0.f, 0.f, 1.f)))) * 180 / PI;
		int decimalPlaces = 2;
		float factor = (FMath::Pow(10.f, decimalPlaces));
		float Truncated = FMath::TruncToFloat(DotProduct * factor) / factor;
		
		if (Truncated < 0.f)
		{
			float targetSpeed = velocityChar - (slopeAngle *1000);
			CurrentSpeed = FMath::FInterpTo(velocityChar, targetSpeed, GetWorld()->GetDeltaSeconds(), 1.f);
		
		}
		else if (Truncated > 0.f)
		{
			float targetSpeed = velocityChar + slopeAngle *15;
			CurrentSpeed = FMath::FInterpTo(velocityChar, targetSpeed, GetWorld()->GetDeltaSeconds(), 1.f);
			
		}
		else if (Truncated == 0.f)
		{
			float targetSpeed = velocityChar - 1000;
			CurrentSpeed = FMath::FInterpTo(velocityChar, targetSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
			
		}
		NewVel = Velocity * CurrentSpeed;
		changeVelCrouch = NewVel.Size();
		ControlledCharacter->GetCharacterMovement()->Velocity = NewVel;
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = changeVelCrouch;  
		UE_LOG(LogTemp, Warning, TEXT("subida: Velocidad actualizada: %f"), changeVelCrouch);

		
		
		
	}
	else
	{
		CurrentSpeed = FMath::FInterpTo(velocityChar, ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed, GetWorld()->GetDeltaSeconds(), 0.f);
		changeVelCrouch = ControlledCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched;
	}
	
}
//===========================================================================================
//Funion para manejar el crouch y UnCrouch con mas control AUN NO IMPLEMENTADA
void AGasGasPlayerController::CrouchUnCrouchPosition(FVector CameraBoomPositionObj, FVector ActualCameraBoomPosition,  float interpSeconds)
{
	CameraBoomPositionObj = FMath::VInterpTo(CameraBoomPositionObj, ActualCameraBoomPosition, GetWorld()->GetDeltaSeconds(), 2.0f);
}
//===========================================================================================
//Creacion de una parabola simple para predecir donde va a caer el personaje (para el parkour system)
void AGasGasPlayerController::JumpDistance()
{
	FVector StartLocation = ControlledCharacter->GetActorLocation();

	// Configuraci�n de trazados
	float TraceDistanceDown = 1000.0f; // Distancia m�xima hacia abajo

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

		// Solo actualiza la distancia m�xima si es v�lida
		if (CurrentDistance > MaxDistanceToGround && FrontDiagonalDistance > 600.f)
		{
			MaxDistanceToGround = CurrentDistance;

			// Muestra la nueva distancia m�xima en pantalla
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Nueva distancia m�xima al suelo: %f"), MaxDistanceToGround));
		}
		if (MaxDistanceToGround > 400)
		{
			bCanSlide = false;
		}

		// Dibuja el trazado hacia abajo para depuraci�n
		//DrawDebugLine(GetWorld(), StartLocation, HitResultDown.ImpactPoint, FColor::Blue, false, 2.0f, 0, 1.0f);
	}


	FVector ForwardVector = ControlledCharacter->GetActorForwardVector(); // Direcci�n hacia adelante
	FVector UpVector = FVector(0, 0, -1); // Direcci�n hacia arriba (Z)

	float ForwardDistance = 1500.f; // Distancia m�xima hacia adelante
	float DiagonalAngle = 0.5f; // Ajuste de inclinaci�n (0.5 = 45 grados)

	// Direcci�n diagonal combinando adelante y arriba
	FVector DiagonalDirection = ForwardVector + (UpVector * DiagonalAngle);
	DiagonalDirection.Normalize(); // Normalizar para mantener la magnitud uniforme

	// Posici�n final del trazado
	FVector EndLocationDiagonal = StartLocation + (DiagonalDirection * ForwardDistance);

	// Configuraci�n del Line Trace
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

		// Dibuja el trazado para depuraci�n
		//DrawDebugLine(GetWorld(), StartLocation, HitResultDiagonal.ImpactPoint, FColor::Purple, false, 2.0f, 0, 1.0f);
	}
}
//===========================================================================================
//obtencion de la velicidad en Z para reprodicir diferentes tipos de Aterrizajes (AnimationBlueprint(ABP))
void AGasGasPlayerController::JumpZVelocity()
{
	JumpVelocityZ = ControlledCharacter->GetCharacterMovement()->Velocity.Z;

	//UE_LOG(LogTemp, Warning, TEXT("velocidad Z es  %f:"), JumpVelocityZ);

	if (DistanceToFloorRecieved > 300.f && JumpVelocityZ < -800.f)
	{
		MaxDistanceToGround = DistanceToFloorRecieved;
		//UE_LOG(LogTemp, Warning, TEXT("Distancia %f:"), MaxDistanceToGround);
	}
	else if (JumpVelocityZ < -800.f)
	{
		MaxDistanceToGround = 301.f;
	}

	else if (!ControlledCharacter->GetCharacterMovement()->IsFalling())
	{
		GetWorld()->GetTimerManager().ClearTimer(JumpDistances);
	}
}
//===========================================================================================
//Funcion para controlar el crouch uncrouch del personaje en el slide (Implementando funcion en algunos casos) TESTEO
void AGasGasPlayerController::ResetMovements(FRotator RotationToFrwPosition, bool crouch, FVector charMeshSlidePosition,FVector cameraPosition, float crouchSpeed, bool IsSliding , UAnimMontage* AnimStop,UAnimMontage* AnimPlay)
{
	ControlledCharacter->SetActorRotation(RotationToFrwPosition);
	if (!crouch)
	{
		ControlledCharacter->UnCrouch();
		ControlledCharacter->Jump();
		ControlledCharacter->GetMesh()->SetRelativeLocation(charMeshSlidePosition);
		ControlledCharacter->GetCameraBoom()->SetRelativeLocation(cameraPosition);
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = crouchSpeed;
		bIsSliding = IsSliding;
		if (AnimStop)
		{
			ControlledCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.3f, AnimStop);
		}
		if (AnimPlay)
		{
			ControlledCharacter->GetMesh()->GetAnimInstance()->Montage_Play(AnimPlay, 1.f);
		}
		GetWorld()->GetTimerManager().SetTimer(JumpDistances, this, &AGasGasPlayerController::JumpDistance, 0.1f, true);
		GetWorld()->GetTimerManager().ClearTimer(ActSlidePosition);
	}
	else
	{
		return;
	}
	
}
//===========================================================================================












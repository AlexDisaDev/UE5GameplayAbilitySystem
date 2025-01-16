// Fill out your copyright notice in the Description page of Project Settings.


#include "GasPlayerState.h"
#include "GasCharacter.h"

AGasPlayerState::AGasPlayerState()
{

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

    AttributeSet = CreateDefaultSubobject<UAS_Ere>(TEXT("attribute Set"));; //CreateDefaultSubobject<UAS_Ere>(TEXT("Attribute Set"));
   

    NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* AGasPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

UAS_Ere* AGasPlayerState::GetAttributeSet() const
{
    return AttributeSet;
}

bool AGasPlayerState::IsAlive() const
{
    return GetHealth() > 0.f;
}

bool AGasPlayerState::IsDead() const
{
    return GetHealth() <= 0.f;
}

float AGasPlayerState::GetHealth() const
{
    return GetAttributeSet()->GetHealth();
}

float AGasPlayerState::GetMaxHealth() const
{
    return GetAttributeSet()->GetMaxHealth();
}

float AGasPlayerState::GetHealthRegen() const
{
    return GetAttributeSet()->GetHealthRegen();
}

float AGasPlayerState::GetInput() const
{
    return GetAttributeSet()->GetInput();
}

void AGasPlayerState::BeginPlay()
{
    Super::BeginPlay();
       
     if (IsValid(AbilitySystemComponent))
     {
         HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAttributeSet()->GetHealthAttribute()).AddUObject(this, &AGasPlayerState::OnHealthChanged);
         InputChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAttributeSet()->GetInputAttribute()).AddUObject(this, &AGasPlayerState::OnInputChanged);
     }
}

void AGasPlayerState::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    if (!IsAlive() && IsValid(AbilitySystemComponent) && GetLocalRole() == ROLE_Authority)
    {
        AGasCharacter* CharacterRef = Cast<AGasCharacter>(GetPawn());
        if (IsValid(CharacterRef))
        {
            AActor* KillerActor = Data.GEModData->EffectSpec.GetEffectContext().GetEffectCauser();
            if (IsValid(KillerActor))
            {
                AGasCharacter* killerCharacter = Cast<AGasCharacter>(KillerActor);
                if (IsValid(killerCharacter))
                {
                    CharacterRef->serverDie(killerCharacter);
                }
            }

        }
    }
}

void AGasPlayerState::OnInputChanged(const FOnAttributeChangeData& Data)
{
    if (!PlayerController) 
    {
        PlayerController = Cast<APlayerController>(GetOwner());
        
        if (PlayerController && GetInput() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Owner PlayerController %s"), *PlayerController->GetName());

            PlayerController->DisableInput(PlayerController);
            UE_LOG(LogTemp, Warning, TEXT("inputdisable"))

        }
        else if (PlayerController && GetInput() == 1)
        {
            PlayerController->EnableInput(PlayerController);
            UE_LOG(LogTemp, Warning, TEXT("inputEnableble"))
        }
    }
    else
    {
        if (PlayerController && GetInput() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Owner PlayerController 2 %s"), *PlayerController->GetName());
            PlayerController->DisableInput(PlayerController);
            UE_LOG(LogTemp, Warning, TEXT("inputdisable"))

        }
        else if (PlayerController && GetInput() == 1)
        {
            PlayerController->EnableInput(PlayerController);
            UE_LOG(LogTemp, Warning, TEXT("inputEnableble"))
        }
    }
   
   
   
}



void AGasPlayerState::ScoreKill()
{
    killCount++;
}

void AGasPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->CancelAbilities(nullptr);
    }
}

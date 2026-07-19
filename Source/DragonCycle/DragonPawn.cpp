#include "DragonPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DragonAbilitySystemComponent.h"
#include "DragonAttributeSet.h"
#include "DragonBodyStateComponent.h"
#include "DragonSpeciesDataAsset.h"

ADragonPawn::ADragonPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    DragonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DragonMesh"));
    SetRootComponent(DragonMesh);
    DragonMesh->SetCollisionProfileName(TEXT("Pawn"));

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(DragonMesh);
    CameraBoom->TargetArmLength = 1200.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 6.0f;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    AbilitySystem = CreateDefaultSubobject<UDragonAbilitySystemComponent>(TEXT("AbilitySystem"));
    AbilitySystem->SetIsReplicated(false);

    Attributes = CreateDefaultSubobject<UDragonAttributeSet>(TEXT("Attributes"));
    BodyState = CreateDefaultSubobject<UDragonBodyStateComponent>(TEXT("BodyState"));
}

void ADragonPawn::BeginPlay()
{
    Super::BeginPlay();

    AbilitySystem->InitAbilityActorInfo(this, this);
    if (SpeciesData)
    {
        BodyState->InitializeFromSpecies(SpeciesData);
    }
}

UAbilitySystemComponent* ADragonPawn::GetAbilitySystemComponent() const
{
    return AbilitySystem;
}

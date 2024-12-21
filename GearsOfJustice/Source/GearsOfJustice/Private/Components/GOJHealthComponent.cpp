// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/GOJHealthComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogGOJHealthComponent, All, All);

UGOJHealthComponent::UGOJHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UGOJHealthComponent::IsHealthFull() const
{
    return Health == MaxHealth;
}

void UGOJHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    check(MaxHealth > 0);

    SetHealth(MaxHealth);

    AActor* ComponentOwner = GetOwner();
    if (ComponentOwner)
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &UGOJHealthComponent::OnTakeAnyDamage);
    }
}

void UGOJHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGOJHealthComponent::OnTakeAnyDamage(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || IsDead() || !GetWorld()) return;

    SetHealth(Health - Damage);

    SetIsUnderAtack(true);
    GetWorld()->GetTimerManager().SetTimer(UnderAttackTimerHandle, this, &UGOJHealthComponent::OnUnderAttackTimerFinished, UnderAttackDelay);

    if (IsDead())
    {
        OnDeath.Broadcast();
    }
}

void UGOJHealthComponent::SetHealth(float NewHealth)
{
    const float NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    const float HealthDelta = NextHealth - Health;

    Health = NextHealth;
    OnHealthChanged.Broadcast(NextHealth, HealthDelta);
}

void UGOJHealthComponent::Dead(AController* KillerController)
{
    if (!GetWorld()) return;
}

void UGOJHealthComponent::OnUnderAttackTimerFinished()
{
    SetIsUnderAtack(false);
    UE_LOG(LogGOJHealthComponent, Display, TEXT("bIsUnderAttack was changed to: %s"),
        bIsUnderAttack ? TEXT("True") : TEXT("False"));
}

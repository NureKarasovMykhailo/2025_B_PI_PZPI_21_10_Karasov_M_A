// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/GOJMoveToPlayerAndAttackTask.h"
#include "AIController.h"
#include "AI/GOJAICharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/GOJCombatComponent.h"
#include "Engine/World.h"
#include "NavigationSystem.h"

UGOJMoveToPlayerAndAttackTask::UGOJMoveToPlayerAndAttackTask() 
{
    NodeName = TEXT("Move To Player And Attack");
}

EBTNodeResult::Type UGOJMoveToPlayerAndAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // �������� AIController � ���������
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    AGOJBaseCharacter* OwnerCharacter = Cast<AGOJBaseCharacter>(AIController->GetPawn());
    if (!OwnerCharacter) return EBTNodeResult::Failed;

    // �������� ������ �� ������
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return EBTNodeResult::Failed;

    // ��������� ���������� �� ������
    float DistanceToPlayer = FVector::Dist(OwnerCharacter->GetActorLocation(), PlayerPawn->GetActorLocation());

    // ���� �� ���������� ������, ��������� ����
    if (DistanceToPlayer <= AttackRange)
    {
        // �������� �� cooldown �����
        if (GetWorld()->GetTimeSeconds() - LastAttackTime >= AttackCooldown)
        {
            // ��������� ����
            PerformAttack(AIController, OwnerCharacter);

            // ������������� ����� ��������� �����
            LastAttackTime = GetWorld()->GetTimeSeconds();

            return EBTNodeResult::Succeeded;
        }
    }
    else
    {
        // ���� �� ������� ������, ���������� ��������
        AIController->MoveToActor(PlayerPawn);
    }

    return EBTNodeResult::InProgress;
}

void UGOJMoveToPlayerAndAttackTask::PerformAttack(AAIController* AIController, AGOJBaseCharacter* OwnerCharacter)
{
    UGOJCombatComponent* CombatComponent = OwnerCharacter->FindComponentByClass<UGOJCombatComponent>();
    if (CombatComponent)
    {
        // �������� ��������� ��� �����
        EStrikeType RandomStrike = static_cast<EStrikeType>(FMath::RandRange(0, 2));
        CombatComponent->PerformStrike(RandomStrike);
    }
}

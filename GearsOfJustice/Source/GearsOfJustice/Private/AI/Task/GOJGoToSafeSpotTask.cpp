// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/GOJGoToSafeSpotTask.h"
#include "AIController.h"
#include "AI/GOJAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "Navigation/PathFollowingComponent.h"


UGOJGoToSafeSpotTask::UGOJGoToSafeSpotTask()
{
    NodeName = "Move To Safe Location";
    bNotifyTick = true;  // �������� ����� TickTask
}

EBTNodeResult::Type UGOJGoToSafeSpotTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const auto Controller = OwnerComp.GetAIOwner();
    if (!Controller) return EBTNodeResult::Failed;

    ControlledPawn = Controller->GetPawn();
    if (!ControlledPawn) return EBTNodeResult::Failed;

    const auto Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard) return EBTNodeResult::Failed;

    // �������� ���������� ����� �� Blackboard
    SafeLocation = Blackboard->GetValueAsVector("SafeLocation");

    // �������� ������
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return EBTNodeResult::Failed;

    // ������� MoveTo ��� ��
    Controller->MoveToLocation(SafeLocation);

    return EBTNodeResult::InProgress;  // ��������� � ��������� ����������
}

void UGOJGoToSafeSpotTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (!ControlledPawn || !PlayerPawn) return;

    // ��������� ������������ �� � ������� ������
    FVector DirectionToPlayer = PlayerPawn->GetActorLocation() - ControlledPawn->GetActorLocation();
    DirectionToPlayer.Z = 0.0f;  // ���������� ��� Z ��� �������� �� ���������

    FRotator NewRotation = DirectionToPlayer.Rotation();
    ControlledPawn->SetActorRotation(FMath::RInterpTo(ControlledPawn->GetActorRotation(), NewRotation, DeltaSeconds, 5.0f));

     //�������� ���������� ��������
    const auto Controller = OwnerComp.GetAIOwner();
    if (Controller && Controller->GetMoveStatus() == EPathFollowingStatus::Idle)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
    
}


#include "AI/Task/GOJMoveToPlayerTask.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "AI/GOJAICharacter.h"
#include "Kismet/GameplayStatics.h"

UGOJMoveToPlayerTask::UGOJMoveToPlayerTask()
{
    NodeName = "Move to Player";
    bNotifyTick = true;  // �������� ����������� �������� TickTask
}

EBTNodeResult::Type UGOJMoveToPlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // �������� AIController
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    // �������� ��������� ��
    AGOJAICharacter* AICharacter = Cast<AGOJAICharacter>(AIController->GetPawn());
    if (!AICharacter) return EBTNodeResult::Failed;

    // �������� ������
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(AICharacter->GetWorld(), 0);
    if (!PlayerCharacter) return EBTNodeResult::Failed;

    // ������������ ��������� �� ������
    float DistanceToPlayer = FVector::Dist(AICharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());

    // ���������, ��������� �� �� �� ������ ��������� ��� �����
    const float AdjustedAttackRange = AICharacter->GetSmallestAttackRange() - 10.0f;  // ��������� ������ �� 10 ������
    if (DistanceToPlayer <= AdjustedAttackRange)
    {
        AIController->StopMovement();
        return EBTNodeResult::Succeeded;
    }

    // ������� ������ �� �������� � ������
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(PlayerCharacter);
    MoveRequest.SetAcceptanceRadius(AdjustedAttackRange);  // ������������� ����������������� ������

    // �������� ��������� ��������
    FNavPathSharedPtr NavPath;
    EPathFollowingRequestResult::Type MoveResult = AIController->MoveTo(MoveRequest, &NavPath);

    if (MoveResult == EPathFollowingRequestResult::Failed)
    {
        return EBTNodeResult::Failed;
    }

    return EBTNodeResult::InProgress;
}

void UGOJMoveToPlayerTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    // �������� AIController
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // �������� ��������� ��
    AGOJAICharacter* AICharacter = Cast<AGOJAICharacter>(AIController->GetPawn());
    if (!AICharacter)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // �������� ������
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(AICharacter->GetWorld(), 0);
    if (!PlayerCharacter)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // ������������ ��������� �� ������
    float DistanceToPlayer = FVector::Dist(AICharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());

    // ���������, ������ �� �� ����������������� ���������
    const float AdjustedAttackRange = AICharacter->GetSmallestAttackRange() - 10.0f;  // ��������� ������ �� 10 ������
    if (DistanceToPlayer <= AdjustedAttackRange)
    {
        AIController->StopMovement();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // ��������� ������ ��������
    const auto PathFollowingComponent = AIController->GetPathFollowingComponent();
    if (PathFollowingComponent && PathFollowingComponent->GetStatus() == EPathFollowingStatus::Idle)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
}

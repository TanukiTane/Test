#include "Widget/ACPP_NotificationWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ACPP_Character.h"
#include "Actor/ACPP_TaskComponent.h"

void UACPP_NotificationWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // ѕытаемс€ найти персонажа игрока и его компонент задач
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    if (UACPP_TaskComponent* TaskComp = PlayerPawn->FindComponentByClass<UACPP_TaskComponent>())
    {
        TaskComp->OnTaskStateChanged.AddDynamic(this, &UACPP_NotificationWidget::OnTaskStateUpdated);

        FTaskData ActiveTask = TaskComp->GetActiveTask();
        if (!ActiveTask.Task.IsNone() && ActiveTask.State == ETaskState::Started)
        {
            OnTaskStateUpdated(ActiveTask, ETaskState::Started);
        }
    }
}

void UACPP_NotificationWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
    Super::OnAnimationFinished_Implementation(Animation);

   if (Animation == FadeInOutAnimation && bIsFinalTaskCompleted)
    {
        bIsFinalTaskCompleted = false; // —брасываем флаг безопасности

        AACPP_Character* Char = Cast<AACPP_Character>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
        if (Char)
        {
            Char->PlayAnimation(); 
        }
    }
}

void UACPP_NotificationWidget::OnTaskStateUpdated(FTaskData Task, ETaskState NewState)
{
    if (!TaskText || !FadeInOutAnimation) return;

    if (NewState == ETaskState::Started)
    {
        TaskText->SetText(Task.TaskDescription);
        SetVisibility(ESlateVisibility::SelfHitTestInvisible);

        PlayAnimation(FadeInOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
    }
    else if (NewState == ETaskState::Completed)
    {
        if (bIsFinalTaskProcessed) return;

        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            if (UACPP_TaskComponent* TaskComp = PlayerPawn->FindComponentByClass<UACPP_TaskComponent>())
            {
                if (TaskComp->IsFinalTask(Task))
                {
                    bIsFinalTaskCompleted = true;
                    bIsFinalTaskProcessed = true;
                }
            }
        }

        FText Pattern = FText::FromString(TEXT("Completed: {0}"));
        FText CompletedText = FText::Format(Pattern, Task.TaskDescription);
        TaskText->SetText(CompletedText);

        PlayAnimation(FadeInOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);

       // FTimerHandle HideTimerHandle;
        //GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, [this]()
          //  {
            //    if (FadeInOutAnimation)
              //  {
                //    PlayAnimation(FadeInOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
                //}
            //}, 3.0f, false);
    }
}
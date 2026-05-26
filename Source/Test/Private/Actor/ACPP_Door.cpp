#include "Actor/ACPP_Door.h"
#include "Blueprint/UserWidget.h"
#include "Actor/ACPP_TaskComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/ACPP_KeypadWidget.h"

AACPP_Door::AACPP_Door()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(Root);
}

void AACPP_Door::BeginPlay()
{
	Super::BeginPlay();	
}

void AACPP_Door::OnInteract_Implementation(AActor* Interactor)
{
    // Если виджет уже открыт — ничего не делаем
    if (ActiveKeypadWidget || !KeypadWidget) return;

    APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (PC)
    {
        // Создаем виджет ввода пароля и выводим на экран
        if (UACPP_KeypadWidget* Keypad = CreateWidget<UACPP_KeypadWidget>(PC, KeypadWidget))
        {
            ActiveKeypadWidget = Keypad;
            Keypad->Door = this;
            Keypad->AddToViewport();

            // Переводим управление на мышь/UI
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(ActiveKeypadWidget->TakeWidget());
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
    }
}

bool AACPP_Door::CheckPassword(const FString& EnteredCode, AActor* Interactor)
{
    APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

    if (EnteredCode != CorrectPassword)
    {
        return false;
    }

    OpenDoorTimeline();

    if (ActiveKeypadWidget)
    {
        ActiveKeypadWidget->RemoveFromParent();
        ActiveKeypadWidget = nullptr;
    }

    if (PC)
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }

    if (Interactor)
    {
        if (UACPP_TaskComponent* TaskComp = Interactor->FindComponentByClass<UACPP_TaskComponent>())
        {
            // Дверь сообщает комноненту о выполнении текущей активной задачи.
            FTaskData ActiveTask = TaskComp->GetActiveTask();
            if (!ActiveTask.Task.IsNone())
            {
                TaskComp->CompleteTask(ActiveTask.Task);
            }
        }
    }
    return true;
}
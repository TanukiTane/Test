#include "Actor/ACPP_WorldMarker.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ACPP_Character.h"
#include "Actor/ACPP_TaskComponent.h"

// Sets default values
AACPP_WorldMarker::AACPP_WorldMarker()
{ 	
	PrimaryActorTick.bCanEverTick = false;

    USceneComponent* NewRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = NewRoot;
       
    MarkerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MarkerWidgetComponent"));
    MarkerWidgetComponent->SetupAttachment(RootComponent);

    MarkerWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    MarkerWidgetComponent->SetDrawAtDesiredSize(true);

    // Маркер скрыт до начала миссии
    MarkerWidgetComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void AACPP_WorldMarker::BeginPlay()
{
	Super::BeginPlay();
	
    // Подписка на список задач
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        if (UACPP_TaskComponent* TaskComp = PlayerPawn->FindComponentByClass<UACPP_TaskComponent>())
        {
            TaskComp->OnTaskStateChanged.AddDynamic(this, &AACPP_WorldMarker::HandleTaskChanged);

            // Если игра уже идет (интро завершено) и эта задача активна — показываем маркер
            FTaskData ActiveTask = TaskComp->GetActiveTask();
            if (ActiveTask.Task == TargetTaskName && ActiveTask.State == ETaskState::Started)
            {
                MarkerWidgetComponent->SetVisibility(true);
            }
        }
    }
}

void AACPP_WorldMarker::HandleTaskChanged(FTaskData Task, ETaskState NewState)
{
    if (!MarkerWidgetComponent) return;

    //Сверка имени текущей миссии с именем, назначенным этому маркеру
    if (Task.Task == TargetTaskName)
    {
        if (NewState == ETaskState::Started)
        {
            // Проверяем персонажа
            AACPP_Character* Char = Cast<AACPP_Character>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

            // Если персонаж еще делает интро — маркер НЕ показываем
            if (Char && Char->IsDoingIntroCamera())
            {
                MarkerWidgetComponent->SetVisibility(false);
                return;
            }

            MarkerWidgetComponent->SetVisibility(true);
        }
        else if (NewState == ETaskState::Completed)
        {
            MarkerWidgetComponent->SetVisibility(false);
            Destroy(); // Уничтожаем маркер, чтобы он не висел в памяти
        }
    }
}
#include "Actor/ACPP_TaskTrigger.h"
#include "Actor/ACPP_TaskComponent.h"

void AACPP_TaskTrigger::BeginPlay()
{
    Super::BeginPlay();

    OnActorBeginOverlap.AddDynamic(this, &AACPP_TaskTrigger::OnVolumeOverlap);
}

void AACPP_TaskTrigger::OnVolumeOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && OtherActor != this)
	{
		// Ищем компонент задач у вошедшего актора
		UACPP_TaskComponent* TaskComp = OtherActor->FindComponentByClass<UACPP_TaskComponent>();
		if (TaskComp)
		{
			FTaskData FoundTask;
			bool bTaskExists = false;

			// Проходимся циклом по списку задач компонента (переменная TaskList из вашего класса)
			for (const FTaskData& Task : TaskComp->TaskList)
			{
				if (Task.Task == TargetTask)
				{
					FoundTask = Task;
					bTaskExists = true;
					break;
				}
			}

			// Если задача найдена и её текущий статус равен Started
			if (bTaskExists && FoundTask.State == ETaskState::Started)
			{
				// Завершаем её. Это переключит логику на следующую миссию
				TaskComp->CompleteTask(TargetTask);

				// Отписываем триггер от события, чтобы он больше не тратил ресурсы
				OnActorBeginOverlap.RemoveDynamic(this, &AACPP_TaskTrigger::OnVolumeOverlap);
			}
		}
	}
}
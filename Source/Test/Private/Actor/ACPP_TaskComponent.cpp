#include "Actor/ACPP_TaskComponent.h"
#include "Character/ACPP_Character.h"

UACPP_TaskComponent::UACPP_TaskComponent()
{	
	PrimaryComponentTick.bCanEverTick = false;
}

void UACPP_TaskComponent::BeginPlay()
{
	Super::BeginPlay();

	if (TaskList.Num() > 0) //Авто запуск первой задачи, если список не пуст
	{
		StartTask(TaskList[0].Task); 
	}
}

void UACPP_TaskComponent::StartTask(FName Task)
{
	for (int32 i = 0; i < TaskList.Num(); ++i)
	{
		if (TaskList[i].Task == Task) 
		{
			TaskList[i].State = ETaskState::Started;
			CurrentTaskIndex = i;

			OnTaskStateChanged.Broadcast(TaskList[i], ETaskState::Started); 
			break;
		}
	}
}

void UACPP_TaskComponent::CompleteTask(FName Task)
{
	int32 FoundIndex = INDEX_NONE;

	// Ищем задачу и переводим в Completed
	for (int32 i = 0; i < TaskList.Num(); ++i)
	{
		if (TaskList[i].Task == Task)
		{
			TaskList[i].State = ETaskState::Completed;
			FoundIndex = i;

			OnTaskStateChanged.Broadcast(TaskList[i], ETaskState::Completed);
			break;
		}
	}

	if (FoundIndex == INDEX_NONE) return;

	// Проверка, была ли задача финальной
	if (IsFinalTask(TaskList[FoundIndex]))
	{		
		return;
	}

	// Активавция следующей задачи
	if (TaskList.IsValidIndex(FoundIndex + 1))
	{
		TaskList[FoundIndex + 1].State = ETaskState::Started;
		CurrentTaskIndex = FoundIndex + 1;
		OnTaskStateChanged.Broadcast(TaskList[FoundIndex + 1], ETaskState::Started);
	}
}

FTaskData UACPP_TaskComponent::GetActiveTask() const
{
	return TaskList.IsValidIndex(CurrentTaskIndex) ? TaskList[CurrentTaskIndex] : FTaskData();
}

bool UACPP_TaskComponent::IsFinalTask(const FTaskData& Task) const
{
	// Если список задач пуст
	if (TaskList.Num() == 0) return false;

	// Проверяем, совпадает ли имя переданной задачи с именем последнего элемента в массиве
	return TaskList.Last().Task == Task.Task;
}

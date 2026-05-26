#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACPP_TaskComponent.generated.h"

UENUM(BlueprintType) //Перечисление возможных состояний задачи
enum class ETaskState :uint8
{
	NotStarted,
	Started,
	Completed
};

USTRUCT(BlueprintType)
struct FTaskData //Структура задачи
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tasks")
	FName Task;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tasks")
	FText TaskDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tasks")
	ETaskState State = ETaskState::NotStarted;	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTaskStateChanged, FTaskData, Task, ETaskState, NewState); //Делегат для UI

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_API UACPP_TaskComponent : public UActorComponent
{
	GENERATED_BODY()

public:		
	UACPP_TaskComponent();

protected:	
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Task") //Делегат для подписки виджетов
	FOnTaskStateChanged OnTaskStateChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Task") //Список задач, заполняется в BP
	TArray<FTaskData> TaskList;

	UFUNCTION(BlueprintCallable, Category = "Task")
	void StartTask(FName Task);

	UFUNCTION(BlueprintCallable, Category = "Task")
	void CompleteTask(FName Task);

	UFUNCTION(BlueprintPure, Category = "Task")
	FTaskData GetActiveTask() const;

	UFUNCTION(BlueprintPure, Category = "Task")
	bool IsFinalTask(const FTaskData& Task) const;

private:
	int32 CurrentTaskIndex = -1;
};

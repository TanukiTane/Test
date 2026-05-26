#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "ACPP_WorldMarker.generated.h"

UCLASS()
class TEST_API AACPP_WorldMarker : public AActor
{
	GENERATED_BODY()
	
public:		
	AACPP_WorldMarker();

protected:	
	virtual void BeginPlay() override;

    // Компонент виджета
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* MarkerWidgetComponent;

    // Метод для переключения видимости маркера на основе активной миссии
    UFUNCTION()
    void HandleTaskChanged(struct FTaskData Task, enum ETaskState NewState);

public:
    // Имя задачи, за которой следит этот маркер
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Link")
    FName TargetTaskName = FName("GoToDoor");
};
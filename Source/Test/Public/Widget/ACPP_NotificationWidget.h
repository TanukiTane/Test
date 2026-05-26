#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ACPP_NotificationWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;
struct FTaskData;
enum class ETaskState : uint8;

UCLASS()
class TEST_API UACPP_NotificationWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

    virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

    // Ссылка на текстовый блок миссии
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TaskText;

    // Ссылка на UMG-анимацию
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* FadeInOutAnimation;

    // Функция-обработчик изменения состояния задачи
    UFUNCTION()
    void OnTaskStateUpdated(FTaskData Task, ETaskState NewState);

private:
    bool bIsFinalTaskCompleted = false;
    bool bIsFinalTaskProcessed = false;
};
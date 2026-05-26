#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "ACPP_KeypadWidget.generated.h"

class ACPP_Door;

UCLASS()
class TEST_API UACPP_KeypadWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;

    UPROPERTY(meta = (BindWidget))
    UEditableText* Number_0;

    UPROPERTY(meta = (BindWidget))
    UEditableText* Number_1;

    UPROPERTY(meta = (BindWidget))
    UEditableText* Number_2;

    UPROPERTY(meta = (BindWidget))
    UEditableText* Number_3;

    // КНОПКИ ВВЕРХ 
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Up_0;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Up_1;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Up_2;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Up_3;

    // КНОПКИ ВНИЗ 
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Down_0;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Down_1;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Down_2;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Down_3;

    // Функции обработки нажатий кнопок
    UFUNCTION() void OnClickUp0() { ChangeNumberValue(Number_0, 1); }
    UFUNCTION() void OnClickUp1() { ChangeNumberValue(Number_1, 1); }
    UFUNCTION() void OnClickUp2() { ChangeNumberValue(Number_2, 1); }
    UFUNCTION() void OnClickUp3() 
    {
        ChangeNumberValue(Number_3, 1);
        // Запуск проверки при изменении 4-й значения
        GetWorld()->GetTimerManager().ClearTimer(CodeCheckTimerHandle);
        GetWorld()->GetTimerManager().SetTimer(CodeCheckTimerHandle, this, &UACPP_KeypadWidget::TriggerPasswordCheck, 0.5f, false);
    }

    UFUNCTION() void OnClickDown0() { ChangeNumberValue(Number_0, -1); }
    UFUNCTION() void OnClickDown1() { ChangeNumberValue(Number_1, -1); }
    UFUNCTION() void OnClickDown2() { ChangeNumberValue(Number_2, -1); }
    UFUNCTION() void OnClickDown3() 
    { 
        ChangeNumberValue(Number_3, -1);
        // Запуск проверки при изменении 4-й значения
        GetWorld()->GetTimerManager().ClearTimer(CodeCheckTimerHandle);
        GetWorld()->GetTimerManager().SetTimer(CodeCheckTimerHandle, this, &UACPP_KeypadWidget::TriggerPasswordCheck, 0.5f, false);
    }

    UFUNCTION()
    void OnNumberTextChanged(const FText& Text, UEditableText* CurrentNumber, UEditableText* NextNumber);
   
    UFUNCTION() void OnNumber0Changed(const FText& Text) { OnNumberTextChanged(Text, Number_0, Number_1); }
    UFUNCTION() void OnNumber1Changed(const FText& Text) { OnNumberTextChanged(Text, Number_1, Number_2); }
    UFUNCTION() void OnNumber2Changed(const FText& Text) { OnNumberTextChanged(Text, Number_2, Number_3); }
    UFUNCTION() void OnNumber3Changed(const FText& Text) { OnNumberTextChanged(Text, Number_3, nullptr); }

    FString GetEnteredCode() const;
   
    FTimerHandle ErrorResetTimerHandle;
    void ResetErrorState();

    void ChangeNumberValue(UEditableText* TargetField, int32 Amount);
    TArray<UEditableText*> NumberFields;

    void TriggerPasswordCheck();
    FTimerHandle CodeCheckTimerHandle;

public:  
    UPROPERTY(BlueprintReadWrite, Category = "Keypad", meta = (ExposeOnSpawn = true))
    class AACPP_Door* Door;

    UFUNCTION(BlueprintImplementableEvent, Category = "Keypad")
    void VisualiseError(bool bIsVisible);
};
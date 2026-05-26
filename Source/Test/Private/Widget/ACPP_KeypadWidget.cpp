#include "Widget/ACPP_KeypadWidget.h"
#include "Actor/ACPP_Door.h"
#include "Kismet/GameplayStatics.h"

bool UACPP_KeypadWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    //Ïðèâÿçêà äåëåãàòîâ äëÿ èçìåíåíèÿ òåêñòà
    if (Number_0)
    {
        Number_0->OnTextChanged.AddDynamic(this, &UACPP_KeypadWidget::OnNumber0Changed);
    }
    if (Number_1)
    {
        Number_1->OnTextChanged.AddDynamic(this, &UACPP_KeypadWidget::OnNumber1Changed);
    }
    if (Number_2)
    {
        Number_2->OnTextChanged.AddDynamic(this, &UACPP_KeypadWidget::OnNumber2Changed);
    }
    if (Number_3)
    {
        Number_3->OnTextChanged.AddDynamic(this, &UACPP_KeypadWidget::OnNumber3Changed);
    }
   
    // Ìàññèâ äëÿ íàâèãàöèè
    NumberFields.Empty();
    if (Number_0) NumberFields.Add(Number_0);
    if (Number_1) NumberFields.Add(Number_1);
    if (Number_2) NumberFields.Add(Number_2);
    if (Number_3) NumberFields.Add(Number_3);

    // ÏÐÈÂßÇÊÀ ÊÍÎÏÎÊ "ÂÂÅÐÕ"
    if (Btn_Up_0) Btn_Up_0->OnClicked.AddDynamic(this, &UACPP_KeypadWidget::OnClickUp0);
    if (Btn_Up_1) Btn_Up_1->OnClicked.AddDynamic(this, &UACPP_KeypadWidget::OnClickUp1);
    if (Btn_Up_2) Btn_Up_2->OnClicked.AddDynamic(this, &UACPP_KeypadWidget::OnClickUp2);
    if (Btn_Up_3) Btn_Up_3->OnClicked.AddDynamic(this, &UACPP_KeypadWidget::OnClickUp3);

    // ÏÐÈÂßÇÊÀ ÊÍÎÏÎÊ "ÂÍÈÇ"
    if (Btn_Down_0) Btn_Down_0->OnClicked.AddDynamic(this, &UACPP_KeypadWidget::OnClickDown0);
    if (Btn_Down_1) Btn_Down_1->OnClicked.AddDynamic(this, &UACPP_KeypadWidget::OnClickDown1);
    if (Btn_Down_2) Btn_Down_2->OnClicked.AddDynamic(this, &UACPP_KeypadWidget::OnClickDown2);
    if (Btn_Down_3) Btn_Down_3->OnClicked.AddDynamic(this, &UACPP_KeypadWidget::OnClickDown3);

    return true;
}

void UACPP_KeypadWidget::OnNumberTextChanged(const FText& Text, UEditableText* CurrentNumber, UEditableText* NextNumber)
{
    FString StringText = Text.ToString();

    if (StringText.Len() > 0 && !StringText.IsNumeric())
    {
        CurrentNumber->SetText(FText::GetEmpty());
        return;
    }

    if (StringText.Len() > 1)
    {
        CurrentNumber->SetText(FText::FromString(StringText.Left(1)));
        return;
    }

    if (StringText.Len() == 1 && NextNumber)
    {
        NextNumber->SetFocus();
    }
    else if (StringText.Len() == 1 && !NextNumber)
    {
        GetWorld()->GetTimerManager().ClearTimer(CodeCheckTimerHandle);
        GetWorld()->GetTimerManager().SetTimer(CodeCheckTimerHandle, this, &UACPP_KeypadWidget::TriggerPasswordCheck, 0.5f, false);
    }
}

FString UACPP_KeypadWidget::GetEnteredCode() const
{
    if (!Number_0 || !Number_1 || !Number_2 || !Number_3) return FString();

    // Ìàññèâ ñòðîê êîäà
    return Number_0->GetText().ToString() +
        Number_1->GetText().ToString() +
        Number_2->GetText().ToString() +
        Number_3->GetText().ToString();

}

void UACPP_KeypadWidget::ResetErrorState()
{
    VisualiseError(false);

    if (Number_0) Number_0->SetText(FText::GetEmpty());
    if (Number_1) Number_1->SetText(FText::GetEmpty());
    if (Number_2) Number_2->SetText(FText::GetEmpty());
    if (Number_3) Number_3->SetText(FText::GetEmpty());

    if (Number_0) Number_0->SetFocus();
}

void UACPP_KeypadWidget::ChangeNumberValue(UEditableText* TargetField, int32 Amount)
{
    if (!TargetField) return;

    FString CurrentStr = TargetField->GetText().ToString();
    int32 CurrentNum = 0;

    if (!CurrentStr.IsEmpty() && CurrentStr.IsNumeric())
    {
        CurrentNum = FCString::Atoi(*CurrentStr);
    }
    else
    {
        CurrentNum = 0;
    }

    CurrentNum += Amount;
    if (CurrentNum > 9) CurrentNum = 0;
    if (CurrentNum < 0) CurrentNum = 9;

    TargetField->SetText(FText::AsNumber(CurrentNum));    
}

void UACPP_KeypadWidget::TriggerPasswordCheck()
{
    AActor* PlayerChar = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Door && PlayerChar)
    {
        FString FinalCode = GetEnteredCode();
        bool bIsCorrect = Door->CheckPassword(FinalCode, PlayerChar);

        if (!bIsCorrect)
        {
            VisualiseError(true);
            // Ïîäñâåòêà îøèáêè
            GetWorld()->GetTimerManager().SetTimer(ErrorResetTimerHandle, this, &UACPP_KeypadWidget::ResetErrorState, 2.0f, false);
        }
    }
}
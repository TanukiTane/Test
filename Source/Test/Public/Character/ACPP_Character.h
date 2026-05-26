#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Actor/ACPP_TaskComponent.h" 
#include "ACPP_Character.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class TEST_API AACPP_Character : public ACharacter
{
	GENERATED_BODY()

public:
	AACPP_Character();
	
	bool IsDoingIntroCamera() const { return bDoingIntroCamera; }
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	//Камера для режима 3е лицо
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraTPS;

	//Камера для режима 1е лицо
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraFPS;

	//Компонент управления задачами
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tasks", meta = (AllowPrivateAccess = "true"))
	UACPP_TaskComponent* TaskComponent;

	//Виджет оповещений
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> NotificationWidgetClass;

	UPROPERTY()
	UUserWidget* ActiveNotificationWidget;	

	//Настройки Input
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* InteractAction;

	//Анимация победы
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* Victory;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact();

	void SwitchToFP();

	// Переменные для начальной интерполяции камеры
	bool bDoingIntroCamera = true;
	float IntroTimer = 0.0f;
	const float IntroDuration = 3.0f; // Время анимации камеры

	//Флаг предотвращения повторного вызова анимации
	bool bHasWon = false;
	//Флаг для включения работы камеры
	bool bCameraRotate = false;

public:	
	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void PlayAnimation();

	virtual void Tick(float DeltaTime) override;
};
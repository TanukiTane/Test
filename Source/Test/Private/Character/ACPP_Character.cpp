#include "Character/ACPP_Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Widget/ACPP_NotificationWidget.h"
#include "Actor/ACPP_InteractableInterface.h"

AACPP_Character::AACPP_Character()
{ 	
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.f;
	SpringArm->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	SpringArm->bUsePawnControlRotation = false;

	CameraTPS = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCamera"));
	CameraTPS->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CameraTPS->bUsePawnControlRotation = false;

	CameraFPS = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	CameraFPS->SetupAttachment(GetMesh(), TEXT("Head"));
	CameraFPS->bUsePawnControlRotation = true;
	CameraFPS->FieldOfView = 90.f;

	CameraFPS->SetActive(false);
	CameraTPS->SetActive(true);

	TaskComponent = CreateDefaultSubobject<UACPP_TaskComponent>(TEXT("TaskComponent"));
}

void AACPP_Character::BeginPlay()
{
	Super::BeginPlay();	

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		PC->PlayerCameraManager->ViewPitchMin = -80.f;
		PC->PlayerCameraManager->ViewPitchMax = 80.f;

		if (GetCharacterMovement())
		{
			GetCharacterMovement()->DisableMovement();
		}
	}

	FTimerHandle CameraSwitchTimer;
	GetWorldTimerManager().SetTimer(CameraSwitchTimer, this, &AACPP_Character::SwitchToFP, IntroDuration, false);
}

void AACPP_Character::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AACPP_Character::Look(const FInputActionValue& Value)
{	
	if (!bCameraRotate) return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AACPP_Character::Interact()
{
	FVector Start = CameraFPS->GetComponentLocation();
	FVector End = Start + (CameraFPS->GetForwardVector() * 250.f); 

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		if (HitResult.GetActor() && HitResult.GetActor()->Implements<UACPP_InteractableInterface>())
		{
			IACPP_InteractableInterface::Execute_OnInteract(HitResult.GetActor(), this);
		}
	}
}

void AACPP_Character::SwitchToFP()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(0.f, 1.f, 0.5f, FLinearColor::Black, false, true);
		
		FTimerHandle FadeOutTimer;
		GetWorldTimerManager().SetTimer(FadeOutTimer,[this, PC]()
		{
			CameraTPS->SetActive(false);
			CameraFPS->SetActive(true);
			bCameraRotate = true;
			bUseControllerRotationYaw = true;				

			if (GetCharacterMovement())
			{
				GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}

			if (NotificationWidgetClass && !ActiveNotificationWidget)
			{
				ActiveNotificationWidget = CreateWidget<UUserWidget>(PC, NotificationWidgetClass);
				if (ActiveNotificationWidget)
				{
					ActiveNotificationWidget->AddToViewport();
				}
			}
			
			if (TaskComponent)
			{
				FTaskData ActiveTask = TaskComponent->GetActiveTask();
				if (!ActiveTask.Task.IsNone())
				{
					// Перезапуск рассылки события
					TaskComponent->OnTaskStateChanged.Broadcast(ActiveTask, ActiveTask.State);
				}
			}

			PC->PlayerCameraManager->StartCameraFade(1.f, 0.f, 0.5f, FLinearColor::Black, false, false);
		}, 0.5f, false);
	}
}

void AACPP_Character::PlayAnimation()
{
	if (bHasWon || !Victory) return;

	bHasWon = true;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	if (CameraFPS && CameraTPS)
	{
		//Возврат к камере от 3его лица для красивого вида
		CameraFPS->SetActive(false);
		
		SpringArm->SetRelativeRotation(FRotator(-45.f, -90.f, 0.f));
		SpringArm->TargetArmLength = 500.f;

		CameraTPS->SetActive(true);
	}

	PlayAnimMontage(Victory);
}

void AACPP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Плавный разворот SpringArm с -90 до 0 и приближение с 500 до 100
	if (bDoingIntroCamera)
	{
		IntroTimer += DeltaTime;
		float Alpha = FMath::Clamp(IntroTimer / IntroDuration, 0.0f, 1.0f);

		// Плавный подлёт
		SpringArm->TargetArmLength = FMath::Lerp(500.f, 100.f, Alpha);

		// Плавный поворот
		FRotator CurrentRot = SpringArm->GetRelativeRotation();
		CurrentRot.Yaw = FMath::Lerp(-90.f, 0.f, Alpha);
		SpringArm->SetRelativeRotation(CurrentRot);

		if (Alpha >= 1.0f)
		{
			bDoingIntroCamera = false;
		}
	}
}

void AACPP_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EI = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EI->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AACPP_Character::Move);
		EI->BindAction(LookAction, ETriggerEvent::Triggered, this, &AACPP_Character::Look);
		EI->BindAction(InteractAction, ETriggerEvent::Started, this, &AACPP_Character::Interact);
	}
}
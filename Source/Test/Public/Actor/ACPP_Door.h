#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor/ACPP_InteractableInterface.h"
#include "ACPP_Door.generated.h"

UCLASS()
class TEST_API AACPP_Door : public AActor, public IACPP_InteractableInterface
{
	GENERATED_BODY()
	
public:
	AACPP_Door();

protected:	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Door; 
	
	//Код можно задать в редакторе
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Security")
	FString CorrectPassword = "1234"; 

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UACPP_KeypadWidget> KeypadWidget;

	UPROPERTY()
	UUserWidget* ActiveKeypadWidget;

public:	
	virtual void OnInteract_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintCallable, Category = "Security")
	bool CheckPassword(const FString& EnteredCode, AActor* Interactor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void OpenDoorTimeline();
};
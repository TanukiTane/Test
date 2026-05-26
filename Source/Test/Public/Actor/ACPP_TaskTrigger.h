#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "ACPP_TaskTrigger.generated.h"

UCLASS()
class TEST_API AACPP_TaskTrigger : public ATriggerVolume
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	FName TargetTask = FName("GoToDoor");

	UFUNCTION()
	void OnVolumeOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
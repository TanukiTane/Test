#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ACPP_InteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UACPP_InteractableInterface :public UInterface
{
	GENERATED_BODY()
};

class TEST_API IACPP_InteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	void OnInteract(AActor* Interactor);
};
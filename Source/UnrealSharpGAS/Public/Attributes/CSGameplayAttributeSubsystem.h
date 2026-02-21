#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "AttributeSet.h"
#include "CSGameplayAttributeSubsystem.generated.h"

UCLASS(BlueprintType)
class UNREALSHARPGAS_API UCSGameplayAttributeSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// End of USubsystem interface

	void GetCachedAttributeNamesForClass(const FString& AttributeSetClassName, TArray<FString>& OutAttributeNames) const;

	UFUNCTION(BlueprintCallable)
	static UCSGameplayAttributeSubsystem* Get();

	UFUNCTION(BlueprintCallable)
	static FGameplayAttribute FindGameplayAttributeByName(const FString& AttributeSetClassName, const FString& PropertyName);

	static void GetAllAttributeProperties(UClass* AttributeSetClass, TArray<FProperty*>& OutProperties);
	
private:
	void CacheAllGameplayAttributes();

	TMap<FString, FGameplayAttribute> CachedAttributes;
};
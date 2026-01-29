#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Abilities/GameplayAbilityTypes.h"

#include "CSAttributeSet.generated.h"

USTRUCT(BlueprintType)
struct FCSGameplayAttributeData : public FGameplayAttributeData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Attribute")
	FName AttributeName;

	FCSGameplayAttributeData() 
		: FGameplayAttributeData(0.0f)
	{
	}

	FCSGameplayAttributeData(float InitialValue) 
		: FGameplayAttributeData(InitialValue)
	{
	}
};

UCLASS()
class UNREALSHARPGAS_API UCSAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// UAttributeSet interface
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitProperties() override;

	UFUNCTION(meta = (ScriptMethod))
	virtual void InitFromMetaDataTable(const UDataTable* DataTable) override;
	// End of UAttributeSet interface
	
	UFUNCTION(BlueprintNativeEvent)
	bool K2_OnInitFromMetaDataTable(const UDataTable* DataTable);
	
	UFUNCTION(BlueprintNativeEvent)
	bool K2_PreGameplayEffectExecute(const FGameplayEffectSpec& EffectSpec, FGameplayModifierEvaluatedData& EvaluatedData, class UCSAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_PostGameplayEffectExecute(const FGameplayEffectSpec& EffectSpec, FGameplayModifierEvaluatedData& EvaluatedData, class UCSAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const;

	UFUNCTION(BlueprintImplementableEvent)
	void K2_PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const;

	UFUNCTION(meta = (ScriptMethod))
	AActor* K2_GetOwningActor() const;
	
	UFUNCTION(meta = (ScriptMethod))
	UCSAbilitySystemComponent* K2_GetOwningAbilitySystemComponent() const;
	
	UFUNCTION(meta = (ScriptMethod))
	FGameplayAbilityActorInfo& K2_GetActorInfo() const;
	
	UFUNCTION(meta = (ScriptMethod))
	bool TrySetAttributeBaseValue(FName AttributeName, float NewBaseValue) const;

	UFUNCTION(meta = (ScriptMethod))
	bool TryGetAttributeCurrentValue(FName AttributeName, float& OutCurrentValue) const;

	UFUNCTION(meta = (ScriptMethod))
	bool TryGetAttributeBaseValue(FName AttributeName, float& OutBaseValue) const;
	
	UFUNCTION(meta = (ScriptMethod))
	static FGameplayAttribute GetGameplayAttribute(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName);
	
	UFUNCTION(meta = (ScriptMethod))
	static bool TryGetGameplayAttribute(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, FGameplayAttribute& OutGameplayAttribute);
	
	UFUNCTION(meta = (ScriptMethod))
	static bool CompareGameplayAttributes(const FGameplayAttribute& First, const FGameplayAttribute& Second);

private:
	UFUNCTION()
	void OnRep_Attribute(FCSGameplayAttributeData& OldAttributeData);
};

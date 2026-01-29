#pragma once

#include "CoreMinimal.h"
#include "CSAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemComponent.h"
#include "CSAbilitySystemComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALSHARPGAS_API FCSModifiedAttribute
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Effect Data")
	FName Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Effect Data")
	float OldValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Effect Data")
	float NewValue;

	FCSModifiedAttribute() : Name("Unnamed"), OldValue(0.0f), NewValue(0.0f) {}
	FCSModifiedAttribute(FName InName, float InOldValue, float InNewValue) : Name(InName), OldValue(InOldValue), NewValue(InNewValue) {}
};

USTRUCT(BlueprintType)
struct UNREALSHARPGAS_API FCSInputBindData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Bind Data")
	FName ConfirmTargetCommand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Bind Data")
	FName CancelTargetCommand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Bind Data")
	FTopLevelAssetPath EnumName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Bind Data")
	int32 ConfirmTargetInputID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Bind Data")
	int32 CancelTargetInputID;

	FCSInputBindData()
		: ConfirmTargetCommand(NAME_None)
		, CancelTargetCommand(NAME_None)
		, EnumName(nullptr)
		, ConfirmTargetInputID(-1)
		, CancelTargetInputID(-1)
	{}
};

USTRUCT(BlueprintType)
struct UNREALSHARPGAS_API FCSAttributeChangedData
{
	GENERATED_BODY()
public:
	FOnAttributeChangeData WrappedData;
};

UCLASS()
class UNREALSHARPGAS_API UCSAttributeChangedDataExtensions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(meta=(ExtensionMethod, ScriptMethod))
	static const FGameplayAttribute& GetGameplayAttribute(const FCSAttributeChangedData& Data)
	{
		return Data.WrappedData.Attribute;
	}

	UFUNCTION(meta=(ExtensionMethod, ScriptMethod))
	static float GetNewValue(const FCSAttributeChangedData& Data)
	{
		return Data.WrappedData.NewValue;
	}

	UFUNCTION(meta=(ExtensionMethod, ScriptMethod))
	static float GetOldValue(const FCSAttributeChangedData& Data)
	{
		return Data.WrappedData.OldValue;
	}

	UFUNCTION(meta=(ExtensionMethod, ScriptMethod))
	static const FGameplayEffectSpec& GetEffectSpec(const FCSAttributeChangedData& Data, bool& bIsValid);

	UFUNCTION(meta = (ScriptMethod))
	static const FGameplayModifierEvaluatedData& GetGameplayModifierEvaluatedData(const FCSAttributeChangedData& Data, bool& bIsValid);

	UFUNCTION(meta=(ExtensionMethod, ScriptMethod))
	static UAbilitySystemComponent* GetTargetAbilitySystemComponent(const FCSAttributeChangedData& Data);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInitAbilityActorInfoDelegate, AActor*, InOwnerActor, AActor*, InAvatarActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityGivenDelegate, const FGameplayAbilitySpec&, AbilitySpec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityRemovedDelegate, const FGameplayAbilitySpec&, AbilitySpec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeChangedDelegate, const FCSModifiedAttribute&, AttributeChangeData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeSetRegisteredDelegate, class UCSAttributeSet*, NewAttributeSet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOwnedTagUpdatedDelegate, const FGameplayTag&, Tag, bool, TagExists);

UCLASS()
class UNREALSHARPGAS_API UCSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UCSAbilitySystemComponent();
	
	// UAbilitySystemComponent interface
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual FGameplayAbilitySpecHandle GiveAbility_Internal(TSubclassOf<UGameplayAbility> InAbilityClass, int32 Level, int32 OptionalInputID, UObject* OptionalSourceObject);
	virtual FGameplayAbilitySpecHandle GiveAbilityAndActivateOnce_Internal(TSubclassOf<UGameplayAbility> InAbilityClass, int32 Level, int32 OptionalInputID, UObject* OptionalSourceObject);
	virtual void OnGameplayEffectEnded(const FActiveGameplayEffect& EndedGameplayEffect);
	virtual void OnAttributeChangedTrampoline(const FOnAttributeChangeData& AttributeChangeData);
	// End of UAbilitySystemComponent interface
	
	UFUNCTION(meta = (ScriptMethod))
	FGameplayAbilityActorInfo& GetAbilityActorInfo() const;

	UFUNCTION(meta = (ScriptMethod))
	AActor* GetAvatar() const;

	UFUNCTION(meta = (ScriptMethod))
	APlayerController* GetPlayerController() const;

	UFUNCTION(meta = (ScriptMethod))
	UCSAttributeSet* RegisterAttributeSet(TSubclassOf<UCSAttributeSet> AttributeSetClass);

	UFUNCTION(meta = (ScriptMethod))
	void OnAttributeSetRegistered(UObject* InObject, FName InFunctionName);

	UFUNCTION(meta = (ScriptMethod))
	void RegisterAttributeChangedCallback(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, UObject* CallbackObject, FName CallbackFunctionName);

	UFUNCTION(meta = (ScriptMethod))
	void GetAndRegisterAttributeChangedCallback(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, UObject* CallbackObject, FName CallbackFunctionName, float& OutCurrentValue);

	UFUNCTION(meta = (ScriptMethod))
	void K2_InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);

	UFUNCTION(meta = (ScriptMethod))
	void ModAttributeUnsafe(const FGameplayAttribute& GameplayAttribute, TEnumAsByte<EGameplayModOp::Type> ModifierOp, float ModifierMagnitude);

	UFUNCTION(meta = (ScriptMethod))
	void SetAttributeBaseValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float NewBaseValue);

	UFUNCTION(meta = (ScriptMethod))
	float GetAttributeCurrentValueChecked(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName) const;

	UFUNCTION(meta = (ScriptMethod))
	float GetAttributeBaseValueChecked(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName) const;

	UFUNCTION(meta = (ScriptMethod))
	float GetAttributeCurrentValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float DefaultValue = 0.f) const;

	UFUNCTION(meta = (ScriptMethod))
	float GetAttributeBaseValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float DefaultValue = 0.f) const;

	UFUNCTION(meta = (ScriptMethod))
	bool TrySetAttributeBaseValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float NewBaseValue);

	UFUNCTION(meta = (ScriptMethod))
	bool TryGetAttributeCurrentValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float& OutCurrentValue) const;

	UFUNCTION(meta = (ScriptMethod))
	bool TryGetAttributeBaseValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float& OutBaseValue) const;

	UFUNCTION(meta = (ScriptMethod))
	FGameplayAbilitySpecHandle GiveAbilityAndActivateOnceWithEvent(TSubclassOf<UGameplayAbility> InAbilityClass, FGameplayEventData EventData, int32 Level = 1, int32 OptionalInputID = -1, UObject* OptionalSourceObject = nullptr);
	
	UFUNCTION(Server, Reliable, meta = (ScriptMethod))
	void ServerGiveAbilityAndActivateOnceWithEvent(TSubclassOf<UGameplayAbility> InAbilityClass, FGameplayEventData EventData, int32 Level = 1, int32 OptionalInputID = -1, UObject* OptionalSourceObject = nullptr);

	UFUNCTION(meta = (ScriptMethod))
	void K2_SetRemoveAbilityOnEnd(FGameplayAbilitySpecHandle AbilitySpecHandle);

	UFUNCTION(meta = (ScriptMethod))
	bool TryActivateAbilitySpec(const FGameplayAbilitySpecHandle& Handle, bool bAllowRemoteActivation = true);

	UFUNCTION(meta = (ScriptMethod))
	void CancelAbility(TSubclassOf<UGameplayAbility> InAbilityClass);

	UFUNCTION(meta = (ScriptMethod))
	void CancelAbilitiesByTags(const FGameplayTagContainer& WithTags, const FGameplayTagContainer& WithoutTags, UGameplayAbility* Ignore = nullptr);

	UFUNCTION(meta = (ScriptMethod))
	void CancelAbilityByHandle(const FGameplayAbilitySpecHandle& AbilityHandle);

	UFUNCTION(meta = (ScriptMethod))
	bool IsAbilityActive(TSubclassOf<UGameplayAbility> InAbilityClass) const;

	UFUNCTION(meta = (ScriptMethod))
	bool HasActiveAbilityWithTag(FGameplayTag Tag) const;

	UFUNCTION(meta = (ScriptMethod))
	bool HasActiveAbilityWithTags(const FGameplayTagContainer& GameplayTagContainer) const;

	UFUNCTION(meta = (ScriptMethod))
	void GetActiveAbilitiesWithTag(FGameplayTag Tag, TArray<UGameplayAbility*>& ActiveAbilities) const;

	UFUNCTION(meta = (ScriptMethod, DeterminesOutputType = "InAbilityClass"))
	void GetActiveAbilitiesByClass(TSubclassOf<UGameplayAbility> InAbilityClass, TArray<UGameplayAbility*>& ActiveAbilities) const;

	UFUNCTION(meta = (ScriptMethod))
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UGameplayAbility*>& ActiveAbilities) const;

	UFUNCTION(meta = (ScriptMethod))
	void GetAbilitiesWithDynamicTags(const FGameplayTagContainer& GameplayTagContainer, TArray<FGameplayAbilitySpec>& Abilities) const;

	UFUNCTION(meta = (ScriptMethod))
	bool ActivateAbilitiesUsingTags(const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation = true);

	UFUNCTION(meta = (ScriptMethod))
	bool CanActivateAbilityByClass(TSubclassOf<UGameplayAbility> InAbilityToActivate) const;

	UFUNCTION(meta = (ScriptMethod))
	bool CanActivateAbilitySpec(FGameplayAbilitySpecHandle AbilitySpecHandle) const;

	UFUNCTION(meta = (ScriptMethod))
	void SetAbilitySpecSourceObject(FGameplayAbilitySpecHandle AbilitySpecHandle, UObject* NewSourceObject) const;

	UFUNCTION(meta = (ScriptMethod))
	UObject* GetAbilitySpecSourceObject(FGameplayAbilitySpecHandle AbilitySpecHandle) const;

	UFUNCTION(meta = (ScriptMethod))
	bool HasAbility(TSubclassOf<UGameplayAbility> InAbilityClass) const;

	UFUNCTION(meta = (ScriptMethod))
	float GetCooldownTimeRemaining(TSubclassOf<UGameplayAbility> InAbilityClass) const;

	UFUNCTION(meta = (ScriptMethod))
	void BindInput(UInputComponent* InputComponent, FCSInputBindData BindData);

	UFUNCTION(meta = (ScriptMethod))
	bool HasGameplayTag(FGameplayTag TagToCheck) const;

	UFUNCTION(meta = (ScriptMethod))
	bool HasAllGameplayTags(const FGameplayTagContainer& TagContainer) const;

	UFUNCTION(meta = (ScriptMethod))
	bool HasAnyGameplayTags(const FGameplayTagContainer& TagContainer) const;
	
	UPROPERTY(BlueprintAssignable, Category = "Ability System Callbacks")
	FInitAbilityActorInfoDelegate OnInitAbilityActorInfo;

	UPROPERTY(BlueprintAssignable, Category = "Ability System Callbacks")
	FAbilityGivenDelegate OnAbilityGiven;

	UPROPERTY(BlueprintAssignable, Category = "Ability System Callbacks")
	FAbilityRemovedDelegate OnAbilityRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FAttributeChangedDelegate OnAttributeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Tag Callbacks")
	FOwnedTagUpdatedDelegate OnOwnedTagUpdated;

private:
	FAttributeSetRegisteredDelegate OnAttributeSetRegisteredDelegate;
};

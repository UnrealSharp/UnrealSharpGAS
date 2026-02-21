#include "CSAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemLog.h"
#include "GameplayCueManager.h"
#include "GameplayEffectExtension.h"
#include "UnrealSharpGAS.h"

UCSAbilitySystemComponent::UCSAbilitySystemComponent()
{
	ActiveGameplayEffects.OnActiveGameplayEffectRemovedDelegate.AddUObject(this, &UCSAbilitySystemComponent::OnGameplayEffectEnded);
}

void UCSAbilitySystemComponent::OnGameplayEffectEnded(const FActiveGameplayEffect& EndedGameplayEffect)
{
}

void UCSAbilitySystemComponent::OnAttributeChangedTrampoline(const FOnAttributeChangeData& AttributeChangeData)
{
	if (!OnAttributeChanged.IsBound())
	{
		return;
	}

	const FName AttributeName(*AttributeChangeData.Attribute.AttributeName);
	FCSModifiedAttribute Payload(AttributeName, AttributeChangeData.OldValue, AttributeChangeData.NewValue);
	OnAttributeChanged.Broadcast(Payload);
}

struct FOnAttributeSetRegisteredDynArgs
{
	UCSAttributeSet* CSAttributeSet;
};

void UCSAbilitySystemComponent::OnAttributeSetRegistered(UObject* InObject, FName InFunctionName)
{
	if (!IsValid(InObject))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Error, "Null object passed to AddUFunction.");
		return;
	}

	if (InFunctionName.IsNone())
	{
		UE_LOGFMT(LogUnrealSharpGAS, Error, "Empty function name passed to AddUFunction.");
		return;
	}

	UFunction* CallFunction = InObject->FindFunction(InFunctionName);
	if (CallFunction == nullptr)
	{
		UE_LOGFMT(LogUnrealSharpGAS, Error, "Could not find function in object with this name. Is it declared UFUNCTION()?");
		return;
	}

	FScriptDelegate Delegate;
	Delegate.BindUFunction(InObject, InFunctionName);
	OnAttributeSetRegisteredDelegate.AddUnique(Delegate);
	
	const TArray<UAttributeSet*>& SpawnedSets = GetSpawnedAttributes();
	if (SpawnedSets.IsEmpty())
	{
		return;
	}

	for (UAttributeSet* AttributeSet : SpawnedSets)
	{
		UCSAttributeSet* Attribute = Cast<UCSAttributeSet>(AttributeSet);
		if (!IsValid(Attribute))
		{
			continue;
		}

		FOnAttributeSetRegisteredDynArgs Args;
		Args.CSAttributeSet = Attribute;
		InObject->ProcessEvent(CallFunction, &Args);
	}
}

UCSAttributeSet* UCSAbilitySystemComponent::RegisterAttributeSet(TSubclassOf<UCSAttributeSet> AttributeSetClass)
{
	if (AttributeSetClass.Get() == nullptr)
	{
		return nullptr;
	}
	
	const TArray<UAttributeSet*>& SpawnedSets = GetSpawnedAttributes();
	for (UAttributeSet* AttributeSet : SpawnedSets)
	{
		if (AttributeSet->IsA(AttributeSetClass))
		{
			return static_cast<UCSAttributeSet*>(AttributeSet);
		}
	}
	
	UCSAttributeSet* NewAttributeSet = NewObject<UCSAttributeSet>(GetOwner(), AttributeSetClass);
	if (NewAttributeSet == nullptr)
	{
		return nullptr;
	}

	AddSpawnedAttribute(NewAttributeSet);
	OnAttributeSetRegisteredDelegate.Broadcast(NewAttributeSet);
	return NewAttributeSet;
}

void UCSAbilitySystemComponent::RegisterAttributeChangedCallback(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, UObject* CallbackObject, FName CallbackFunctionName)
{
	const FGameplayAttribute Attribute = UCSAttributeSet::GetGameplayAttribute(AttributeSetClass, AttributeName);

	if (!Attribute.IsValid())
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Must provide valid attribute data");
		return;
	}
	
	if (!IsValid(CallbackObject))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Must provide valid callback object");
		return;
	}
	
	if (CallbackFunctionName.IsNone())
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Must provide valid callback method name");
		return;
	}

	FOnGameplayAttributeValueChange& ChangeDelegate = ActiveGameplayEffects.GetGameplayAttributeValueChangeDelegate(Attribute);
	if (ChangeDelegate.IsBoundToObject(CallbackObject))
	{
		return;
	}

	ChangeDelegate.AddUFunction(CallbackObject, CallbackFunctionName);
}

void UCSAbilitySystemComponent::GetAndRegisterAttributeChangedCallback(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, UObject* CallbackObject, FName CallbackFunctionName, float& OutCurrentValue)
{
	const FGameplayAttribute Attribute = UCSAttributeSet::GetGameplayAttribute(AttributeSetClass, AttributeName);

	if (!Attribute.IsValid())
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Must provide valid attribute data");
		return;
	}
	
	if (!IsValid(CallbackObject))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Must provide valid callback object");
		return;
	}
	
	if (CallbackFunctionName.IsNone())
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Must provide valid callback method name");
		return;
	}

	FOnGameplayAttributeValueChange& ChangeDelegate = ActiveGameplayEffects.GetGameplayAttributeValueChangeDelegate(Attribute);
	if (!ChangeDelegate.IsBoundToObject(CallbackObject))
	{
		ChangeDelegate.AddUFunction(CallbackObject, CallbackFunctionName);
	}

	OutCurrentValue = GetNumericAttribute(Attribute);
}

void UCSAbilitySystemComponent::K2_InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}

void UCSAbilitySystemComponent::ModAttributeUnsafe(const FGameplayAttribute& GameplayAttribute, TEnumAsByte<EGameplayModOp::Type> ModifierOp, float ModifierMagnitude)
{
	ApplyModToAttributeUnsafe(GameplayAttribute, ModifierOp, ModifierMagnitude);
}

void UCSAbilitySystemComponent::SetAttributeBaseValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float NewBaseValue)
{
	if (!TrySetAttributeBaseValue(AttributeSetClass, AttributeName, NewBaseValue))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Could not set attribute base value for attribute <{}>", AttributeName.ToString());
	}
}

float UCSAbilitySystemComponent::GetAttributeCurrentValueChecked(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName) const
{
	float OutCurrentValue = 0.f;
	if (!TryGetAttributeCurrentValue(AttributeSetClass, AttributeName, OutCurrentValue))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Could not get attribute current value for attribute <{}>", AttributeName.ToString());
	}
	return OutCurrentValue;
}

float UCSAbilitySystemComponent::GetAttributeBaseValueChecked(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName) const
{
	float OutBaseValue = 0.f;
	
	if (!TryGetAttributeBaseValue(AttributeSetClass, AttributeName, OutBaseValue))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Could not get attribute base value for attribute <{}>", AttributeName.ToString());
	}
	
	return OutBaseValue;
}

float UCSAbilitySystemComponent::GetAttributeCurrentValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float DefaultValue) const
{
	float OutCurrentValue = DefaultValue;
	(void)TryGetAttributeCurrentValue(AttributeSetClass, AttributeName, OutCurrentValue);
	return OutCurrentValue;
}

float UCSAbilitySystemComponent::GetAttributeBaseValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float DefaultValue) const
{
	float OutBaseValue = DefaultValue;
	(void)TryGetAttributeBaseValue(AttributeSetClass, AttributeName, OutBaseValue);
	return OutBaseValue;
}

bool UCSAbilitySystemComponent::TrySetAttributeBaseValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float NewBaseValue)
{
	const UAttributeSet* AttributeSet = GetAttributeSubobject(AttributeSetClass);
	if (!IsValid(AttributeSet))
	{
		return false;
	}

	FGameplayAttribute Attribute;
	if (!UCSAttributeSet::TryGetGameplayAttribute(AttributeSetClass, AttributeName, Attribute))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning,
		          "Attribute <{}> doesn't exist inside the attribute set <{}>",
		          AttributeName.ToString(), GetNameSafe(AttributeSetClass.Get()));
		return false;
	}

	SetNumericAttributeBase(Attribute, NewBaseValue);
	return true;
}

bool UCSAbilitySystemComponent::TryGetAttributeCurrentValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float& OutCurrentValue) const
{
	const UAttributeSet* AttributeSet = GetAttributeSubobject(AttributeSetClass);
	if (!IsValid(AttributeSet))
	{
		return false;
	}

	FGameplayAttribute Attribute;
	if (!UCSAttributeSet::TryGetGameplayAttribute(AttributeSetClass, AttributeName, Attribute))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning,
		          "Attribute <{}> doesn't exist inside the attribute set <{}>",
		          AttributeName.ToString(), GetNameSafe(AttributeSetClass.Get()));
		return false;
	}

	OutCurrentValue = Attribute.GetNumericValue(AttributeSet);
	return true;
}

bool UCSAbilitySystemComponent::TryGetAttributeBaseValue(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, float& OutBaseValue) const
{
	const UAttributeSet* AttributeSet = GetAttributeSubobject(AttributeSetClass);
	if (!IsValid(AttributeSet))
	{
		return false;
	}

	FGameplayAttribute Attribute;
	if (!UCSAttributeSet::TryGetGameplayAttribute(AttributeSetClass, AttributeName, Attribute))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning,"Attribute <{}> doesn't exist inside the attribute set <{}>", AttributeName.ToString(), GetNameSafe(AttributeSetClass.Get()));
		return false;
	}

	OutBaseValue = GetNumericAttributeBase(Attribute);
	return true;
}

void UCSAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UGameplayAbility*>& ActiveAbilities) const
{
	TArray<FGameplayAbilitySpec*> Specs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, Specs, false);

	for (FGameplayAbilitySpec* Spec : Specs)
	{
		if (Spec == nullptr)
		{
			continue;
		}

		const TArray<UGameplayAbility*>& Instances = Spec->GetAbilityInstances();
		for (UGameplayAbility* Instance : Instances)
		{
			if (Instance != nullptr)
			{
				ActiveAbilities.Add(Instance);
			}
		}
	}
}

void UCSAbilitySystemComponent::GetAbilitiesWithDynamicTags(const FGameplayTagContainer& GameplayTagContainer, TArray<FGameplayAbilitySpec>& Abilities) const
{
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (!IsValid(Spec.Ability))
		{
			continue;
		}

		if (!Spec.GetDynamicSpecSourceTags().HasAnyExact(GameplayTagContainer))
		{
			continue;
		}

		Abilities.Add(Spec);
	}
}

bool UCSAbilitySystemComponent::ActivateAbilitiesUsingTags(const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation)
{
	return TryActivateAbilitiesByTag(GameplayTagContainer, bAllowRemoteActivation);
}

bool UCSAbilitySystemComponent::CanActivateAbilityByClass(TSubclassOf<UGameplayAbility> InAbilityToActivate) const
{
	if (InAbilityToActivate.Get() == nullptr)
	{
		ABILITY_LOG(Warning, TEXT("CanActivateAbilityByClass failed because InAbilityToActivate was invalid"));
		return false;
	}

	const UGameplayAbility* AbilityCDO = InAbilityToActivate.GetDefaultObject();
	const FGameplayAbilityActorInfo* Info = AbilityActorInfo.Get();
	if (AbilityCDO == nullptr || Info == nullptr)
	{
		return false;
	}

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability == AbilityCDO)
		{
			return Spec.Ability->CanActivateAbility(Spec.Handle, Info, nullptr, nullptr, nullptr);
		}
	}

	return false;
}

bool UCSAbilitySystemComponent::CanActivateAbilitySpec(FGameplayAbilitySpecHandle AbilitySpecHandle) const
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (!Spec)
	{
		ABILITY_LOG(Warning, TEXT("CanActivateAbilitySpec called with invalid Handle"));
		return false;
	}

	UGameplayAbility* Ability = Spec->Ability;
	if (!IsValid(Ability))
	{
		return false;
	}

	return Ability->CanActivateAbility(AbilitySpecHandle, AbilityActorInfo.Get());
}

void UCSAbilitySystemComponent::SetAbilitySpecSourceObject(FGameplayAbilitySpecHandle AbilitySpecHandle, UObject* NewSourceObject) const
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (Spec == nullptr)
	{
		ABILITY_LOG(Warning, TEXT("SetAbilitySpecSourceObject called with invalid Handle"));
		return;
	}

	Spec->SourceObject = NewSourceObject;
}

UObject* UCSAbilitySystemComponent::GetAbilitySpecSourceObject(FGameplayAbilitySpecHandle AbilitySpecHandle) const
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (!Spec)
	{
		ABILITY_LOG(Warning, TEXT("GetAbilitySpecSourceObject called with invalid Handle"));
		return nullptr;
	}

	return Spec->SourceObject.Get();
}

bool UCSAbilitySystemComponent::HasAbility(TSubclassOf<UGameplayAbility> InAbilityClass) const
{
	if (IsValid(InAbilityClass))
	{
		ABILITY_LOG(Warning, TEXT("HasAbility failed because InAbilityClass was invalid"));
		return false;
	}

	const UGameplayAbility* AbilityCDO = InAbilityClass.GetDefaultObject();
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability == AbilityCDO)
		{
			return true;
		}
	}

	return false;
}

FGameplayAbilitySpecHandle UCSAbilitySystemComponent::GiveAbilityAndActivateOnceWithEvent(
	TSubclassOf<UGameplayAbility> InAbilityClass, FGameplayEventData EventData, int32 Level, int32 OptionalInputID,
	UObject* OptionalSourceObject)
{
	FGameplayAbilitySpec Spec(InAbilityClass, Level, OptionalInputID, OptionalSourceObject);
	return GiveAbilityAndActivateOnce(Spec, &EventData);
	
}

void UCSAbilitySystemComponent::ServerGiveAbilityAndActivateOnceWithEvent_Implementation(
	TSubclassOf<UGameplayAbility> InAbilityClass, FGameplayEventData EventData, int32 Level, int32 OptionalInputID,
	UObject* OptionalSourceObject)
{
	GiveAbilityAndActivateOnceWithEvent(InAbilityClass, EventData, Level, OptionalInputID, OptionalSourceObject);
}

void UCSAbilitySystemComponent::ServerSendGameplayEventToActor_Implementation(AActor* Actor, const FGameplayTag& EventTag, const FGameplayEventData& Payload)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, EventTag, Payload);
}

void UCSAbilitySystemComponent::K2_SetRemoveAbilityOnEnd(FGameplayAbilitySpecHandle AbilitySpecHandle)
{
	SetRemoveAbilityOnEnd(AbilitySpecHandle);
}

bool UCSAbilitySystemComponent::TryActivateAbilitySpec(const FGameplayAbilitySpecHandle& Handle, bool bAllowRemoteActivation)
{
	return TryActivateAbility(Handle, bAllowRemoteActivation);
}

FGameplayAbilitySpecHandle UCSAbilitySystemComponent::GiveAbility_Internal(TSubclassOf<UGameplayAbility> InAbilityClass, int32 Level, int32 OptionalInputID, UObject* OptionalSourceObject)
{
	if (InAbilityClass == nullptr)
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Please provide a valid InAbilityClass to GiveAbility()");
		return FGameplayAbilitySpecHandle();
	}

	FGameplayAbilitySpec Spec(InAbilityClass, Level);
	Spec.SourceObject = OptionalSourceObject;
	Spec.InputID = OptionalInputID;
	return GiveAbility(Spec);
}

FGameplayAbilitySpecHandle UCSAbilitySystemComponent::GiveAbilityAndActivateOnce_Internal(TSubclassOf<UGameplayAbility> InAbilityClass, int32 Level, int32 OptionalInputID, UObject* OptionalSourceObject)
{
	if (InAbilityClass == nullptr)
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Please provide a valid InAbilityClass to GiveAbilityAndActivateOnce()");
		return FGameplayAbilitySpecHandle();
	}

	FGameplayAbilitySpec Spec(InAbilityClass, Level);
	Spec.SourceObject = OptionalSourceObject;
	Spec.InputID = OptionalInputID;
	return GiveAbilityAndActivateOnce(Spec);
}

void UCSAbilitySystemComponent::CancelAbility(TSubclassOf<UGameplayAbility> InAbilityClass)
{
	if (InAbilityClass == nullptr)
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Please provide a valid InAbilityClass to CancelAbility()");
		return;
	}

	Super::CancelAbility(InAbilityClass.GetDefaultObject());
}

bool UCSAbilitySystemComponent::IsAbilityActive(TSubclassOf<UGameplayAbility> InAbilityClass) const
{
	if (!IsValid(InAbilityClass))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Please provide a valid InAbilityClass to IsAbilityActive()");
		return false;
	}

	const UGameplayAbility* Ability = InAbilityClass.GetDefaultObject();
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability == Ability)
		{
			return Spec.IsActive();
		}
	}

	return false;
}

bool UCSAbilitySystemComponent::HasActiveAbilityWithTag(FGameplayTag Tag) const
{
	TArray<UGameplayAbility*> ActiveAbilities;
	GetActiveAbilitiesWithTag(Tag, ActiveAbilities);
	return ActiveAbilities.Num() > 0;
}

bool UCSAbilitySystemComponent::HasActiveAbilityWithTags(const FGameplayTagContainer& GameplayTagContainer) const
{
	TArray<UGameplayAbility*> ActiveAbilities;
	GetActiveAbilitiesWithTags(GameplayTagContainer, ActiveAbilities);
	return ActiveAbilities.Num() > 0;
}

void UCSAbilitySystemComponent::GetActiveAbilitiesWithTag(const FGameplayTag Tag, TArray<UGameplayAbility*>& ActiveAbilities) const
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(Tag);
	GetActiveAbilitiesWithTags(TagContainer, ActiveAbilities);
}

void UCSAbilitySystemComponent::GetActiveAbilitiesByClass(TSubclassOf<UGameplayAbility> InAbilityClass,
	TArray<UGameplayAbility*>& ActiveAbilities) const
{
	if (!IsValid(InAbilityClass))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Please provide a valid InAbilityClass to GetActiveAbilitiesByClass()");
		return;
	}
	
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (!Spec.Ability->GetClass()->IsChildOf(InAbilityClass))
		{
			continue;
		}

		const TArray<UGameplayAbility*>& Instances = Spec.GetAbilityInstances();
		for (UGameplayAbility* Instance : Instances)
		{
			if (!IsValid(Instance))
			{
				continue;
			}

			ActiveAbilities.Add(Instance);
		}
	}
}

void UCSAbilitySystemComponent::CancelAbilitiesByTags(const FGameplayTagContainer& WithTags, const FGameplayTagContainer& WithoutTags, UGameplayAbility* Ignore)
{
	CancelAbilities(&WithTags, &WithoutTags, Ignore);
}

void UCSAbilitySystemComponent::CancelAbilityByHandle(const FGameplayAbilitySpecHandle& AbilityHandle)
{
	CancelAbilityHandle(AbilityHandle);
}

FGameplayAbilityActorInfo& UCSAbilitySystemComponent::GetAbilityActorInfo() const
{
	if (!AbilityActorInfo.IsValid())
	{
		UE_LOGFMT(LogUnrealSharpGAS, Error, "GetAbilityActorInfo called but AbilityActorInfo is invalid!");
	}
	
	return *AbilityActorInfo;
}

AActor* UCSAbilitySystemComponent::GetAvatar() const
{
	return GetAvatarActor();
}

APlayerController* UCSAbilitySystemComponent::GetPlayerController() const
{
	check(AbilityActorInfo.IsValid());
	return AbilityActorInfo->PlayerController.Get();
}

void UCSAbilitySystemComponent::BindInput(UInputComponent* InputComponent, FCSInputBindData BindData)
{
	FGameplayAbilityInputBinds Binds(
		BindData.ConfirmTargetCommand.ToString(),
		BindData.CancelTargetCommand.ToString(),
		BindData.EnumName,
		BindData.ConfirmTargetInputID,
		BindData.CancelTargetInputID);

	BindAbilityActivationToInputComponent(InputComponent, Binds);
}

bool UCSAbilitySystemComponent::HasGameplayTag(FGameplayTag TagToCheck) const
{
	return HasMatchingGameplayTag(TagToCheck);
}

bool UCSAbilitySystemComponent::HasAllGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return HasAllMatchingGameplayTags(TagContainer);
}

bool UCSAbilitySystemComponent::HasAnyGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return HasAnyMatchingGameplayTags(TagContainer);
}

const FGameplayEffectSpec& UCSAttributeChangedDataExtensions::GetEffectSpec(
	const FCSAttributeChangedData& Data, bool& bIsValid)
{
	if (Data.WrappedData.GEModData)
	{
		bIsValid = true;
		return Data.WrappedData.GEModData->EffectSpec;
	}
	static FGameplayEffectSpec Dummy;
	bIsValid = false;
	return Dummy;
}

const FGameplayModifierEvaluatedData& UCSAttributeChangedDataExtensions::GetGameplayModifierEvaluatedData(
	const FCSAttributeChangedData& Data, bool& bIsValid)
{
	if (Data.WrappedData.GEModData)
	{
		bIsValid = true;
		return Data.WrappedData.GEModData->EvaluatedData;
	}
	static FGameplayModifierEvaluatedData Dummy;
	bIsValid = false;
	return Dummy;
}

UAbilitySystemComponent* UCSAttributeChangedDataExtensions::GetTargetAbilitySystemComponent(
	const FCSAttributeChangedData& Data)
{
	return Data.WrappedData.GEModData ? &Data.WrappedData.GEModData->Target : nullptr;
}

void UCSAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (OnInitAbilityActorInfo.IsBound())
	{
		OnInitAbilityActorInfo.Broadcast(InOwnerActor, InAvatarActor);
	}
}

void UCSAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	Super::OnTagUpdated(Tag, TagExists);

	if (OnOwnedTagUpdated.IsBound())
	{
		OnOwnedTagUpdated.Broadcast(Tag, TagExists);
	}
}

void UCSAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	if (OnAbilityGiven.IsBound())
	{
		OnAbilityGiven.Broadcast(AbilitySpec);
	}
}

void UCSAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);

	if (OnAbilityRemoved.IsBound())
	{
		OnAbilityRemoved.Broadcast(AbilitySpec);
	}
}

float UCSAbilitySystemComponent::GetCooldownTimeRemaining(TSubclassOf<UGameplayAbility> InAbilityClass) const
{
	if (!HasAbility(InAbilityClass))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "GetCooldownTimeRemaining called for class without ability instance");
		return -1.f;
	}

	const UGameplayAbility* AbilityCDO = InAbilityClass.GetDefaultObject();
	if (IsValid(AbilityCDO))
	{
		return 0.f;
	}

	const FGameplayTagContainer* CooldownTags = AbilityCDO->GetCooldownTags();
	if (CooldownTags == nullptr || CooldownTags->IsEmpty())
	{
		return 0.f;
	}

	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*CooldownTags);
	TArray<float> Durations = GetActiveEffectsTimeRemaining(Query);
	if (Durations.IsEmpty())
	{
		return 0.f;
	}

	Durations.Sort();
	return Durations.Last();
}

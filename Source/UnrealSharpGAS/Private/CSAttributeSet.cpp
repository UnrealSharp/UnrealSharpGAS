#include "CSAttributeSet.h"
#include "CSAbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Runtime/Engine/Public/Net/UnrealNetwork.h"

namespace
{
	bool IsCSAttributeStruct(const FStructProperty* Prop)
	{
		return Prop != nullptr
			&& Prop->Struct != nullptr
			&& Prop->Struct->IsChildOf(FCSGameplayAttributeData::StaticStruct());
	}
}

bool UCSAttributeSet::K2_PreGameplayEffectExecute_Implementation(const FGameplayEffectSpec& EffectSpec, FGameplayModifierEvaluatedData& EvaluatedData, UCSAbilitySystemComponent* AbilitySystemComponent)
{
	return true;
}

bool UCSAttributeSet::K2_OnInitFromMetaDataTable_Implementation(const UDataTable* DataTable)
{
	return false;
}

bool UCSAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	UCSAbilitySystemComponent* AbilitySystem = Cast<UCSAbilitySystemComponent>(&Data.Target);
	if (!IsValid(AbilitySystem))
	{
		return true;
	}

	return K2_PreGameplayEffectExecute(Data.EffectSpec, Data.EvaluatedData, AbilitySystem);
}

void UCSAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	UCSAbilitySystemComponent* AbilitySystem = Cast<UCSAbilitySystemComponent>(&Data.Target);
	if (!IsValid(AbilitySystem))
	{
		return;
	}

	K2_PostGameplayEffectExecute(Data.EffectSpec, Data.EvaluatedData, AbilitySystem);
}

void UCSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	K2_PreAttributeChange(Attribute, NewValue);
}

void UCSAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	K2_PostAttributeChange(Attribute, OldValue, NewValue);
}

void UCSAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	K2_PreAttributeBaseChange(Attribute, NewValue);
}

void UCSAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	K2_PostAttributeBaseChange(Attribute, OldValue, NewValue);
}

void UCSAttributeSet::InitFromMetaDataTable(const UDataTable* DataTable)
{
	bool bHandledInBP = K2_OnInitFromMetaDataTable(DataTable);
	if (bHandledInBP)
	{
		return;
	}

	Super::InitFromMetaDataTable(DataTable);
}

void UCSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	for (TFieldIterator<FStructProperty> It(GetClass()); It; ++It)
	{
		const FStructProperty* StructProp = *It;
		if (!IsCSAttributeStruct(StructProp))
		{
			continue;
		}

		if (!StructProp->HasAllPropertyFlags(CPF_Net))
		{
			continue;
		}

		RegisterReplicatedLifetimeProperty(StructProp, OutLifetimeProps, FDoRepLifetimeParams());
	}
}

void UCSAttributeSet::PostInitProperties()
{
	Super::PostInitProperties();

	for (TFieldIterator<FStructProperty> It(GetClass()); It; ++It)
	{
		FStructProperty* StructProp = *It;
		if (!IsCSAttributeStruct(StructProp))
		{
			continue;
		}

		FCSGameplayAttributeData* AttributeData = StructProp->ContainerPtrToValuePtr<FCSGameplayAttributeData>(this);
		if (AttributeData == nullptr)
		{
			continue;
		}

		FName AttributeName(*StructProp->GetName());
		AttributeData->AttributeName = AttributeName;
	}
}

void UCSAttributeSet::OnRep_Attribute(FCSGameplayAttributeData& OldAttributeData)
{
	FProperty* FoundProperty = FindFProperty<FProperty>(GetClass(), OldAttributeData.AttributeName);
	FStructProperty* StructProp = CastField<FStructProperty>(FoundProperty);
	if (!IsCSAttributeStruct(StructProp))
	{
		return;
	}

	FCSGameplayAttributeData* NewAttributeData = StructProp->ContainerPtrToValuePtr<FCSGameplayAttributeData>(this);
	if (NewAttributeData == nullptr)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystem = GetOwningAbilitySystemComponent();
	if (!IsValid(AbilitySystem))
	{
		return;
	}

	AbilitySystem->SetBaseAttributeValueFromReplication(FGameplayAttribute(StructProp), *NewAttributeData, OldAttributeData);
}

AActor* UCSAttributeSet::K2_GetOwningActor() const
{
	return GetOwningActor();
}

UCSAbilitySystemComponent* UCSAttributeSet::K2_GetOwningAbilitySystemComponent() const
{
	return Cast<UCSAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

FGameplayAbilityActorInfo& UCSAttributeSet::K2_GetActorInfo() const
{
	return *GetActorInfo();
}

bool UCSAttributeSet::TrySetAttributeBaseValue(FName AttributeName, float NewBaseValue) const
{
	UCSAbilitySystemComponent* AbilitySystem = K2_GetOwningAbilitySystemComponent();
	if (!IsValid(AbilitySystem))
	{
		return false;
	}

	return AbilitySystem->TrySetAttributeBaseValue(GetClass(), AttributeName, NewBaseValue);
}

bool UCSAttributeSet::TryGetAttributeCurrentValue(FName AttributeName, float& OutCurrentValue) const
{
	UCSAbilitySystemComponent* AbilitySystem = K2_GetOwningAbilitySystemComponent();
	if (!IsValid(AbilitySystem))
	{
		return false;
	}

	return AbilitySystem->TryGetAttributeCurrentValue(GetClass(), AttributeName, OutCurrentValue);
}

bool UCSAttributeSet::TryGetAttributeBaseValue(FName AttributeName, float& OutBaseValue) const
{
	UCSAbilitySystemComponent* AbilitySystem = K2_GetOwningAbilitySystemComponent();
	if (!IsValid(AbilitySystem))
	{
		return false;
	}

	return AbilitySystem->TryGetAttributeBaseValue(GetClass(), AttributeName, OutBaseValue);
}

FGameplayAttribute UCSAttributeSet::GetGameplayAttribute(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName)
{
	FGameplayAttribute Attribute;

	bool bSuccess = TryGetGameplayAttribute(AttributeSetClass, AttributeName, Attribute);
	if (!bSuccess || !Attribute.IsValid())
	{
		return FGameplayAttribute();
	}

	return Attribute;
}

bool UCSAttributeSet::TryGetGameplayAttribute(TSubclassOf<UCSAttributeSet> AttributeSetClass, FName AttributeName, FGameplayAttribute& OutGameplayAttribute)
{
	if (!IsValid(AttributeSetClass))
	{
		return false;
	}

	FProperty* Property = AttributeSetClass->FindPropertyByName(AttributeName);
	if (!Property)
	{
		return false;
	}

	OutGameplayAttribute = FGameplayAttribute(Property);
	return true;
}

bool UCSAttributeSet::CompareGameplayAttributes(const FGameplayAttribute& First, const FGameplayAttribute& Second)
{
	return First == Second;
}
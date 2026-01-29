#include "CSGameplayAbility.h"

#include "CSAbilityTask.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayCueNotify_Static.h"

void UCSGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	K2_InputPressed();
}

void UCSGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	K2_InputReleased();
}

void UCSGameplayAbility::K2_ExecuteGameplayCue_Actor(TSubclassOf<AGameplayCueNotify_Actor> GameplayCue, FGameplayEffectContextHandle Context)
{
	if (!IsValid(GameplayCue))
	{
		return;
	}

	K2_ExecuteGameplayCue(GameplayCue.GetDefaultObject()->GameplayCueTag, Context);
}

void UCSGameplayAbility::K2_ExecuteGameplayCueWithParams_Actor(TSubclassOf<AGameplayCueNotify_Actor> GameplayCue, const FGameplayCueParameters& GameplayCueParameters)
{
	if (!IsValid(GameplayCue))
	{
		return;
	}
	
	K2_ExecuteGameplayCueWithParams(GameplayCue.GetDefaultObject()->GameplayCueTag, GameplayCueParameters);
}

void UCSGameplayAbility::K2_AddGameplayCue_Actor(TSubclassOf<AGameplayCueNotify_Actor> GameplayCue, FGameplayEffectContextHandle Context, bool bRemoveOnAbilityEnd)
{
	if (!IsValid(GameplayCue))
	{
		return;
	}

	K2_AddGameplayCue(GameplayCue.GetDefaultObject()->GameplayCueTag, Context, bRemoveOnAbilityEnd);
}

void UCSGameplayAbility::K2_AddGameplayCueWithParams_Actor(TSubclassOf<AGameplayCueNotify_Actor> GameplayCue, const FGameplayCueParameters& GameplayCueParameter, bool bRemoveOnAbilityEnd)
{
	if (!IsValid(GameplayCue))
	{
		return;
	}

	K2_AddGameplayCueWithParams(GameplayCue.GetDefaultObject()->GameplayCueTag, GameplayCueParameter, bRemoveOnAbilityEnd);
}

void UCSGameplayAbility::K2_RemoveGameplayCue_Actor(TSubclassOf<AGameplayCueNotify_Actor> GameplayCue)
{
	if (!IsValid(GameplayCue))
	{
		return;
	}

	K2_RemoveGameplayCue(GameplayCue.GetDefaultObject()->GameplayCueTag);
}

void UCSGameplayAbility::K2_ExecuteGameplayCue_Static(TSubclassOf<UGameplayCueNotify_Static> GameplayCue, FGameplayEffectContextHandle Context)
{
	if (!IsValid(GameplayCue))
	{
		return;
	}

	K2_ExecuteGameplayCue(GameplayCue.GetDefaultObject()->GameplayCueTag, Context);
}

void UCSGameplayAbility::K2_ExecuteGameplayCueWithParams_Static(TSubclassOf<UGameplayCueNotify_Static> GameplayCue, const FGameplayCueParameters& GameplayCueParameters)
{
	if (!IsValid(GameplayCue))
	{
		return;
	}

	K2_ExecuteGameplayCueWithParams(GameplayCue.GetDefaultObject()->GameplayCueTag, GameplayCueParameters);
}

void UCSGameplayAbility::K2_AddGameplayCue_Static(TSubclassOf<UGameplayCueNotify_Static> GameplayCue, FGameplayEffectContextHandle Context, bool bRemoveOnAbilityEnd)
{
	if (!IsValid(GameplayCue))
	{
		return;
	}

	K2_AddGameplayCue(GameplayCue.GetDefaultObject()->GameplayCueTag, Context, bRemoveOnAbilityEnd);
}

void UCSGameplayAbility::K2_AddGameplayCueWithParams_Static(TSubclassOf<UGameplayCueNotify_Static> GameplayCue, const FGameplayCueParameters& GameplayCueParameter, bool bRemoveOnAbilityEnd)
{
	if (!IsValid(GameplayCue))
	{
		return;
	}

	K2_AddGameplayCueWithParams(GameplayCue.GetDefaultObject()->GameplayCueTag, GameplayCueParameter, bRemoveOnAbilityEnd);
}

void UCSGameplayAbility::K2_RemoveGameplayCue_Static(TSubclassOf<UGameplayCueNotify_Static> GameplayCue)
{
	if (!IsValid(GameplayCue))
	{
		return;
	}

	K2_RemoveGameplayCue(GameplayCue.GetDefaultObject()->GameplayCueTag);
}

void UCSGameplayAbility::K2_GetDynamicSourceTags(FGameplayTagContainer& TagContainer) const
{
	FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();

	if (!AbilitySpec)
	{
		return;
	}

	TagContainer = AbilitySpec->GetDynamicSpecSourceTags();
}

UCSAbilityTask* UCSGameplayAbility::CreateAbilityTask(TSubclassOf<UCSAbilityTask> TaskClass)
{
	return UCSAbilityTask::CreateAbilityTask(TaskClass, this);
}

UCSAbilityTask* UCSGameplayAbility::CreateAbilityTaskAndRunIt(TSubclassOf<UCSAbilityTask> TaskClass)
{
	return UCSAbilityTask::CreateAbilityTaskAndRunIt(TaskClass, this);
}


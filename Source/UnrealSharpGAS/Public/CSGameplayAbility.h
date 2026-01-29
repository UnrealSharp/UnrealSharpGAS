#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"

#include "CSGameplayAbility.generated.h"

class UCSAbilityTask;
class AGameplayCueNotify_Actor;
class AGameplayCueNotify_Static;

UCLASS(Abstract, Blueprintable)
class UNREALSHARPGAS_API UCSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	// UGameplayAbility interface
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	// End of UGameplayAbility interface

	UFUNCTION(meta = (ScriptMethod))
	bool K2_IsActive() const { return IsActive(); }

	UFUNCTION(meta = (ScriptMethod))
	bool K2_IsTriggered() const { return IsTriggered(); }

	UFUNCTION(meta = (ScriptMethod))
	bool K2_IsPredictingClient() const { return IsPredictingClient(); }

	UFUNCTION(meta = (ScriptMethod))
	bool K2_IsForRemoteClient() const { return IsForRemoteClient(); }
	
	UFUNCTION(meta = (ScriptMethod))
	void K2_ExecuteGameplayCue_Actor(TSubclassOf<AGameplayCueNotify_Actor> GameplayCue, FGameplayEffectContextHandle Context);
	
	UFUNCTION(meta = (ScriptMethod))
	void K2_ExecuteGameplayCueWithParams_Actor(TSubclassOf<AGameplayCueNotify_Actor> GameplayCue, const FGameplayCueParameters& GameplayCueParameters);
	
	UFUNCTION(meta = (ScriptMethod))
	void K2_AddGameplayCue_Actor(TSubclassOf<AGameplayCueNotify_Actor> GameplayCue, FGameplayEffectContextHandle Context, bool bRemoveOnAbilityEnd = true);

	UFUNCTION(meta = (ScriptMethod))
	void K2_AddGameplayCueWithParams_Actor(TSubclassOf<AGameplayCueNotify_Actor> GameplayCue, const FGameplayCueParameters& GameplayCueParameter, bool bRemoveOnAbilityEnd = true);
	
	UFUNCTION(meta = (ScriptMethod))
	void K2_RemoveGameplayCue_Actor(TSubclassOf<AGameplayCueNotify_Actor> GameplayCue);
	
	UFUNCTION(meta = (ScriptMethod))
	void K2_ExecuteGameplayCue_Static(TSubclassOf<UGameplayCueNotify_Static> GameplayCue, FGameplayEffectContextHandle Context);
	
	UFUNCTION(meta = (ScriptMethod))
	void K2_ExecuteGameplayCueWithParams_Static(TSubclassOf<UGameplayCueNotify_Static> GameplayCue, const FGameplayCueParameters& GameplayCueParameters);
	
	UFUNCTION(meta = (ScriptMethod))
	void K2_AddGameplayCue_Static(TSubclassOf<UGameplayCueNotify_Static> GameplayCue, FGameplayEffectContextHandle Context, bool bRemoveOnAbilityEnd = true);
	
	UFUNCTION(meta = (ScriptMethod))
	void K2_AddGameplayCueWithParams_Static(TSubclassOf<UGameplayCueNotify_Static> GameplayCue, const FGameplayCueParameters& GameplayCueParameter, bool bRemoveOnAbilityEnd = true);
	
	UFUNCTION(meta = (ScriptMethod))
	void K2_RemoveGameplayCue_Static(TSubclassOf<UGameplayCueNotify_Static> GameplayCue);

	UFUNCTION(meta = (ScriptMethod))
	void K2_GetDynamicSourceTags(FGameplayTagContainer& TagContainer) const;

	UFUNCTION(meta = (ScriptMethod, DeterminesOutputType = "TaskClass"))
	UCSAbilityTask* CreateAbilityTask(TSubclassOf<UCSAbilityTask> TaskClass);
	
	UFUNCTION(meta = (ScriptMethod, DeterminesOutputType = "TaskClass"))
	UCSAbilityTask* CreateAbilityTaskAndRunIt(TSubclassOf<UCSAbilityTask> TaskClass);
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void K2_InputPressed();
	
	UFUNCTION(BlueprintImplementableEvent)
	void K2_InputReleased();
};

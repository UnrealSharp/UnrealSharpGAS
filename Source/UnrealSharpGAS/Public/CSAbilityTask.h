#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "CSAbilityTask.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UGameplayTasksComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSAbilityTaskDelegate, UCSAbilityTask*, AbilityTask);

UCLASS()
class UNREALSHARPGAS_API UCSAbilityTask : public UAbilityTask
{
	GENERATED_BODY()

public:

	UCSAbilityTask(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UAbilityTask interface
	virtual void TickTask(float DeltaTime) override;
	virtual void InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent) override;
	virtual bool IsWaitingOnRemotePlayerdata() const override;
	virtual bool IsWaitingOnAvatar() const override;
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	// End of UAbilityTask interface

	UFUNCTION(BlueprintImplementableEvent)
	void K2_Activate();

	UFUNCTION(BlueprintImplementableEvent)
	void K2_Tick(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnDestroy(bool bInOwnerFinished);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_InitSimulatedTask(UGameplayTasksComponent* InGameplayTasksComponent);

	UFUNCTION(BlueprintNativeEvent)
	bool K2_IsWaitingOnRemotePlayerData() const;

	UFUNCTION(BlueprintNativeEvent)
	bool K2_IsWaitingOnAvatar() const;
	
	UFUNCTION(meta = (ScriptMethod))
	bool GetIsTickingTask() const { return bTickingTask; }

	UFUNCTION(meta = (ScriptMethod))
	void SetIsTickingTask(bool bNewState) { bTickingTask =  bNewState; }

	UFUNCTION(meta = (ScriptMethod))
	bool GetIsPausable() const { return bIsPausable; }

	UFUNCTION(meta = (ScriptMethod))
	void SetIsPausable(bool bNewState) { bIsPausable = bNewState; }

	UFUNCTION(meta = (ScriptMethod))
	bool GetIsSimulatedTask() const { return bSimulatedTask; }

	UFUNCTION(meta = (ScriptMethod))
	void SetIsSimulatedTask(bool bNewState) { bSimulatedTask = bNewState; }

	UFUNCTION(meta = (ScriptMethod))
	bool GetIsSimulating() const { return bIsSimulating; }
	
	UFUNCTION(meta = (ScriptMethod))
	void K2_SetAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent) { SetAbilitySystemComponent(InAbilitySystemComponent); }

	UFUNCTION(meta = (ScriptMethod))
	UAbilitySystemComponent* K2_GetAbilitySystemComponent() const { return AbilitySystemComponent.Get(); }

	UFUNCTION(meta = (ScriptMethod))
	FGameplayAbilitySpecHandle K2_GetAbilitySpecHandle() const { return GetAbilitySpecHandle(); }

	UFUNCTION(meta = (ScriptMethod))
	UGameplayAbility* K2_GetAbility() const { return Ability; }

	UFUNCTION(meta = (ScriptMethod))
	bool K2_IsPredictingClient() const { return IsPredictingClient(); }

	UFUNCTION(meta = (ScriptMethod))
	bool K2_IsForRemoteClient() const { return IsForRemoteClient(); }

	UFUNCTION(meta = (ScriptMethod))
	bool K2_IsLocallyControlled() const { return IsLocallyControlled(); }

	UFUNCTION(meta = (ScriptMethod))
	bool K2_ShouldBroadcastAbilityTaskDelegates() const { return ShouldBroadcastAbilityTaskDelegates(); }
	
	UFUNCTION(meta = (ScriptMethod))
	void K2_SetWaitingOnRemotePlayerData() { SetWaitingOnRemotePlayerData(); }

	UFUNCTION(meta = (ScriptMethod))
	void K2_ClearWaitingOnRemotePlayerData() { ClearWaitingOnRemotePlayerData(); }

	UFUNCTION(meta = (ScriptMethod))
	void K2_SetWaitingOnAvatar() { SetWaitingOnAvatar(); }

	UFUNCTION(meta = (ScriptMethod))
	void K2_ClearWaitingOnAvatar() { ClearWaitingOnAvatar(); }

	UFUNCTION(meta = (ScriptMethod))
	AActor* K2_GetAvatarActor() const { return GetAvatarActor(); }

	UFUNCTION(meta = (ScriptMethod))
	APlayerController* K2_GetPlayerController() const;

	UFUNCTION(meta = (ScriptMethod))
	AController* K2_GetController() const;

	UFUNCTION(meta = (ScriptMethod))
	AActor* K2_GetOwnerActor() const { return GetOwnerActor(); }
	
	UFUNCTION(meta = (ScriptMethod))
	static UCSAbilityTask* CreateAbilityTask(TSubclassOf<UCSAbilityTask> Task, UGameplayAbility* InstigatorAbility, FName TaskInstanceName = NAME_None);
	
	UFUNCTION(meta = (ScriptMethod))
	static UCSAbilityTask* CreateAbilityTaskAndRunIt(TSubclassOf<UCSAbilityTask> Task, UGameplayAbility* InstigatorAbility, FName TaskInstanceName = NAME_None);
	
	UPROPERTY(BlueprintAssignable)
	FCSAbilityTaskDelegate OnTaskActivated;
	
	UPROPERTY(BlueprintAssignable)
	FCSAbilityTaskDelegate OnTaskEnded;
};
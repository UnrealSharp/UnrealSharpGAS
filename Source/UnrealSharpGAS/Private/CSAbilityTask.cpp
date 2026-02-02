#include "CSAbilityTask.h"

#include "UnrealSharpGAS.h"
#include "UObject/Package.h"
#include "Utilities/CSClassUtilities.h"

void UCSAbilityTask::Activate()
{
	Super::Activate();
	
	K2_Activate();
	OnTaskActivated.Broadcast(this);
}

UCSAbilityTask::UCSAbilityTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static FName TickFunctionName = "K2_Tick";
	bool HasImplementedTick = FCSClassUtilities::HasImplementedFunction(GetClass(), TickFunctionName);
	SetIsTickingTask(HasImplementedTick);
}

void UCSAbilityTask::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	K2_Tick(DeltaTime);
}

void UCSAbilityTask::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	
	K2_OnDestroy(bInOwnerFinished);
	OnTaskEnded.Broadcast(this);
}

void UCSAbilityTask::InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent)
{
	Super::InitSimulatedTask(InGameplayTasksComponent);
	K2_InitSimulatedTask(&InGameplayTasksComponent);
}

bool UCSAbilityTask::IsWaitingOnRemotePlayerdata() const
{
	return K2_IsWaitingOnRemotePlayerData();
}

bool UCSAbilityTask::IsWaitingOnAvatar() const
{
	return K2_IsWaitingOnAvatar();
}

bool UCSAbilityTask::K2_IsWaitingOnRemotePlayerData_Implementation() const
{
	return Super::IsWaitingOnRemotePlayerdata();
}

bool UCSAbilityTask::K2_IsWaitingOnAvatar_Implementation() const
{
	return Super::IsWaitingOnAvatar();
}

APlayerController* UCSAbilityTask::K2_GetPlayerController() const
{
	AController* Controller = K2_GetController();

	if (!IsValid(Controller))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "K2_GetPlayerController called but Controller is invalid.");
		return nullptr;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!IsValid(PlayerController))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "K2_GetPlayerController called but Controller is not a PlayerController.");
		return nullptr;
	}

	return PlayerController;
}

AController* UCSAbilityTask::K2_GetController() const
{
	APawn* AvatarActor = Cast<APawn>(GetAvatarActor());
	
	if (!IsValid(AvatarActor))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "K2_GetController called but AvatarActor is invalid.");
		return nullptr;
	}

	return AvatarActor->GetController();
}

UCSAbilityTask* UCSAbilityTask::CreateAbilityTask(TSubclassOf<UCSAbilityTask> Task, UGameplayAbility* InstigatorAbility, FName TaskInstanceName)
{
	if (!IsValid(Task))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Error, "CreateAbilityTask called with invalid task type.");
		return nullptr;
	}

	if (!IsValid(InstigatorAbility))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Error, "CreateAbilityTask called without valid InstigatorAbility.");
		return nullptr;
	}

	UCSAbilityTask* NewTask = NewObject<UCSAbilityTask>(GetTransientPackage(), Task.Get());
	if (!IsValid(NewTask))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Error, "CreateAbilityTask failed to create new instance of task type '{0}'.", *Task->GetName());
		return nullptr;
	}

	NewTask->InitTask(*InstigatorAbility, InstigatorAbility->GetGameplayTaskDefaultPriority());
	NewTask->InstanceName = TaskInstanceName;

	return NewTask;
}


UCSAbilityTask* UCSAbilityTask::CreateAbilityTaskAndRunIt(TSubclassOf<UCSAbilityTask> Task, UGameplayAbility* InstigatorAbility, FName TaskInstanceName)
{
	UCSAbilityTask* NewTask = CreateAbilityTask(Task, InstigatorAbility, TaskInstanceName);
	
	if (!IsValid(NewTask))
	{
		UE_LOGFMT(LogUnrealSharpGAS, Warning, "Could not create ability task of class <{}>", *GetNameSafe(Task.Get()));
		return nullptr;
	}

	NewTask->ReadyForActivation();
	return NewTask;
}

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "Engine/DataAsset.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"

#include "GameplayAbilitySpecHandle.h"
#include "CSAbilitySet.generated.h"

class UCSGameplayAbility;
class UCSAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UObject;

USTRUCT(BlueprintType)
struct UNREALSHARPGAS_API FCSAbilitySet_GameplayAbility
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCSGameplayAbility> Ability = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
	
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

USTRUCT(BlueprintType)
struct UNREALSHARPGAS_API FCSAbilitySet_GameplayEffect
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

USTRUCT(BlueprintType)
struct UNREALSHARPGAS_API FCSAbilitySet_AttributeSet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};

USTRUCT(BlueprintType)
struct UNREALSHARPGAS_API FCSAbilitySet_GrantedHandles
{
	GENERATED_BODY()

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(UCSAbilitySystemComponent* AbilitySystemComponent);

protected:
	
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
	
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;
	
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

UCLASS(BlueprintType, Const)
class UNREALSHARPGAS_API UCSAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	void GiveToAbilitySystem(UCSAbilitySystemComponent* AbilitySystemComponent, FCSAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;
protected:
	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FCSAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FCSAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FCSAbilitySet_AttributeSet> GrantedAttributes;
};

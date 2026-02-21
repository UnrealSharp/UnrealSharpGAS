#pragma once

#include "CoreMinimal.h"
#include "UnrealSharpRuntimeGlue/Public/CSGlueGenerator.h"
#include "CSGameplayAttributesGlueGenerator.generated.h"

class UCSGameplayAttributeSubsystem;

UCLASS(meta=(NoGlue), MinimalAPI)
class UCSGameplayAttributesGlueGenerator : public UCSGlueGenerator
{
	GENERATED_BODY()
public:
	// UCSGlueGenerator interface
	virtual void Initialize() override;
	virtual void ForceRefresh() override { ProcessGameplayAttributes(); }
	// End of UCSGlueGenerator interface
private:
	void ProcessGameplayAttributes();
	
	static void FindAllAttributeSetClasses(TArray<UClass*>& OutAttributeSetClasses);
	static void GenerateAttributesForClass(const UClass* AttributeSetClass, FCSScriptBuilder& ScriptBuilder, const UCSGameplayAttributeSubsystem* AttributeSubsystem);
	
	static FString PropertyNameToVariableName(const FString& PropertyName);
	static FString ClassNameToValidName(const FString& ClassName);
	
	void OnEditorInitialized(double Duration);
};

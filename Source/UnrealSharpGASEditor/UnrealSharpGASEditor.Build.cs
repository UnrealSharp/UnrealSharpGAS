using UnrealBuildTool;

public class UnrealSharpGASEditor : ModuleRules
{
    public UnrealSharpGASEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", 
                "GameplayAbilities", 
                "UnrealSharpRuntimeGlue",
                "UnrealEd",
                "UnrealSharpGAS"
            }
        );
        
        PublicDefinitions.Add("FlattenGlue=1");
    }
}
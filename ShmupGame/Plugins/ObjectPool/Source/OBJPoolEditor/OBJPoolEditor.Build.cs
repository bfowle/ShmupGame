using UnrealBuildTool;

public class OBJPoolEditor : ModuleRules {
    public OBJPoolEditor(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;
		//
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "Engine",
                "OBJPool",
                "CoreUObject",
                "OBJPoolDeveloper"
            }
        );
        //
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Slate",
                "Projects",
                "UnrealEd",
                "SlateCore",
                "AssetTools",
                "EditorStyle",
                "LevelEditor"
            }
        );
    }
}
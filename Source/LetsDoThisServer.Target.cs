using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class LetsDoThisServerTarget : TargetRules   // Change this line as shown previously
{
       public LetsDoThisServerTarget(TargetInfo Target) : base(Target)  // Change this line as shown previously
       {
        Type = TargetType.Server;
        ExtraModuleNames.Add("LetsDoThis");    // Change this line as shown previously
       }
}
// FLevelStreamInstanceInfo
// taken from // https://github.com/EverNewJoy/VictoryPlugin/pull/14/files
// by KrisRedbeard to EverNewJoy

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/LevelStreamingKismet.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "TriHard.generated.h"

USTRUCT(BlueprintType)
struct FLevelStreamInstanceInfo {
  GENERATED_USTRUCT_BODY()

  UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
  FName PackageName;

  UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
  FName PackageNameToLoad;

  UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
  FVector Location;

  UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
  FRotator Rotation;

  UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
  uint8 bShouldBeLoaded : 1;

  UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
  uint8 bShouldBeVisible : 1;

  UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
  uint8 bShouldBlockOnLoad : 1;

  UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
  int32 LODIndex;

  FLevelStreamInstanceInfo() {}
  FLevelStreamInstanceInfo(ULevelStreamingKismet* LevelInstance);

  FString ToString() const {
    return FString::Printf(TEXT("PackageName: %s\nPackageNameToLoad:%s\nLocation:%s\nRotation:%s\nbShouldBeLoaded:%s\nbShouldBeVisible:%s\nbShouldBlockOnLoad:%s\nLODIndex:%i")
                           , *PackageName.ToString()
                           , *PackageNameToLoad.ToString()
                           , *Location.ToString()
                           , *Rotation.ToString()
                           , (bShouldBeLoaded) ? TEXT("True") : TEXT("False")
                           , (bShouldBeVisible) ? TEXT("True") : TEXT("False")
                           , (bShouldBlockOnLoad) ? TEXT("True") : TEXT("False")
                           , LODIndex);
  }
};

/**
 * 
 */
UCLASS()

class LETSDOTHIS_API UTriHard : public UBlueprintFunctionLibrary {
  GENERATED_BODY()

  UFUNCTION(Category = "LevelStreaming", BlueprintCallable)
  static FLevelStreamInstanceInfo GetLevelInstanceInfo(ULevelStreamingKismet* LevelInstance);

  UFUNCTION(Category = "LevelStreaming", BlueprintCallable, Meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
  static void AddToStreamingLevels(UObject* WorldContextObject, const FLevelStreamInstanceInfo& LevelInstanceInfo);


};

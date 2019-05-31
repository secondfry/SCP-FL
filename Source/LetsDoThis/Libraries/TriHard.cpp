// FLevelStreamInstanceInfo, GetLevelInstanceInfo, AddToStreamingLevels
// taken from // https://github.com/EverNewJoy/VictoryPlugin/pull/14/files
// by KrisRedbeard to EverNewJoy

#include "TriHard.h"

FLevelStreamInstanceInfo::FLevelStreamInstanceInfo(ULevelStreamingDynamic* LevelInstance) {
  PackageName = LevelInstance->GetWorldAssetPackageFName();
  PackageNameToLoad = LevelInstance->PackageNameToLoad;
  Location = LevelInstance->LevelTransform.GetLocation();
  Rotation = LevelInstance->LevelTransform.GetRotation().Rotator();
  bShouldBeLoaded = LevelInstance->ShouldBeLoaded();
  bShouldBeVisible = LevelInstance->ShouldBeVisible();
  bShouldBlockOnLoad = LevelInstance->bShouldBlockOnLoad;
  LODIndex = LevelInstance->GetLevelLODIndex();
};

FLevelStreamInstanceInfo UTriHard::GetLevelInstanceInfo(ULevelStreamingDynamic* LevelInstance) {
  return FLevelStreamInstanceInfo(LevelInstance);
}

void UTriHard::AddToStreamingLevels(UObject* WorldContextObject, const FLevelStreamInstanceInfo& LevelInstanceInfo) {
  UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
  if (World == nullptr) {
    return;
  }

  bool bAlreadyExists = false;
  
  for (auto StreamingLevel : World->GetStreamingLevels()) {
    if (StreamingLevel->GetWorldAssetPackageFName() == LevelInstanceInfo.PackageName) {
      bAlreadyExists = true;
      // KRIS : Would normally log a warning here! Is there a LogVictory?
      break;
    }
  }

  if (bAlreadyExists) {
    return;
  }

  FName PackageName = LevelInstanceInfo.PackageName;
  // For PIE Networking: remap the packagename to our local PIE packagename
  FString PackageNameStr = PackageName.ToString();
  if (GEngine->NetworkRemapPath(World->GetNetDriver(), PackageNameStr, true)) {
    PackageName = FName(*PackageNameStr);
  }

  // WHY?
  GEngine->DelayGarbageCollection();

  // Setup streaming level object that will load specified map
  ULevelStreamingDynamic* StreamingLevel = NewObject<ULevelStreamingDynamic>(World, ULevelStreamingDynamic::StaticClass(), NAME_None, RF_Transient, nullptr);
  StreamingLevel->SetWorldAssetByPackageName(PackageName);
  StreamingLevel->LevelColor = FColor::MakeRandomColor();
  StreamingLevel->SetShouldBeLoaded(LevelInstanceInfo.bShouldBeLoaded);
  StreamingLevel->SetShouldBeVisible(LevelInstanceInfo.bShouldBeVisible);
  StreamingLevel->bShouldBlockOnLoad = LevelInstanceInfo.bShouldBlockOnLoad;
  StreamingLevel->SetShouldBeVisible(true);
  // Transform
  StreamingLevel->LevelTransform = FTransform(LevelInstanceInfo.Rotation, LevelInstanceInfo.Location);
  // Map to Load
  StreamingLevel->PackageNameToLoad = LevelInstanceInfo.PackageNameToLoad;
  // Add the new level to world.
  World->AddStreamingLevel(StreamingLevel);
  
  // WHY?
  // World->FlushLevelStreaming(EFlushLevelStreamingType::Full);
}

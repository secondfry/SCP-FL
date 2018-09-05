// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Maze/RoomDataStruct.h"
#include "Maze/Maze.h"

#include "LetsDoThisGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class LETSDOTHIS_API ALetsDoThisGameModeBase : public AGameModeBase {
  GENERATED_BODY()

public:
  AActor* ChoosePlayerStart_Implementation(AController* Player) override {
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(reinterpret_cast<UObject*>(this->GetWorld()), APlayerStart::StaticClass(), FoundActors);

    for (AActor* actor : FoundActors) {
      APlayerStart* PlayerStart = static_cast<APlayerStart*>(actor);
      if (PlayerStart == nullptr) { continue; }

      bool IsDefault = PlayerStart->ActorHasTag("default");
      if (IsDefault) { return actor; }

      continue;

      bool IsTaken = PlayerStart->ActorHasTag("taken");
      if (IsTaken) { return actor; }

      PlayerStart->Tags.Add("taken");
      
      return actor;
    }

    return FoundActors[0];
  }

  UFUNCTION(BlueprintCallable, Category = "Map Generation")
  static TArray<FRoomDataStruct> generateMap() {
    return Maze::generateMap();
  }
};

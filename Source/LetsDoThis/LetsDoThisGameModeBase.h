// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
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
  UFUNCTION(BlueprintCallable, Category = "Map Generation")
  static TArray<FRoomDataStruct> GenerateMap(TArray<FString> requestedRooms, FString& seedOut, FString seed = "");
};

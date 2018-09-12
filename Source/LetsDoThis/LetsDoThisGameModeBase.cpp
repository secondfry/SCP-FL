// Fill out your copyright notice in the Description page of Project Settings.

#include "LetsDoThisGameModeBase.h"

TArray<FRoomDataStruct> ALetsDoThisGameModeBase::GenerateMap(FString seed) {
  if (seed == "") {
    seed = Maze::GenerateSeed();
  }

  return *Maze::GenerateMap(seed);
}

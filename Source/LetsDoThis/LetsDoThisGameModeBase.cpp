// Fill out your copyright notice in the Description page of Project Settings.

#include "LetsDoThisGameModeBase.h"

TArray<FRoomDataStruct> ALetsDoThisGameModeBase::GenerateMap(FString& seedOut, FString seed) {
  if (seed == "") {
    seed = Maze::GenerateSeed();
  }

  auto ret = *Maze::GenerateMap(seed);
  seedOut = Maze::GetSeed();
  return ret;
}

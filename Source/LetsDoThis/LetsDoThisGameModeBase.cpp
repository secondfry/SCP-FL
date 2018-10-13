// Fill out your copyright notice in the Description page of Project Settings.

#include "LetsDoThisGameModeBase.h"

TArray<FRoomDataStruct> ALetsDoThisGameModeBase::GenerateMap(TArray<FString> requestedRooms, FString& seedOut, FString seed) {
  auto ret = *Maze::GenerateMap(seed, requestedRooms);
  seedOut = Maze::GetSeed();
  return ret;
}

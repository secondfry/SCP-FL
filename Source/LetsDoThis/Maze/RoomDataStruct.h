// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "RoomDataStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct LETSDOTHIS_API FRoomDataStruct
{
  GENERATED_BODY()

  UPROPERTY(BlueprintReadWrite, Category = "Room Data")
  FName roomName;

  UPROPERTY(BlueprintReadWrite, Category = "Room Data")
  int x;

  UPROPERTY(BlueprintReadWrite, Category = "Room Data")
  int y;

  UPROPERTY(BlueprintReadWrite, Category = "Room Data")
  int z;

  UPROPERTY(BlueprintReadWrite, Category = "Room Data")
  int yaw;
};

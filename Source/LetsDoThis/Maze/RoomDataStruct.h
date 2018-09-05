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
  float x;

  UPROPERTY(BlueprintReadWrite, Category = "Room Data")
  float y;

  UPROPERTY(BlueprintReadWrite, Category = "Room Data")
  float z;

  UPROPERTY(BlueprintReadWrite, Category = "Room Data")
  float yaw;

};

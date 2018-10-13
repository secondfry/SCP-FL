// Config Registry
// by Rustam @Second_Fry Gubaydullin

#pragma once

#include "../external/json.hpp"
using json = nlohmann::json;

#include <string>

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "ConfigRegistry.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSCPFLConfig, Log, All);

UCLASS()
class LETSDOTHIS_API UConfigRegistry : public UBlueprintFunctionLibrary {
  GENERATED_BODY()

public:
  UFUNCTION(Category = "RuntimeConfiguration", BlueprintCallable)
  static void ReadConfig();

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintCallable)
  static void WriteConfig();

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintPure)
  static const bool GetConfigValid();

  template<typename T>
  static const T GetVal(const FString variable, const T defaultValue);

  template<typename T>
  static void SetVal(const FString variable, const T value, const bool shouldWrite);

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintPure)
  static const bool GetBool(const FString variable, const bool defaultValue = false);

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintCallable)
  static void SetBool(const FString variable, const bool value, const bool shouldWrite = false);

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintPure)
  static const int GetInt(const FString variable, const int defaultValue = 0);

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintCallable)
  static void SetInt(const FString variable, const int value, const bool shouldWrite = false);

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintPure)
  static const float GetFloat(const FString variable, const float defaultValue = 0);

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintCallable)
  static void SetFloat(const FString variable, const float value, const bool shouldWrite = false);

  static const std::string GetStdString(const FString variable, const std::string defaultValue = "");
  static void SetStdString(const FString variable, const std::string value, const bool shouldWrite = false);

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintPure)
  static const FString GetString(const FString variable, const FString defaultValue = TEXT(""));

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintCallable)
  static void SetString(const FString variable, const FString value, const bool shouldWrite = false);

  static const std::vector<std::string> GetStdStringVector(const FString variable, const std::vector<std::string> defaultValue);
  static void SetStdStringVector(const FString variable, const std::vector<std::string> value, const bool shouldWrite = false);

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintPure)
  static const TArray<FString> GetStringArray(const FString variable, const TArray<FString> defaultValue);

  UFUNCTION(Category = "RuntimeConfiguration", BlueprintCallable)
  static void SetStringArray(const FString variable, const TArray<FString> value, const bool shouldWrite = false);

protected:
  static json config;

private:
  static bool isConfigValid;
  static const std::string GetConfigLocation();

};

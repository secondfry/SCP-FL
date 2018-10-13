// Config Registry
// by Rustam @Second_Fry Gubaydullin

#include "ConfigRegistry.h"

#include <iomanip>
#include <fstream>
#include <string>
#include <sys/stat.h>

#include "../external/json.hpp"
using json = nlohmann::json;


DEFINE_LOG_CATEGORY(LogSCPFLConfig);

bool UConfigRegistry::isConfigValid = false;
json UConfigRegistry::config;

void UConfigRegistry::ReadConfig() {
  std::string configPath = UConfigRegistry::GetConfigLocation();

  struct stat buffer;
  if (stat(configPath.c_str(), &buffer) != 0) {
    UConfigRegistry::WriteConfig();
    UConfigRegistry::isConfigValid = true;
    return;
  }

  std::ifstream configFile(configPath);
  try {
    configFile >> UConfigRegistry::config;
  } catch (nlohmann::detail::parse_error e) {
    UE_LOG(LogSCPFLConfig, Error, TEXT("JSON parsing failed! Error: %s"), e.what());
    return;
  }
  UConfigRegistry::isConfigValid = true;
}

void UConfigRegistry::WriteConfig() {
  std::string configPath = UConfigRegistry::GetConfigLocation();
  std::ofstream configFile(configPath);
  configFile << std::setw(2) << UConfigRegistry::config << std::endl;
}

const bool UConfigRegistry::GetConfigValid() {
  return UConfigRegistry::isConfigValid;
}

template<typename T>
const T UConfigRegistry::GetVal(const FString variable, const T defaultValue) {
  std::string var = TCHAR_TO_UTF8(*variable);
  try {
    return UConfigRegistry::config.at(var);
  } catch (nlohmann::detail::out_of_range e) {
    UE_LOG(LogSCPFLConfig, Warning, TEXT("%s is not part of config!"), var.c_str());
    UE_LOG(LogSCPFLConfig, Warning, TEXT("Using default value for %s and saving config with it"), var.c_str());
    SetVal(variable, defaultValue, true);
    return defaultValue;
  } catch (nlohmann::detail::exception e) {
    UE_LOG(LogSCPFLConfig, Error, TEXT("JSON access failed! Error: %s"), e.what());
    UE_LOG(LogSCPFLConfig, Warning, TEXT("Using default value for %s and saving config with it"), var.c_str());
    SetVal(variable, defaultValue, true);
    return defaultValue;
  }
}

template<typename T>
void UConfigRegistry::SetVal(const FString variable, const T value, const bool shouldWrite) {
  std::string var = TCHAR_TO_UTF8(*variable);
  UConfigRegistry::config[var] = value;

  if (!shouldWrite) {
    return;
  }

  UConfigRegistry::WriteConfig();
}

const bool UConfigRegistry::GetBool(const FString variable, const bool defaultValue) {
  return GetVal<bool>(variable, defaultValue);
}

void UConfigRegistry::SetBool(const FString variable, const bool value, const bool shouldWrite) {
  return SetVal<bool>(variable, value, shouldWrite);
}

const int UConfigRegistry::GetInt(const FString variable, const int defaultValue) {
  return GetVal<int>(variable, defaultValue);
}

void UConfigRegistry::SetInt(const FString variable, const int value, const bool shouldWrite) {
  return SetVal<int>(variable, value, shouldWrite);
}

const float UConfigRegistry::GetFloat(const FString variable, const float defaultValue) {
  return GetVal<float>(variable, defaultValue);
}

void UConfigRegistry::SetFloat(const FString variable, const float value, const bool shouldWrite) {
  return SetVal<float>(variable, value, shouldWrite);
}

const std::string UConfigRegistry::GetStdString(const FString variable, const std::string defaultValue) {
  return GetVal<std::string>(variable, defaultValue);
}

void UConfigRegistry::SetStdString(const FString variable, const std::string value, const bool shouldWrite) {
  return SetVal<std::string>(variable, value, shouldWrite);
}

const FString UConfigRegistry::GetString(const FString variable, const FString defaultValue) {
  // Convert default value
  std::string defaultStdValue = TCHAR_TO_UTF8(*defaultValue);

  // Get with std types
  std::string retStd = UConfigRegistry::GetStdString(variable, defaultStdValue);

  // Convert to output type
  FString ret = UTF8_TO_TCHAR(retStd.c_str());
  return ret;
}

void UConfigRegistry::SetString(const FString variable, const FString value, const bool shouldWrite) {
  // Convert value
  std::string stdValue = TCHAR_TO_UTF8(*value);

  // Set with std types
  return UConfigRegistry::SetStdString(variable, stdValue, shouldWrite);
}

const std::vector<std::string> UConfigRegistry::GetStdStringVector(const FString variable, const std::vector<std::string> defaultValue) {
  return GetVal<std::vector<std::string>>(variable, defaultValue);
}

void UConfigRegistry::SetStdStringVector(const FString variable, const std::vector<std::string> value, const bool shouldWrite) {
  return SetVal<std::vector<std::string>>(variable, value, shouldWrite);
}

const TArray<FString> UConfigRegistry::GetStringArray(const FString variable, const TArray<FString> defaultValue) {
  // Convert default value
  std::vector<std::string> defaultStdValue {};
  for (const FString& elem : defaultValue) {
    defaultStdValue.push_back(TCHAR_TO_UTF8(*elem));
  }

  // Get with std types
  std::vector<std::string> retStd = UConfigRegistry::GetStdStringVector(variable, defaultStdValue);

  // Convert to output type
  TArray<FString> ret {};
  for (const std::string& elem : retStd) {
    ret.Add(UTF8_TO_TCHAR(elem.c_str()));
  }
  return ret;
}

void UConfigRegistry::SetStringArray(const FString variable, const TArray<FString> value, const bool shouldWrite) {
  // Convert value
  std::vector<std::string> stdValue {};
  for (const FString& elem : value) {
    stdValue.push_back(TCHAR_TO_UTF8(*elem));
  }

  // Set with std types
  return UConfigRegistry::SetStdStringVector(variable, stdValue, shouldWrite);
}

const std::string UConfigRegistry::GetConfigLocation() {

#ifdef _WIN32
  const char* homedir = getenv("appdata");
#elif __linux__
  #include <unistd.h>
  #include <sys/types.h>
  #include <pwd.h>

  struct passwd* pw = getpwuid(getuid());
  const char* homedir = pw->pw_dir;
#endif

  std::string path(homedir);
  path += "/SCPFL Indoctrination/config.json";

#ifdef POINTERS_I_HAVE_NO_IDEA_WHATS_GOING_ON_WITH_POINTERS
#ifdef _WIN32
  free((char*) homedir);
#elif __linux__
  free(pw);
  free((char*) homedir);
#endif
#endif

  return path;
}

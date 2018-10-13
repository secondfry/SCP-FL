// Fill out your copyright notice in the Description page of Project Settings.

#include "SCPFL_GameInstance.h"

#include "Engine.h"

#include "Libraries/ConfigRegistry.h"


USCPFL_GameInstance::USCPFL_GameInstance(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  /** Bind function for CREATING a Session */
  OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &USCPFL_GameInstance::OnCreateSessionComplete);
  OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &USCPFL_GameInstance::OnStartSessionComplete);

  /** Bind function for FINDING a Session */
  OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &USCPFL_GameInstance::OnFindSessionsComplete);

  /** Bind function for JOINING a Session */
  OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &USCPFL_GameInstance::OnJoinSessionComplete);

  /** Bind function for DESTROYING a Session */
  OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &USCPFL_GameInstance::OnDestroySessionComplete);
}

bool USCPFL_GameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId)
{
  // Get the Online Subsystem to work with
  IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

  if (!OnlineSub) {
    UE_LOG(LogWindows, Error, TEXT("Failed to create session : No online SubSystem"));
    return false;
  }

  // Get the Session Interface, so we can call the "CreateSession" function on it
  IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

  if (!Sessions.IsValid()){
    UE_LOG(LogWindows, Error, TEXT("Failed to create session : Session is invalid"));
    return false;
  }

  if (!UserId.IsValid()) {
    UE_LOG(LogWindows, Error, TEXT("Failed to create session : UserID is invalid"));
    return false;
  }
  /*
  Fill in all the Session Settings that we want to use.

  There are more with SessionSettings.Set(...);
  For example the Map or the GameMode/Type.
  */
  SessionSettings = MakeShareable(new FOnlineSessionSettings());

  SessionSettings->bIsLANMatch = this->shouldBeOnLAN;
  SessionSettings->bUsesPresence = this->shouldUsePresence;
  SessionSettings->NumPublicConnections = this->requestedNumberOfPlayers;
  SessionSettings->NumPrivateConnections = 0;
  SessionSettings->bAllowInvites = true;
  SessionSettings->bAllowJoinInProgress = true;
  SessionSettings->bShouldAdvertise = true;
  SessionSettings->bAllowJoinViaPresence = true;
  SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
  SessionSettings->bIsDedicated = true;
  // TODO 
  //SessionSettings-> = FString("My cool game");

  SessionSettings->Set(SETTING_MAPNAME, FString("PersistentComplex"), EOnlineDataAdvertisementType::ViaOnlineService);

  // Set the delegate to the Handle of the SessionInterface
  OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

  // Our delegate should get called when this is complete (doesn't need to be successful!)
  return Sessions->CreateSession(0, this->requestedSessionName, *SessionSettings);
}

void USCPFL_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
  // Get the OnlineSubsystem so we can get the Session Interface
  IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
  if (!OnlineSub)
  {
    UE_LOG(LogWindows, Error, TEXT("Online subsystem disappeared"));
    return;
  }

  // Get the Session Interface to call the StartSession function
  IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
  if (!Sessions.IsValid())
  {
    UE_LOG(LogWindows, Error, TEXT("Session is not valid"));
    return;
  }

  // Clear the SessionComplete delegate handle, since we finished this call
  Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
  if (!bWasSuccessful)
  {
    return;
  }

  // Set the StartSession delegate handle
  OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

  // Our StartSessionComplete delegate should get called after this
  Sessions->StartSession(SessionName);
}

void USCPFL_GameInstance::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
  // Get the Online Subsystem so we can get the Session Interface
  IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
  if (!OnlineSub)
  {
    UE_LOG(LogWindows, Error, TEXT("Online subsystem disappeared"));
    return;
  }
  
  // Get the Session Interface to clear the Delegate
  IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
  if (!Sessions.IsValid())
  {
    UE_LOG(LogWindows, Error, TEXT("Session is not valid"));
    return;
  }
  
  // Clear the delegate, since we are done with this call
  Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
  
  // If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
  if (!bWasSuccessful)
  {
    UE_LOG(LogWindows, Error, TEXT("Start session failed"));
  }
  UGameplayStatics::OpenLevel(GetWorld(), "PersistentComplex", true, "listen");
  UE_LOG(LogWindows, Display, TEXT("Start session succeeded"));
  
  auto namedSession = Sessions->GetNamedSession(SessionName);
  DumpNamedSession(namedSession);
  DumpSessionSettings(&namedSession->SessionSettings);
}

void USCPFL_GameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId)
{
  // Get the OnlineSubsystem we want to work with
  IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

  if (!OnlineSub)
  {
    UE_LOG(LogWindows, Error, TEXT("Find session failed : no online subsystem"));
    OnFindSessionsComplete(false);
    return;
  }

  // Get the SessionInterface from our OnlineSubsystem
  IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

  if (!Sessions.IsValid())
  {
    UE_LOG(LogWindows, Error, TEXT("Find session failed : no valid session"));
    OnFindSessionsComplete(false);
    return;
  }
  if (!UserId.IsValid())
  {
    UE_LOG(LogWindows, Error, TEXT("Find session failed : user ID is not valid"));
    OnFindSessionsComplete(false);
    return;
  }

  /*
  Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
  */
  SessionSearch = MakeShareable(new FOnlineSessionSearch());

  SessionSearch->bIsLanQuery = this->shouldBeOnLAN;
  SessionSearch->MaxSearchResults = this->requestedSearchResults;
  SessionSearch->PingBucketSize = this->requestedPingBucket;

  // We only want to set this Query Setting if "bIsPresence" is true
  if (this->shouldUsePresence)
  {
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, this->shouldUsePresence, EOnlineComparisonOp::Equals);
  }

  TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();



  // Set the Delegate to the Delegate Handle of the FindSession function
  OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

  // Finally call the SessionInterface function. The Delegate gets called once this is finished
  Sessions->FindSessions(*UserId, SearchSettingsRef);
}

void USCPFL_GameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{ 
  // Get OnlineSubsystem we want to work with
  IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
  if (!OnlineSub)
  {
    UE_LOG(LogWindows, Error, TEXT("Find session complete failed : no online subsystem"));
    return;
  }

  // Get SessionInterface of the OnlineSubsystem
  IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
  if (!Sessions.IsValid())
  {
    UE_LOG(LogWindows, Error, TEXT("Find session complete failed : no valid session"));
    return;
  }

  // Clear the Delegate handle, since we finished this call
  Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

  // Just debugging the Number of Search results. Can be displayed in UMG or something later on
  //GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));

  // If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
  if (SessionSearch->SearchResults.Num() <= 0)
  {
    UE_LOG(LogWindows, Error, TEXT("Find session complete failed : search result is 0"));
    return;
  }

  // "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
  // This can be customized later on with your own classes to add more information that can be set and displayed
  //Populate sessionsMap to expose to BPs for widget use
  for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
  {
    auto& searchResult = this->SessionSearch->SearchResults[SearchIdx];
    auto searchResultInfoStruct = FSearchResultInfo
    {
      searchResult.Session.OwningUserName,
      searchResult.Session.SessionSettings.NumPublicConnections,
      searchResult.Session.NumOpenPublicConnections
    };
    
    this->sessionsMap.Add(SearchIdx, searchResultInfoStruct);
  }
  this->OnSearchCompleted(sessionsMap);
}

bool USCPFL_GameInstance::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
  // Return bool
  bool bSuccessful = false;

  // Get OnlineSubsystem we want to work with
  IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

  if (!OnlineSub)
  {
    UE_LOG(LogWindows, Error, TEXT("Join session complete failed : no online subsystem"));
    return false;
  }

  // Get SessionInterface from the OnlineSubsystem
  IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

  if (!Sessions.IsValid())
  {
    UE_LOG(LogWindows, Error, TEXT("Join session complete failed : session is not valid"));
    return false; 
  }

  if (!UserId.IsValid())
  {
    UE_LOG(LogWindows, Error, TEXT("Find session complete failed : user ID is not valid"));
    return false;
  }

  // Set the Handle again
  OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

  // Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
  // "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
  bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
    
  return bSuccessful;
}

void USCPFL_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    // Get the OnlineSubsystem we want to work with
  IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
  if (!OnlineSub)
  {
    UE_LOG(LogWindows, Error, TEXT("Join session complete failed : no online subsystem"));
    return;
  }

  // Get SessionInterface from the OnlineSubsystem
  IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

  if (!Sessions.IsValid())
  {
    UE_LOG(LogWindows, Error, TEXT("Join session complete failed : session is not valid"));
    return;
  }
  
  // Clear the Delegate again
  Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

  // Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
  // This is something the Blueprint Node "Join Session" does automatically!
  APlayerController * const PlayerController = GetFirstLocalPlayerController();

  // We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
  // String for us by giving him the SessionName and an empty String. We want to do this, because
  // Every OnlineSubsystem uses different TravelURLs
  FString TravelURL;

  if (!PlayerController)
  {
    UE_LOG(LogOnline, Error, TEXT("Find session complete failed : No playerController"));
    return;
  }

  if (!Sessions->GetResolvedConnectString(SessionName, TravelURL))
  {
    UE_LOG(LogOnline, Error, TEXT("Find session complete failed : No resolved connect string"));
    UE_LOG(LogOnline, Error, TEXT("SessionName was %s, TravelURL was %s"), TCHAR_TO_UTF8(*SessionName.ToString()), TCHAR_TO_UTF8(*TravelURL));
    return;
  }

  // Finally call the ClienTravel. If you want, you could print the TravelURL to see
  // how it really looks like
  PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
  UE_LOG(LogOnline, Display, TEXT("SessionName was %s, TravelURL was %s"), TCHAR_TO_UTF8(*SessionName.ToString()), TCHAR_TO_UTF8(*TravelURL));
}

void USCPFL_GameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
  // Get the OnlineSubsystem we want to work with
  IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
  if (!OnlineSub)
  {
    UE_LOG(LogWindows, Error, TEXT("Destroy session complete failed : no online subsystem"));
    return;
  }
  // Get the SessionInterface from the OnlineSubsystem
  IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

  if (!Sessions.IsValid())
  {
    UE_LOG(LogWindows, Error, TEXT("Destroy session complete failed : session is not valid"));
    return;
  }
  // Clear the Delegate
  Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

  // If it was successful, we just load another level (could be a MainMenu!)
  if (bWasSuccessful)
  {
    UGameplayStatics::OpenLevel(GetWorld(), "PersistentComplex", true);
  }
}



void USCPFL_GameInstance::StartOnlineGame(bool shouldUsePresence, bool shouldBeOnLAN, FName requestedSessionName, int requestedNumberOfPlayers)
{
  this->shouldUsePresence = static_cast<bool>(shouldUsePresence);
  this->shouldBeOnLAN = static_cast<bool>(shouldBeOnLAN);
  this->requestedSessionName = requestedSessionName;
  this->requestedNumberOfPlayers = requestedNumberOfPlayers;

  // Creating a local player where we can get the UserID from
  ULocalPlayer* const Player = GetFirstGamePlayer();

  // Call our custom HostSession function. GameSessionName is a GameInstance variable
  HostSession(Player->GetPreferredUniqueNetId().GetUniqueNetId());
}

void USCPFL_GameInstance::FindOnlineGames(bool shouldUsePresence, bool shouldBeOnLAN, int requestedSearchResults, int requestedPingBucket)
{
  this->shouldUsePresence = static_cast<bool>(shouldUsePresence);
  this->shouldBeOnLAN = static_cast<bool>(shouldBeOnLAN);
  this->requestedSearchResults = requestedSearchResults;
  this->requestedPingBucket = requestedPingBucket;

  ULocalPlayer* const Player = GetFirstGamePlayer();
  FindSessions(Player->GetPreferredUniqueNetId().GetUniqueNetId());
}

void USCPFL_GameInstance::JoinOnlineGame(int sessionIndex)
{
  ULocalPlayer* const Player = GetFirstGamePlayer();

  // Just a SearchResult where we can save the one we want to use, for the case we find more than one!
  FOnlineSessionSearchResult SearchResult;

  // If the Array is not empty, we can go through it
  if (SessionSearch->SearchResults.Num() <= 0)
  {
    UE_LOG(LogWindows, Error, TEXT("Join online game failed : search result is 0"));
    return;
  }

  // To avoid something crazy, we filter sessions from ourself
  if (SessionSearch->SearchResults[sessionIndex].Session.OwningUserId == Player->GetPreferredUniqueNetId().GetUniqueNetId())
  {
    UE_LOG(LogWindows, Error, TEXT("Join online game failed : tried to join your own session"));
    return;
  }

  SearchResult = SessionSearch->SearchResults[sessionIndex];
  JoinSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, SearchResult);
}

void USCPFL_GameInstance::DestroySessionAndLeaveGame()
{
  IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
  if (!OnlineSub)
  {
    UE_LOG(LogWindows, Error, TEXT("Destroy session failed : no online subsystem"));
    return;
  }
  IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
  if (!Sessions.IsValid())
  {
    UE_LOG(LogWindows, Error, TEXT("Destroy session failed : no valid session"));
    return;
  }
  Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
  Sessions->DestroySession(GameSessionName);
}

void USCPFL_GameInstance::Init() {
  UGameInstance::Init();
  UConfigRegistry::ReadConfig();
}

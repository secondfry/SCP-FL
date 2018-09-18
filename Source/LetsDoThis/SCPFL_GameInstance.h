// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Online.h"

#include "SCPFL_GameInstance.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSearchResultInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Category = "Network|Steam", BlueprintReadOnly)
	FString presentedSessionName;

	UPROPERTY(Category = "Network|Steam", BlueprintReadOnly)
	int presentedMaxNumberOfPlayers;

	UPROPERTY(Category = "Network|Steam", BlueprintReadOnly)
	int presentedCurrentNumberOfPlayers;

};

UCLASS()
class LETSDOTHIS_API USCPFL_GameInstance : public UGameInstance
{
	GENERATED_BODY()
private:

		/**
	*	Function to host a game!
	*
	*	@Param		UserID			User that started the request
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId);

	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	USCPFL_GameInstance(const FObjectInitializer& ObjectInitializer);

	/**
	*	Function fired when a session create request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	*	Function fired when a session start request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	
	/**
	*	Find an online session
	*
	*	@param UserId user that initiated the request
	*
	*	@returns TArray<FOnlineSessionSearchResult> array of search results
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId);

	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	/** Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	/**
	*	Delegate fired when a session search query has completed
	*
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnFindSessionsComplete(bool bWasSuccessful);

	/**
	*	Joins a session via a search result
	*
	*	@param SessionName name of session
	*	@param SearchResult Session to join
	*
	*	@return bool true if successful, false otherwise
	*/
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/** Delegate for joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Handle to registered delegate for joining a session */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	/**
	*	Delegate fired when a session join request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	/**
	*	Delegate fired when a destroying an online session has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	bool shouldUsePresence = false;
	bool shouldBeOnLAN = false;
	FName requestedSessionName = FName(TEXT("SCP:FL Community Server"));
	int requestedNumberOfPlayers = 20;
	int requestedSearchResults = 200000;
	int requestedPingBucket = 999;
	TMap<int, FSearchResultInfo> sessionsMap;

public:

	UFUNCTION(BlueprintCallable, Category = "Network|Steam")
	void StartOnlineGame(bool shouldUsePresence, bool shouldBeOnLAN, FName requestedSessionName, int requestedNumberOfPlayers);

	UFUNCTION(BlueprintCallable, Category = "Network|Steam")
	void FindOnlineGames(bool shouldUsePresence, bool shouldBeOnLAN, int requestedSearchResults, int requestedPingBucket);

	UFUNCTION(BlueprintCallable, Category = "Network|Steam")
	void JoinOnlineGame(int sessionIndex);

	UFUNCTION(BlueprintCallable, Category = "Network|Steam")
	void DestroySessionAndLeaveGame();

  UFUNCTION(BlueprintImplementableEvent, Category = "Network|Steam")
  void OnSearchCompleted(const TMap<int, FSearchResultInfo>& searchResults);

};

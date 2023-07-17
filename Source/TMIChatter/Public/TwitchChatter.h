// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "IWebSocket.h"
#include "TMIParser.h"

#include "TwitchChatter.generated.h"



#define TWITCH_CHATTER_DEV_COLLECTION 0
#define TWITCH_CHATTER_DEV_TESTING 0

DECLARE_LOG_CATEGORY_EXTERN(LogTwitchChatter, Log, All);

#define TWITCH_LOG(Lvl, Fmt, ...) UE_LOG(LogTwitchChatter, Lvl, Fmt, __VA_ARGS__)




// Blueprint dynamic delegates
// see https://dev.twitch.tv/docs/irc/commands#clearchat and https://dev.twitch.tv/docs/irc/tags#clearchat-tags
UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClearChat, const FClearChatMessage&, Message);

// see https://dev.twitch.tv/docs/irc/commands#clearmsg and https://dev.twitch.tv/docs/irc/tags#clearmsg-tags
UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClearMessage, const FClearMsgMessage&, Message);

// See https://dev.twitch.tv/docs/irc/send-receive-messages#receiving-chat-messages and https://dev.twitch.tv/docs/irc/tags#privmsg-tags
UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessage, const FPrivMsgMessage&, Message);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnChatCommand, const FPrivMsgMessage&, Message, const FString&, Command, const FString&, Params);

// See https://dev.twitch.tv/docs/irc/commands#globaluserstate and https://dev.twitch.tv/docs/irc/tags#globaluserstate-tags
UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalUserState, const FGlobalUserStateMessage&, Message);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWhispered, const FWhisperMessage&, Message);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotice, const FNoticeMessage&, Message);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserNotice, const FUserNoticeMessage&, Message);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserSubscribed, const FUserNoticeMessage&, Message, const FSubscriptionNoticeTags&, SubscriptionTags);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserSubsGifted, const FUserNoticeMessage&, Message, const FSubgiftNoticeTags&, SubGiftNoticeTags);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserBitsBadgeTier, const FUserNoticeMessage&, Message, const FBitsBadgeTierNoticeTags&, BitsBadgeTierTags);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserRaid, const FUserNoticeMessage&, Message, const FRaidNoticeTags&, SubscriptionTags);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserRitual, const FUserNoticeMessage&, Message, const FRitualNoticeTags&, SubscriptionTags);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserSubGiftPaidForward, const FUserNoticeMessage&, Message, const FSubPaidForwardNoticeTags&, SubscriptionTags);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserState, const FUserStateMessage&, Message);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomState, const FRoomStateMessage&, Message);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChannel, const FString&, Channel);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatus);

// C++ events
DECLARE_EVENT_ThreeParams(UTwitchChatter, FChatCommandEvent, const FPrivMsgMessage& /*Message*/, const FString& /*Command*/, const FString& /*Params*/);
DECLARE_EVENT_OneParam(UTwitchChatter, FClearChatEvent, const FClearChatMessage& /*Message*/);
DECLARE_EVENT_OneParam(UTwitchChatter, FClearMsgEvent, const FClearMsgMessage& /*Message*/);
DECLARE_EVENT_OneParam(UTwitchChatter, FPrivMsgEvent, const FPrivMsgMessage& /*Message*/);
DECLARE_EVENT_OneParam(UTwitchChatter, FWhisperedEvent, const FWhisperMessage& /*Message*/);
DECLARE_EVENT_OneParam(UTwitchChatter, FNoticeEvent, const FNoticeMessage& /*Message*/);
DECLARE_EVENT_OneParam(UTwitchChatter, FUserNoticeEvent, const FUserNoticeMessage& /*Message*/);
DECLARE_EVENT_TwoParams(UTwitchChatter, FUserSubscribedEvent, const FUserNoticeMessage& /*Message*/, const FSubscriptionNoticeTags& /*Tags*/);
DECLARE_EVENT_TwoParams(UTwitchChatter, FUserSubsGiftedEvent, const FUserNoticeMessage& /*Message*/, const FSubgiftNoticeTags& /*Tags*/);
DECLARE_EVENT_TwoParams(UTwitchChatter, FUserSubsPaidForwardEvent, const FUserNoticeMessage& /*Message*/, const FSubPaidForwardNoticeTags& /*Tags*/);
DECLARE_EVENT_TwoParams(UTwitchChatter, FUserRaidEvent, const FUserNoticeMessage& /*Message*/, const FRaidNoticeTags& /*Tags*/);
DECLARE_EVENT_TwoParams(UTwitchChatter, FUserRitualEvent, const FUserNoticeMessage& /*Message*/, const FRitualNoticeTags& /*Tags*/);
DECLARE_EVENT_TwoParams(UTwitchChatter, FUserBitsBadgeEvent, const FUserNoticeMessage& /*Message*/, const FBitsBadgeTierNoticeTags& /*Tags*/);
DECLARE_EVENT_OneParam(UTwitchChatter, FChannelEvent, const FString& /*Channel*/);
DECLARE_EVENT_OneParam(UTwitchChatter, FUserStateEvent, const FUserStateMessage& /*Message*/);
DECLARE_EVENT_OneParam(UTwitchChatter, FRoomStateEvent, const FRoomStateMessage& /*Message*/);
DECLARE_EVENT_OneParam(UTwitchChatter, FGlobalUserStateEvent, const FGlobalUserStateMessage& /*Message*/);
DECLARE_EVENT_OneParam(UTwitchChatter, FSentMessageEvent, const FString& /*Raw Message*/);
DECLARE_EVENT(UTwitchChatter, FStatusEvent);

typedef FChatCommandEvent::FDelegate FCommandEventDelegate;

UCLASS(Transient, BlueprintType, Blueprintable, MinimalAPI)
class UTwitchChatter : public UObject
{
	GENERATED_BODY()

public:
	UTwitchChatter();
	~UTwitchChatter();

	UFUNCTION(BlueprintCallable)
		void Connect(const FString& Username, const FString& Password, const TArray<FString> &AutoJoinChannels, bool AutoReconnect = true, bool FastMode = false);

	UFUNCTION(BlueprintCallable)
		void Disconnect();

	UFUNCTION(BlueprintCallable)
		void JoinChannel(const FString& Channel);

	UFUNCTION(BlueprintCallable)
		void JoinChannels(const TArray<FString> &Channels);

	UFUNCTION(BlueprintCallable)
		void PartChannel(const FString& Channel);

	UFUNCTION(BlueprintCallable)
		void Send(const FString& Channel, const FString& Message) const;

	UFUNCTION(BlueprintCallable)
		void Broadcast(const FString& Message) const;

	UFUNCTION(BlueprintCallable)
		void BindCommandMessage(const FString& Command, UObject* BindObject, const FName& FunctionName);

	UFUNCTION(BlueprintCallable)
		void UnbindCommandMessage(const FString& Command, UObject* BindObject, const FName& FunctionName);

	UFUNCTION(BlueprintCallable)
		void ClearOnChatCommand(const FString& Command);

	UFUNCTION(BlueprintCallable)
		void ClearAllCommandsMessages();

	UFUNCTION(BlueprintCallable)
		void ResetEventHandlers(bool ResetEvents = true, bool ResetDelegates = true);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 MaxReconnectTime = 60;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString CommandPrefix = TEXT("!");

	UPROPERTY(BlueprintReadOnly)
		TArray<FString> ConnectedChannels;

	/* Begin C++ Event Interface */

	/* See https://docs.unrealengine.com/5.0/en-US/event-programming-in-unreal-engine/ for more information */
	FPrivMsgEvent EventChatBits;			// Fired anytime someone cheered bits in their message
	FPrivMsgEvent EventChatMessage;		// Fired anytime a message is received
	FChatCommandEvent EventChatCommand;		// Fired anytime a message is received with the Command Prefix as the first character
	FClearChatEvent EventChatCleared;
	FClearMsgEvent EventMsgCleared;
	FWhisperedEvent EventWhispered;		// Fired anytime a message is received
	FUserNoticeEvent EventUserNotice;
	FUserSubscribedEvent EventUserSubscribed;
	FUserSubscribedEvent EventUserResubscribed;
	FUserSubsGiftedEvent EventSubsGifted;
	FUserSubsPaidForwardEvent EventSubPaidForward;
	FUserBitsBadgeEvent EventNewBitsBadge;
	FUserRaidEvent EventRaided;
	FUserRitualEvent EventRitual;
	FNoticeEvent EventNotice;
	FGlobalUserStateEvent EventAuthSuccess;

	// Fired when THIS BOT itself joins or leaves a channel.
	// It's not reliable to watch joined and parted notices for others
	// That functionality is hidden in undocumented twitch API polling functions as of now
	// and it is still unreliable, just far less so.
	FChannelEvent EventJoinedChannel;
	FChannelEvent EventPartedChannel;

	FStatusEvent EventAuthFailure;
	FStatusEvent EventSocketConnected;
	FStatusEvent EventSocketError;
	FStatusEvent EventSocketClosed;

	TSharedPtr<FChatCommandEvent> FindOrAddCommand(const FString& Command);
	void ClearAllCommands();

	/* End C++ Event Interface */

private:
	
	template<class T>
	void ClearAllCommands_impl(TMap<FString, TSharedPtr<T>>& Container);

private:
	void Reconnect();
	void SendAuthInfo() const;
	void HandleMessage(const FString& Message);
	void SendRaw(const FString& Message) const;

	/* Blueprint Interface, slower as it uses multicast delegates, but can be used by C++ as well for a unified path */
	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnMessage OnChatBits;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnUserSubscribed OnChatSubscriber;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnUserSubscribed OnChatReSubscriber;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnUserSubsGifted OnSubsGifted;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnUserSubGiftPaidForward OnSubsPaidForward;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnUserBitsBadgeTier OnNewBitsBadge;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnUserRaid OnRaided;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnUserRitual OnRitual;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnMessage OnChatMessage;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnWhispered OnWhispered;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnChatCommand OnChatCommand;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnNotice OnNotice;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnUserNotice OnUserNotice;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnChannel OnJoinedChannel;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnChannel OnPartedChannel;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnGlobalUserState OnAuthenticationSuccess;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnStatus OnAuthenticationFailed;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnClearChat OnClearChat;

	UPROPERTY(BlueprintAssignable, Category = "TwitchChat")
		FOnClearMessage OnClearMsg;

	UPROPERTY(BlueprintAssignable, Category = "TwitchSocket")
		FOnStatus OnSocketConnected;

	UPROPERTY(BlueprintAssignable, Category = "TwitchSocket")
		FOnStatus OnSocketError;

	UPROPERTY(BlueprintAssignable, Category = "TwitchSocket")
		FOnStatus OnSocketClosed;

	FString BotUsername;
	FString BotPassword;
	TArray<FString> AutoJoinChannels;

	bool bFastMode = false;
	bool bReconnect = false;
	int32 ReconnectTime = 2;
	bool bAuthenticated = false;

	// C++ Interface
	TMap<FString, TSharedPtr<FChatCommandEvent>> CommandCallbacks;
	TMap<FString, TSharedPtr<FOnChatCommand>> MulticastCommandCallbacks;

	TSharedPtr<IWebSocket> Socket = nullptr;

	const FString TwitchTMI_WebsocketURL = TEXT("wss://irc-ws.chat.twitch.tv:443/");
	FDelegateHandle MessageDelegateHandle;
	FDelegateHandle SocketErrorDelegateHandle;
	FDelegateHandle SocketClosedDelegateHandle;
	FDelegateHandle MsgSentDelegateHandle;

	FSentMessageEvent EventSentMessage;
	friend class TwitchChatterSpec;	// Yes the test class gets to look at all the bits
};

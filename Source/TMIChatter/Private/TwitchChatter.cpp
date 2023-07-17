// Fill out your copyright notice in the Description page of Project Settings.

#include "TwitchChatter.h"
#include "WebSocketsModule.h"

#include "Modules/ModuleManager.h"

#ifdef TWITCH_CHATTER_DEV_COLLECTION
#include "Misc/FileHelper.h"
#endif



DEFINE_LOG_CATEGORY(LogTwitchChatter);



UTwitchChatter::UTwitchChatter()
{
  const FString& Protocol = TwitchTMI_WebsocketURL.Left(TwitchTMI_WebsocketURL.Find(":"));

  FWebSocketsModule* _WebSocketModule = &FModuleManager::LoadModuleChecked<FWebSocketsModule>(TEXT("WebSockets"));

  check(_WebSocketModule != nullptr);

  Socket = _WebSocketModule->CreateWebSocket(
    TwitchTMI_WebsocketURL,
    Protocol
  );

  Socket->OnConnected().AddLambda([&]() -> void {
    TWITCH_LOG(Log, TEXT("TWITCH: Socket Connected"));

    ConnectedChannels.Empty();

    EventSocketConnected.Broadcast();
    OnSocketConnected.Broadcast();

    SendAuthInfo();
  });

  SocketErrorDelegateHandle = Socket->OnConnectionError().AddLambda([&](const FString& Error) -> void {
    TWITCH_LOG(Log, TEXT("TWITCH: Socket Connection Error: %s"), *Error);

    if (EventSocketError.IsBound())
      EventSocketError.Broadcast();

    if (OnSocketError.IsBound())
      OnSocketError.Broadcast();

    if (bReconnect)
      Reconnect();
  });

  SocketClosedDelegateHandle = Socket->OnClosed().AddLambda([&](int32 StatusCode, const FString &Reason, bool WasClean) -> void {
    TWITCH_LOG(Log, TEXT("TWITCH: Socket Closed: %s"), *Reason);

    if (EventSocketClosed.IsBound())
      EventSocketClosed.Broadcast();

    if (OnSocketClosed.IsBound())
      OnSocketClosed.Broadcast();
  });

  MessageDelegateHandle = Socket->OnMessage().AddLambda([&](const FString &Message) -> void {
    HandleMessage(Message);
  });

#ifdef TWITCH_CHATTER_DEV_TESTING
  MsgSentDelegateHandle = Socket->OnMessageSent().AddLambda([&](const FString& Message) -> void {
    EventSentMessage.Broadcast(Message);
  });
#endif
}



UTwitchChatter::~UTwitchChatter()
{
#ifdef TWITCH_CHATTER_DEV_TESTING
  Socket->OnMessageSent().Remove(MsgSentDelegateHandle);
#endif

  Socket->OnClosed().Remove(SocketClosedDelegateHandle);
  Socket->OnConnectionError().Remove(SocketErrorDelegateHandle);
  Socket->OnMessage().Remove(MessageDelegateHandle);

  Disconnect();
  Socket.Reset();
}



void UTwitchChatter::ResetEventHandlers(bool ResetEvents, bool ResetDelegates)
{
  if (ResetEvents)
  {
    EventSocketConnected.Clear();
    EventSocketError.Clear();
    EventSocketClosed.Clear();
    EventChatBits.Clear();
    EventChatCommand.Clear();
    EventChatCleared.Clear();
    EventMsgCleared.Clear();
    EventChatMessage.Clear();
    EventJoinedChannel.Clear();
    EventPartedChannel.Clear();
    EventAuthFailure.Clear();
    EventAuthSuccess.Clear();
    EventWhispered.Clear();
    EventUserNotice.Clear();
    EventUserSubscribed.Clear();
    EventUserResubscribed.Clear();
    EventRaided.Clear();
    EventRitual.Clear();
    EventNewBitsBadge.Clear();
    EventSubsGifted.Clear();
    EventSubPaidForward.Clear();
    EventNotice.Clear();
  }

  if (ResetDelegates)
  {
    OnChatBits.Clear();
    OnChatSubscriber.Clear();
    OnChatReSubscriber.Clear();
    OnSubsGifted.Clear();
    OnSubsPaidForward.Clear();
    OnNewBitsBadge.Clear();
    OnRaided.Clear();
    OnRitual.Clear();
    OnChatMessage.Clear();
    OnWhispered.Clear();
    OnChatCommand.Clear();
    OnNotice.Clear();
    OnUserNotice.Clear();
    OnJoinedChannel.Clear();
    OnPartedChannel.Clear();
    OnAuthenticationSuccess.Clear();
    OnAuthenticationFailed.Clear();
    OnClearChat.Clear();
    OnClearMsg.Clear();
    OnSocketConnected.Clear();
    OnSocketError.Clear();
    OnSocketClosed.Clear();
  }
}



void UTwitchChatter::Connect(const FString& TryUsername, const FString& TryPassword, const TArray<FString>& MyAutoJoinChannels, bool AutoReconnect, bool EnableFastMode)
{
  if (Socket->IsConnected())
    return;

  bFastMode = EnableFastMode;

  if (Socket.IsValid())
  {
    BotUsername = TryUsername.ToLower();
    BotPassword = TryPassword;
    bReconnect = AutoReconnect;

    AutoJoinChannels.Empty();
    AutoJoinChannels.Append(MyAutoJoinChannels);

    Socket->Connect();
  }
  else
  {
    EventSocketError.Broadcast();
    OnSocketError.Broadcast();
  }
}



void UTwitchChatter::Reconnect()
{
  if (!Socket.IsValid())
    return;

  if (Socket->IsConnected())
    Disconnect();

  Socket->Connect();
}



void UTwitchChatter::Disconnect()
{
  if (Socket != nullptr)
  {
    for (const FString &Channel : ConnectedChannels)
    {
      SendRaw("PART #" + Channel);
    }

    SendRaw("QUIT :Goodbye");

    Socket->Close();
  }

  BotUsername = TEXT("");
  BotPassword = TEXT("");
}



void UTwitchChatter::SendAuthInfo() const
{
  if (Socket.IsValid())
  {
    if (!bFastMode)
      SendRaw("CAP REQ :twitch.tv/tags");

    SendRaw("CAP REQ :twitch.tv/commands");
    SendRaw("PASS oauth:" + BotPassword);
    SendRaw("NICK " + BotUsername);
  }
}



void UTwitchChatter::JoinChannel(const FString& Channel)
{
  FString Chan = Channel.ToLower();

  if (Socket.IsValid() && Socket->IsConnected() && !Chan.IsEmpty() && !ConnectedChannels.Contains(Chan))
  {
    SendRaw("JOIN #" + Chan);
    ConnectedChannels.Add(Chan);
  }
}



void UTwitchChatter::JoinChannels(const TArray<FString>& Channels)
{
  for (const FString &Channel : Channels)
  {
    JoinChannel(Channel);
  }
}



void UTwitchChatter::PartChannel(const FString& Channel)
{
  FString Chan = Channel.ToLower();

  if (Socket.IsValid() && !Chan.IsEmpty() && ConnectedChannels.Contains(Chan))
  {
    SendRaw("PART #" + Chan);
  }
}



void UTwitchChatter::SendRaw(const FString& Message) const
{
  Socket->Send(Message);
  Socket->Send("\r\n");
}



void UTwitchChatter::Send(const FString& Channel, const FString& Message) const
{
  if (!Channel.IsEmpty() && !Message.IsEmpty())
    SendRaw("PRIVMSG #" + Channel.ToLower() + " :" + Message);
}



void UTwitchChatter::Broadcast(const FString& Message) const
{
  if (Message.IsEmpty())
    return;

  for (const FString&Channel : ConnectedChannels)
  {
    Send(Channel, Message);
  }
}



void UTwitchChatter::HandleMessage(const FString &TMIString)
{
  if (TMIString.IsEmpty())
    return;

  TArray<FString> Lines;

  TMIString.ParseIntoArray(Lines, TEXT("\r\n"));

  for (const FString &Line : Lines)
  {
    TMIParser::MessageBundle Bundle = TMIParser::SplitRawMessage(Line);

#ifdef TWITCH_CHATTER_DEV_COLLECTION
    if (!Bundle.RawCommand.IsNumeric())
    {
      const FString SaveFile = FString::Printf(TEXT("%s/DevCollection/%s.txt"), *FPaths::ProjectDir(), *Bundle.RawCommand);
      FFileHelper::SaveStringToFile(Line + TEXT("\n"), *SaveFile, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
    }
#endif

    switch (Bundle.Command)
    {
    case EIRCCommand::PRIVMSG:
    {
      if (Bundle.Source.Contains(BotUsername))
        break;

      FPrivMsgMessage Message = TMIParser::ParseMessage<FPrivMsgMessage>(Bundle);

      EventChatMessage.Broadcast(Message);
      OnChatMessage.Broadcast(Message);

      if (Message.Tags.Bits > 0)
      {
        EventChatBits.Broadcast(Message);
        OnChatBits.Broadcast(Message);
      }

      if (Message.Message.StartsWith(CommandPrefix))
      {
        FString Command, Params;
        int32 Endex;

        if (!Message.Message.FindChar(TCHAR(' '), Endex))
          Endex = Message.Message.Len();

        Command = Message.Message.Mid(1, Endex-1);

        if (Endex < Message.Message.Len())
          Params = Message.Message.Mid(Endex);

        EventChatCommand.Broadcast(Message, Command, Params);
        OnChatCommand.Broadcast(Message, Command, Params);
      }
      break;
    }

    case EIRCCommand::CLEARCHAT:
    {
      FClearChatMessage Message = TMIParser::ParseMessage<FClearChatMessage>(Bundle);
      OnClearChat.Broadcast(Message);
      EventChatCleared.Broadcast(Message);
      break;
    }

    case EIRCCommand::CLEARMSG:
    {
      FClearMsgMessage Message = TMIParser::ParseMessage<FClearMsgMessage>(Bundle);
      OnClearMsg.Broadcast(Message);
      EventMsgCleared.Broadcast(Message);
      break;
    }

    case EIRCCommand::WHISPER:
    {
      FWhisperMessage Message = TMIParser::ParseMessage<FWhisperMessage>(Bundle);
      EventWhispered.Broadcast(Message);
      OnWhispered.Broadcast(Message);
      break;
    }

    case EIRCCommand::USERNOTICE:
    {
      FUserNoticeMessage Message = TMIParser::ParseMessage<FUserNoticeMessage>(Bundle);
      OnUserNotice.Broadcast(Message);
      EventUserNotice.Broadcast(Message);

      switch (Message.Tags.MsgID)
      {
      case ETWUserNoticeMsgId::Resubscription:
        EventUserResubscribed.Broadcast(Message, FSubscriptionNoticeTags(Message.Tags.MessageParams));
        OnChatReSubscriber.Broadcast(Message, FSubscriptionNoticeTags(Message.Tags.MessageParams));
        break;

      case ETWUserNoticeMsgId::Subscription:
        EventUserSubscribed.Broadcast(Message, FSubscriptionNoticeTags(Message.Tags.MessageParams));
        OnChatSubscriber.Broadcast(Message, FSubscriptionNoticeTags(Message.Tags.MessageParams));
        break;

      case ETWUserNoticeMsgId::SubscriptionGift:
        EventSubsGifted.Broadcast(Message, FSubgiftNoticeTags(Message.Tags.MessageParams));
        OnSubsGifted.Broadcast(Message, FSubgiftNoticeTags(Message.Tags.MessageParams));
        break;

      case ETWUserNoticeMsgId::BitsBadgeTier:
        EventNewBitsBadge.Broadcast(Message, FBitsBadgeTierNoticeTags(Message.Tags.MessageParams));
        OnNewBitsBadge.Broadcast(Message, FBitsBadgeTierNoticeTags(Message.Tags.MessageParams));
        break;

      case ETWUserNoticeMsgId::Raid:
        EventRaided.Broadcast(Message, FRaidNoticeTags(Message.Tags.MessageParams));
        OnRaided.Broadcast(Message, FRaidNoticeTags(Message.Tags.MessageParams));
        break;

      case ETWUserNoticeMsgId::Ritual:
        EventRitual.Broadcast(Message, FRitualNoticeTags(Message.Tags.MessageParams));
        OnRitual.Broadcast(Message, FRitualNoticeTags(Message.Tags.MessageParams));
        break;

      case ETWUserNoticeMsgId::CommunityPayForward:
      case ETWUserNoticeMsgId::StandardPayForward:
        EventSubPaidForward.Broadcast(Message, FSubPaidForwardNoticeTags(Message.Tags.MessageParams));
        OnSubsPaidForward.Broadcast(Message, FSubPaidForwardNoticeTags(Message.Tags.MessageParams));
        break;
      }
      break;
    }

    case EIRCCommand::NOTICE:
    {
      FNoticeMessage Message = TMIParser::ParseMessage<FNoticeMessage>(Bundle);

      if (!bAuthenticated)
      {
        if (!Message.Channel.Compare("*"))
        {
          EventAuthFailure.Broadcast();
          OnAuthenticationFailed.Broadcast();
        }
      }
      else
      {
        EventNotice.Broadcast(Message);
        OnNotice.Broadcast(Message);
      }
      break;
    }

    case EIRCCommand::PING:
      Socket->Send(TEXT("PONG :") + Bundle.Params);
      break;

    case EIRCCommand::RECONNECT:
      TWITCH_LOG(Log, TEXT("We've been asked to reconnect"));
      Reconnect();
      break;

    case EIRCCommand::JOIN:
      EventJoinedChannel.Broadcast(Bundle.Target);
      OnJoinedChannel.Broadcast(Bundle.Target);
      break;

    case EIRCCommand::PART:
      EventPartedChannel.Broadcast(Bundle.Target);
      OnPartedChannel.Broadcast(Bundle.Target);
      break;

    case EIRCCommand::GLOBALUSERSTATE:
    {
      FGlobalUserStateMessage Message = TMIParser::ParseMessage<FGlobalUserStateMessage>(Bundle);

      bAuthenticated = true;
      EventAuthSuccess.Broadcast(Message);
      OnAuthenticationSuccess.Broadcast(Message);
      JoinChannels(AutoJoinChannels);
      break;
    }
    }
  }
}



TSharedPtr<FChatCommandEvent> UTwitchChatter::FindOrAddCommand(const FString& Command)
{
  return CommandCallbacks.FindOrAdd(Command);
}



void UTwitchChatter::ClearOnChatCommand(const FString& Command)
{
  auto MsgEvent = MulticastCommandCallbacks.FindOrAdd(Command);

  check(MsgEvent.IsValid());

  MsgEvent->Clear();
}



template<class T>
void UTwitchChatter::ClearAllCommands_impl(TMap<FString, TSharedPtr<T>>& Container)
{
  for (TPair<FString, TSharedPtr<T>> MsgPair : Container)
  {
    check(MsgPair.Value.IsValid());
    MsgPair.Value->Clear();
  }

  Container.Empty();
}



void UTwitchChatter::BindCommandMessage(const FString& Command, UObject* BindObject, const FName& FunctionName)
{
  auto MsgEvents = MulticastCommandCallbacks.FindOrAdd(Command);

  // We may need to save this locally... we probably need to save this locally
  FOnChatCommand::FDelegate Deleg;
  Deleg.BindUFunction(BindObject, FunctionName);

  MsgEvents->Add(Deleg);
}



void UTwitchChatter::UnbindCommandMessage(const FString& Command, UObject* BindObject, const FName& FunctionName)
{
  auto MsgEvents = MulticastCommandCallbacks.FindOrAdd(Command);
  MsgEvents->Remove(BindObject, FunctionName);
}



void UTwitchChatter::ClearAllCommandsMessages()
{
  ClearAllCommands_impl(MulticastCommandCallbacks);
}



void UTwitchChatter::ClearAllCommands()
{
  ClearAllCommands_impl(CommandCallbacks);
}

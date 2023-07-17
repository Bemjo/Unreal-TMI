#include "TMIParser.h"

#include "Misc/DefaultValueHelper.h"

DEFINE_LOG_CATEGORY(LogTwitchParser);

FString GetIRCCommandString(EIRCCommand Command)
{
  switch (Command)
  {
  case EIRCCommand::NOTICE:	return TEXT("NOTICE");
  case EIRCCommand::JOIN: return TEXT("JOIN");
  case EIRCCommand::PART:	return TEXT("PART");
  case EIRCCommand::PING:	return TEXT("PING");
  case EIRCCommand::PRIVMSG:	return TEXT("PRIVMSG");
  case EIRCCommand::CLEARCHAT:	return TEXT("CLEARCHAT");
  case EIRCCommand::GLOBALUSERSTATE:	return TEXT("GLOBALUSERSTATE");
  case EIRCCommand::HOSTTARGET:	return TEXT("HOSTTARGET");
  case EIRCCommand::RECONNECT:	return TEXT("RECONNECT");
  case EIRCCommand::ROOMSTATE:	return TEXT("ROOMSTATE");
  case EIRCCommand::USERNOTICE:	return TEXT("USERNOTICE");
  case EIRCCommand::USERSTATE:	return TEXT("USERSTATE");
  case EIRCCommand::WHISPER:	return TEXT("WHISPER");
  case EIRCCommand::CAP:	return TEXT("CAP");
  case EIRCCommand::AUTHFAILED: return TEXT("AUTHFAILED");
  default: return TEXT("UNKNOWN");
  }
}

const TMap<FString, EIRCCommand> TwitchStringToIRCCommand = {
  {TEXT("PRIVMSG"), EIRCCommand::PRIVMSG},
  {TEXT("JOIN"), EIRCCommand::JOIN},
  {TEXT("PART"), EIRCCommand::PART},
  {TEXT("PING"), EIRCCommand::PING},
  {TEXT("GLOBALUSERSTATE"), EIRCCommand::GLOBALUSERSTATE},
  {TEXT("USERSTATE"), EIRCCommand::USERSTATE},
  {TEXT("NOTICE"), EIRCCommand::NOTICE},
  {TEXT("USERNOTICE"), EIRCCommand::USERNOTICE},
  {TEXT("CLEARCHAT"), EIRCCommand::CLEARCHAT},
  {TEXT("CLEARMSG"), EIRCCommand::CLEARMSG},
  {TEXT("CAP"), EIRCCommand::CAP},
  {TEXT("HOSTTARGET"), EIRCCommand::HOSTTARGET},
  {TEXT("RECONNECT"), EIRCCommand::RECONNECT},
  {TEXT("ROOMSTATE"), EIRCCommand::ROOMSTATE},
  {TEXT("WHISPER"), EIRCCommand::WHISPER}
};


const TMap<FString, ETwitchTagType> TwitchTagToType = {
  {TEXT("ban-duration"), ETwitchTagType::BanDuration},

  /* Common Tags */
  {TEXT("badge-info"), ETwitchTagType::BadgeInfo},
  {TEXT("badges"), ETwitchTagType::Badges},
  {TEXT("bits"), ETwitchTagType::Bits},
  {TEXT("color"), ETwitchTagType::NameColor},
  {TEXT("display-name"), ETwitchTagType::DisplayName},
  {TEXT("emotes"), ETwitchTagType::Emotes},
  {TEXT("id"), ETwitchTagType::ID},
  {TEXT("mod"), ETwitchTagType::Mod},
  {TEXT("room-id"), ETwitchTagType::RoomId},
  {TEXT("subscriber"), ETwitchTagType::Subscriber},
  {TEXT("tmi-sent-ts"), ETwitchTagType::TMISentTS},
  {TEXT("turbo"), ETwitchTagType::Turbo},
  {TEXT("user-id"), ETwitchTagType::UserId},
  {TEXT("user-type"), ETwitchTagType::UserType},
  {TEXT("target-msg-id"), ETwitchTagType::TargetMsgID},
  {TEXT("target-user-id"), ETwitchTagType::TargetUserID},
  {TEXT("login"), ETwitchTagType::Login},
  {TEXT("emote-sets"), ETwitchTagType::EmoteSets},
  {TEXT("msg-id"), ETwitchTagType::MsgID},
  {TEXT("system-msg"), ETwitchTagType::SystemMessage},

  /* PRIVMSG Tags */
  {TEXT("reply-parent-msg-id"), ETwitchTagType::ReplyParentMsgID},
  {TEXT("reply-parent-user-id"), ETwitchTagType::ReplyParentUserID},
  {TEXT("reply-parent-user-login"), ETwitchTagType::ReplyParentUserLogin},
  {TEXT("reply-parent-display-name"), ETwitchTagType::ReplyParentDisplayName},
  {TEXT("reply-parent-msg-body"), ETwitchTagType::ReplyParentMsgBody},
  {TEXT("first-msg"), ETwitchTagType::FirstMsg},
  {TEXT("returning-chatter"), ETwitchTagType::ReturningChatter},
  {TEXT("vip"), ETwitchTagType::VIP},
  {TEXT("custom-reward-id"), ETwitchTagType::CustomRewardID},

  /* USERNOTICE Type https://dev.twitch.tv/docs/irc/tags#usernotice-tags */
  {TEXT("msg-param-cumulative-months"), ETwitchTagType::MsgParamCumulativeMonths},
  {TEXT("msg-param-displayName"), ETwitchTagType::MsgParamDisplayName},
  {TEXT("msg-param-login"), ETwitchTagType::MsgParamLogin},
  {TEXT("msg-param-months"), ETwitchTagType::MsgParamMonths},
  {TEXT("msg-param-promo-gift-total"), ETwitchTagType::MsgParamGiftTotal},
  {TEXT("msg-param-recipient-display-name"), ETwitchTagType::MsgParamRecipientDisplayName},
  {TEXT("msg-param-recipient-id"), ETwitchTagType::MsgParamRecipientID},
  {TEXT("msg-param-recipient-user-name"), ETwitchTagType::MsgParamRecipientUserName},
  {TEXT("msg-param-sender-login"), ETwitchTagType::MsgParamSenderLogin},
  {TEXT("msg-param-sender-name"), ETwitchTagType::MsgParamSenderName},
  {TEXT("msg-param-should-share-streak"), ETwitchTagType::MsgParamShouldShareStreak},
  {TEXT("msg-param-streak-months"), ETwitchTagType::MsgParamStreakMonths},
  {TEXT("msg-param-sub-plan"), ETwitchTagType::MsgParamSubPlan},
  {TEXT("msg-param-sub-plan-name"), ETwitchTagType::MsgParamSubPlanName},
  {TEXT("msg-param-viewerCount"), ETwitchTagType::MsgParamViewerCount},
  {TEXT("msg-param-ritual-name"), ETwitchTagType::MsgParamRitualName},
  {TEXT("msg-param-threshold"), ETwitchTagType::MsgParamThreadhold},
  {TEXT("msg-param-gift-months"), ETwitchTagType::MsgParamGiftMonths},
  {TEXT("msg-param-multimonth-duration"), ETwitchTagType::MsgParamMultimonthDuration},
  {TEXT("msg-param-multimonth-tenure"), ETwitchTagType::MsgParamMultimonthTenure},
  {TEXT("msg-param-was-gifted"), ETwitchTagType::MsgParamWasGifted},
  {TEXT("msg-param-mass-gift-count"), ETwitchTagType::MsgParamMassGiftCount},
  {TEXT("msg-param-goal-contribution-type"), ETwitchTagType::MsgParamGoalContributionType},
  {TEXT("msg-param-goal-current-contributions"), ETwitchTagType::MsgParamGoalCurrentContributions},
  {TEXT("msg-param-goal-target-contributions"), ETwitchTagType::MsgParamGoalTargetContributions},
  {TEXT("msg-param-goal-user-contributions"), ETwitchTagType::MsgParamGoalUserContributions},
  {TEXT("msg-param-color"), ETwitchTagType::MsgParamColor},
  {TEXT("msg-param-profileImageURL"), ETwitchTagType::MsgParamProfileImageURL},

  /* ROOMSTATE Tags https://dev.twitch.tv/docs/irc/tags#roomstate-tags */
  {TEXT("emote-only"), ETwitchTagType::EmoteOnly},
  {TEXT("followers-only"), ETwitchTagType::FollowersOnly},
  {TEXT("r9k"), ETwitchTagType::R9K},
  {TEXT("rituals"), ETwitchTagType::Rituals},
  {TEXT("slow"), ETwitchTagType::Slow},
  {TEXT("subs-only"), ETwitchTagType::SubsOnly},

  /* WHISPER Tags */
  {TEXT("thread-id"), ETwitchTagType::ThreadID},
  {TEXT("message-id"), ETwitchTagType::MessageID},

  {TEXT("msg-param-prior-gifter-anonymous"), ETwitchTagType::MsgParamPriorGifterAnon},
  {TEXT("msg-param-prior-gifter-display-name"), ETwitchTagType::MsgParamPriorGifterDisplayName},
  {TEXT("msg-param-prior-gifter-id"), ETwitchTagType::MsgParamPriorGifterID},
  {TEXT("msg-param-prior-gifter-user-name"), ETwitchTagType::MsgParamPriorGifterUsername}
};

TSet<FString> IgnoredTwitchTags =
{
  TEXT("flags"),
  TEXT("client-nonce"),
  TEXT("msg-param-origin-id"),
  TEXT("msg-param-sender-count")
};

TSet<FString> IgnoredIRCCommands =
{
  TEXT("001"),
  TEXT("002"),
  TEXT("003"),
  TEXT("004"),
  TEXT("375"),
  TEXT("372"),
  TEXT("376"),
  TEXT("353"),
  TEXT("366")
};

const TMap<FString, ETWUserNoticeMsgId> UserNoticeIDStringToUserNoticeMsgID =
{
  {"sub", ETWUserNoticeMsgId::Subscription},
  {"resub", ETWUserNoticeMsgId::Resubscription},
  {"subgift", ETWUserNoticeMsgId::SubscriptionGift},
  {"submysterygift", ETWUserNoticeMsgId::SubscriptionMysterGift},
  {"giftpaidupgrade", ETWUserNoticeMsgId::GiftPaidUpgrade},
  {"rewardgift", ETWUserNoticeMsgId::GiftReward},
  {"anongiftpaidupgrade", ETWUserNoticeMsgId::AnonymousGiftPaidUpgrade},
  {"raid", ETWUserNoticeMsgId::Raid},
  {"unraid", ETWUserNoticeMsgId::None}, // We ignore unraid
  {"ritual", ETWUserNoticeMsgId::Ritual},
  {"bitsbadgetier", ETWUserNoticeMsgId::BitsBadgeTier},
  {"announcement", ETWUserNoticeMsgId::Announcement},
  {"communitypayforward", ETWUserNoticeMsgId::CommunityPayForward},
  {"standardpayforward", ETWUserNoticeMsgId::StandardPayForward}
};

TMap<FString, TwitchSubscriptionPlan> SubPlansToPlanID =
{
  {"Prime", TwitchSubscriptionPlan::Prime},
  {"1000", TwitchSubscriptionPlan::Tier1},
  {"2000", TwitchSubscriptionPlan::Tier2},
  {"3000", TwitchSubscriptionPlan::Tier3},
};

TMap<FString, ETWUserType> UserTypeStringToUserType =
{
  {"", ETWUserType::Normal},
  {"mod", ETWUserType::Mod},
  {"admin", ETWUserType::Admin},
  {"global_mod", ETWUserType::GlobalMod},
  {"staff", ETWUserType::Staff}
};

TMap<FString, ETWGoalContributionType> GoalStringToGoalType =
{
  {"SUBS", ETWGoalContributionType::Subs},
  {"SUB_POINTS", ETWGoalContributionType::SubPoints}
};

TMIParser::MessageBundle TMIParser::SplitRawMessage(const FString& Message)
{
  TArray<FString> Tags;
  FString OutSource;
  FString OutTarget;
  FString OutParams;
  FString RawCommand;
  EIRCCommand Command = EIRCCommand::UNKNOWN;

  if (!Message.IsEmpty())
  {
    int32 Index = 0, Endex = 0;

    // Do we have tags to parse?
    if (Message[Index] == TCHAR('@'))
    {
      Message.FindChar(TCHAR(' '), Endex);

      Message.Mid(Index + 1, Endex - 1).ParseIntoArray(Tags, TEXT(";"));
      Index = Endex + 1;
    }

    // Get source component, nickname and host origin, of this message
    // Otherwise this is a ping command
    if (Message[Index] == TCHAR(':'))
    {
      Endex = Message.Find(" ", ESearchCase::CaseSensitive, ESearchDir::FromStart, Index);

      OutSource = ParseSource(Message.Mid(Index + 1, Endex - Index - 1));
      Index = Endex + 1;
    }

    // Find where the IRC command parameters might start

    if ((Endex = Message.Find(":", ESearchCase::CaseSensitive, ESearchDir::FromStart, Index)) <= 0)
    {
      Endex = Message.Len();
    }

    RawCommand = Message.Mid(Index, Endex - Index);
    RawCommand.TrimEndInline();

    int32 cmdtarget = -1;
    if (RawCommand.FindChar(TCHAR(' '), cmdtarget))
    {
      OutTarget = RawCommand.RightChop(cmdtarget + 1 + (RawCommand[cmdtarget + 1] == TCHAR('#') ? 1 : 0));
      RawCommand.LeftInline(cmdtarget);
    }

    // Parse out the parameters we found earlier
    if (Endex < Message.Len())
    {
      OutParams = Message.Mid(Endex + 1);
    }
  }

  return TMIParser::MessageBundle(ParseCommand(RawCommand), RawCommand, Tags, OutSource, OutTarget, OutParams);
}

EIRCCommand TMIParser::ParseCommand(const FString& CommandStr)
{
  const EIRCCommand* Cmd = TwitchStringToIRCCommand.Find(CommandStr);

  if (Cmd == nullptr)
  {
    if (!IgnoredIRCCommands.Contains(CommandStr))
      PARSER_LOG(Log, TEXT("Encountered unknown IRC Command: %s"), *CommandStr);

    return EIRCCommand::UNKNOWN;
  }

  return *Cmd;
}

ETWUserNoticeMsgId TMIParser::ParseUserNoticeMsgID(const FString& MsgID)
{
  const ETWUserNoticeMsgId* UserNoticeID = UserNoticeIDStringToUserNoticeMsgID.Find(MsgID);

  if (UserNoticeID == nullptr)
  {
    PARSER_LOG(Log, TEXT("Encountered unknown USERNOTICE MsgID: %s"), *MsgID);
    return ETWUserNoticeMsgId::None;
  }

  return *UserNoticeID;
}

void TMIParser::ParseBadges(FString& BadgeStr, TMap<FString, int32>& OutBadges)
{
  TArray<FString> Badges;

  if (BadgeStr.ParseIntoArray(Badges, TEXT(",")) > 0)
  {
    for (FString& Badge : Badges)
    {
      FString BadgeName, BadgeVersionStr;
      Badge.Split(TEXT("/"), &BadgeName, &BadgeVersionStr);

      int32 BadgeVersion;
      FDefaultValueHelper::ParseInt(BadgeVersionStr, BadgeVersion);

      OutBadges.Add(BadgeName, BadgeVersion);
    }
  }
}

void TMIParser::ParseEmotes(FString& EmoteStr, TMap<FString, FTWEmoteData>& OutEmotes)
{
  TArray<FString> Emotes;

  if (!EmoteStr.IsEmpty() && EmoteStr[0] != TCHAR('/') && EmoteStr.ParseIntoArray(Emotes, TEXT("/")) > 0)
  {
    for (FString& Emote : Emotes)
    {
      FString EmoteID;
      FString EmotePosStr;
      Emote.Split(TEXT(":"), &EmoteID, &EmotePosStr);

      FString StartPosStr;
      FString EndPosStr;
      TArray<FString> EmotePositionsArr;
      EmotePosStr.ParseIntoArray(EmotePositionsArr, TEXT(","));

      TArray<FTWEmotePositions> EmotePositions;

      for (FString& EmotePos : EmotePositionsArr)
      {
        EmotePos.Split(TEXT("-"), &StartPosStr, &EndPosStr);

        int32 StartPos;
        int32 EndPos;
        FDefaultValueHelper::ParseInt(StartPosStr, StartPos);
        FDefaultValueHelper::ParseInt(EndPosStr, EndPos);
        EmotePositions.Emplace(FTWEmotePositions(StartPos, EndPos));
      }

      OutEmotes.Add(EmoteID, FTWEmoteData(EmotePositions));
    }
  }
}

FString TMIParser::ParseSource(const FString& InStr)
{
  int32 Endex;
  if (InStr.FindChar(TCHAR('!'), Endex))
  {
    return InStr.Left(Endex);
  }
  return InStr;
}

TwitchSubscriptionPlan TMIParser::ParseSubPlan(const FString& PlanID)
{
  TwitchSubscriptionPlan* id = SubPlansToPlanID.Find(PlanID);

  if (id == nullptr)
  {
    PARSER_LOG(Log, TEXT("Encountered unknown subscription plan type: %s"), *PlanID);
    return TwitchSubscriptionPlan::None;
  }

  return *id;
}

bool TMIParser::ParseTags(EIRCCommand ParsingCommand, const TArray<FString>& InTags, TwitchTagsMaster& OutTags)
{
  bool bValid = InTags.Num() > 0;

  for (const FString& Tag : InTags)
  {
    int32 Index;

    if (Tag.FindChar(TCHAR('='), Index))
    {
      FString Key = Tag.Left(Index);

      if (IgnoredTwitchTags.Contains(Key))
        continue;

      const ETwitchTagType* TagType = TwitchTagToType.Find(Key);

      if (TagType == nullptr)
      {
        PARSER_LOG(Log, TEXT("Unknown tag encountered for command %s - [%s]"), *GetIRCCommandString(ParsingCommand), *Tag);
        continue;
      }

      FString Value = Tag.Mid(Index + 1);

      switch (*TagType)
      {
      case ETwitchTagType::EmoteSets:
      {
        TArray<FString> EmoteSets;
        if (Value.ParseIntoArray(EmoteSets, TEXT(",")) > 0)
        {
          OutTags.EmoteSets = EmoteSets;
        }
        break;
      }

      case ETwitchTagType::TargetUserID:
        OutTags.TargetUserID = Value;
        break;

      case ETwitchTagType::TargetMsgID:
        OutTags.TargetMsgID = Value;
        break;

      case ETwitchTagType::Login:
        OutTags.Login = Value;
        break;

      case ETwitchTagType::ID:
        if (Value.Len() > 0)
        {
          if (!FGuid::ParseExact(Value, EGuidFormats::DigitsWithHyphens, OutTags.ID))
          {
            PARSER_LOG(Log, TEXT("Unable to parse ID GUID, invalid format? %s"), *Value);
          }
        }
        break;

      case ETwitchTagType::MsgID:
        if (ParsingCommand == EIRCCommand::NOTICE)
        {
          OutTags.NoticeMsgID = Value;
        }
        else if (ParsingCommand == EIRCCommand::USERNOTICE)
        {
          OutTags.UserNoticeMsgID = ParseUserNoticeMsgID(Value);
        }
        else
          OutTags.MsgID = Value;
        break;

      case ETwitchTagType::CustomRewardID:
        // This tag may be present with no value set?
        if (Value.Len() > 0)
        {
          if (!FGuid::ParseExact(Value, EGuidFormats::DigitsWithHyphens, OutTags.CustomRewardID))
          {
            PARSER_LOG(Log, TEXT("Unable to parse CustomRewardID GUID, invalid format? %s"), *Value);
          }
        }
        break;

        // PRIVMSG Reply Tags
      case ETwitchTagType::ReplyParentMsgID:
        if (Value.Len() > 0)
        {
          if (!FGuid::ParseExact(Value, EGuidFormats::DigitsWithHyphens, OutTags.ReplyParentMsgID))
          {
            PARSER_LOG(Log, TEXT("Unable to parse ReplyParentMsgID GUID, invalid format? %s"), *Value);
          }
        }
        break;

      case ETwitchTagType::ReplyParentUserID:
        OutTags.ReplyParentUserID = Value;
        break;

      case ETwitchTagType::ReplyParentUserLogin:
        OutTags.ReplyParentUserLogin = Value;
        break;

      case ETwitchTagType::ReplyParentDisplayName:
        OutTags.ReplyParentDisplayName = Value;
        break;

      case ETwitchTagType::ReplyParentMsgBody:
        OutTags.ReplyParentMsgBody = Value;
        break;

      case ETwitchTagType::SystemMessage:
          OutTags.SystemMessage = Value.Replace(TEXT("\\s"), TEXT(" "), ESearchCase::CaseSensitive);
        break;

      // USERNOTICE Parameter Tags
      case ETwitchTagType::MsgParamColor:
        OutTags.MessageParams.Color = Value;
        break;

      case ETwitchTagType::MsgParamCumulativeMonths:
        FDefaultValueHelper::ParseInt(Value, OutTags.MessageParams.CumulativeMonths);
        break;

      case ETwitchTagType::MsgParamDisplayName:
        OutTags.MessageParams.DisplayName = Value;
        break;

      case ETwitchTagType::MsgParamLogin:
        OutTags.MessageParams.Login = Value;
        break;

      case ETwitchTagType::MsgParamMonths:
        FDefaultValueHelper::ParseInt(Value, OutTags.MessageParams.Months);
        break;

      case ETwitchTagType::MsgParamGiftTotal:
        FDefaultValueHelper::ParseInt(Value, OutTags.MessageParams.PromoGiftTotal);
        break;

      case ETwitchTagType::MsgParamRecipientDisplayName:
        OutTags.MessageParams.RecipientDisplayName = Value;
        break;

      case ETwitchTagType::MsgParamRecipientID:
        OutTags.MessageParams.RecipientID = Value;
        break;

      case ETwitchTagType::MsgParamRecipientUserName:
        OutTags.MessageParams.RecipientUsername = Value;
        break;

      case ETwitchTagType::MsgParamSenderLogin:
        OutTags.MessageParams.SenderLogin = Value;
        break;

      case ETwitchTagType::MsgParamSenderName:
        OutTags.MessageParams.SenderName = Value;
        break;

      case ETwitchTagType::MsgParamShouldShareStreak:
        OutTags.MessageParams.ShouldShareStreak = Value.ToBool();
        break;

      case ETwitchTagType::MsgParamStreakMonths:
        FDefaultValueHelper::ParseInt(Value, OutTags.MessageParams.StreakMonths);
        break;

      case ETwitchTagType::MsgParamSubPlan:
        OutTags.MessageParams.SubPlan = ParseSubPlan(Value);
        break;

      case ETwitchTagType::MsgParamSubPlanName:
        OutTags.MessageParams.SubPlanName = Value;
        break;

      case ETwitchTagType::MsgParamViewerCount:
        FDefaultValueHelper::ParseInt(Value, OutTags.MessageParams.ViewerCount);
        break;

      case ETwitchTagType::MsgParamRitualName:
        OutTags.MessageParams.RitualName = Value;
        break;

      case ETwitchTagType::MsgParamThreadhold:
        FDefaultValueHelper::ParseInt(Value, OutTags.MessageParams.Threshold);
        break;

      case ETwitchTagType::MsgParamGiftMonths:
        FDefaultValueHelper::ParseInt(Value, OutTags.MessageParams.GiftMonths);
        break;

      case ETwitchTagType::MsgParamMassGiftCount:
        FDefaultValueHelper::ParseInt(Value, OutTags.MessageParams.MassGiftCount);
        break;

      case ETwitchTagType::MsgParamGoalContributionType:
        OutTags.MessageParams.GoalType = ParseUserNoticeGoalType(Value);
        break;

      case ETwitchTagType::MsgParamGoalCurrentContributions:
        FDefaultValueHelper::ParseInt(Value, OutTags.MessageParams.GoalCurrentContributions);
        break;

      case ETwitchTagType::MsgParamGoalTargetContributions:
        FDefaultValueHelper::ParseInt(Value, OutTags.MessageParams.GoalTargetContributions);
        break;

      case ETwitchTagType::MsgParamGoalUserContributions:
        FDefaultValueHelper::ParseInt(Value, OutTags.MessageParams.GoalUserContributions);
        break;

      case ETwitchTagType::MsgParamPriorGifterAnon:
        OutTags.MessageParams.bPriorGifterIsAnon = Value.ToBool();
        break;

      case ETwitchTagType::MsgParamPriorGifterID:
        OutTags.MessageParams.PriorGifterID = Value;
        break;

      case ETwitchTagType::MsgParamPriorGifterDisplayName:
        OutTags.MessageParams.PriorGifterDisplayName = Value;
        break;

      case ETwitchTagType::MsgParamPriorGifterUsername:
        OutTags.MessageParams.PriorGifterUsername = Value;
        break;

      case ETwitchTagType::MsgParamProfileImageURL:
        OutTags.MessageParams.ProfileImageURL = Value;
        break;

      // END USERNOTICE Parameter Tags
      case ETwitchTagType::EmoteOnly:
        OutTags.EmoteOnly = Value.ToBool();
        break;

      case ETwitchTagType::BadgeInfo:
        ParseBadges(Value, OutTags.BadgesInfo);
        break;

      case ETwitchTagType::Badges:
        ParseBadges(Value, OutTags.Badges);
        break;

      case ETwitchTagType::DisplayName:
        OutTags.DisplayName = Value;
        break;

      case ETwitchTagType::Emotes:
        ParseEmotes(Value, OutTags.Emotes);
        break;

      case ETwitchTagType::Bits:
        FDefaultValueHelper::ParseInt(Value, OutTags.Bits);
        break;

      case ETwitchTagType::NameColor:
      {
        int32 R = FParse::HexNumber(*Value.Mid(1, 2));
        int32 G = FParse::HexNumber(*Value.Mid(3, 2));
        int32 B = FParse::HexNumber(*Value.Mid(5, 2));

        OutTags.NameColor = FLinearColor(R / 255.0f, G / 255.0f, B / 255.0f);
        break;
      }

      case ETwitchTagType::MessageID:
        OutTags.MessageID = Value;
        break;

      case ETwitchTagType::BanDuration:
        FDefaultValueHelper::ParseInt(Value, OutTags.BanDuration);
        break;

      case ETwitchTagType::FollowersOnly:
        FDefaultValueHelper::ParseInt(Value, OutTags.FollowersOnlyMinMinutes);
        break;

      case ETwitchTagType::SubsOnly:
        OutTags.SubscribersOnly = Value.ToBool();
        break;

      case ETwitchTagType::Slow:
        OutTags.SlowMode = Value.ToBool();
        break;

      case ETwitchTagType::ThreadID:
        OutTags.ThreadID = Value;
        break;

      case ETwitchTagType::R9K:
        OutTags.R9K = Value.ToBool();
        break;

      case ETwitchTagType::Mod:
        OutTags.Mod = Value.ToBool();
        break;

      case ETwitchTagType::Turbo:
        OutTags.Turbo = Value.ToBool();
        break;

      case ETwitchTagType::Subscriber:
        OutTags.Subscriber = Value.ToBool();
        break;

        // The mere presence of this tag is enough as per the api docs
      case ETwitchTagType::VIP:
        OutTags.VIP = true;
        break;

      case ETwitchTagType::TMISentTS:
        FDefaultValueHelper::ParseInt64(Value, OutTags.TMISentTS);
        break;

      case ETwitchTagType::UserId:
        OutTags.UserID = Value;
        break;

      case ETwitchTagType::UserType:
        OutTags.UserType = ParseUserType(Value);
        break;
      }
    }
  }

  return bValid;
}

ETWUserType TMIParser::ParseUserType(const FString& UserType)
{
  ETWUserType* Type = UserTypeStringToUserType.Find(UserType);

  if (Type == nullptr)
  {
    PARSER_LOG(Log, TEXT("Unknown user type encountered: %s"), *UserType);
    return ETWUserType::Unknown;
  }

  return *Type;
}

ETWGoalContributionType TMIParser::ParseUserNoticeGoalType(const FString& Goal)
{
  ETWGoalContributionType* Type = GoalStringToGoalType.Find(Goal);

  if (Type == nullptr)
  {
    PARSER_LOG(Log, TEXT("Unknown user notice goal encountered: %s"), *Goal);
    return ETWGoalContributionType::Unknown;
  }

  return *Type;
}


template<typename MsgType>
MsgType TMIParser::ParseMessage(MessageBundle& InMessage)
{
  TwitchTagsMaster Tags;

  bool bTagsValid = ParseTags(InMessage.Command, InMessage.Tags, Tags);

  return MsgType(InMessage, bTagsValid, Tags);
}
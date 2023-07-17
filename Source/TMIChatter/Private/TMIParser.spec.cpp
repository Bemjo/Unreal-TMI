#include "TMIParser.h"

inline FLinearColor HexToLinearColor(const FString& xR, const FString& xG, const FString& xB)
{
  float R = FParse::HexNumber(*xR) / 255.0f;
  float G = FParse::HexNumber(*xG) / 255.0f;
  float B = FParse::HexNumber(*xB) / 255.0f;

  return FLinearColor(R, G, B);
}

template<typename MsgType>
struct ExpectedTestData {
  ExpectedTestData() {}

  ExpectedTestData(const FString& RawInput, const TMIParser::MessageBundle& Bundle, const MsgType& Message)
    : RawInput(RawInput), Bundle(Bundle), Message(Message) {}

  ExpectedTestData(const ExpectedTestData& RHS) : RawInput(RHS.RawInput), Bundle(RHS.Bundle), Message(RHS.Message) {}

  FString RawInput;
  TMIParser::MessageBundle Bundle;
  MsgType Message;
};

BEGIN_DEFINE_SPEC(TMIParserSpec, "TMIParser", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
TArray<ExpectedTestData<FClearChatMessage>> ClearChatTestMessages;
ExpectedTestData<FClearChatMessage> ExpectedClearChat;
FClearChatMessage ParsedClearChat;

TArray<ExpectedTestData<FClearMsgMessage>> ClearMsgTestMessages;
ExpectedTestData<FClearMsgMessage> ExpectedClearMsg;
FClearMsgMessage ParsedClearMsg;

TArray<ExpectedTestData<FRoomStateMessage>> RoomStateTestMessages;
ExpectedTestData<FRoomStateMessage> ExpectedRoomState;
FRoomStateMessage ParsedRoomState;

TArray<ExpectedTestData<FPrivMsgMessage>> PrivMsgTestMessages;
ExpectedTestData<FPrivMsgMessage> ExpectedPrivMsg;
FPrivMsgMessage ParsedPrivMsg;

TArray<ExpectedTestData<FUserStateMessage>> UserStateTestMessages;
ExpectedTestData<FUserStateMessage> ExpectedUserState;
FUserStateMessage ParsedUserState;

TArray<ExpectedTestData<FUserNoticeMessage>> UserNoticeTestMessages;
ExpectedTestData<FUserNoticeMessage> ExpectedUserNotice;
FUserNoticeMessage ParsedUserNotice;

TArray<ExpectedTestData<FNoticeMessage>> NoticeTestMessages;
ExpectedTestData<FNoticeMessage> ExpectedNotice;
FNoticeMessage ParsedNotice;

TArray<ExpectedTestData<FGlobalUserStateMessage>> GlobalUserStateTestMessages;
ExpectedTestData<FGlobalUserStateMessage> ExpectedGlobalUserState;
FGlobalUserStateMessage ParsedGlobalUserState;

TArray<ExpectedTestData<FWhisperMessage>> WhisperTestMessages;
ExpectedTestData<FWhisperMessage> ExpectedWhisper;
FWhisperMessage ParsedWhisper;

TMIParser::MessageBundle ParsedBundle;

FPrivMsgMessage EmptyPrivMessage;

void LoadTestMessages();

template<typename TagStruct, const EIRCCommand Command>
void TagTests(const TagStruct& Tags, const TagStruct& Expected)
{
  if constexpr (Command == EIRCCommand::PRIVMSG)
  {
    TestEqual("FirstMsg", Tags.FirstMsg, Expected.FirstMsg);
    TestEqual("ReturningChatter", Tags.ReturningChatter, Expected.ReturningChatter);
    TestEqual("CustomRewardID", Tags.CustomRewardID, Expected.CustomRewardID);
    TestEqual("Bits", Tags.Bits, Expected.Bits);
    TestEqual("VIP", Tags.VIP, Expected.VIP);
    TestEqual("ReplyParentMsgID", Tags.ReplyParentMsgID, Expected.ReplyParentMsgID);
    TestEqual("ReplyParentUserID", Tags.ReplyParentUserID, Expected.ReplyParentUserID);
    TestEqual("ReplyParentUserLogin", Tags.ReplyParentUserLogin, Expected.ReplyParentUserLogin);
    TestEqual("ReplyParentDisplayName", Tags.ReplyParentDisplayName, Expected.ReplyParentDisplayName);
    TestEqual("ReplyParentMsgBody", Tags.ReplyParentMsgBody, Expected.ReplyParentMsgBody);
  }

  if constexpr (Command == EIRCCommand::PRIVMSG || Command == EIRCCommand::USERSTATE || Command == EIRCCommand::USERNOTICE)
  {
    TestEqual("Mod", Tags.Mod, Expected.Mod);
    TestEqual("Subscriber", Tags.Subscriber, Expected.Subscriber);
    TestEqual("ID", Tags.ID, Expected.ID);
  }

  if constexpr (Command == EIRCCommand::ROOMSTATE)
  {
    TestEqual("EmoteOnly", Tags.EmoteOnly, Expected.EmoteOnly);
    TestEqual("FollowersOnlyMinMinutes", Tags.FollowersOnlyMinMinutes, Expected.FollowersOnlyMinMinutes);
    TestEqual("R9K", Tags.R9K, Expected.R9K);
    TestEqual("SlowMode", Tags.SlowMode, Expected.SlowMode);
    TestEqual("SubscribersOnly", Tags.SubscribersOnly, Expected.SubscribersOnly);
  }

  if constexpr (Command == EIRCCommand::CLEARCHAT)
  {
    TestEqual("BanDuration", Tags.BanDuration, Expected.BanDuration);
  }

  if constexpr (Command == EIRCCommand::CLEARMSG)
  {
    TestEqual("TargetMsgID", Tags.TargetMsgID, Expected.TargetMsgID);
  }

  if constexpr (Command == EIRCCommand::USERNOTICE)
  {
    TestEqual("SystemMessage", Tags.SystemMessage, Expected.SystemMessage);

    TestEqual("MsgParam CumulativeMonths", Tags.MessageParams.CumulativeMonths, Expected.MessageParams.CumulativeMonths);
    TestEqual("MsgParam DisplayName", Tags.MessageParams.DisplayName, Expected.MessageParams.DisplayName);
    TestEqual("MsgParam Login", Tags.MessageParams.Login, Expected.MessageParams.Login);
    TestEqual("MsgParam Months", Tags.MessageParams.Months, Expected.MessageParams.Months);
    TestEqual("MsgParam PromoGiftTotal", Tags.MessageParams.PromoGiftTotal, Expected.MessageParams.PromoGiftTotal);
    TestEqual("MsgParam PromoName", Tags.MessageParams.PromoName, Expected.MessageParams.PromoName);
    TestEqual("MsgParam RecipientDisplayName", Tags.MessageParams.RecipientDisplayName, Expected.MessageParams.RecipientDisplayName);
    TestEqual("MsgParam RecipientID", Tags.MessageParams.RecipientID, Expected.MessageParams.RecipientID);
    TestEqual("MsgParam RecipientUsername", Tags.MessageParams.RecipientUsername, Expected.MessageParams.RecipientUsername);
    TestEqual("MsgParam SenderLogin", Tags.MessageParams.SenderLogin, Expected.MessageParams.SenderLogin);
    TestEqual("MsgParam SenderName", Tags.MessageParams.SenderName, Expected.MessageParams.SenderName);
    TestEqual("MsgParam ShouldShareStreak", Tags.MessageParams.ShouldShareStreak, Expected.MessageParams.ShouldShareStreak);
    TestEqual("MsgParam StreakMonths", Tags.MessageParams.StreakMonths, Expected.MessageParams.StreakMonths);
    TestEqual("MsgParam SubPlan", Tags.MessageParams.SubPlan, Expected.MessageParams.SubPlan);
    TestEqual("MsgParam SubPlanName", Tags.MessageParams.SubPlanName, Expected.MessageParams.SubPlanName);
    TestEqual("MsgParam ViewerCount", Tags.MessageParams.ViewerCount, Expected.MessageParams.ViewerCount);
    TestEqual("MsgParam RitualName", Tags.MessageParams.RitualName, Expected.MessageParams.RitualName);
    TestEqual("MsgParam Threshold", Tags.MessageParams.Threshold, Expected.MessageParams.Threshold);
    TestEqual("MsgParam GiftMonths", Tags.MessageParams.GiftMonths, Expected.MessageParams.GiftMonths);
    TestEqual("MsgParam MassGiftCount", Tags.MessageParams.MassGiftCount, Expected.MessageParams.MassGiftCount);
    TestEqual("MsgParam GoalType", Tags.MessageParams.GoalType, Expected.MessageParams.GoalType);
    TestEqual("MsgParam GoalCurrentContributions", Tags.MessageParams.GoalCurrentContributions, Expected.MessageParams.GoalCurrentContributions);
    TestEqual("MsgParam GoalTargetContributions", Tags.MessageParams.GoalTargetContributions, Expected.MessageParams.GoalTargetContributions);
    TestEqual("MsgParam GoalUserContributions", Tags.MessageParams.GoalUserContributions, Expected.MessageParams.GoalUserContributions);
    TestEqual("MsgParam PriorGifterIsAnon", Tags.MessageParams.bPriorGifterIsAnon, Expected.MessageParams.bPriorGifterIsAnon);
    TestEqual("MsgParam PriorGifterDisplayName", Tags.MessageParams.PriorGifterDisplayName, Expected.MessageParams.PriorGifterDisplayName);
    TestEqual("MsgParam PriorGifterID", Tags.MessageParams.PriorGifterID, Expected.MessageParams.PriorGifterID);
    TestEqual("MsgParam PriorGifterUsername", Tags.MessageParams.PriorGifterUsername, Expected.MessageParams.PriorGifterUsername);
    TestEqual("MsgParam Color", Tags.MessageParams.Color, Expected.MessageParams.Color);
    TestEqual("MsgParam ProfileImageURL", Tags.MessageParams.ProfileImageURL, Expected.MessageParams.ProfileImageURL);
  }

  if constexpr (Command == EIRCCommand::NOTICE || Command == EIRCCommand::CLEARCHAT)
  {
    TestEqual("TargetUserID", Tags.TargetUserID, Expected.TargetUserID);
  }

  if constexpr (Command == EIRCCommand::WHISPER)
  {
    TestEqual("MessageID", Tags.MessageID, Expected.MessageID);
    TestEqual("ThreadID", Tags.ThreadID, Expected.ThreadID);
  }

  if constexpr (Command == EIRCCommand::CLEARMSG || Command == EIRCCommand::USERNOTICE)
  {
    TestEqual("Login", Tags.Login, Expected.Login);
  }

  if constexpr (Command == EIRCCommand::USERNOTICE || Command == EIRCCommand::NOTICE)
  {
    TestEqual("MsgID", Tags.MsgID, Expected.MsgID);
  }

  if constexpr (
    Command == EIRCCommand::PRIVMSG || Command == EIRCCommand::GLOBALUSERSTATE || Command == EIRCCommand::USERNOTICE || Command == EIRCCommand::USERSTATE)
  {
    TestEqual("Badge Info Count", Tags.BadgesInfo.Num(), Expected.BadgesInfo.Num());

    for (const TPair<FString, int32>& ExpectedBadge : Expected.BadgesInfo)
    {
      const int32* Value = Tags.BadgesInfo.Find(ExpectedBadge.Key);

      TestTrue("Has Key", Value != nullptr);

      if (Value != nullptr)
      {
        TestEqual("Key has Value", *Value, ExpectedBadge.Value);
      }
    }
  }

  if constexpr (
    Command == EIRCCommand::PRIVMSG || Command == EIRCCommand::GLOBALUSERSTATE ||
    Command == EIRCCommand::USERNOTICE || Command == EIRCCommand::USERSTATE || Command == EIRCCommand::WHISPER)
  {
    TestEqual("Badge Count", Tags.Badges.Num(), Expected.Badges.Num());

    for (const TPair<FString, int32>& ExpectedBadge : Expected.Badges)
    {
      const int32* Value = Tags.Badges.Find(ExpectedBadge.Key);

      TestTrue("Has Key", Value != nullptr);

      if (Value != nullptr)
      {
        TestEqual("Key has Value", *Value, ExpectedBadge.Value);
      }
    }

    TestEqual("DisplayName", Tags.DisplayName, Expected.DisplayName);
    TestEqual("NameColor", Tags.NameColor, Expected.NameColor);
    TestEqual("UserType", Tags.UserType, Expected.UserType);
    TestEqual("Turbo", Tags.Turbo, Expected.Turbo);
  }

  if constexpr (
    Command == EIRCCommand::PRIVMSG || Command == EIRCCommand::GLOBALUSERSTATE ||
    Command == EIRCCommand::USERNOTICE || Command == EIRCCommand::WHISPER)
  {
    TestEqual("UserID", Tags.UserID, Expected.UserID);
  }

  if constexpr (
    Command == EIRCCommand::CLEARCHAT || Command == EIRCCommand::CLEARMSG ||
    Command == EIRCCommand::PRIVMSG || Command == EIRCCommand::USERNOTICE) {
    TestEqual("TMISentTS", Tags.TMISentTS, Expected.TMISentTS);
  }

  if constexpr (Command == EIRCCommand::PRIVMSG || Command == EIRCCommand::WHISPER)
  {
    TestEqual("Emotes Count", Tags.Emotes.Num(), Expected.Emotes.Num());

    for (const TPair<FString, FTWEmoteData>& ExpectedEmote : Expected.Emotes)
    {
      const FTWEmoteData* ParsedEmoteData = Tags.Emotes.Find(ExpectedEmote.Key);

      TestTrue("Has EmoteID", ParsedEmoteData != nullptr);

      if (ParsedEmoteData != nullptr)
      {
        TestEqual("Emote ID Count", ParsedEmoteData->EmotePositions.Num(), ExpectedEmote.Value.EmotePositions.Num());

        if (ParsedEmoteData->EmotePositions.Num() == ExpectedEmote.Value.EmotePositions.Num())
        {
          for (int i = 0; i < ExpectedEmote.Value.EmotePositions.Num(); ++i)
          {
            TestEqual("Emote Start Position", ParsedEmoteData->EmotePositions[i].Start, ExpectedEmote.Value.EmotePositions[i].Start);
            TestEqual("Emote End Position", ParsedEmoteData->EmotePositions[i].End, ExpectedEmote.Value.EmotePositions[i].End);
          }
        }
      }
    }
  }

  if constexpr (Command == EIRCCommand::USERSTATE || Command == EIRCCommand::GLOBALUSERSTATE)
  {
    TestEqual("Emote Sets Count", Tags.EmoteSets.Num(), Expected.EmoteSets.Num());

    if (Tags.EmoteSets.Num() == Expected.EmoteSets.Num())
    {
      for (int i = 0; i < Tags.EmoteSets.Num(); ++i)
      {
        TestEqual("Emote Set", Tags.EmoteSets[i], Expected.EmoteSets[i]);
      }
    }
  }
}

void BundleTests(const TMIParser::MessageBundle& Bundle, const TMIParser::MessageBundle& Expected)
{
  TestEqual("Command", ParsedBundle.Command, Expected.Command);
  TestEqual("RawCommand", ParsedBundle.RawCommand, Expected.RawCommand);
  TestEqual("Tag Count", ParsedBundle.Tags.Num(), Expected.Tags.Num());
  TestEqual("Source", ParsedBundle.Source, Expected.Source);
  TestEqual("Target", ParsedBundle.Target, Expected.Target);
  TestEqual("Params", ParsedBundle.Params, Expected.Params);

  if (ParsedBundle.Tags.Num() == Expected.Tags.Num())
  {
    for (int i = 0; i < Expected.Tags.Num(); ++i)
    {
      TestEqual("Raw Tag", ParsedBundle.Tags[i], Expected.Tags[i]);
    }
  }
}

END_DEFINE_SPEC(TMIParserSpec);

void TMIParserSpec::LoadTestMessages()
{
  // PRIVCHAT
  FString RawPrivMsgString = TEXT("@badge-info=;badges=turbo/1;color=#0D4200;display-name=TheJollyBeardoBOT;emotes=25:0-4,12-16/1902:6-10;id=b34ccfc7-4977-403a-8a94-33c6bac34fb8;mod=0;room-id=1337;subscriber=0;tmi-sent-ts=1507246572675;turbo=1;user-id=1337;user-type=global_mod :thejollybeardobot!thejollybeardobot@thejollybeardobot.tmi.twitch.tv PRIVMSG #ronni :Kappa Keepo Kappa");

  TMIParser::MessageBundle PrivMsgBundle(
    EIRCCommand::PRIVMSG,
    "PRIVMSG",
    {
      "badge-info=",
      "badges=turbo/1",
      "color=#0D4200",
      "display-name=TheJollyBeardoBOT",
      "emotes=25:0-4,12-16/1902:6-10",
      "id=b34ccfc7-4977-403a-8a94-33c6bac34fb8",
      "mod=0",
      "room-id=1337",
      "subscriber=0",
      "tmi-sent-ts=1507246572675",
      "turbo=1",
      "user-id=1337",
      "user-type=global_mod"
    },
    "thejollybeardobot",
    "ronni",
    "Kappa Keepo Kappa"
  );

  FTWPrivMsgTags PrivMsgTags(
    {
      {"turbo", 1}
    },
    {},
    0,
    HexToLinearColor("0D", "42", "00"),
    TEXT("TheJollyBeardoBOT"),
    {
      {"25", FTWEmoteData({FTWEmotePositions(0, 4), FTWEmotePositions(12, 16)}) },
      {"1902", FTWEmoteData({ FTWEmotePositions(6, 10) }) }
    },
    TEXT("b34ccfc7-4977-403a-8a94-33c6bac34fb8"),
    true,
    false,
    false,
    false,
    false,
    false,
    TEXT(""),
    TEXT(""),
    TEXT(""),
    TEXT(""),
    TEXT(""),
    TEXT(""),
    TEXT("1337"),
    ETWUserType::GlobalMod,
    1507246572675
  );

  // CLEARCHAT
  FString RawClearChatString = TEXT("@ban-duration=360;room-id=12345678;target-user-id=87654321;tmi-sent-ts=1642715756806 :tmi.twitch.tv CLEARCHAT #dallas :ronni");

  TMIParser::MessageBundle ClearChatBundle(EIRCCommand::CLEARCHAT, "CLEARCHAT",
    {
      "ban-duration=360",
      "room-id=12345678",
      "target-user-id=87654321",
      "tmi-sent-ts=1642715756806"
    }, "tmi.twitch.tv", "dallas", "ronni"
  );

  FTWClearChatTags ClearChatTags("87654321", 360, 1642715756806);

  // ROOMSTATE
  FString RawRoomStateString = TEXT("@emote-only=0;followers-only=-1;r9k=0;rituals=0;room-id=12345678;slow=0;subs-only=0 :tmi.twitch.tv ROOMSTATE #bar");

  TMIParser::MessageBundle RoomStateBundle(EIRCCommand::ROOMSTATE, "ROOMSTATE", {
    "emote-only=0",
    "followers-only=-1",
    "r9k=0",
    "rituals=0",
    "room-id=12345678",
    "slow=0",
    "subs-only=0"
    }, "tmi.twitch.tv", "bar", ""
  );

  FTWRoomStateTags RoomStateTags(false, -1, false, false, false, false);

  // WHISPER
  FString RawWhisperString = TEXT("@badges=staff/1,bits-charity/1;color=#8A2BE2;display-name=PetsgomOO;emotes=;message-id=306;thread-id=12345678_87654321;turbo=0;user-id=87654321;user-type=staff :petsgomoo!petsgomoo@petsgomoo.tmi.twitch.tv WHISPER foo :hello");

  TMIParser::MessageBundle WhisperBundle(EIRCCommand::WHISPER, "WHISPER", {
    "badges=staff/1,bits-charity/1",
    "color=#8A2BE2",
    "display-name=PetsgomOO",
    "emotes=",
    "message-id=306",
    "thread-id=12345678_87654321",
    "turbo=0",
    "user-id=87654321",
    "user-type=staff"
    }, "petsgomoo", "foo", "hello");

  FTWWhisperTags WhisperTags(
    { {"staff", 1}, {"bits-charity", 1} },
    HexToLinearColor("8A", "2B", "E2"),
    "PetsgomOO",
    {},
    "306",
    "12345678_87654321",
    false,
    "87654321",
    ETWUserType::Staff
  );

  FString RawGlobalUserString = TEXT("@badge-info=subscriber/8;badges=subscriber/6;color=#0D4200;display-name=dallas;emote-sets=0,33,50,237,793,2126,3517,4578,5569,9400,10337,12239;turbo=0;user-id=12345678;user-type=admin :tmi.twitch.tv GLOBALUSERSTATE");

  TMIParser::MessageBundle GlobalUserStateBundle(EIRCCommand::GLOBALUSERSTATE, "GLOBALUSERSTATE", {
    "badge-info=subscriber/8",
    "badges=subscriber/6",
    "color=#0D4200",
    "display-name=dallas",
    "emote-sets=0,33,50,237,793,2126,3517,4578,5569,9400,10337,12239",
    "turbo=0",
    "user-id=12345678",
    "user-type=admin"
    }, "tmi.twitch.tv", "", "");

  FTWGlobalUserStateTags GlobalUserStateTags({ {"subscriber", 6} }, { {"subscriber", 8} }, HexToLinearColor("0D", "42", "00"), "dallas",
    { "0", "33", "50", "237", "793", "2126", "3517", "4578", "5569", "9400", "10337", "12239" }, false, "12345678", ETWUserType::Admin);

  FString RawClearMsgString = TEXT("@login=foo;room-id=;target-msg-id=94e6c7ff-bf98-4faa-af5d-7ad633a158a9;tmi-sent-ts=1642720582342 :tmi.twitch.tv CLEARMSG #bar :what a great day");

  TMIParser::MessageBundle ClearMsgBundle(EIRCCommand::CLEARMSG, "CLEARMSG", {
    "login=foo",
    "room-id=",
    "target-msg-id=94e6c7ff-bf98-4faa-af5d-7ad633a158a9",
    "tmi-sent-ts=1642720582342"
    }, "tmi.twitch.tv", "bar", "what a great day");

  FTWClearMsgTags ClearMsgTags("foo", "94e6c7ff-bf98-4faa-af5d-7ad633a158a9", 1642720582342);

  FString RawUserStateString = TEXT("@badge-info=;badges=staff/1;color=#0D4200;display-name=ronni;emote-sets=0,33,50,237,793,2126,3517,4578,5569,9400,10337,12239;mod=1;subscriber=1;turbo=1;user-type=staff :tmi.twitch.tv USERSTATE #dallas");

  TMIParser::MessageBundle UserStateBundle(EIRCCommand::USERSTATE, "USERSTATE", {
    "badge-info=",
    "badges=staff/1",
    "color=#0D4200",
    "display-name=ronni",
    "emote-sets=0,33,50,237,793,2126,3517,4578,5569,9400,10337,12239",
    "mod=1",
    "subscriber=1",
    "turbo=1",
    "user-type=staff"
    }, "tmi.twitch.tv", "dallas", "");

  FTWUserStateTags UserStateTags(
    { {"staff", 1} },
    {},
    HexToLinearColor("0D", "42", "00"),
    "ronni",
    {"0", "33", "50", "237", "793", "2126", "3517", "4578", "5569", "9400", "10337", "12239"},
    "", true, true, true, ETWUserType::Staff);

  FString RawUserNoticeString = TEXT("@badge-info=;badges=staff/1,broadcaster/1,turbo/1;color=#008000;display-name=ronni;emotes=;id=db25007f-7a18-43eb-9379-80131e44d633;login=ronni;mod=0;msg-id=resub;msg-param-cumulative-months=6;msg-param-streak-months=2;msg-param-should-share-streak=1;msg-param-sub-plan=Prime;msg-param-sub-plan-name=Prime;room-id=12345678;subscriber=1;system-msg=ronni\\shas\\ssubscribed\\sfor\\s6\\smonths!;tmi-sent-ts=1507246572675;turbo=1;user-id=87654321;user-type=staff :tmi.twitch.tv USERNOTICE #dallas :Great stream -- keep it up!");
  
  TMIParser::MessageBundle UserNoticeBundle(EIRCCommand::USERNOTICE, "USERNOTICE",
    {
      "badge-info=",
      "badges=staff/1,broadcaster/1,turbo/1",
      "color=#008000",
      "display-name=ronni",
      "emotes=",
      "id=db25007f-7a18-43eb-9379-80131e44d633",
      "login=ronni",
      "mod=0",
      "msg-id=resub",
      "msg-param-cumulative-months=6",
      "msg-param-streak-months=2",
      "msg-param-should-share-streak=1",
      "msg-param-sub-plan=Prime",
      "msg-param-sub-plan-name=Prime",
      "room-id=12345678",
      "subscriber=1",
      "system-msg=ronni\\shas\\ssubscribed\\sfor\\s6\\smonths!",
      "tmi-sent-ts=1507246572675",
      "turbo=1",
      "user-id=87654321",
      "user-type=staff"
    }, "tmi.twitch.tv", "dallas", "Great stream -- keep it up!");

  FTWUserNoticeTags UserNoticeTags(
    {
      {"staff", 1},
      {"broadcaster", 1},
      {"turbo", 1}
    },
    {},
    HexToLinearColor("00", "80", "00"),
    "ronni",
    {},
    "db25007f-7a18-43eb-9379-80131e44d633",
    "ronni",
    ETWUserNoticeMsgId::Resubscription,
    "ronni has subscribed for 6 months!",
    true,
    false,
    true,
    "87654321",
    ETWUserType::Staff,
    1507246572675,
    FUserNoticeMessageParams(6,
      "",
      "",
      0,
      0,
      "",
      "",
      "",
      "",
      "",
      "",
      true,
      2,
      TwitchSubscriptionPlan::Prime,
      "Prime",
      0,
      "",
      0,
      0,
      0,
      ETWGoalContributionType::Unknown, 0, 0, 0, "", false, "", "", "", ""
    ));

  FString RawNoticeString = TEXT("@msg-id=delete_message_success :tmi.twitch.tv NOTICE #bar :The message from foo is now deleted.");

  TMIParser::MessageBundle NoticeBundle(EIRCCommand::NOTICE, "NOTICE", {
    "msg-id=delete_message_success"
    }, "tmi.twitch.tv", "bar", "The message from foo is now deleted.");

  FTWNoticeTags NoticeTags("delete_message_success", "");

  PrivMsgTestMessages.Empty();
  ClearChatTestMessages.Empty();
  ClearMsgTestMessages.Empty();
  RoomStateTestMessages.Empty();
  WhisperTestMessages.Empty();
  GlobalUserStateTestMessages.Empty();
  UserStateTestMessages.Empty();
  NoticeTestMessages.Empty();
  UserNoticeTestMessages.Empty();

  PrivMsgTestMessages.Emplace(ExpectedTestData<FPrivMsgMessage>(
    RawPrivMsgString,
    PrivMsgBundle,
    FPrivMsgMessage(PrivMsgBundle, true, PrivMsgTags)
  ));

  ClearChatTestMessages.Emplace(ExpectedTestData<FClearChatMessage>(
    RawClearChatString,
    ClearChatBundle,
    FClearChatMessage(ClearChatBundle, true, ClearChatTags)
  ));

  ClearMsgTestMessages.Emplace(ExpectedTestData<FClearMsgMessage>(
    RawClearMsgString,
    ClearMsgBundle,
    FClearMsgMessage(ClearMsgBundle, true, ClearMsgTags)
  ));

  RoomStateTestMessages.Emplace(ExpectedTestData<FRoomStateMessage>(
    RawRoomStateString,
    RoomStateBundle,
    FRoomStateMessage(RoomStateBundle, true, RoomStateTags)
  ));

  WhisperTestMessages.Emplace(ExpectedTestData<FWhisperMessage>(
    RawWhisperString,
    WhisperBundle,
    FWhisperMessage(WhisperBundle, true, WhisperTags)
  ));
  
  GlobalUserStateTestMessages.Emplace(ExpectedTestData<FGlobalUserStateMessage>(
    RawGlobalUserString,
    GlobalUserStateBundle,
    FGlobalUserStateMessage(GlobalUserStateBundle, true, GlobalUserStateTags)
  ));

  UserStateTestMessages.Emplace(ExpectedTestData<FUserStateMessage>(
    RawUserStateString,
    UserStateBundle,
    FUserStateMessage(UserStateBundle, true, UserStateTags)
  ));

  NoticeTestMessages.Emplace(ExpectedTestData<FNoticeMessage>(
    RawNoticeString,
    NoticeBundle,
    FNoticeMessage(NoticeBundle, true, NoticeTags)
  ));

  UserNoticeTestMessages.Emplace(ExpectedTestData<FUserNoticeMessage>(
    RawUserNoticeString,
    UserNoticeBundle,
    FUserNoticeMessage(UserNoticeBundle, true, UserNoticeTags)
  ));
}

void TMIParserSpec::Define()
{
  static const int32 FuzzTestCount = 1;
  static const int32 FuzzMinStringLen = 20;
  static const int32 FuzzMaxStringLen = 1024;

  LoadTestMessages();

  Describe("Parsing", [this]()
  {
    Describe("Bad Inputs", [this]()
    {
      It("should parse an empty string into an unknown command bundle", [this]()
      {
        TMIParser::MessageBundle Bundle = TMIParser::SplitRawMessage("");
        TestEqual("Valid Bundle", Bundle.Command, EIRCCommand::UNKNOWN);
      });

      It("should parse a single word into an unknown command bundle", [this]()
      {
        TMIParser::MessageBundle Bundle = TMIParser::SplitRawMessage("testing");
        TestEqual("Valid Bundle", Bundle.Command, EIRCCommand::UNKNOWN);
      });

      It("should parse a single real command word into a real command bundle", [this]()
      {
        TMIParser::MessageBundle Bundle = TMIParser::SplitRawMessage("PRIVMSG");
        TestEqual("Valid Bundle", Bundle.Command, EIRCCommand::PRIVMSG);
      });
      
      It("should parse randoms strings into an unknown command bundle", [this]()
      {
        TMIParser::MessageBundle Bundle = TMIParser::SplitRawMessage("@This is just a test :o!f a whole bunc!h of wor@ds with :punctu@ation");
        TestEqual("Valid Bundle", Bundle.Command, EIRCCommand::UNKNOWN);
        
        Bundle = TMIParser::SplitRawMessage("17\\yv 81y8!1gxd1 827gvh1l;84r1g/h290 784gh_v0nv13-2d4sx1v @451234");
        TestEqual("Valid Bundle", Bundle.Command, EIRCCommand::UNKNOWN);

        Bundle = TMIParser::SplitRawMessage("\n\r\r\n");
        TestEqual("Valid Bundle", Bundle.Command, EIRCCommand::UNKNOWN);

        Bundle = TMIParser::SplitRawMessage("@This is just a test :o!f a whole bunc!h of wor@ds with :punctu@ation");
        TestEqual("Valid Bundle", Bundle.Command, EIRCCommand::UNKNOWN);
      });

      Describe("Fuzzing", [this]()
      {
        for (int32 Index = 0; Index < FuzzTestCount; ++Index)
        {
          It(FString::Printf(TEXT("should pass test [%d / %d] of random input strings"), Index + 1, FuzzTestCount), [this]()
          {
          });
        }
      });
    });

    Describe("CLEARCHAT", [this]()
    {
      for (int Index = 0; Index < ClearChatTestMessages.Num(); ++Index)
      {
        Describe(FString::Printf(TEXT("Input [%d]"), Index), [this, Index]()
        {
          BeforeEach([this, Index]()
          {
            ExpectedClearChat = ClearChatTestMessages[Index];
            ParsedBundle = TMIParser::SplitRawMessage(ExpectedClearChat.RawInput);
            ParsedClearChat = TMIParser::ParseMessage<FClearChatMessage>(ParsedBundle);
          });

          It("should parse into a valid message structure", [this]()
          {
              BundleTests(ParsedBundle, ExpectedClearChat.Bundle);

              TestEqual("Channel", ParsedClearChat.Channel, ExpectedClearChat.Message.Channel);
              TestEqual("User", ParsedClearChat.User, ExpectedClearChat.Message.User);
              TestEqual("Tags Valid", ParsedClearChat.bTagsValid, ExpectedClearChat.Message.bTagsValid);

              if (ExpectedClearChat.Message.bTagsValid)
              {
                TagTests<FTWClearChatTags, EIRCCommand::CLEARCHAT>(ParsedClearChat.Tags, ExpectedClearChat.Message.Tags);
              }
          });
        });
      }
    }); // End Describe CLEARCHAT

    Describe("CLEARMSG", [this]()
    {
      for (int Index = 0; Index < ClearMsgTestMessages.Num(); ++Index)
      {
        Describe(FString::Printf(TEXT("Input [%d]"), Index), [this, Index]()
        {
          BeforeEach([this, Index]()
          {
            ExpectedClearMsg = ClearMsgTestMessages[Index];
            ParsedBundle = TMIParser::SplitRawMessage(ExpectedClearMsg.RawInput);
            ParsedClearMsg = TMIParser::ParseMessage<FClearMsgMessage>(ParsedBundle);
          });

          It("should parse into a valid message structure", [this]()
          {
            BundleTests(ParsedBundle, ExpectedClearMsg.Bundle);
            TestEqual("Channel", ParsedClearMsg.Channel, ExpectedClearMsg.Message.Channel);
            TestEqual("Message", ParsedClearMsg.Message, ExpectedClearMsg.Message.Message);

            TestEqual("Tags Valid", ParsedClearMsg.bTagsValid, ExpectedClearMsg.Message.bTagsValid);

            if (ExpectedClearMsg.Message.bTagsValid)
            {
              TagTests<FTWClearMsgTags, EIRCCommand::CLEARMSG>
                (ParsedClearMsg.Tags, ExpectedClearMsg.Message.Tags);
            }
          });
        });
      }
    }); // End Describe CLEARMSG

    Describe("RECONNECT", [this]()
    {
      It("should correctly parse out a reconnect command into a message bundle", [this]()
      {
        TMIParser::MessageBundle ParsedBundle = TMIParser::SplitRawMessage(":tmi.twitch.tv RECONNECT");

        TestEqual("Command", ParsedBundle.Command, EIRCCommand::RECONNECT);
        TestEqual("RawCommand", ParsedBundle.RawCommand, TEXT("RECONNECT"));
        TestTrue("No Tags", ParsedBundle.Tags.IsEmpty());
        TestEqual("Source", ParsedBundle.Source, "tmi.twitch.tv");
        TestTrue("No Target", ParsedBundle.Target.IsEmpty());
        TestTrue("No Params", ParsedBundle.Params.IsEmpty());
      });
    }); // End Describe RECONNECT

    Describe("ROOMSTATE", [this]()
    {
      for (int Index = 0; Index < RoomStateTestMessages.Num(); ++Index)
      {
        Describe(FString::Printf(TEXT("Input [%d]"), Index), [this, Index]()
        {
          BeforeEach([this, Index]() {
            ExpectedRoomState = RoomStateTestMessages[Index];
            ParsedBundle = TMIParser::SplitRawMessage(ExpectedRoomState.RawInput);
            ParsedRoomState = TMIParser::ParseMessage<FRoomStateMessage>(ParsedBundle);
          });

          It("should correctly parse out a message", [this]()
          {
            BundleTests(ParsedBundle, ExpectedRoomState.Bundle);

            TestEqual("Channel", ParsedRoomState.Channel, ExpectedRoomState.Message.Channel);
            TestEqual("Tags Valid", ParsedRoomState.bTagsValid, ExpectedRoomState.Message.bTagsValid);

            if (ExpectedRoomState.Message.bTagsValid)
            {
              TagTests<FTWRoomStateTags, EIRCCommand::ROOMSTATE>(ParsedRoomState.Tags, ExpectedRoomState.Message.Tags);
            }
          });
        });
      }
    }); // End Describe ROOMSTATE

    Describe("HOSTTARGET", [this]()
    {
      It("should correctly parse out a now hosting target message bundle", [this]()
      {
        TMIParser::MessageBundle ParsedBundle = TMIParser::SplitRawMessage(":tmi.twitch.tv HOSTTARGET #abc :xyz 10");

        TestEqual("Command", ParsedBundle.Command, EIRCCommand::HOSTTARGET);
        TestEqual("RawCommand", ParsedBundle.RawCommand, TEXT("HOSTTARGET"));
        TestTrue("No Tags", ParsedBundle.Tags.IsEmpty());
        TestEqual("Source", ParsedBundle.Source, TEXT("tmi.twitch.tv"));
        TestEqual("Target", ParsedBundle.Target, TEXT("abc"));
        TestEqual("Params", ParsedBundle.Params, TEXT("xyz 10"));
      }); 

      It("should correctly parse out a stopped hosting message bundle", [this]()
      {
        TMIParser::MessageBundle ParsedBundle = TMIParser::SplitRawMessage(":tmi.twitch.tv HOSTTARGET #rawr :- 10");

        TestEqual("Command", ParsedBundle.Command, EIRCCommand::HOSTTARGET);
        TestEqual("RawCommand", ParsedBundle.RawCommand, TEXT("HOSTTARGET"));
        TestTrue("No Tags", ParsedBundle.Tags.IsEmpty());
        TestEqual("Source", ParsedBundle.Source, TEXT("tmi.twitch.tv"));
        TestEqual("Target", ParsedBundle.Target, TEXT("rawr"));
        TestEqual("Params", ParsedBundle.Params, TEXT("- 10"));
      });
    }); // End Describe HOSTTARGET

    Describe("PING", [this]()
    {
      It("should correctly parse out a ping command into a message bundle", [this]()
      {
        TMIParser::MessageBundle ParsedBundle = TMIParser::SplitRawMessage("PING :tmi.twitch.tv");

        TestEqual("Command", ParsedBundle.Command, EIRCCommand::PING);
        TestEqual("RawCommand", ParsedBundle.RawCommand, TEXT("PING"));
        TestTrue("No Tags", ParsedBundle.Tags.IsEmpty());
        TestTrue("No Source", ParsedBundle.Source.IsEmpty());
        TestTrue("No Target", ParsedBundle.Target.IsEmpty());
        TestEqual("Params", ParsedBundle.Params, "tmi.twitch.tv");
      });
    }); // End Describe PING

    Describe("PRIVMSG", [this]()
    {
      for (int32 Index = 0; Index < PrivMsgTestMessages.Num(); ++Index)
      {
        Describe(FString::Printf(TEXT("Input [%d]"), Index), [this, Index]()
        {
          BeforeEach([this, Index]()
          {
            ExpectedPrivMsg = PrivMsgTestMessages[Index];
            ParsedBundle = TMIParser::SplitRawMessage(ExpectedPrivMsg.RawInput);
            ParsedPrivMsg = TMIParser::ParseMessage<FPrivMsgMessage>(ParsedBundle);
          });

          It(TEXT("should parse into a valid message structure"), [this]()
          {
            BundleTests(ParsedBundle, ExpectedPrivMsg.Bundle);

            TestEqual("Mesage", ParsedPrivMsg.Message, ExpectedPrivMsg.Message.Message);
            TestEqual("FromUser", ParsedPrivMsg.FromUser, ExpectedPrivMsg.Message.FromUser);
            TestEqual("channel", ParsedPrivMsg.Channel, ExpectedPrivMsg.Message.Channel);
            TestEqual("Valid Tags", ParsedPrivMsg.bTagsValid, ExpectedPrivMsg.Message.bTagsValid);

            if (ExpectedPrivMsg.Message.bTagsValid)
            {
              TagTests<FTWPrivMsgTags, EIRCCommand::PRIVMSG>(ParsedPrivMsg.Tags, ExpectedPrivMsg.Message.Tags);
            }
          }); // It should parse a private msg
        }); // End Describe Parsing Test Input i
      } // End For Loop
    }); // End Describe PRIVMSG

    Describe("WHISPER", [this]()
    {
      for (int32 Index = 0; Index < WhisperTestMessages.Num(); ++Index)
      {
        Describe(FString::Printf(TEXT("Input [%d]"), Index), [this, Index]()
        {
          BeforeEach([this, Index]()
          {
            ExpectedWhisper = WhisperTestMessages[Index];
            ParsedBundle = TMIParser::SplitRawMessage(ExpectedWhisper.RawInput);
            ParsedWhisper = TMIParser::ParseMessage<FWhisperMessage>(ParsedBundle);
          });

          It(TEXT("should parse into a valid message structure"), [this]()
          {
            BundleTests(ParsedBundle, ExpectedWhisper.Bundle);

            TestEqual("Mesage", ParsedWhisper.Message, ExpectedWhisper.Message.Message);
            TestEqual("FromUser", ParsedWhisper.FromUser, ExpectedWhisper.Message.FromUser);
            TestEqual("Valid Tags", ParsedWhisper.bTagsValid, ExpectedWhisper.Message.bTagsValid);

            if (ExpectedWhisper.Message.bTagsValid)
            {
              TagTests<FTWWhisperTags, EIRCCommand::WHISPER>(ParsedWhisper.Tags, ExpectedWhisper.Message.Tags);
            }
          }); // It should parse a private msg
        }); // End Describe Parsing Test Input i
      } // End For Loop
    }); // End Describe WHISPER


    Describe("USERNOTICE", [this]()
    {
      for (int32 Index = 0; Index < UserNoticeTestMessages.Num(); ++Index)
      {
        Describe(FString::Printf(TEXT("Input [%d]"), Index), [this, Index]()
        {
          BeforeEach([this, Index]()
          {
            ExpectedUserNotice = UserNoticeTestMessages[Index];
            ParsedBundle = TMIParser::SplitRawMessage(ExpectedUserNotice.RawInput);
            ParsedUserNotice = TMIParser::ParseMessage<FUserNoticeMessage>(ParsedBundle);
          });

          It(TEXT("should parse into a valid message structure"), [this]()
          {
            BundleTests(ParsedBundle, ExpectedUserNotice.Bundle);

            TestEqual("Channel", ParsedUserNotice.Channel, ExpectedUserNotice.Message.Channel);
            TestEqual("Mesage", ParsedUserNotice.Message, ExpectedUserNotice.Message.Message);
            TestEqual("Valid Tags", ParsedUserNotice.bTagsValid, ExpectedUserNotice.Message.bTagsValid);

            if (ExpectedUserNotice.Message.bTagsValid)
            {
              TagTests<FTWUserNoticeTags, EIRCCommand::USERNOTICE>(ParsedUserNotice.Tags, ExpectedUserNotice.Message.Tags);
            }
          }); // It should parse a private msg
        }); // End Describe Parsing Test Input i
      } // End For Loop
    }); // End Describe USERNOTICE

    Describe("USERSTATE", [this]()
    {
      for (int32 Index = 0; Index < UserStateTestMessages.Num(); ++Index)
      {
        Describe(FString::Printf(TEXT("Input [%d]"), Index), [this, Index]()
        {
          BeforeEach([this, Index]()
          {
            ExpectedUserState = UserStateTestMessages[Index];
            ParsedBundle = TMIParser::SplitRawMessage(ExpectedUserState.RawInput);
            ParsedUserState = TMIParser::ParseMessage<FUserStateMessage>(ParsedBundle);
          });

          It(TEXT("should parse into a valid message structure"), [this]()
          {
            BundleTests(ParsedBundle, ExpectedUserState.Bundle);

            TestEqual("Channel", ParsedUserState.Channel, ExpectedUserState.Message.Channel);
            TestEqual("Valid Tags", ParsedUserState.bTagsValid, ExpectedUserState.Message.bTagsValid);

            if (ExpectedUserState.Message.bTagsValid)
            {
              TagTests<FTWUserStateTags, EIRCCommand::USERSTATE>(ParsedUserState.Tags, ExpectedUserState.Message.Tags);
            }
          }); // It should parse a private msg
        }); // End Describe Parsing Test Input i
      } // End For Loop
    }); // End Describe USERSTATE

    Describe("GLOBALUSERSTATE", [this]()
    {
      for (int32 Index = 0; Index < GlobalUserStateTestMessages.Num(); ++Index)
      {
        Describe(FString::Printf(TEXT("Input [%d]"), Index), [this, Index]()
        {
          BeforeEach([this, Index]()
          {
            ExpectedGlobalUserState = GlobalUserStateTestMessages[Index];
            ParsedBundle = TMIParser::SplitRawMessage(ExpectedGlobalUserState.RawInput);
            ParsedGlobalUserState = TMIParser::ParseMessage<FGlobalUserStateMessage>(ParsedBundle);
          });

          It(TEXT("should parse into a valid message structure"), [this]()
          {
            BundleTests(ParsedBundle, ExpectedGlobalUserState.Bundle);

            TestEqual("Valid Tags", ParsedGlobalUserState.bTagsValid, ExpectedGlobalUserState.Message.bTagsValid);

            if (ExpectedGlobalUserState.Message.bTagsValid)
            {
              TagTests<FTWGlobalUserStateTags, EIRCCommand::GLOBALUSERSTATE>(ParsedGlobalUserState.Tags, ExpectedGlobalUserState.Message.Tags);
            }
          }); // It should parse a private msg
        }); // End Describe Parsing Test Input i
      } // End For Loop
    }); // End Describe GLOBALUSERSTATE

    Describe("NOTICE", [this]()
    {
      for (int32 Index = 0; Index < GlobalUserStateTestMessages.Num(); ++Index)
      {
        Describe(FString::Printf(TEXT("Input [%d]"), Index), [this, Index]()
        {
          BeforeEach([this, Index]()
          {
            ExpectedNotice = NoticeTestMessages[Index];
            ParsedBundle = TMIParser::SplitRawMessage(ExpectedNotice.RawInput);
            ParsedNotice = TMIParser::ParseMessage<FNoticeMessage>(ParsedBundle);
          });

          It(TEXT("should parse into a valid message structure"), [this]()
          {
            BundleTests(ParsedBundle, ExpectedNotice.Bundle);

            TestEqual("Channel", ParsedNotice.Channel, ExpectedNotice.Message.Channel);
            TestEqual("Message", ParsedNotice.Message, ExpectedNotice.Message.Message);
            TestEqual("Valid Tags", ParsedNotice.bTagsValid, ExpectedNotice.Message.bTagsValid);

            if (ExpectedNotice.Message.bTagsValid)
            {
              TagTests<FTWNoticeTags, EIRCCommand::NOTICE>(ParsedNotice.Tags, ExpectedNotice.Message.Tags);
            }
          }); // It should parse a private msg
        }); // End Describe Parsing Test Input i
      } // End For Loop
    }); // End Describe Notice
  }); // End Describe Parsing
}
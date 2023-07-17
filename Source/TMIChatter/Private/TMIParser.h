#pragma once

#include "CoreMinimal.h"

#include "TMIParser.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTwitchParser, Log, All);

#define PARSER_LOG(Lvl, Format, ...) UE_LOG(LogTwitchParser, Lvl, Format, __VA_ARGS__)

UENUM()
enum class EIRCCommand : uint8 {
	/* IRC Messages*/
	NOTICE,
	JOIN,
	PART,
	PING,
	PRIVMSG,

	/* TMI Messages*/
	CLEARCHAT,					// Sent when the bot or moderator removes all messages from the chat room or removes all messages for the specified user.
	CLEARMSG,						// Sent when the bot removes a single message from the chat room.
	GLOBALUSERSTATE,		// Sent after the bot authenticates with the server.
	HOSTTARGET,					// Sent when a channel starts or stops hosting viewers from another channel.
	RECONNECT,					// Sent when twitch is requesting we reconnect, as the server will be forcefully disconnecting us soon
	ROOMSTATE,					// Sent when the bot joins a channel or when the channel’s chat settings change.
	USERNOTICE,					// Sent when events like someone subscribing to the channel occurs.
	USERSTATE,					// Sent when the bot joins a channel or sends a PRIVMSG message.
	WHISPER,						// Sent when a user sends a WHISPER message to this bot. Requires a verified bot permission from Twitch themselves for this to work.
	CAP,								// Capability requests

	/* Our custom messages */
	AUTHFAILED,					// Custom command for us to signify that login has failed as this is passed in a NOTICE message normally
	UNKNOWN
};

FString GetIRCCommandString(EIRCCommand Command);

/* TMI Tags https://dev.twitch.tv/docs/irc/tags */
UENUM()
enum class ETwitchTagType : uint8 {
	INVALID,
	BanDuration,
	BadgeInfo,
	Badges,
	NameColor,
	DisplayName,
	Bits,
	Emotes,
	EmoteSets,
	Mod,
	RoomId,
	Subscriber,
	Turbo,
	UserId,
	UserType,
	TMISentTS,
	TargetUserID,
	TargetMsgID,
	Login,
	ID,
	MsgID,
	SystemMessage,
	ReplyParentMsgID,
	ReplyParentUserID,
	ReplyParentUserLogin,
	ReplyParentDisplayName,
	ReplyParentMsgBody,
	MsgParamCumulativeMonths,
	MsgParamDisplayName,
	MsgParamLogin,
	MsgParamMonths,
	MsgParamGiftTotal,
	MsgParamRecipientDisplayName,
	MsgParamRecipientID,
	MsgParamRecipientUserName,
	MsgParamSenderLogin,
	MsgParamSenderName,
	MsgParamShouldShareStreak,
	MsgParamStreakMonths,
	MsgParamSubPlan,
	MsgParamSubPlanName,
	MsgParamViewerCount,
	MsgParamRitualName,
	MsgParamThreadhold,
	MsgParamGiftMonths,
	MsgParamMultimonthDuration,
	MsgParamMultimonthTenure,
	MsgParamWasGifted,
	MsgParamMassGiftCount,
	MsgParamGoalContributionType,
	MsgParamGoalCurrentContributions,
	MsgParamGoalTargetContributions,
	MsgParamGoalUserContributions,
	MsgParamColor,
	MsgParamProfileImageURL,
	MsgParamPriorGifterAnon,
	MsgParamPriorGifterDisplayName,
	MsgParamPriorGifterID,
	MsgParamPriorGifterUsername,
	EmoteOnly,
	FollowersOnly,
	R9K,
	Rituals,
	Slow,
	SubsOnly,
	ThreadID,
	MessageID,
	FirstMsg,
	ReturningChatter,
	VIP,
	CustomRewardID
};

UENUM(BlueprintType)
enum class ETWUserNoticeMsgId : uint8
{
	None,
	Subscription,								// User has subscribed for the first time
	Resubscription,							// User has resubscribed
	SubscriptionGift,						// User was gifted a subscription from another user
	SubscriptionMysterGift,
	GiftPaidUpgrade,						// A user continued their gift subcription
	GiftReward,									// ???
	AnonymousGiftPaidUpgrade,		// 
	Raid,												// This channel has been raided
	Ritual,
	BitsBadgeTier,
	Announcement,
	CommunityPayForward,
	StandardPayForward
};

UENUM(BlueprintType)
enum class ETWUserType : uint8
{
	Normal,
	Mod,
	Admin,
	GlobalMod,
	Staff,
	Unknown
};

UENUM(BlueprintType)
enum class ETWGoalContributionType : uint8
{
	Subs,
	SubPoints,
	Unknown,
};


USTRUCT(BlueprintType)
struct FTWEmotePositions
{
	GENERATED_USTRUCT_BODY();

	FTWEmotePositions(int32 Start = -1, int32 End = -1) : Start(Start), End(End)
	{
	}

	~FTWEmotePositions() {}

	FTWEmotePositions(const FTWEmotePositions& RHS)
		: Start(RHS.Start), End(RHS.End) {}

	FTWEmotePositions& operator=(const FTWEmotePositions& RHS)
	{
		Start = RHS.Start;
		End = RHS.End;
		return *this;
	}

	bool operator==(const FTWEmotePositions& RHS)
	{
		return Start == RHS.Start && End == RHS.End;
	}

	bool operator==(const TPair<int32, int32>& RHS)
	{
		return Start == RHS.Key && End == RHS.Value;
	}

	UPROPERTY(BlueprintReadOnly)
		int32 Start;

	UPROPERTY(BlueprintReadOnly)
		int32 End;
};

USTRUCT(BlueprintType)
struct FTWEmoteData
{
	GENERATED_USTRUCT_BODY();

	FTWEmoteData() {}
	~FTWEmoteData() {}

	FTWEmoteData(const TArray<FTWEmotePositions>& EmotePositions)
		: EmotePositions(EmotePositions)
	{
	}

	FTWEmoteData& operator=(const FTWEmoteData& RHS)
	{
		EmotePositions = RHS.EmotePositions;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		TArray<FTWEmotePositions> EmotePositions;
};

UENUM(BlueprintType)
enum class TwitchSubscriptionPlan : uint8 {
	None,
	Prime,
	Tier1,
	Tier2,
	Tier3
};

USTRUCT(BlueprintType)
struct FUserNoticeMessageParams
{
	GENERATED_USTRUCT_BODY();

	FUserNoticeMessageParams() {}
	FUserNoticeMessageParams(int32 CumulativeMonths,
		const FString& DisplayName,
		const FString& Login,
		int32 Months,
		int32 PromoGiftTotal,
		const FString& PromoName,
		const FString& RecipientDisplayName,
		const FString& RecipientID,
		const FString& RecipientUsername,
		const FString& SenderLogin,
		const FString& SenderName,
		bool ShouldShareStreak,
		int32 StreakMonths,
		TwitchSubscriptionPlan SubPlan,
		const FString& SubPlanName,
		int32 ViewerCount,
		const FString& RitualName,
		int32 Threshold,
		int32 GiftMonths,
		int32 MassGiftCount,
		ETWGoalContributionType GoalType,
		int32 GoalCurrentContributions,
		int32 GoalTargetContributions,
		int32 GoalUserContributions,
		const FString& Color,
		bool PriorGifterIsAnon,
		const FString& PriorGifterDisplayName,
		const FString& PriorGifterID,
		const FString& PriorGifterUsername,
		const FString& ProfileImageURL
		)
		: CumulativeMonths(CumulativeMonths), DisplayName(DisplayName), Login(Login), Months(Months), PromoGiftTotal(PromoGiftTotal), PromoName(PromoName),
		RecipientDisplayName(RecipientDisplayName), RecipientID(RecipientID), RecipientUsername(RecipientUsername), SenderLogin(SenderLogin), SenderName(SenderName), 
		ShouldShareStreak(ShouldShareStreak), StreakMonths(StreakMonths), SubPlan(SubPlan), SubPlanName(SubPlanName), ViewerCount(ViewerCount), RitualName(RitualName), 
		Threshold(Threshold), GiftMonths(GiftMonths), MassGiftCount(MassGiftCount), Color(Color), ProfileImageURL(ProfileImageURL),
		bPriorGifterIsAnon(PriorGifterIsAnon),PriorGifterDisplayName(PriorGifterDisplayName), PriorGifterID(PriorGifterID), PriorGifterUsername(PriorGifterUsername),
		GoalType(GoalType), GoalCurrentContributions(GoalCurrentContributions), GoalTargetContributions(GoalTargetContributions), GoalUserContributions(GoalUserContributions)
	{}

	UPROPERTY(BlueprintReadOnly)
		int32 CumulativeMonths = 0;

	UPROPERTY(BlueprintReadOnly)
		FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
		FString Login;

	UPROPERTY(BlueprintReadOnly)
		int32 Months = 0;

	UPROPERTY(BlueprintReadOnly)
		int32 PromoGiftTotal = 0;

	UPROPERTY(BlueprintReadOnly)
		FString PromoName;

	UPROPERTY(BlueprintReadOnly)
		FString RecipientDisplayName;

	UPROPERTY(BlueprintReadOnly)
		FString RecipientID;

	UPROPERTY(BlueprintReadOnly)
		FString RecipientUsername;

	UPROPERTY(BlueprintReadOnly)
		FString SenderLogin;

	UPROPERTY(BlueprintReadOnly)
		FString SenderName;

	UPROPERTY(BlueprintReadOnly)
		bool ShouldShareStreak = false;

	UPROPERTY(BlueprintReadOnly)
		int32 StreakMonths= 0;

	UPROPERTY(BlueprintReadOnly)
		TwitchSubscriptionPlan SubPlan = TwitchSubscriptionPlan::None;

	UPROPERTY(BlueprintReadOnly)
		FString SubPlanName;

	UPROPERTY(BlueprintReadOnly)
		int32 ViewerCount = 0;

	UPROPERTY(BlueprintReadOnly)
		FString RitualName;

	UPROPERTY(BlueprintReadOnly)
		int32 Threshold = 0;

	UPROPERTY(BlueprintReadOnly)
		int32 GiftMonths = 0;

	UPROPERTY(BlueprintReadOnly)
		int32 MassGiftCount = 0;

	UPROPERTY(BlueprintReadOnly)
		ETWGoalContributionType GoalType = ETWGoalContributionType::Unknown;

	UPROPERTY(BlueprintReadOnly)
		int32 GoalCurrentContributions = 0;

	UPROPERTY(BlueprintReadOnly)
		int32 GoalTargetContributions = 0;

	UPROPERTY(BlueprintReadOnly)
		int32 GoalUserContributions = 0;

	UPROPERTY(BlueprintReadOnly)
		FString Color;

	UPROPERTY(BlueprintReadOnly)
		bool bPriorGifterIsAnon = false;

	UPROPERTY(BlueprintReadOnly)
		FString PriorGifterDisplayName;

	UPROPERTY(BlueprintReadOnly)
		FString PriorGifterID;

	UPROPERTY(BlueprintReadOnly)
		FString PriorGifterUsername;

	UPROPERTY(BlueprintReadOnly)
		FString ProfileImageURL;
};

USTRUCT(BlueprintType)
struct FSubscriptionNoticeTags
{
	GENERATED_USTRUCT_BODY();

	FSubscriptionNoticeTags() {}
	FSubscriptionNoticeTags(const FSubscriptionNoticeTags& RHS)
		: CumulativeMonths(RHS.CumulativeMonths), ShouldShareStreak(RHS.ShouldShareStreak), StreakMonths(RHS.StreakMonths),
		SubPlan(RHS.SubPlan), SubPlanName(RHS.SubPlanName) {}

	FSubscriptionNoticeTags(const FUserNoticeMessageParams& RHS)
		: CumulativeMonths(RHS.CumulativeMonths), ShouldShareStreak(RHS.ShouldShareStreak), StreakMonths(RHS.StreakMonths),
		SubPlan(RHS.SubPlan), SubPlanName(RHS.SubPlanName) {}

	FSubscriptionNoticeTags& operator=(const FSubscriptionNoticeTags& RHS)
	{
		CumulativeMonths = RHS.CumulativeMonths;
		ShouldShareStreak = RHS.ShouldShareStreak;
		StreakMonths = RHS.StreakMonths;
		SubPlan = RHS.SubPlan;
		SubPlanName = RHS.SubPlanName;
		return *this;
	}

	FSubscriptionNoticeTags& operator=(const FUserNoticeMessageParams& RHS)
	{
		CumulativeMonths = RHS.CumulativeMonths;
		ShouldShareStreak = RHS.ShouldShareStreak;
		StreakMonths = RHS.StreakMonths;
		SubPlan = RHS.SubPlan;
		SubPlanName = RHS.SubPlanName;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
	int32 CumulativeMonths;

	UPROPERTY(BlueprintReadOnly)
	bool ShouldShareStreak;

	UPROPERTY(BlueprintReadOnly)
	int32 StreakMonths;

	UPROPERTY(BlueprintReadOnly)
	TwitchSubscriptionPlan SubPlan;

	UPROPERTY(BlueprintReadOnly)
	FString SubPlanName;
};

USTRUCT(BlueprintType)
struct FRaidNoticeTags
{
	GENERATED_USTRUCT_BODY();

	FRaidNoticeTags() {}
	FRaidNoticeTags(const FRaidNoticeTags& RHS)
		: DisplayName(RHS.DisplayName), Login(RHS.Login), ViewerCount(RHS.ViewerCount), ProfileImageURL(RHS.ProfileImageURL) {}

	FRaidNoticeTags(const FUserNoticeMessageParams& RHS)
		: DisplayName(RHS.DisplayName), Login(RHS.Login), ViewerCount(RHS.ViewerCount), ProfileImageURL(RHS.ProfileImageURL) {}

	FRaidNoticeTags& operator=(const FRaidNoticeTags& RHS)
	{
		DisplayName = RHS.DisplayName;
		Login = RHS.Login;
		ViewerCount = RHS.ViewerCount;
		ProfileImageURL = RHS.ProfileImageURL;
		return *this;
	}

	FRaidNoticeTags& operator=(const FUserNoticeMessageParams& RHS)
	{
		DisplayName = RHS.DisplayName;
		Login = RHS.Login;
		ViewerCount = RHS.ViewerCount;
		ProfileImageURL = RHS.ProfileImageURL;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
		FString Login;

	UPROPERTY(BlueprintReadOnly)
		int32 ViewerCount;

	UPROPERTY(BlueprintReadOnly)
		FString ProfileImageURL;
};

USTRUCT(BlueprintType)
struct FSubgiftNoticeTags
{
	GENERATED_USTRUCT_BODY();

	FSubgiftNoticeTags() {}
	FSubgiftNoticeTags(const FSubgiftNoticeTags& RHS)
		: Months(RHS.Months), RecipientDisplayName(RHS.RecipientDisplayName), RecipientID(RHS.RecipientID), RecipientUsername(RHS.RecipientUsername),
		SubPlan(RHS.SubPlan), SubPlanName(RHS.SubPlanName), GiftMonths(RHS.GiftMonths) {}
	
	FSubgiftNoticeTags(const FUserNoticeMessageParams& RHS)
		: Months(RHS.Months), RecipientDisplayName(RHS.RecipientDisplayName), RecipientID(RHS.RecipientID), RecipientUsername(RHS.RecipientUsername),
		SubPlan(RHS.SubPlan), SubPlanName(RHS.SubPlanName), GiftMonths(RHS.GiftMonths) {}
	
	FSubgiftNoticeTags& operator=(const FSubgiftNoticeTags& RHS)
	{
		Months = RHS.Months;
		RecipientDisplayName = RHS.RecipientDisplayName;
		RecipientID = RHS.RecipientID;
		RecipientUsername = RHS.RecipientUsername;
		SubPlan = RHS.SubPlan;
		SubPlanName = RHS.SubPlanName;
		GiftMonths = RHS.GiftMonths;
		return *this;
	}
	
	FSubgiftNoticeTags& operator=(const FUserNoticeMessageParams& RHS)
	{
		Months = RHS.Months;
		RecipientDisplayName = RHS.RecipientDisplayName;
		RecipientID = RHS.RecipientID;
		RecipientUsername = RHS.RecipientUsername;
		SubPlan = RHS.SubPlan;
		SubPlanName = RHS.SubPlanName;
		GiftMonths = RHS.GiftMonths;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
	int32 Months;

	UPROPERTY(BlueprintReadOnly)
	FString RecipientDisplayName;

	UPROPERTY(BlueprintReadOnly)
	FString RecipientID;

	UPROPERTY(BlueprintReadOnly)
	FString RecipientUsername;

	UPROPERTY(BlueprintReadOnly)
	TwitchSubscriptionPlan SubPlan;

	UPROPERTY(BlueprintReadOnly)
	FString SubPlanName;

	UPROPERTY(BlueprintReadOnly)
	int32 GiftMonths;
};

USTRUCT(BlueprintType)
struct FGiftPaidUpgrade
{
	GENERATED_USTRUCT_BODY();

	FGiftPaidUpgrade() {}
	FGiftPaidUpgrade(const FGiftPaidUpgrade& RHS)
		: PromoGiftTotal(RHS.PromoGiftTotal), PromoName(RHS.PromoName), SenderLogin(RHS.SenderLogin), SenderName(RHS.SenderName) {}

	FGiftPaidUpgrade(const FUserNoticeMessageParams& RHS)
		: PromoGiftTotal(RHS.PromoGiftTotal), PromoName(RHS.PromoName), SenderLogin(RHS.SenderLogin), SenderName(RHS.SenderName) {}

	FGiftPaidUpgrade& operator=(const FGiftPaidUpgrade& RHS)
	{
		PromoGiftTotal = RHS.PromoGiftTotal;
		PromoName = RHS.PromoName;
		SenderLogin = RHS.SenderLogin;
		SenderName = RHS.SenderName;
		return *this;
	}
	FGiftPaidUpgrade& operator=(const FUserNoticeMessageParams& RHS)
	{
		PromoGiftTotal = RHS.PromoGiftTotal;
		PromoName = RHS.PromoName;
		SenderLogin = RHS.SenderLogin;
		SenderName = RHS.SenderName;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		int32 PromoGiftTotal;

	UPROPERTY(BlueprintReadOnly)
		FString PromoName;

	UPROPERTY(BlueprintReadOnly)
		FString SenderLogin;

	UPROPERTY(BlueprintReadOnly)
		FString SenderName;
};

USTRUCT(BlueprintType)
struct FSubPaidForwardNoticeTags
{
	GENERATED_USTRUCT_BODY();

	FSubPaidForwardNoticeTags() {}
	FSubPaidForwardNoticeTags(const FSubPaidForwardNoticeTags& RHS)
		: bPriorGifterIsAnon(RHS.bPriorGifterIsAnon), PriorGifterID(RHS.PriorGifterID), PriorGifterDisplayName(RHS.PriorGifterDisplayName), PriorGifterUsername(RHS.PriorGifterUsername)
	{}

	FSubPaidForwardNoticeTags(const FUserNoticeMessageParams& RHS)
		: bPriorGifterIsAnon(RHS.bPriorGifterIsAnon), PriorGifterID(RHS.PriorGifterID), PriorGifterDisplayName(RHS.PriorGifterDisplayName), PriorGifterUsername(RHS.PriorGifterUsername)
	{}

	FSubPaidForwardNoticeTags& operator=(const FSubPaidForwardNoticeTags& RHS)
	{ 
		bPriorGifterIsAnon = RHS.bPriorGifterIsAnon;
		PriorGifterID = RHS.PriorGifterID;
		PriorGifterDisplayName = RHS.PriorGifterDisplayName;
		PriorGifterUsername = RHS.PriorGifterUsername;
		return *this;
	}

	FSubPaidForwardNoticeTags& operator=(const FUserNoticeMessageParams& RHS)
	{
		bPriorGifterIsAnon = RHS.bPriorGifterIsAnon;
		PriorGifterID = RHS.PriorGifterID;
		PriorGifterDisplayName = RHS.PriorGifterDisplayName;
		PriorGifterUsername = RHS.PriorGifterUsername;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		bool bPriorGifterIsAnon;

	UPROPERTY(BlueprintReadOnly)
		FString PriorGifterID;

	UPROPERTY(BlueprintReadOnly)
		FString PriorGifterDisplayName;

	UPROPERTY(BlueprintReadOnly)
		FString PriorGifterUsername;
};

USTRUCT(BlueprintType)
struct FRitualNoticeTags
{
	GENERATED_USTRUCT_BODY();

	FRitualNoticeTags() {}
	FRitualNoticeTags(const FRitualNoticeTags& RHS) : RitualName(RHS.RitualName) {}
	FRitualNoticeTags(const FUserNoticeMessageParams& RHS) : RitualName(RHS.RitualName) {}
	FRitualNoticeTags& operator=(const FRitualNoticeTags& RHS) { RitualName = RHS.RitualName; return *this; }
	FRitualNoticeTags& operator=(const FUserNoticeMessageParams& RHS) { RitualName = RHS.RitualName; return *this; }

	UPROPERTY(BlueprintReadOnly)
		FString RitualName;
};

USTRUCT(BlueprintType)
struct FBitsBadgeTierNoticeTags
{
	GENERATED_USTRUCT_BODY();

	FBitsBadgeTierNoticeTags() : Threshold(0) {}
	FBitsBadgeTierNoticeTags(const FBitsBadgeTierNoticeTags& RHS) : Threshold(RHS.Threshold) {}
	FBitsBadgeTierNoticeTags(const FUserNoticeMessageParams& RHS) : Threshold(RHS.Threshold) {}
	FBitsBadgeTierNoticeTags& operator=(const FBitsBadgeTierNoticeTags& RHS) { Threshold = RHS.Threshold; return *this; }
	FBitsBadgeTierNoticeTags& operator=(const FUserNoticeMessageParams& RHS) { Threshold = RHS.Threshold; return *this; }

	UPROPERTY(BlueprintReadOnly)
		int32 Threshold;
};

// This master struct is here to create 1 parser for every possible tag used by twitch
// Each specific message tag type has a constructor that takes this struct as a parameter
struct TwitchTagsMaster
{
	TwitchTagsMaster() {}
	~TwitchTagsMaster() {}

	TwitchTagsMaster(const TwitchTagsMaster& RHS)
		: Bits(RHS.Bits), BanDuration(RHS.BanDuration), TargetUserID(RHS.TargetUserID), TargetMsgID(RHS.TargetMsgID),
		Badges(RHS.Badges), BadgesInfo(RHS.BadgesInfo), NameColor(RHS.NameColor), DisplayName(RHS.DisplayName), Emotes(RHS.Emotes),
		MessageID(RHS.MessageID), Turbo(RHS.Turbo), Mod(RHS.Mod), Subscriber(RHS.Subscriber), UserID(RHS.UserID), UserType(RHS.UserType),
		EmoteSets(RHS.EmoteSets), Login(RHS.Login), SystemMessage(RHS.SystemMessage), ReturningChatter(RHS.ReturningChatter),
		TMISentTS(RHS.TMISentTS), UserNoticeMsgID(RHS.UserNoticeMsgID), NoticeMsgID(RHS.NoticeMsgID), MsgID(RHS.MsgID), FirstMsg(RHS.FirstMsg),
		EmoteOnly(RHS.EmoteOnly), FollowersOnlyMinMinutes(RHS.FollowersOnlyMinMinutes),
		R9K(RHS.R9K), Rituals(RHS.Rituals), SlowMode(RHS.SlowMode), SubscribersOnly(RHS.SubscribersOnly), VIP(RHS.VIP),
		MessageParams(RHS.MessageParams), CustomRewardID(RHS.CustomRewardID), ReplyParentMsgID(RHS.ReplyParentMsgID), ReplyParentUserID(RHS.ReplyParentUserID),
		ReplyParentUserLogin(RHS.ReplyParentUserLogin), ReplyParentDisplayName(RHS.ReplyParentDisplayName), ReplyParentMsgBody(RHS.ReplyParentMsgBody)
	{
	}

	TwitchTagsMaster& operator=(const TwitchTagsMaster& RHS)
	{
		UserNoticeMsgID = RHS.UserNoticeMsgID;
		NoticeMsgID = RHS.NoticeMsgID;
		MsgID = RHS.MsgID;
		Bits = RHS.Bits;
		BanDuration = RHS.BanDuration;
		TargetUserID = RHS.TargetUserID;
		TargetMsgID = RHS.TargetMsgID;
		BadgesInfo = RHS.BadgesInfo;
		Badges = RHS.Badges;
		NameColor = RHS.NameColor;
		DisplayName = RHS.DisplayName;
		Emotes = RHS.Emotes;
		MessageID = RHS.MessageID;
		Turbo = RHS.Turbo;
		Mod = RHS.Mod;
		Subscriber = RHS.Subscriber;
		UserID = RHS.UserID;
		UserType = RHS.UserType;
		EmoteSets = RHS.EmoteSets;
		Login = RHS.Login;
		SystemMessage = RHS.SystemMessage;
		TMISentTS = RHS.TMISentTS;
		EmoteOnly = RHS.EmoteOnly;
		FollowersOnlyMinMinutes = RHS.FollowersOnlyMinMinutes;
		R9K = RHS.R9K;
		Rituals = RHS.Rituals;
		SlowMode = RHS.SlowMode;
		SubscribersOnly = RHS.SubscribersOnly;
		FirstMsg = RHS.FirstMsg;
		ReturningChatter = RHS.ReturningChatter;
		VIP = RHS.VIP;
		MessageParams = RHS.MessageParams;
		CustomRewardID = RHS.CustomRewardID;
	}

	ETWUserNoticeMsgId UserNoticeMsgID = ETWUserNoticeMsgId::None;
	FString NoticeMsgID;
	FString MsgID;
	bool FirstMsg = false;
	bool ReturningChatter = false;
	int32 Bits = 0;
	int32 BanDuration = 0;
	FString TargetUserID;
	FString TargetMsgID;
	TMap<FString, int32> BadgesInfo;
	TMap<FString, int32> Badges;
	FLinearColor NameColor;
	FString DisplayName;
	TMap<FString, FTWEmoteData> Emotes;
	FString MessageID;
	FGuid ID;
	bool Turbo = false;
	bool Mod = false;
	bool Subscriber = false;
	FString UserID;
	ETWUserType UserType = ETWUserType::Normal;
	TArray<FString> EmoteSets;		// The Emote ID is the key, with a tuple of (StartPos:EndPos) in terms of indices in the command parameter string
	FString Login;											// Login name of user who generated this notice
	FString SystemMessage;
	int64 TMISentTS = 0;			// Unix Timestamp
	bool EmoteOnly = false;
	int32 FollowersOnlyMinMinutes = -1;	// if == -1 It is not in followers only mode
	bool R9K = false;
	bool SlowMode = false;
	bool SubscribersOnly = false;
	bool Rituals = false;
	bool VIP = false;
	FString ThreadID;
	FUserNoticeMessageParams MessageParams;
	FGuid CustomRewardID;

	FGuid ReplyParentMsgID;
	FString ReplyParentUserID;
	FString ReplyParentUserLogin;
	FString ReplyParentDisplayName;
	FString ReplyParentMsgBody;
};

class TMIParser {
public:
	struct MessageBundle
	{
		MessageBundle() : Command(EIRCCommand::UNKNOWN) {}
		~MessageBundle() {}

		MessageBundle(EIRCCommand Command, const FString& RawCommand, const TArray<FString>& Tags, const FString& Source, const FString& Target, const FString& Params)
			: Command(Command), RawCommand(RawCommand), Tags(Tags), Source(Source), Target(Target), Params(Params)
		{}

		MessageBundle(const MessageBundle& RHS)
			: Command(RHS.Command), RawCommand(RHS.RawCommand), Tags(RHS.Tags), Source(RHS.Source), Target(RHS.Target), Params(RHS.Params)
		{}

		MessageBundle& operator=(const MessageBundle& RHS)
		{
			Command = RHS.Command;
			RawCommand = RHS.RawCommand;
			Tags = RHS.Tags;
			Source = RHS.Source;
			Target = RHS.Target;
			Params = RHS.Params;
			return *this;
		}

		EIRCCommand Command;
		FString RawCommand;
		TArray<FString> Tags;
		FString Source;
		FString Target;
		FString Params;
	};

	static MessageBundle SplitRawMessage(const FString& RawMessage);

	template<typename MsgType>
	static MsgType ParseMessage(MessageBundle& InMessage);

private:
	static bool ParseTags(EIRCCommand ParsingCommand, const TArray<FString>& InTags, TwitchTagsMaster& OutTags);
	static ETWUserNoticeMsgId ParseUserNoticeMsgID(const FString& MsgID);
	static void ParseBadges(FString& BadgeStr, TMap<FString, int32>& OutBadges);
	static void ParseEmotes(FString& Emotestr, TMap<FString, FTWEmoteData>& OutEmotes);
	static FString ParseSource(const FString& InStr);
	static EIRCCommand ParseCommand(const FString& CommandStr);
	static TwitchSubscriptionPlan ParseSubPlan(const FString& PlanID);
	static ETWUserType ParseUserType(const FString& UserType);
	static ETWGoalContributionType ParseUserNoticeGoalType(const FString& Goal);

	TMIParser() = delete;
	TMIParser(const TMIParser& RHS) = delete;
};

USTRUCT(blueprintable)
struct FTWClearChatTags
{
	GENERATED_USTRUCT_BODY();

	FTWClearChatTags() {}
	~FTWClearChatTags() {}

	FTWClearChatTags(const FString& TargetUserID, int32 BanDuration = 0, int64 TMISentTS = 0)
		: BanDuration(BanDuration), TargetUserID(TargetUserID), TMISentTS(TMISentTS) {}

	explicit FTWClearChatTags(const FTWClearChatTags& RHS)
		: BanDuration(RHS.BanDuration), TargetUserID(RHS.TargetUserID), TMISentTS(RHS.TMISentTS) {}

	explicit FTWClearChatTags(const TwitchTagsMaster& RHS)
		: BanDuration(RHS.BanDuration), TargetUserID(RHS.TargetUserID), TMISentTS(RHS.TMISentTS) {}

	UPROPERTY(BlueprintReadOnly)
		int32 BanDuration;

	UPROPERTY(BlueprintReadOnly)
		FString TargetUserID;

	UPROPERTY(BlueprintReadOnly)
		int64 TMISentTS;			// The UNIX timestamp.
};

USTRUCT(blueprintable)
struct FTWClearMsgTags
{
	GENERATED_USTRUCT_BODY();

	FTWClearMsgTags() {}
	~FTWClearMsgTags() {}

	FTWClearMsgTags(const FString& Login, const FString& TargetMsgID, int64 TMISentTS = 0)
		: Login(Login), TargetMsgID(TargetMsgID), TMISentTS(TMISentTS) {}

	explicit FTWClearMsgTags(const FTWClearMsgTags& RHS)
		: Login(RHS.Login), TargetMsgID(RHS.TargetMsgID), TMISentTS(RHS.TMISentTS) {}

	explicit FTWClearMsgTags(const TwitchTagsMaster& RHS)
		: Login(RHS.Login), TargetMsgID(RHS.TargetMsgID), TMISentTS(RHS.TMISentTS) {}

	UPROPERTY(BlueprintReadOnly)
		FString Login;				// The name of the user who sent the message.

	UPROPERTY(BlueprintReadOnly)
		FString TargetMsgID;	// A UUID that identifies the message that was removed.

	UPROPERTY(BlueprintReadOnly)
		int64 TMISentTS;		// The UNIX timestamp.
};

USTRUCT(blueprintable)
struct FTWGlobalUserStateTags
{
	GENERATED_USTRUCT_BODY();

	FTWGlobalUserStateTags() {}
	~FTWGlobalUserStateTags() {}

	FTWGlobalUserStateTags(
		const TMap<FString, int32>& Badges,
		const TMap<FString, int32>& BadgesInfo,
		const FLinearColor& NameColor,
		const FString& DisplayName,
		const TArray<FString>& EmoteSets,
		bool bTurbo,
		const FString& UserID,
		ETWUserType UserType
	) : Badges(Badges), BadgesInfo(BadgesInfo), NameColor(NameColor), DisplayName(DisplayName), EmoteSets(EmoteSets), Turbo(bTurbo), UserID(UserID), UserType(UserType)
	{}

	explicit FTWGlobalUserStateTags(const FTWGlobalUserStateTags& RHS)
		: Badges(RHS.Badges),
		BadgesInfo(RHS.BadgesInfo),
		NameColor(RHS.NameColor),
		DisplayName(RHS.DisplayName),
		EmoteSets(RHS.EmoteSets),
		Turbo(RHS.Turbo),
		UserID(RHS.UserID),
		UserType(RHS.UserType)
	{}

	explicit FTWGlobalUserStateTags(const TwitchTagsMaster& RHS)
		: Badges(RHS.Badges),
		BadgesInfo(RHS.BadgesInfo),
		NameColor(RHS.NameColor),
		DisplayName(RHS.DisplayName),
		EmoteSets(RHS.EmoteSets),
		Turbo(RHS.Turbo),
		UserID(RHS.UserID),
		UserType(RHS.UserType)
	{}

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, int32> Badges;

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, int32> BadgesInfo;

	UPROPERTY(BlueprintReadOnly)
		FLinearColor NameColor;

	UPROPERTY(BlueprintReadOnly)
		FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
		TArray<FString> EmoteSets;

	UPROPERTY(BlueprintReadOnly)
		bool Turbo = false;

	UPROPERTY(BlueprintReadOnly)
		FString UserID;

	UPROPERTY(BlueprintReadOnly)
		ETWUserType UserType = ETWUserType::Normal;


};

USTRUCT(Blueprintable)
struct FTWNoticeTags
{
	GENERATED_USTRUCT_BODY();

	FTWNoticeTags() {}
	~FTWNoticeTags() {}

	FTWNoticeTags(const FString& MsgID, const FString& TargetUserID)
		: MsgID(MsgID), TargetUserID(TargetUserID) {}

	explicit FTWNoticeTags(const FTWNoticeTags& RHS)
		: MsgID(RHS.MsgID), TargetUserID(RHS.TargetUserID) {}

	explicit FTWNoticeTags(const TwitchTagsMaster& RHS)
		: MsgID(RHS.NoticeMsgID), TargetUserID(RHS.TargetUserID) {}

	UPROPERTY(BlueprintReadOnly)
		FString MsgID;

	UPROPERTY(BlueprintReadOnly)
		FString TargetUserID;
};

USTRUCT(Blueprintable)
struct FTWPrivMsgTags
{
	GENERATED_USTRUCT_BODY();

	FTWPrivMsgTags() {}
	~FTWPrivMsgTags() {}

	FTWPrivMsgTags(const TMap<FString, int32>& Badges,
		const TMap<FString, int32>& BadgesInfo,
		int32 Bits,
		const FLinearColor& NameColor,
		const FString& DisplayName,
		const TMap<FString, FTWEmoteData>& Emotes,
		const FString& ID,
		bool Turbo,
		bool Mod,
		bool Subscriber,
		bool VIP,
		bool FirstMsg,
		bool ReturningChatter,
		const FString& ReplyParentMsgID,
		const FString& ReplyParentUserID,
		const FString& ReplyParentUserLogin,
		const FString& ReplyParentDisplayName,
		const FString& ReplyParentMsgBody,
		const FString& CustomRewardID,
		const FString& UserID,
		ETWUserType UserType,
		int64 TMISentTS
	) : Badges(Badges), BadgesInfo(BadgesInfo), Bits(Bits), NameColor(NameColor), DisplayName(DisplayName), Emotes(Emotes), ID(ID),
		Turbo(Turbo), Mod(Mod), Subscriber(Subscriber), VIP(VIP), FirstMsg(FirstMsg), ReturningChatter(ReturningChatter),
		UserID(UserID), UserType(UserType), TMISentTS(TMISentTS), CustomRewardID(CustomRewardID), ReplyParentMsgID(ReplyParentMsgID), ReplyParentUserID(ReplyParentUserID),
		ReplyParentUserLogin(ReplyParentUserLogin), ReplyParentDisplayName(ReplyParentDisplayName), ReplyParentMsgBody(ReplyParentMsgBody)
	{}

	explicit FTWPrivMsgTags(const FTWPrivMsgTags& RHS)
		: Badges(RHS.Badges),
		BadgesInfo(RHS.BadgesInfo),
		Bits(RHS.Bits),
		NameColor(RHS.NameColor),
		DisplayName(RHS.DisplayName),
		Emotes(RHS.Emotes),
		Turbo(RHS.Turbo),
		Mod(RHS.Mod),
		VIP(RHS.VIP),
		ID(RHS.ID),
		Subscriber(RHS.Subscriber),
		UserID(RHS.UserID),
		UserType(RHS.UserType),
		TMISentTS(RHS.TMISentTS),
		FirstMsg(RHS.FirstMsg),
		ReturningChatter(RHS.ReturningChatter),
		CustomRewardID(RHS.CustomRewardID), ReplyParentMsgID(RHS.ReplyParentMsgID), ReplyParentUserID(RHS.ReplyParentUserID),
		ReplyParentUserLogin(RHS.ReplyParentUserLogin), ReplyParentDisplayName(RHS.ReplyParentDisplayName), ReplyParentMsgBody(RHS.ReplyParentMsgBody)
	{}

	explicit FTWPrivMsgTags(const TwitchTagsMaster& RHS)
		: Badges(RHS.Badges),
		BadgesInfo(RHS.BadgesInfo),
		Bits(RHS.Bits),
		NameColor(RHS.NameColor),
		DisplayName(RHS.DisplayName),
		Emotes(RHS.Emotes),
		Turbo(RHS.Turbo),
		Mod(RHS.Mod),
		VIP(RHS.VIP),
		ID(RHS.ID),
		Subscriber(RHS.Subscriber),
		UserID(RHS.UserID),
		UserType(RHS.UserType),
		TMISentTS(RHS.TMISentTS),
		FirstMsg(RHS.FirstMsg),
		ReturningChatter(RHS.ReturningChatter),
		CustomRewardID(RHS.CustomRewardID), ReplyParentMsgID(RHS.ReplyParentMsgID), ReplyParentUserID(RHS.ReplyParentUserID),
		ReplyParentUserLogin(RHS.ReplyParentUserLogin), ReplyParentDisplayName(RHS.ReplyParentDisplayName), ReplyParentMsgBody(RHS.ReplyParentMsgBody)
	{}

	FTWPrivMsgTags& operator=(const FTWPrivMsgTags& RHS)
	{
		BadgesInfo = RHS.BadgesInfo;
		Badges = RHS.Badges;
		Bits = RHS.Bits;
		NameColor = RHS.NameColor;
		DisplayName = RHS.DisplayName;
		Emotes = RHS.Emotes;
		Turbo = RHS.Turbo;
		Mod = RHS.Mod;
		VIP = RHS.VIP;
		ID = RHS.ID;
		Subscriber = RHS.Subscriber;
		UserID = RHS.UserID;
		UserType = RHS.UserType;
		TMISentTS = RHS.TMISentTS;
		FirstMsg = RHS.FirstMsg;
		ReturningChatter = RHS.ReturningChatter;
		CustomRewardID = RHS.CustomRewardID;
		ReplyParentMsgID = RHS.ReplyParentMsgID;
		ReplyParentUserID = RHS.ReplyParentUserID;
		ReplyParentUserLogin = RHS.ReplyParentUserLogin;
		ReplyParentDisplayName = RHS.ReplyParentDisplayName;
		ReplyParentMsgBody = RHS.ReplyParentMsgBody;
		return *this;
	}

	FTWPrivMsgTags& operator=(const TwitchTagsMaster& RHS)
	{
		BadgesInfo = RHS.BadgesInfo;
		Badges = RHS.Badges;
		Bits = RHS.Bits;
		NameColor = RHS.NameColor;
		DisplayName = RHS.DisplayName;
		Emotes = RHS.Emotes;
		Turbo = RHS.Turbo;
		Mod = RHS.Mod;
		ID = RHS.ID;
		Subscriber = RHS.Subscriber;
		VIP = RHS.VIP;
		UserID = RHS.UserID;
		UserType = RHS.UserType;
		TMISentTS = RHS.TMISentTS;
		FirstMsg = RHS.FirstMsg;
		ReturningChatter = RHS.ReturningChatter;
		CustomRewardID = RHS.CustomRewardID;
		ReplyParentMsgID = RHS.ReplyParentMsgID;
		ReplyParentUserID = RHS.ReplyParentUserID;
		ReplyParentUserLogin = RHS.ReplyParentUserLogin;
		ReplyParentDisplayName = RHS.ReplyParentDisplayName;
		ReplyParentMsgBody = RHS.ReplyParentMsgBody;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, int32> Badges;

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, int32> BadgesInfo;

	UPROPERTY(BlueprintReadOnly)
		int32 Bits = 0;

	UPROPERTY(BlueprintReadOnly)
		FLinearColor NameColor;

	UPROPERTY(BlueprintReadOnly)
		FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, FTWEmoteData> Emotes;		// The Emote ID is the key, with a tuple of (StartPos:EndPos) in terms of indices in the command parameter string

	UPROPERTY(BlueprintReadOnly)
		FGuid ID;

	UPROPERTY(BlueprintReadOnly)
		bool Turbo = false;

	UPROPERTY(BlueprintReadOnly)
		bool Mod = false;

	UPROPERTY(BlueprintReadOnly)
		bool Subscriber = false;

	UPROPERTY(BlueprintReadOnly)
		bool VIP = false;

	UPROPERTY(BlueprintReadOnly)
		bool FirstMsg = false;

	UPROPERTY(BlueprintReadOnly)
		bool ReturningChatter = false;

	UPROPERTY(BlueprintReadOnly)
		FString UserID;

	UPROPERTY(BlueprintReadOnly)
		ETWUserType UserType = ETWUserType::Normal;

	UPROPERTY(BlueprintReadOnly)
		int64 TMISentTS = 0;					// Unix Timestamp of message

	UPROPERTY(BlueprintReadOnly)
		FGuid CustomRewardID;					// GUID of a redeemed channel reward, only useful in an API call

	UPROPERTY(BlueprintReadOnly)
		FGuid ReplyParentMsgID;

	UPROPERTY(BlueprintReadOnly)
		FString ReplyParentUserID;

	UPROPERTY(BlueprintReadOnly)
		FString ReplyParentUserLogin;

	UPROPERTY(BlueprintReadOnly)
		FString ReplyParentDisplayName;

	UPROPERTY(BlueprintReadOnly)
		FString ReplyParentMsgBody;
};

USTRUCT(Blueprintable)
struct FTWRoomStateTags
{
	GENERATED_USTRUCT_BODY();

	FTWRoomStateTags() {}
	~FTWRoomStateTags() {}

	FTWRoomStateTags(bool EmoteOnly, int32 FollowersOnlyMinMinutes, bool R9K, bool Rituals, bool SlowMode, bool SubscribersOnly)
		: EmoteOnly(EmoteOnly),
		FollowersOnlyMinMinutes(FollowersOnlyMinMinutes),
		R9K(R9K),
		Rituals(Rituals),
		SlowMode(SlowMode),
		SubscribersOnly(SubscribersOnly)
	{}

	explicit FTWRoomStateTags(const FTWRoomStateTags& RHS)
		: EmoteOnly(RHS.EmoteOnly),
		FollowersOnlyMinMinutes(RHS.FollowersOnlyMinMinutes),
		R9K(RHS.R9K),
		Rituals(RHS.Rituals),
		SlowMode(RHS.SlowMode),
		SubscribersOnly(RHS.SubscribersOnly)
	{}

	explicit FTWRoomStateTags(const TwitchTagsMaster& RHS)
		: EmoteOnly(RHS.EmoteOnly),
		FollowersOnlyMinMinutes(RHS.FollowersOnlyMinMinutes),
		R9K(RHS.R9K),
		Rituals(RHS.Rituals),
		SlowMode(RHS.SlowMode),
		SubscribersOnly(RHS.SubscribersOnly)
	{}

	FTWRoomStateTags& operator=(const FTWRoomStateTags& RHS)
	{
		EmoteOnly = RHS.EmoteOnly;
		FollowersOnlyMinMinutes = RHS.FollowersOnlyMinMinutes;
		R9K = RHS.R9K;
		Rituals = RHS.Rituals;
		SlowMode = RHS.SlowMode;
		SubscribersOnly = RHS.SubscribersOnly;
		return *this;
	}

	FTWRoomStateTags& operator=(const TwitchTagsMaster& RHS)
	{
		EmoteOnly = RHS.EmoteOnly;
		FollowersOnlyMinMinutes = RHS.FollowersOnlyMinMinutes;
		R9K = RHS.R9K;
		Rituals = RHS.Rituals;
		SlowMode = RHS.SlowMode;
		SubscribersOnly = RHS.SubscribersOnly;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		bool EmoteOnly;

	UPROPERTY(BlueprintReadOnly)
		int32 FollowersOnlyMinMinutes;	// if == -1 It is not in followers only mode

	UPROPERTY(BlueprintReadOnly)
		bool R9K;

	UPROPERTY(BlueprintReadOnly)
		bool Rituals;

	UPROPERTY(BlueprintReadOnly)
		bool SlowMode;

	UPROPERTY(BlueprintReadOnly)
		bool SubscribersOnly;
};

USTRUCT(blueprintable)
struct FTWUserNoticeTags
{
	GENERATED_USTRUCT_BODY();

	FTWUserNoticeTags() {}
	~FTWUserNoticeTags() {}

	FTWUserNoticeTags(const TMap<FString, int32>& Badges,
		const TMap<FString, int32>& BadgesInfo,
		const FLinearColor& NameColor,
		const FString& DisplayName,
		const TMap<FString, FTWEmoteData>& Emotes,
		const FString& ID,
		const FString& Login,
		ETWUserNoticeMsgId MsgID,
		const FString& SystemMessage,
		bool Turbo,
		bool Mod,
		bool Subscriber,
		const FString& UserID,
		ETWUserType UserType,
		int64 TMISentTS,
		const FUserNoticeMessageParams& MessageParams)
		: Badges(Badges), BadgesInfo(BadgesInfo), NameColor(NameColor), DisplayName(DisplayName), Emotes(Emotes), ID(ID),
		Login(Login), MsgID(MsgID), SystemMessage(SystemMessage),
		Turbo(Turbo), Mod(Mod), Subscriber(Subscriber), UserID(UserID), UserType(UserType), TMISentTS(TMISentTS), MessageParams(MessageParams)
	{}

	explicit FTWUserNoticeTags(const FTWUserNoticeTags& RHS)
		: Badges(RHS.Badges),
		BadgesInfo(RHS.BadgesInfo),
		NameColor(RHS.NameColor),
		DisplayName(RHS.DisplayName),
		Emotes(RHS.Emotes),
		ID(RHS.ID),
		Login(RHS.Login),
		MsgID(RHS.MsgID),
		SystemMessage(RHS.SystemMessage),
		Turbo(RHS.Turbo),
		Mod(RHS.Mod),
		Subscriber(RHS.Subscriber),
		UserID(RHS.UserID),
		UserType(RHS.UserType),
		TMISentTS(RHS.TMISentTS),
		MessageParams(RHS.MessageParams)
	{}

	explicit FTWUserNoticeTags(const TwitchTagsMaster& RHS)
		: Badges(RHS.Badges),
		BadgesInfo(RHS.BadgesInfo),
		NameColor(RHS.NameColor),
		DisplayName(RHS.DisplayName),
		Emotes(RHS.Emotes),
		ID(RHS.ID),
		Login(RHS.Login),
		MsgID(RHS.UserNoticeMsgID),
		SystemMessage(RHS.SystemMessage),
		Turbo(RHS.Turbo),
		Mod(RHS.Mod),
		Subscriber(RHS.Subscriber),
		UserID(RHS.UserID),
		UserType(RHS.UserType),
		TMISentTS(RHS.TMISentTS),
		MessageParams(RHS.MessageParams)
	{}

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, int32> Badges;

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, int32> BadgesInfo;

	UPROPERTY(BlueprintReadOnly)
		FLinearColor NameColor;

	UPROPERTY(BlueprintReadOnly)
		FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, FTWEmoteData> Emotes;	// The Emote ID is the key, with a tuple of (StartPos:EndPos) in terms of indices in the command parameter string

	UPROPERTY(BlueprintReadOnly)
		FGuid ID;

	UPROPERTY(BlueprintReadOnly)
		FString Login;											// Login name of user who generated this notice

	UPROPERTY(BlueprintReadOnly)
		ETWUserNoticeMsgId MsgID;

	UPROPERTY(BlueprintReadOnly)
		FString SystemMessage;

	UPROPERTY(BlueprintReadOnly)
		bool Turbo = false;

	UPROPERTY(BlueprintReadOnly)
		bool Mod = false;

	UPROPERTY(BlueprintReadOnly)
		bool Subscriber = false;

	UPROPERTY(BlueprintReadOnly)
		FString UserID;

	UPROPERTY(BlueprintReadOnly)
		ETWUserType UserType = ETWUserType::Normal;

	UPROPERTY(BlueprintReadOnly)
		int64 TMISentTS = 0;			// Unix Timestamp

	UPROPERTY(BlueprintReadOnly)
		FUserNoticeMessageParams MessageParams;
};

USTRUCT(Blueprintable)
struct FTWUserStateTags
{
	GENERATED_USTRUCT_BODY();

	FTWUserStateTags() {}
	~FTWUserStateTags() {}

	FTWUserStateTags(const TMap<FString, int32>& Badges,
		const TMap<FString, int32>& BadgesInfo,
		const FLinearColor& NameColor,
		const FString& DisplayName,
		const TArray<FString>& EmoteSets,
		const FString& ID,
		bool Turbo,
		bool Mod,
		bool Subscriber,
		ETWUserType UserType
	) : Badges(Badges), BadgesInfo(BadgesInfo), NameColor(NameColor), DisplayName(DisplayName), EmoteSets(EmoteSets), ID(ID),
		Turbo(Turbo), Mod(Mod), Subscriber(Subscriber), UserType(UserType)
	{}

	explicit FTWUserStateTags(const FTWUserStateTags& RHS)
		: Badges(RHS.Badges),
		BadgesInfo(RHS.BadgesInfo),
		NameColor(RHS.NameColor),
		DisplayName(RHS.DisplayName),
		EmoteSets(RHS.EmoteSets),
		ID(RHS.ID),
		Turbo(RHS.Turbo),
		Mod(RHS.Mod),
		Subscriber(RHS.Subscriber),
		UserType(RHS.UserType)
	{}

	explicit FTWUserStateTags(const TwitchTagsMaster& RHS)
		: Badges(RHS.Badges),
		BadgesInfo(RHS.BadgesInfo),
		NameColor(RHS.NameColor),
		DisplayName(RHS.DisplayName),
		EmoteSets(RHS.EmoteSets),
		ID(RHS.ID),
		Turbo(RHS.Turbo),
		Mod(RHS.Mod),
		Subscriber(RHS.Subscriber),
		UserType(RHS.UserType)
	{}

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, int32> Badges;

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, int32> BadgesInfo;

	UPROPERTY(BlueprintReadOnly)
		FLinearColor NameColor;

	UPROPERTY(BlueprintReadOnly)
		FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
		TArray<FString> EmoteSets;		// The Emote ID is the key, with a tuple of (StartPos:EndPos) in terms of indices in the command parameter string

	UPROPERTY(BlueprintReadOnly)
		FGuid ID;

	UPROPERTY(BlueprintReadOnly)
		bool Turbo;

	UPROPERTY(BlueprintReadOnly)
		bool Mod;

	UPROPERTY(BlueprintReadOnly)
		bool Subscriber;

	UPROPERTY(BlueprintReadOnly)
		ETWUserType UserType = ETWUserType::Normal;
};

USTRUCT(blueprintable)
struct FTWWhisperTags
{
	GENERATED_USTRUCT_BODY();

	FTWWhisperTags() {}
	~FTWWhisperTags() {}

	FTWWhisperTags(const TMap<FString, int32>& Badges,
		const FLinearColor& NameColor,
		const FString& DisplayName,
		const TMap<FString, FTWEmoteData>& Emotes,
		const FString& MessageID,
		const FString& ThreadID,
		bool Turbo,
		const FString& UserID,
		ETWUserType UserType
	) : Badges(Badges), NameColor(NameColor), DisplayName(DisplayName), Emotes(Emotes), MessageID(MessageID), ThreadID(ThreadID),
		Turbo(Turbo), UserID(UserID), UserType(UserType)
	{}

	explicit FTWWhisperTags(const FTWWhisperTags& RHS)
		: Badges(RHS.Badges),
		NameColor(RHS.NameColor),
		DisplayName(RHS.DisplayName),
		Emotes(RHS.Emotes),
		MessageID(RHS.MessageID),
		ThreadID(RHS.ThreadID),
		Turbo(RHS.Turbo),
		UserID(RHS.UserID),
		UserType(RHS.UserType)
	{}

	explicit FTWWhisperTags(const TwitchTagsMaster& RHS)
		: Badges(RHS.Badges),
		NameColor(RHS.NameColor),
		DisplayName(RHS.DisplayName),
		Emotes(RHS.Emotes),
		MessageID(RHS.MessageID),
		ThreadID(RHS.ThreadID),
		Turbo(RHS.Turbo),
		UserID(RHS.UserID),
		UserType(RHS.UserType)
	{}

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, int32> Badges;

	UPROPERTY(BlueprintReadOnly)
		FLinearColor NameColor;

	UPROPERTY(BlueprintReadOnly)
		FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, FTWEmoteData> Emotes;		// The Emote ID is the key, with a tuple of (StartPos:EndPos) in terms of indices in the command parameter string

	UPROPERTY(BlueprintReadOnly)
		FString MessageID;

	UPROPERTY(BlueprintReadOnly)
		FString ThreadID;

	UPROPERTY(BlueprintReadOnly)
		bool Turbo;

	UPROPERTY(BlueprintReadOnly)
		FString UserID;

	UPROPERTY(BlueprintReadOnly)
		ETWUserType UserType;
};

USTRUCT(blueprintable)
struct FClearChatMessage
{
	GENERATED_USTRUCT_BODY();

	FClearChatMessage() : bTagsValid(false) {}
	~FClearChatMessage() {}

	explicit FClearChatMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const TwitchTagsMaster& Tags)
		: Channel(MsgBundle.Target), User(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	explicit FClearChatMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const FTWClearChatTags& Tags)
		: Channel(MsgBundle.Target), User(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	FClearChatMessage(const FClearChatMessage& RHS)
		: Channel(RHS.Channel), User(RHS.User), bTagsValid(RHS.bTagsValid), Tags(RHS.Tags)
	{}

	FClearChatMessage& operator=(const FClearChatMessage& RHS)
	{
		Channel = RHS.Channel;
		User = RHS.User;
		bTagsValid = RHS.bTagsValid;
		Tags = RHS.Tags;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		FString Channel;

	UPROPERTY(BlueprintReadOnly)
		FString User;

	UPROPERTY(BlueprintReadOnly)
		bool bTagsValid;

	UPROPERTY(BlueprintReadOnly)
		FTWClearChatTags Tags;
};

USTRUCT(blueprintable)
struct FClearMsgMessage
{
	GENERATED_USTRUCT_BODY();

	FClearMsgMessage() : bTagsValid(false) {}
	~FClearMsgMessage() {}

	explicit FClearMsgMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const FTWClearMsgTags& Tags)
		: Channel(MsgBundle.Target), Message(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	explicit FClearMsgMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const TwitchTagsMaster& Tags)
		: Channel(MsgBundle.Target), Message(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	FClearMsgMessage(const FClearMsgMessage& RHS)
		: Channel(RHS.Channel), Message(RHS.Message), bTagsValid(RHS.bTagsValid), Tags(RHS.Tags)
	{}

	FClearMsgMessage& operator=(const FClearMsgMessage& RHS)
	{
		Channel = RHS.Channel;
		Message = RHS.Message;
		bTagsValid = RHS.bTagsValid;
		Tags = RHS.Tags;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		FString Channel;

	UPROPERTY(BlueprintReadOnly)
		FString Message;

	UPROPERTY(BlueprintReadOnly)
		bool bTagsValid;

	UPROPERTY(BlueprintReadOnly)
		FTWClearMsgTags Tags;
};

USTRUCT(blueprintable)
struct FPrivMsgMessage
{
	GENERATED_USTRUCT_BODY();

	FPrivMsgMessage() : bTagsValid(false) {}
	~FPrivMsgMessage() {}

	explicit FPrivMsgMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const FTWPrivMsgTags& Tags)
		: Channel(MsgBundle.Target), FromUser(MsgBundle.Source), Message(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	explicit FPrivMsgMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const TwitchTagsMaster& Tags)
		: Channel(MsgBundle.Target), FromUser(MsgBundle.Source), Message(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	FPrivMsgMessage(const FPrivMsgMessage& RHS)
		: Channel(RHS.Channel), FromUser(RHS.FromUser), Message(RHS.Message), bTagsValid(RHS.bTagsValid), Tags(RHS.Tags)
	{}

	FPrivMsgMessage& operator=(const FPrivMsgMessage& RHS)
	{
		Channel = RHS.Channel;
		FromUser = RHS.FromUser;
		Message = RHS.Message;
		bTagsValid = RHS.bTagsValid;
		Tags = RHS.Tags;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		FString Channel;

	UPROPERTY(BlueprintReadOnly)
		FString FromUser;

	UPROPERTY(BlueprintReadOnly)
		FString Message;

	UPROPERTY(BlueprintReadOnly)
		bool bTagsValid;

	UPROPERTY(BlueprintReadOnly)
		FTWPrivMsgTags Tags;
};

USTRUCT(blueprintable)
struct FWhisperMessage
{
	GENERATED_USTRUCT_BODY();

	FWhisperMessage() : bTagsValid(false) {}
	~FWhisperMessage() {}

	explicit FWhisperMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const FTWWhisperTags& Tags)
		: FromUser(MsgBundle.Source), Message(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	explicit FWhisperMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const TwitchTagsMaster& Tags)
		: FromUser(MsgBundle.Source), Message(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	FWhisperMessage(const FWhisperMessage& RHS)
		: FromUser(RHS.FromUser), Message(RHS.Message), bTagsValid(RHS.bTagsValid), Tags(RHS.Tags)
	{}

	FWhisperMessage& operator=(const FWhisperMessage& RHS)
	{
		FromUser = RHS.FromUser;
		Message = RHS.Message;
		bTagsValid = RHS.bTagsValid;
		Tags = RHS.Tags;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		FString FromUser;

	UPROPERTY(BlueprintReadOnly)
		FString Message;

	UPROPERTY(BlueprintReadOnly)
		bool bTagsValid;

	UPROPERTY(BlueprintReadOnly)
		FTWWhisperTags Tags;
};

USTRUCT(Blueprintable)
struct FGlobalUserStateMessage
{
	GENERATED_USTRUCT_BODY();

	FGlobalUserStateMessage() : bTagsValid(false) {}
	~FGlobalUserStateMessage() {}

	explicit FGlobalUserStateMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const FTWGlobalUserStateTags& Tags)
		: bTagsValid(bTagsValid), Tags(Tags)
	{}

	explicit FGlobalUserStateMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const TwitchTagsMaster& Tags)
		: bTagsValid(bTagsValid), Tags(Tags)
	{}

	FGlobalUserStateMessage(const FGlobalUserStateMessage& RHS)
		: bTagsValid(RHS.bTagsValid), Tags(RHS.Tags)
	{}

	FGlobalUserStateMessage& operator=(const FGlobalUserStateMessage& RHS)
	{
		bTagsValid = RHS.bTagsValid;
		Tags = RHS.Tags;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		bool bTagsValid;

	UPROPERTY(BlueprintReadOnly)
		FTWGlobalUserStateTags Tags;
};

USTRUCT(Blueprintable)
struct FNoticeMessage
{
	GENERATED_USTRUCT_BODY();

	FNoticeMessage() : bTagsValid(false) {}
	~FNoticeMessage() {}

	explicit FNoticeMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const FTWNoticeTags& Tags)
		: Channel(MsgBundle.Target), Message(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	explicit FNoticeMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const TwitchTagsMaster& Tags)
		: Channel(MsgBundle.Target), Message(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	FNoticeMessage(const FNoticeMessage& RHS)
		: Channel(RHS.Channel), Message(RHS.Message), bTagsValid(RHS.bTagsValid), Tags(RHS.Tags)
	{}

	FNoticeMessage& operator=(const FNoticeMessage& RHS)
	{
		Channel = RHS.Channel;
		Message = RHS.Message;
		bTagsValid = RHS.bTagsValid;
		Tags = RHS.Tags;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		FString Channel;

	UPROPERTY(BlueprintReadOnly)
		FString Message;

	UPROPERTY(BlueprintReadOnly)
		bool bTagsValid;

	UPROPERTY(BlueprintReadOnly)
		FTWNoticeTags Tags;
};

USTRUCT(Blueprintable)
struct FUserNoticeMessage
{
	GENERATED_USTRUCT_BODY();

	FUserNoticeMessage() : bTagsValid(false) {}
	~FUserNoticeMessage() {}

	explicit FUserNoticeMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const FTWUserNoticeTags& Tags)
		: Channel(MsgBundle.Target), Message(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	explicit FUserNoticeMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const TwitchTagsMaster& Tags)
		: Channel(MsgBundle.Target), Message(MsgBundle.Params), bTagsValid(bTagsValid), Tags(Tags)
	{}

	FUserNoticeMessage(const FUserNoticeMessage& RHS)
		: Channel(RHS.Channel), Message(RHS.Message), bTagsValid(RHS.bTagsValid), Tags(RHS.Tags)
	{}

	FUserNoticeMessage& operator=(const FUserNoticeMessage& RHS)
	{
		Channel = RHS.Channel;
		Message = RHS.Message;
		bTagsValid = RHS.bTagsValid;
		Tags = RHS.Tags;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		FString Channel;

	UPROPERTY(BlueprintReadOnly)
		FString Message;

	UPROPERTY(BlueprintReadOnly)
		bool bTagsValid;

	UPROPERTY(BlueprintReadOnly)
		FTWUserNoticeTags Tags;
};

USTRUCT(Blueprintable)
struct FUserStateMessage
{
	GENERATED_USTRUCT_BODY();

	FUserStateMessage() : bTagsValid(false) {}
	~FUserStateMessage() {}

	explicit FUserStateMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const FTWUserStateTags& Tags)
		: Channel(MsgBundle.Target), bTagsValid(bTagsValid), Tags(Tags)
	{}

	explicit FUserStateMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const TwitchTagsMaster& Tags)
		: Channel(MsgBundle.Target), bTagsValid(bTagsValid), Tags(Tags)
	{}

	FUserStateMessage(const FUserStateMessage& RHS)
		: Channel(RHS.Channel), bTagsValid(RHS.bTagsValid), Tags(RHS.Tags)
	{}

	FUserStateMessage& operator=(const FUserStateMessage& RHS)
	{
		Channel = RHS.Channel;
		bTagsValid = RHS.bTagsValid;
		Tags = RHS.Tags;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		FString Channel;

	UPROPERTY(BlueprintReadOnly)
		bool bTagsValid;

	UPROPERTY(BlueprintReadOnly)
		FTWUserStateTags Tags;
};

USTRUCT(Blueprintable)
struct FRoomStateMessage
{
	GENERATED_USTRUCT_BODY();

	FRoomStateMessage() : bTagsValid(false) {}
	~FRoomStateMessage() {}

	explicit FRoomStateMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const FTWRoomStateTags& Tags)
		: Channel(MsgBundle.Target), bTagsValid(bTagsValid), Tags(Tags)
	{}

	explicit FRoomStateMessage(const TMIParser::MessageBundle& MsgBundle, bool bTagsValid, const TwitchTagsMaster& Tags)
		: Channel(MsgBundle.Target), bTagsValid(bTagsValid), Tags(Tags)
	{}

	FRoomStateMessage(const FRoomStateMessage& RHS)
		: Channel(RHS.Channel), bTagsValid(RHS.bTagsValid), Tags(RHS.Tags)
	{}

	FRoomStateMessage& operator=(const FRoomStateMessage& RHS)
	{
		Channel = RHS.Channel;
		bTagsValid = RHS.bTagsValid;
		Tags = RHS.Tags;
		return *this;
	}

	UPROPERTY(BlueprintReadOnly)
		FString Channel;

	UPROPERTY(BlueprintReadOnly)
		bool bTagsValid;

	UPROPERTY(BlueprintReadOnly)
		FTWRoomStateTags Tags;
};

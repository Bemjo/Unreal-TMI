#include "TwitchChatter.h"

BEGIN_DEFINE_SPEC(TwitchChatterSpec, "TwitchChatter", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

TStrongObjectPtr<UTwitchChatter> TwitchChatter = nullptr;

// Ensure the username is all lowercase
const FString BOT_USERNAME = TEXT("thejollybeardobot");
const FString BOT_PASSWORD = TEXT("zpflp5iqjrdwingf7llywcbeb2ylf1");
const FString BOT_TEST_TEXT = TEXT("This is a test of the botto broadcast system");

bool bJoinedChannel = false;
bool bHasBits = false;

END_DEFINE_SPEC(TwitchChatterSpec);

void TwitchChatterSpec::Define()
{
  if (BOT_USERNAME.Len() <= 3 || BOT_PASSWORD.Len() <= 20)
  {
    UE_LOG(LogTwitchChatter, Log, TEXT("To Run the TwitchChatter Twitch Tests, you MUST supply a valid twitch username, and an OAUTH Password"));
    return;
  }

  Describe("Event Dispatching", [this]()
  {
    Describe("Twitch Connection", [this]()
    {
      LatentBeforeEach([this](const FDoneDelegate& Done)
      {
        TwitchChatter = TStrongObjectPtr<UTwitchChatter>(NewObject<UTwitchChatter>());
        Done.Execute();
      });

      LatentIt("should be able to connect to twitch TMI", [this](const FDoneDelegate& Done)
      {
        TwitchChatter->EventSocketConnected.AddLambda([Done, this]()
        {
          TestTrue("Connected", true);
          Done.Execute();
        });

        TwitchChatter->EventSocketError.AddLambda([Done, this]() {
          TestTrue("Connected", false);
          Done.Execute();
         });

        TwitchChatter->Connect("", "", {}, false, false);
        });

      LatentIt("should be able to detect an authentication failure", [this](const FDoneDelegate& Done) {
        TwitchChatter->EventAuthFailure.AddLambda([Done, this]() {
          TestTrue("Auth Failed", true);
          Done.Execute();
        });

        TwitchChatter->EventAuthSuccess.AddLambda([Done, this](const FGlobalUserStateMessage& GUS) {
          TestTrue("Auth Failed", false);
          Done.Execute();
        });

        TwitchChatter->Connect("", "", {}, false, false);
        });

      LatentIt("should be able to enable fast mode to ignore tag information", [this](const FDoneDelegate& Done)
        {
          TwitchChatter->EventJoinedChannel.AddLambda([this, Done](const FString& Channel) {
            Done.Execute();
          });

          TwitchChatter->EventAuthSuccess.AddLambda([this](const FGlobalUserStateMessage& GUS) {
            TestFalse("Tags should be invalid", GUS.bTagsValid);
            TwitchChatter->JoinChannel(BOT_USERNAME);
          });

          TwitchChatter->EventSentMessage.AddLambda([this](const FString& OurSentMessage) {
            TestNotEqual("Not requesting tags", OurSentMessage, FString("CAP REQ :twitch.tv/tags"));
          });

          TwitchChatter->Connect(BOT_USERNAME, BOT_PASSWORD, {}, false, true);
        });

      // Yes we're testing multiple things
      LatentIt("should be able to authenticate with valid credentials, join a channel, send a message, and leave the channel", [this](const FDoneDelegate& Done)
      {
        TwitchChatter->EventPartedChannel.AddLambda([this, Done](const FString& Channel) {
          TestEqual("Parted Channel", Channel, BOT_USERNAME);
          Done.Execute();
        });

        TwitchChatter->EventSentMessage.AddLambda([this](const FString& RawMessage) {
          if (bJoinedChannel)
          {
            TestEqual("Sent Message", RawMessage, FString::Printf(TEXT("PRIVMSG #%s :%s"), *BOT_USERNAME, *BOT_TEST_TEXT));
            bJoinedChannel = false;
            TwitchChatter->PartChannel(BOT_USERNAME);
          }
        });

        TwitchChatter->EventJoinedChannel.AddLambda([this](const FString& Channel) {
          bJoinedChannel = true;
          TestEqual("Joined Correct Channel", Channel, BOT_USERNAME);

          TwitchChatter->Send(BOT_USERNAME, BOT_TEST_TEXT);
        });

        TwitchChatter->EventSocketError.AddLambda([this, Done]() {
          TestTrue("Socket Connected", false);
          Done.Execute();
        });

        TwitchChatter->EventAuthFailure.AddLambda([this, Done]() {
          TestTrue("Authenticated", false);
          Done.Execute();
        });

        TwitchChatter->EventAuthSuccess.AddLambda([this](const FGlobalUserStateMessage& GUS) {
          TwitchChatter->JoinChannel(BOT_USERNAME);
        });

        bJoinedChannel = false;
        TwitchChatter->Connect(BOT_USERNAME, BOT_PASSWORD, {}, false, false);
      });

      LatentAfterEach([this](const FDoneDelegate& Done)
      {
        TwitchChatter.Reset();
        Done.Execute();
      });
    });

    Describe("Manual Input", [this]()
    {
      LatentBeforeEach([this](const FDoneDelegate& Done)
      {
        if (!TwitchChatter.IsValid())
        {
          TwitchChatter = TStrongObjectPtr<UTwitchChatter>(NewObject<UTwitchChatter>());
          TwitchChatter->bAuthenticated = true;
          TwitchChatter->BotUsername = BOT_USERNAME;
        }

        TwitchChatter->ResetEventHandlers();

        Done.Execute();
      });

      LatentIt("should call a bits event if the bits tag is present in a parsed PRIVMSG", [this](const FDoneDelegate& Done) {
        const TCHAR* RawMessage = TEXT("@badge-info=;badges=staff/1,bits/1000;bits=42069;color=;display-name=ronni;emotes=;id=b34ccfc7-4977-403a-8a94-33c6bac34fb8;mod=0;room-id=12345678;subscriber=0;tmi-sent-ts=1507246572675;turbo=1;user-id=12345678;user-type=staff :ronni!ronni@ronni.tmi.twitch.tv PRIVMSG #ronni :cheer100");

        TwitchChatter->EventChatMessage.AddLambda([this, Done](const FPrivMsgMessage& Message) {
          TestTrue("Tags Valid", Message.bTagsValid);
          TestEqual("Bits Value", Message.Tags.Bits, 42069);

          if (bHasBits)
            Done.Execute();
          else
            bHasBits = true;
        });

        TwitchChatter->EventChatBits.AddLambda([this, Done](const FPrivMsgMessage& Message) {
          TestTrue("Tags Valid", Message.bTagsValid);
          TestEqual("Bits Value", Message.Tags.Bits, 42069);

          if (bHasBits)
            Done.Execute();
          else
            bHasBits = true;
        });
        
        bHasBits = false;
        TwitchChatter->HandleMessage(RawMessage);
      });
    });
  });
}
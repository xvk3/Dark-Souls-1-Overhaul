#include "GameData.h"
#include "Cheats.h"
#include "Character.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "SP/io/keybinds.h"
#include "SP/memory/injection/asm/x64.h"

// Helper functions
extern "C" {
    uint64_t CheatsASMFollow(uint64_t pointer);
    void     CheatsASMHomewardWrapper(uint64_t BaseB, uint64_t Homeward);
    void     CheatsASMReviveCharWrapper();
    void     CheatsASMKickPlayerWrapper();
}

// Stop Durability Damage
extern "C" {
    uint64_t stop_durability_damage_injection_return;
    uint64_t stop_durability_damage_original_jump;
    void stop_durability_damage_hook();
}

// Ignore Leave Messages
extern "C" {
    uint64_t GameMan_base_bIgnoreLeaveMessages = 0x141D10E18;
    void* GameMan_Ptr_bIgnoreLeaveMessages();
    bool GameMan_Get_bIgnoreLeaveMessages();
    bool GameMan_Set_bIgnoreLeaveMessages(byte state);
}

void printBytes(uint64_t pointer, short rows_of_eight);
void printByte(uint64_t pointer);
void printByteRaw(uint64_t pointer);
byte returnByteRaw(uint64_t pointer);

inline bool bitTest(uint64_t pointer, short bit);
inline void bitmod(uint64_t ptr, short bit, bool value);
inline void bitunset(uint64_t ptr, short bit);
inline void bitset(uint64_t ptr, short bit);
inline void bittog(uint64_t ptr, short bit);

bool monitorCharacters(void* unused);
bool delayedVariableUpdateWrapper(void* unused);
bool speedhackOnDeath(void* unused);

void delayedVariableUpdate();
void printPreferences();
void printPosition();

// Variables
uint64_t BaseXOffset = 0x00;
uint64_t BaseX = 0x00;

uint64_t BaseBOffset = 0x00;
uint64_t BaseB = 0x00;

uint64_t BasePOffset = 0x00;
uint64_t BaseP = 0x00;

uint64_t Homeward = 0x00;

uint64_t PlayerBase = 0x00;

uint64_t debug_flagsOffset = 0x00;
uint64_t debug_flags = 0x00;

CHEAT Cheats::noGoodsConsume  = { false };
CHEAT Cheats::noArrowConsume  = { false };
CHEAT Cheats::noMagicConsume  = { false };
CHEAT Cheats::noDead          = { false };
CHEAT Cheats::eventSuperArmor = { false };
CHEAT Cheats::noUpdateAI      = { false };
CHEAT Cheats::noGravity       = { false };
CHEAT Cheats::noHUD           = { true };

Character P0 = { 0x00 };
Character P1 = { 0x00 };
Character P2 = { 0x00 };
Character P3 = { 0x00 };
Character P4 = { 0x00 };
Character P5 = { 0x00 };

bool prev_playerchar_is_loaded = false;
bool variablesUpdated = false;
bool speedhackActivated = false;

Cheats::Cheats()
{
}

Cheats::~Cheats()
{
}

void Cheats::start() {

    ConsoleWriteDebug("-Cheats::start: entered");

    // For information
    ConsoleWriteDebug("--Cheats::start: ds1_base    = 0x%X", Game::ds1_base);

    // Initialise BaseXOffset
    BaseXOffset = (uint64_t)sp::mem::aob_scan("48 8B 05 ?? ?? ?? ?? 48 39 48 68 0F 94 C0 C3");
    ConsoleWriteDebug("--Cheats::start: BaseXOffset = 0x%X", BaseXOffset);

    // Initialise BaseBOffset
    BaseBOffset = (uint64_t)sp::mem::aob_scan("48 8B 05 ?? ?? ?? ?? 45 33 ED 48 8B F1 48 85 C0");
    ConsoleWriteDebug("--Cheats::start: BaseBOffset = 0x%X", BaseBOffset);

    // Initialise BasePOffset
    BasePOffset = 0x141D1B360;
    ConsoleWriteDebug("--Cheats::start: BasePOffset = 0x%X", BasePOffset);
    
    // Initialise Homeward
    Homeward = (uint64_t)sp::mem::aob_scan("48 89 5C 24 08 57 48 83 EC 20 48 8B D9 8B FA 48 8B 49 08 48 85 C9 0F 84 ? ? ? ? E8 ? ? ? ? 48 8B 4B 08");
    ConsoleWriteDebug("--Cheats::start: Homeward    = 0x%X", Homeward);

    // Change the loading screen items to all be Green Blossom
    LoadingScreenItems();

    // Runs until a character is loaded, then updates a couple of pointers and never runs again
    MainLoop::setup_mainloop_callback(delayedVariableUpdateWrapper, NULL, "delayedVariableUpdate");

    // Runs continiously and calls other functions when a character is loaded
    MainLoop::setup_mainloop_callback(monitorCharacters, NULL, "monitorCharacters");

    // Runs continiously monitoring curHP and activating speedhack when player is dead
    // TODO: fix crashing
    // I think it's because curHP results in a nullptr during the loading screen after death
    // Need to hold off running code until the character is back loaded.
    //MainLoop::setup_mainloop_callback(speedhackOnDeath, NULL, "speedhackOnDeath");

    // Print configuration preferences
    printPreferences();

    ConsoleWriteDebug("-Cheats::start: completed\n");
}

bool Cheats::applyCheats() {

    if (Game::playerchar_is_loaded()) {

        noGoodsConsumeApply();
        noArrowConsumeApply();
        noMagicConsumeApply();
        noDeadApply();
        eventSuperArmorApply();
        noUpdateAIApply();
        noGravityApply();
        noHUDApply();
    }

    return true;
}

void RedEyeOrb() {

    // ID = 102; Offset = 0xDA8;
    byte unrestrict_patch[3] = { 0xff, 0xff, 0x63 };// Allow use while hollow
    byte modify_use_animation[1] = { 0x0E };        // Silver Pendant animation
    byte opmeMenuType_patch[1] = { 0x00 };          // Disable dialog on use

    uint64_t RedEyeOrb = CheatsASMFollow(BaseP + 0xF0);
    RedEyeOrb = CheatsASMFollow(RedEyeOrb + 0x38);
    RedEyeOrb = RedEyeOrb + 0xDA8;

    ConsoleWriteDebug("-RedEyeOrb = 0x%X", RedEyeOrb);

    ConsoleWriteDebug("--RedEyeOrb: unrestrict_patch     = 0x%X", RedEyeOrb + 0x42);
    ConsoleWriteDebug("--RedEyeOrb: modify_use_animation = 0x%X", RedEyeOrb + 0x3E);
    ConsoleWriteDebug("--RedEyeOrb: opmeMenuType_patch   = 0x%X", RedEyeOrb + 0x3F);

    memcpy((void*)(RedEyeOrb + 0x42), unrestrict_patch, 3);
    memcpy((void*)(RedEyeOrb + 0x3E), modify_use_animation, 1);
    memcpy((void*)(RedEyeOrb + 0x3F), opmeMenuType_patch, 1);   // Could be combined with the "modify_use_animation" patch
    ConsoleWriteDebug("--RedEyeOrb: no menu, no restrictions, different animation", Mod::output_prefix);
    ConsoleWriteDebug("-RedEyeOrb: completed\n", Mod::output_prefix);
}

void EyeOfDeath() {

    // ID = 109; Offset = F18
    byte unrestrict_patch[3] = { 0xff, 0xff, 0x63 };// Allow use while hollow
    byte modify_use_animation[1] = { 0x0E };        // Silver Pendant animation
    byte opmeMenuType_patch[1] = { 0x00 };          // Disable dialog on use

    uint64_t EyeOfDeath = CheatsASMFollow(BaseP + 0xF0);
    EyeOfDeath = CheatsASMFollow(EyeOfDeath + 0x38);
    EyeOfDeath = EyeOfDeath + 0xF18;

    ConsoleWriteDebug("-EyeOfDeath = 0x%X", EyeOfDeath);

    ConsoleWriteDebug("--EyeOfDeath: unrestrict_patch     = 0x%X", EyeOfDeath + 0x42);
    ConsoleWriteDebug("--EyeOfDeath: modify_use_animation = 0x%X", EyeOfDeath + 0x3E);
    ConsoleWriteDebug("--EyeOfDeath: opmeMenuType_patch   = 0x%X", EyeOfDeath + 0x3F);

    memcpy((void*)(EyeOfDeath + 0x42), unrestrict_patch, 3);
    memcpy((void*)(EyeOfDeath + 0x3E), modify_use_animation, 1);
    memcpy((void*)(EyeOfDeath + 0x3F), opmeMenuType_patch, 1);
    ConsoleWriteDebug("--EyeOfDeath: no menu, no restrictions, different animation", Mod::output_prefix);
    ConsoleWriteDebug("-EyeOfDeath: completed\n", Mod::output_prefix);
}

void GreenBlossom() {

    return;

    // ID = 260; Offset = 1EE8
    byte unrestrict_use_limit[1] = { 0x00 };

    uint64_t GreenBlossom = CheatsASMFollow(BaseP + 0xF0);
    GreenBlossom = CheatsASMFollow(GreenBlossom + 0x38);
    GreenBlossom = GreenBlossom + 0x1EE8;

    ConsoleWriteDebug("-GreenBlossom = 0x%X", GreenBlossom);

    ConsoleWriteDebug("--GreenBlossom: unrestrict_use_limit = 0x%X", GreenBlossom + 0x3B);

    memcpy((void*)(GreenBlossom + 0x3B), unrestrict_use_limit, 1);
    ConsoleWriteDebug("--GreenBlossom: no restrictions", Mod::output_prefix);
    ConsoleWriteDebug("-GreenBlossom: completed\n", Mod::output_prefix);
}

void RedSignSoapstone() {

    //ID = 101; Offset = D4C
    byte unrestrict_patch[3] = { 0xff, 0xff, 0x63 };// Allow use while hollow

    uint64_t RedSignSoapstone = CheatsASMFollow(BaseP + 0xF0);
    RedSignSoapstone = CheatsASMFollow(RedSignSoapstone + 0x38);
    RedSignSoapstone = RedSignSoapstone + 0xD4C;

    ConsoleWriteDebug("-RedSignSoapstone = 0x%X", RedSignSoapstone);

    ConsoleWriteDebug("--RedSignSoapstone: unrestrict_patch = 0x%X", RedSignSoapstone + 0x42);

    memcpy((void*)(RedSignSoapstone + 0x42), unrestrict_patch, 3);
    ConsoleWriteDebug("--RedSignSoapstone: no restrictions\n", Mod::output_prefix);
}

void CrackedRedEyeOrb() {

    // ID = 111; Offset = F74
    byte unrestrict_patch[3] = { 0xff, 0xff, 0x63 };// Allow use while hollow
    byte opmeMenuType_patch[1] = { 0x00 };          // Disable dialog on use
    /* first two bytes are "vowType"s
    bit                  0x63   0xC3    0xBF    0xC4
    0 = enable_live        1      1       1       1
    1 = enable_grey        1      1       1       1
    2 = enable_white       0      0       1       0
    3 = enable_black       0      0       1       0
    4 = enable_multi       0      0       1       0
    5 = disable_offline    1      0       1       1
    6 = isEquip            1      1       0       0
    7 = isConsume          0      1       1       0
    */

    uint64_t CrackedRedEyeOrb = CheatsASMFollow(BaseP + 0xF0);
    CrackedRedEyeOrb = CheatsASMFollow(CrackedRedEyeOrb + 0x38);
    CrackedRedEyeOrb = CrackedRedEyeOrb + 0xF74;

    ConsoleWriteDebug("-CrackedRedEyeOrb = 0x%X", CrackedRedEyeOrb);

    ConsoleWriteDebug("--CrackedRedEyeOrb: unrestrict_patch     = 0x%X", CrackedRedEyeOrb + 0x42);
    ConsoleWriteDebug("--CrackedRedEyeOrb: opmeMenuType_patch   = 0x%X", CrackedRedEyeOrb + 0x3F);

    memcpy((void*)(CrackedRedEyeOrb + 0x42), unrestrict_patch, 3);
    memcpy((void*)(CrackedRedEyeOrb + 0x3F), opmeMenuType_patch, 1);
    ConsoleWriteDebug("--CrackedRedEyeOrb: no menu, no restrictions", Mod::output_prefix);
    ConsoleWriteDebug("-CrackedRedEyeOrb: completed\n", Mod::output_prefix);
}

void Humanity() {

    // ID = 500; Offset = 313C
    byte unrestrict_patch[3] = { 0xff, 0xff, 0xbf };// Allow use with phantoms present

    uint64_t Humanity = CheatsASMFollow(BaseP + 0xF0);
    Humanity = CheatsASMFollow(Humanity + 0x38);
    Humanity = Humanity + 0x313C;

    ConsoleWriteDebug("-Humanity = 0x%X", Humanity);

    ConsoleWriteDebug("--Humanity: unrestrict_patch = 0x%X", Humanity + 0x42);

    memcpy((void*)(Humanity + 0x42), unrestrict_patch, 3);
    ConsoleWriteDebug("--Humanity: no restrictions", Mod::output_prefix);
    ConsoleWriteDebug("-Humanity: completed\n", Mod::output_prefix);
}

void TwinHumanity() {

    // ID = 501; Offset = 3198
    byte unrestrict_patch[3] = { 0xff, 0xff, 0xbf };// Allow use with phantoms present

    uint64_t TwinHumanity = CheatsASMFollow(BaseP + 0xF0);
    TwinHumanity = CheatsASMFollow(TwinHumanity + 0x38);
    TwinHumanity = TwinHumanity + 0x3198;

    ConsoleWriteDebug("-TwinHumanity = 0x%X", TwinHumanity);

    ConsoleWriteDebug("--TwinHumanity: unrestrict_patch = 0x%X", TwinHumanity + 0x42);

    memcpy((void*)(TwinHumanity + 0x42), unrestrict_patch, 3);

    ConsoleWriteDebug("--TwinHumanity: no restrictions", Mod::output_prefix);
    ConsoleWriteDebug("-TwinHumanity: completed\n", Mod::output_prefix);
}

void DivineBlessing() {

    // ID = 240; Offset = 1E8C
    byte unrestrict_patch[3] = { 0xff, 0xff, 0xbf };// Allow use with phantoms present

    uint64_t DivineBlessing = CheatsASMFollow(BaseP + 0xF0);
    DivineBlessing = CheatsASMFollow(DivineBlessing + 0x38);
    DivineBlessing = DivineBlessing + 0x1E8C;

    ConsoleWriteDebug("-DivineBlessing = 0x%X", DivineBlessing);

    ConsoleWriteDebug("--DivineBlessing: unrestrict_patch = 0x%X", DivineBlessing + 0x42);

    memcpy((void*)(DivineBlessing + 0x42), unrestrict_patch, 3);
    ConsoleWriteDebug("--DivineBlessing: no restrictions", Mod::output_prefix);
    ConsoleWriteDebug("-DivineBlessing: completed\n", Mod::output_prefix);
}

void ElizabethMushroom() {

    // ID = 230; Offset = 1E30
    byte unrestrict_patch[3] = { 0xff, 0xff, 0xbf };// Allow use with phantoms present

    uint64_t ElizabethMushroom = CheatsASMFollow(BaseP + 0xF0);
    ElizabethMushroom = CheatsASMFollow(ElizabethMushroom + 0x38);
    ElizabethMushroom = ElizabethMushroom + 0x1E30;

    ConsoleWriteDebug("-ElizabethMushroom = 0x%X", ElizabethMushroom);

    ConsoleWriteDebug("--ElizabethMushroom: unrestrict_patch = 0x%X", ElizabethMushroom + 0x42);

    memcpy((void*)(ElizabethMushroom + 0x42), unrestrict_patch, 3);
    ConsoleWriteDebug("--ElizabethMushroom: no restrictions", Mod::output_prefix);
    ConsoleWriteDebug("-ElizabethMushroom: completed\n", Mod::output_prefix);
}

void HomewardBone() {

    // ID = 330; Offset = 2560
    byte unrestrict_patch[3] = { 0xff, 0xff, 0xbf };// Allow use while online
    byte modify_use_animation[1] = { 0x0E };        // Silver Pendant animation
    byte opmeMenuType_patch[1] = { 0x00 };          // Disable dialog on use

    uint64_t HomewardBone = CheatsASMFollow(BaseP + 0xF0);
    HomewardBone = CheatsASMFollow(HomewardBone + 0x38);
    HomewardBone = HomewardBone + 0x2560;

    ConsoleWriteDebug("-HomewardBone = 0x%X", HomewardBone);

    ConsoleWriteDebug("--HomewardBone: unrestrict_patch     = 0x%X", HomewardBone + 0x42);
    ConsoleWriteDebug("--HomewardBone: modify_use_animation = 0x%X", HomewardBone + 0x3E);
    ConsoleWriteDebug("--HomewardBone: opmeMenuType_patch   = 0x%X", HomewardBone + 0x3F);

    memcpy((void*)(HomewardBone + 0x42), unrestrict_patch, 3);
    memcpy((void*)(HomewardBone + 0x3E), modify_use_animation, 1);
    memcpy((void*)(HomewardBone + 0x3F), opmeMenuType_patch, 1);
    ConsoleWriteDebug("--HomewardBone: no menu, no restrictions, different animation", Mod::output_prefix);
    ConsoleWriteDebug("-HomewardBone: completed\n", Mod::output_prefix);
}

void DriedFinger() {

    // ID = 385; Offset = 2AC4;
    byte modify_use_animation[1] = { 0x0E };        // Silver Pendant animation
    byte modify_sfx_variation[1] = { 0x5C };        // Purple Cowards Crystal SFX

    uint64_t DriedFinger = CheatsASMFollow(BaseP + 0xF0);
    DriedFinger = CheatsASMFollow(DriedFinger + 0x38);
    DriedFinger = DriedFinger + 0x2AC4;
    ConsoleWriteDebug("-DriedFinger = 0x%X", DriedFinger);

    ConsoleWriteDebug("--DriedFinger: modify_use_animation = 0x%X", DriedFinger + 0x3E);
    ConsoleWriteDebug("--DriedFinger: modify_sfx_animation = 0x%X", DriedFinger + 0x04);

    memcpy((void*)(DriedFinger + 0x3E), modify_use_animation, 1);
    memcpy((void*)(DriedFinger + 0x04), modify_sfx_variation, 1);
    ConsoleWriteDebug("--DriedFinger: different animation, different SFX");
    ConsoleWriteDebug("-DriedFinger: completed\n");
}

void CharcoalPineResin() {

    // ID = 310; Offset = 23F0
    byte modify_use_animation[1] = { 0x00 };        // Default use animation (Green Blossom style)

    uint64_t CharcoalPineResin = CheatsASMFollow(BaseP + 0xF0);
    CharcoalPineResin = CheatsASMFollow(CharcoalPineResin + 0x38);
    CharcoalPineResin = CharcoalPineResin + 0x23F0;
    ConsoleWriteDebug("-CharcoalPineResin = 0x%X", CharcoalPineResin);

    ConsoleWriteDebug("--CharcoalPineResin: modify_use_animation = 0x%X", CharcoalPineResin + 0x3E);

    memcpy((void*)(CharcoalPineResin + 0x3E), modify_use_animation, 1);
    ConsoleWriteDebug("--CharcoalPineResin: different animation", Mod::output_prefix);
    ConsoleWriteDebug("-CharcoalPineResin: completed\n", Mod::output_prefix);

}

void RottenPineResin() {

    // ID = 313; Offset = 2504
    byte modify_use_animation[1] = { 0x00 };        // Default use animation (Green Blossom style)

    uint64_t RottenPineResin = CheatsASMFollow(BaseP + 0xF0);
    RottenPineResin = CheatsASMFollow(RottenPineResin + 0x38);
    RottenPineResin = RottenPineResin + 0x2504;
    ConsoleWriteDebug("-RottenPineResin = 0x%X", RottenPineResin);

    ConsoleWriteDebug("--RottenPineResin: modify_use_animation = 0x%X", RottenPineResin + 0x3E);

    memcpy((void*)(RottenPineResin + 0x3E), modify_use_animation, 1);
    ConsoleWriteDebug("--RottenPineResin: different animation", Mod::output_prefix);
    ConsoleWriteDebug("-RottenPineResin: completed\n", Mod::output_prefix);
}

void GoldPineResin() {

    // ID = 311; Offset = 244C
    byte modify_use_animation[1] = { 0x00 };        // Default use animation (Green Blossom style)

    uint64_t GoldPineResin = CheatsASMFollow(BaseP + 0xF0);
    GoldPineResin = CheatsASMFollow(GoldPineResin + 0x38);
    GoldPineResin = GoldPineResin + 0x244C;
    ConsoleWriteDebug("-GoldPineResin = 0x%X", GoldPineResin);

    ConsoleWriteDebug("--GoldPineResin: modify_use_animation = 0x%X", GoldPineResin + 0x3E);

    memcpy((void*)(GoldPineResin + 0x3E), modify_use_animation, 1);
    ConsoleWriteDebug("--GoldPineResin: different animation", Mod::output_prefix);
    ConsoleWriteDebug("-GoldPineResin: completed\n", Mod::output_prefix);
}

void RepairPowder() {

    // ID = 280; Offset = 2110
    byte modify_use_animation[1] = { 0x00 };        // Default use animation (Green Blossom style)

    uint64_t RepairPowder = CheatsASMFollow(BaseP + 0xF0);
    RepairPowder = CheatsASMFollow(RepairPowder + 0x38);
    RepairPowder = RepairPowder + 0x2110;
    ConsoleWriteDebug("-RepairPowder = 0x%X", RepairPowder);

    ConsoleWriteDebug("--RepairPowder: modify_use_animation = 0x%X", RepairPowder + 0x3E);

    memcpy((void*)(RepairPowder + 0x3E), modify_use_animation, 1);
    ConsoleWriteDebug("--RepairPowder: different animation");
    ConsoleWriteDebug("-RepairPowder: completed\n");

}

void EstusFlask() {

    // ID = 215; Offset = 1D78
    byte modify_sfx_variation[1] = { 0x0A };        // Green Blossom SFX

    uint64_t EstusFlask = CheatsASMFollow(BaseP + 0xF0);
    EstusFlask = CheatsASMFollow(EstusFlask + 0x38);
    EstusFlask = EstusFlask + 0x1D78;
    ConsoleWriteDebug("-EstusFlask = 0x%X", EstusFlask);

    ConsoleWriteDebug("--EstusFlask: modify_sfx_animation = 0x%X", EstusFlask + 0x04);

    memcpy((void*)(EstusFlask + 0x04), modify_sfx_variation, 1);
    ConsoleWriteDebug("--EstusFlask: different SFX");
    ConsoleWriteDebug("-EstusFlask: completed\n");

}

void EggVermifuge() {

    return;
    // ID = 275; Offset = 20B4
    byte modify_use_animation[1] = { 0x01 };        // Resin animation
    byte modify_sfx_variation[1] = { 0x0A };        // Green Blossom SFX

    uint64_t EggVermifuge = CheatsASMFollow(BaseP + 0xF0);
    EggVermifuge = CheatsASMFollow(EggVermifuge + 0x38);
    EggVermifuge = EggVermifuge + 0x20B4;
    ConsoleWriteDebug("-EggVermifuge = 0x%X", EggVermifuge);

    ConsoleWriteDebug("--EggVermifuge: modify_use_animation = 0x%X", EggVermifuge + 0x3E);
    ConsoleWriteDebug("--EggVermifuge: modify_sfx_animation = 0x%X", EggVermifuge + 0x04);

    memcpy((void*)(EggVermifuge + 0x3E), modify_use_animation, 1);
    memcpy((void*)(EggVermifuge + 0x04), modify_sfx_variation, 1);
    ConsoleWriteDebug("--EggVermifuge: different animation, different SFX");
    ConsoleWriteDebug("-EggVermifuge: completed\n");

}

void GoldCoin() {

    // ID = 383; Offset = 2A0C
    // Can this be made edible?

}

void PerculiarDoll() {

    // ID = 384; Offset = 2A68
    // Does this have a 3D model?
    // Could try throwing it like a Lloyd's Talisman
}

void RingOfFavorAndProtection() {

    return;

    // Problem with this function, can't seem to get it working

    uint64_t RingOfFavorAndProtection = CheatsASMFollow(BaseP + 0xF0);
    RingOfFavorAndProtection = RingOfFavorAndProtection - 0x300a3d4;

    ConsoleWriteDebug("-RingOfFavorAndProtection0 = 0x%X", RingOfFavorAndProtection);

    bitset(RingOfFavorAndProtection + 0x3C, 1);
    ConsoleWrite("--RingOfFavorAndProtection: doesn't break on unequip");
    ConsoleWrite("-RingOfFavorAndProtection: completed\n");
}

void LoadingScreenItems() {

    uint64_t LoadingScreenItems = 0x141ACE7B0;

    ConsoleWriteDebug("-LoadingScreenItems = 0x%X", LoadingScreenItems);

    for (uint64_t offset = 0x00; offset < 0xF8; offset += 0x04) {
        *(uint32_t*)(LoadingScreenItems + offset) = 0x40000104;
    }

    ConsoleWriteDebug("-LoadingScreenItems: Green Blossom only");
    ConsoleWriteDebug("-LoadingScreenItems: completed\n");

}

// noGoodsConsume
void noGoodsConsumeToggle() {
    Cheats::noGoodsConsume.enabled = !Cheats::noGoodsConsume.enabled;
    ConsoleWriteDebug("Cheats: noGoodsConsume = %s", noGoodsConsumeSet(Cheats::noGoodsConsume.enabled) ? "Active" : "Inactive");
}
void noGoodsConsumeApply() {
    if (BaseX) {
        uint64_t noGoodsConsume = CheatsASMFollow((uint64_t)BaseX + 0x68) + 0x527;
        bitmod(noGoodsConsume, 0, Cheats::noGoodsConsume.enabled);
    }
}
int noGoodsConsumeSet(bool state) {
    if (BaseX) {
        uint64_t noGoodsConsume = CheatsASMFollow((uint64_t)BaseX + 0x68) + 0x527;
        bitmod(noGoodsConsume, 0, state);
        return bitTest(noGoodsConsume, 0);
    }
    return -1;
}

// noArrowConsume
void noArrowConsumeToggle() {
    Cheats::noArrowConsume.enabled = !Cheats::noArrowConsume.enabled;
    ConsoleWriteDebug("Cheats: noArrowConsume = %s", noArrowConsumeSet(Cheats::noArrowConsume.enabled) ? "Active" : "Inactive");
}
void noArrowConsumeApply() {
    if (debug_flags) {
        uint64_t noArrowConsume = debug_flags + 0x04;
        *((byte*)noArrowConsume) = Cheats::noArrowConsume.enabled;
    }
}
int noArrowConsumeSet(bool state) {
    if (debug_flags) {
        uint64_t noArrowConsume = debug_flags + 0x04;
        *((byte*)noArrowConsume) = state;
        return bitTest(noArrowConsume, 0);
    }
    return -1;
}

// noMagicConsume
void noMagicConsumeToggle() {
    Cheats::noMagicConsume.enabled = !Cheats::noMagicConsume.enabled;
    ConsoleWriteDebug("Cheats: noMagicConsume = %s", noMagicConsumeSet(Cheats::noMagicConsume.enabled) ? "Active" : "Inactive");
}
void noMagicConsumeApply() {
    if (debug_flags) {
        uint64_t noMagicConsume = debug_flags + 0x05;
        *((byte*)noMagicConsume) = Cheats::noMagicConsume.enabled;
    }
}
int noMagicConsumeSet(bool state) {
    if (debug_flags) {
        uint64_t noMagicConsume = debug_flags + 0x05;
        *((byte*)noMagicConsume) = state;
        return bitTest(noMagicConsume, 0);
    }
    return -1;
}

// noDead
void noDeadToggle() {
    Cheats::noDead.enabled = !Cheats::noDead.enabled;
    ConsoleWriteDebug("Cheats: noDead = %s", noDeadSet(Cheats::noDead.enabled) ? "Active" : "Inactive");
}
void noDeadApply() {
    if (debug_flags) {
        uint64_t noDead = debug_flags + 0x00;
        *((byte*)noDead) = Cheats::noDead.enabled;
    }
}
int noDeadSet(bool state) {
    if (debug_flags) {
        uint64_t noDead = debug_flags + 0x00;
        *((byte*)noDead) = state;
        return bitTest(noDead, 0);
    }
    return -1;
}

// eventSuperArmor
void eventSuperArmorToggle() {
    Cheats::eventSuperArmor.enabled = !Cheats::eventSuperArmor.enabled;
    ConsoleWriteDebug("Cheats: eventSuperArmor = %s", eventSuperArmorSet(Cheats::eventSuperArmor.enabled) ? "Active" : "Inactive");
}
void eventSuperArmorApply() {
    if (BaseX) {
        uint64_t eventSuperArmor = CheatsASMFollow((uint64_t)BaseX + 0x68) + 0x2A6;
        bitmod(eventSuperArmor, 0, Cheats::eventSuperArmor.enabled);
    }
}
int eventSuperArmorSet(bool state) {
    if (BaseX) {
        uint64_t eventSuperArmor = CheatsASMFollow((uint64_t)BaseX + 0x68) + 0x2A6;
        bitmod(eventSuperArmor, 0, Cheats::eventSuperArmor.enabled);
        return bitTest(eventSuperArmor, 0);
    }
    return -1;
}

// noUpdateAI
void noUpdateAIToggle() {
    Cheats::noUpdateAI.enabled = !Cheats::noUpdateAI.enabled;
    ConsoleWriteDebug("Cheats: noUpdateAI = %s", noUpdateAISet(Cheats::noUpdateAI.enabled) ? "Active" : "Inactive");
}
void noUpdateAIApply() {
    if (debug_flags) {
        uint64_t noUpdateAI = debug_flags + 0x0D;
        *((byte*)noUpdateAI) = Cheats::noUpdateAI.enabled;
    }
}
int noUpdateAISet(bool state) {
    if (debug_flags) {
        uint64_t noUpdateAI = debug_flags + 0x0D;
        *((byte*)noUpdateAI) = state;
        return bitTest(noUpdateAI, 0);
    }
    return -1;
}

// noGravity
void noGravityToggle() {
    Cheats::noGravity.enabled = !Cheats::noGravity.enabled;
    ConsoleWriteDebug("Cheats: noGravity = %s", noGravitySet(Cheats::noGravity.enabled) ? "Active" : "Inactive");
}
void noGravityApply() {
    if (BaseX) {
        uint64_t noGravity = CheatsASMFollow((uint64_t)BaseX + 0x68) + 0x2A5;
        bitmod(noGravity, 0, Cheats::noGravity.enabled);
    }
}
int noGravitySet(bool state) {
    if (BaseX) {
        uint64_t noGravity = CheatsASMFollow(BaseX + 0x68) + 0x2A5;
        bitmod(noGravity, 6, state);
        return bitTest(noGravity, 6);
    }
    return -1;
}

// noHUD
void noHUDToggle() {
    Cheats::noHUD.enabled = !Cheats::noHUD.enabled;
    ConsoleWriteDebug("Cheats: noHUD = %s",noHUDSet(Cheats::noHUD.enabled) ? "Inactive" : "Active");
}
void noHUDApply() {
    if (BaseB) {
        uint64_t noHUD = CheatsASMFollow((uint64_t)BaseB + 0x58) + 0x11;
        *((byte*)noHUD) = Cheats::noHUD.enabled;
    }
}
int noHUDSet(bool state) {
    if (BaseB) {
        uint64_t noHUD = CheatsASMFollow((uint64_t)BaseB + 0x58) + 0x11;
        *((byte*)noHUD) = state;
        return bitTest(noHUD, 0);
    }
    return -1;
}

// No Collision
// Fly mode

bool speedhackOnDeath(void* unused) {


    if (BaseX && Game::playerchar_is_loaded()) {

        sp::mem::pointer tmp;
        tmp.set_base((void*)((uint64_t)BaseX + 0x68));
        uint64_t curHP = (uint64_t)tmp.resolve();

        if (*(uint32_t*)curHP == 0x00) {
            if (speedhackActivated == false) {

                ConsoleWriteDebug("-speedhackOnDeath: entered");
                ConsoleWriteDebug("--speedHackOnDeath: curHP = %d", *(uint32_t*)curHP);
                ConsoleWriteDebug("--speedHackOnDeath: curHP = 0x%X", curHP);

                uint64_t ptr = CheatsASMFollow(BaseX + 0x68);
                ConsoleWriteDebug("--speedHackOnDeath: ptr0 = 0x%X", ptr);

                ptr = CheatsASMFollow(ptr + 0x68);
                ConsoleWriteDebug("--speedHackOnDeath: ptr1 = 0x%X", ptr);

                uint64_t speedModifier = CheatsASMFollow(ptr + 0x18) + 0xA8;
                ConsoleWriteDebug("--speedHackOnDeath: speedModifier = 0x%X", speedModifier);

                *(float*)speedModifier = 5.0f;
                speedhackActivated = true;
            }
        } else {
            speedhackActivated = false;
        }
    }
    return true;
}

void replenishSpells() {

    if (BaseB) {

        uint32_t spells[] = {
            90,          // 3000 - Sorcery: Soul Arrow
            60,          // 3010 - Sorcery: Great Soul Arrow
            36,          // 3020 - Sorcery: Heavy Soul Arrow
            24,          // 3030 - Sorcery: Great Heavy Soul Arrow
            30,          // 3040 - Sorcery: Homing Soulmass
            30,          // 3050 - Sorcery: Homing Crystal Soulmass
            12,          // 3060 - Sorcery: Soul Spear
            12,          // 3070 - Sorcery: Crystal Soul Spear
            0,           // 3080
            0,           // 3090
            15,          // 3100 - Sorcery: Magic Weapon
            9,           // 3110 - Sorcery: Great Magic Weapon
            9,           // 3120 - Sorcery: Crystal Magic Weapon
            0,           // 3130
            0,           // 3140
            0,           // 3150
            0,           // 3160
            0,           // 3170
            0,           // 3180
            0,           // 3190
            0,           // 3200
            0,           // 3210
            0,           // 3220
            0,           // 3230
            0,           // 3240
            0,           // 3250
            0,           // 3260
            0,           // 3270
            0,           // 3280
            0,           // 3290
            15,          // 3300 - Sorcery: Magic Shield
            9,           // 3310 - Sorcery: Strong Magic Shield
            0,           // 3320
            0,           // 3330
            0,           // 3340
            0,           // 3350
            0,           // 3360
            0,           // 3370
            0,           // 3380
            0,           // 3390
            9,           // 3400 - Sorcery: Hidden Weapon
            9,           // 3410 - Sorcery: Hidden Body
            0,           // 3420
            0,           // 3430
            0,           // 3440
            0,           // 3450
            0,           // 3460
            0,           // 3470
            0,           // 3480
            0,           // 3490
            9,           // 3500 - Sorcery: Cast Light
            18,          // 3510 - Sorcery: Hush
            60,          // 3520 - Sorcery: Aural Decoy
            3,           // 3530 - Sorcery: Repair
            30,          // 3540 - Sorcery: Fall Control
            33,          // 3550 - Sorcery: Chameleon
            0,           // 3560
            0,           // 3570
            0,           // 3580
            0,           // 3590
            12,          // 3600 - Sorcery: Resist Curse
            12,          // 3610 - Sorcery: Remedy
            0,           // 3620
            0,           // 3630
            0,           // 3640
            0,           // 3650
            0,           // 3660
            0,           // 3670
            0,           // 3680
            0,           // 3690
            60,          // 3700 - Sorcery: White Dragon Breath
            36,          // 3710 - Sorcery: Dark Orb
            18,          // 3720 - Sorcery: Dark Bead
            6,           // 3730 - Sorcery: Dark Fog
            9,           // 3740 - Sorcery: Pursuers
            0,           // 3750
            0,           // 3760
            0,           // 3770
            0,           // 3780
            0,           // 3790
            0,           // 3800
            0,           // 3810
            0,           // 3820
            0,           // 3830
            0,           // 3840
            0,           // 3850
            0,           // 3860
            0,           // 3870
            0,           // 3880
            0,           // 3890
            0,           // 3900
            0,           // 3910
            0,           // 3920
            0,           // 3930
            0,           // 3940
            0,           // 3950
            0,           // 3960
            0,           // 3970
            0,           // 3980
            0,           // 3990
            24,          // 4000 - Pyromancy: Fireball
            18,          // 4010 - Pyromancy: Fire Orb
            12,          // 4020 - Pyromancy: Great Fireball
            60,          // 4030 - Pyromancy: Firestorm
            60,          // 4040 - Pyromancy: Fire Tempest
            240,         // 4050 - Pyromancy: Fire Surge
            240,         // 4060 - Pyromancy: Fire Whip
            0,           // 4070
            0,           // 4080
            0,           // 4090
            48,          // 4100 - Pyromancy: Combustion
            24,          // 4110 - Pyromancy: Great Combustion
            0,           // 4120
            0,           // 4130
            0,           // 4140
            0,           // 4150
            0,           // 4160
            0,           // 4170
            0,           // 4180
            0,           // 4190
            9,           // 4200 - Pyromancy: Poison Mist
            3,           // 4210 - Pyromancy: Toxic Mist
            6,           // 4220 - Pyromancy: Acid Surge
            0,           // 4230
            0,           // 4240
            0,           // 4250
            0,           // 4260
            0,           // 4270
            0,           // 4280
            0,           // 4290
            9,           // 4300 - Pyromancy: Iron Flesh
            9,           // 4310 - Pyromancy: Flash Sweat
            0,           // 4320
            0,           // 4330
            0,           // 4340
            0,           // 4350
            21,          // 4360 - Pyromancy: Undead Rapport
            0,           // 4370
            0,           // 4380
            0,           // 4390
            3,           // 4400 - Pyromancy: Power Within
            0,           // 4410
            0,           // 4420
            0,           // 4430
            0,           // 4440
            0,           // 4450
            0,           // 4460
            0,           // 4470
            0,           // 4480
            0,           // 4490
            12,          // 4500 - Pyromancy: Great Chaos Fireball
            60,          // 4510 - Pyromancy: Chaos Storm
            240,         // 4520 - Pyromancy: Chaos Fire Whip
            24,          // 4530 - Pyromancy: Black Flame
            0,           // 4540
            0,           // 4550
            0,           // 4560
            0,           // 4570
            0,           // 4580
            0,           // 4590
            0,           // 4600
            0,           // 4610
            0,           // 4620
            0,           // 4630
            0,           // 4640
            0,           // 4650
            0,           // 4660
            0,           // 4670
            0,           // 4680
            0,           // 4690
            0,           // 4700
            0,           // 4710
            0,           // 4720
            0,           // 4730
            0,           // 4740
            0,           // 4750
            0,           // 4760
            0,           // 4770
            0,           // 4780
            0,           // 4790
            0,           // 4800
            0,           // 4810
            0,           // 4820
            0,           // 4830
            0,           // 4840
            0,           // 4850
            0,           // 4860
            0,           // 4870
            0,           // 4880
            0,           // 4890
            0,           // 4900
            0,           // 4910
            0,           // 4920
            0,           // 4930
            0,           // 4940
            0,           // 4950
            0,           // 4960
            0,           // 4970
            0,           // 4980
            0,           // 4990
            15,          // 5000 - Miracle: Heal
            9,           // 5010 - Miracle: Great Heal
            3,           // 5020 - Miracle: Great Heal Excerpt
            9,           // 5030 - Miracle: Soothing Sunlight
            6,           // 5040 - Miracle: Replenishment
            6,           // 5050 - Miracle: Bountiful Sunlight
            0,           // 5060
            0,           // 5070
            0,           // 5080
            0,           // 5090
            120,         // 5100 - Miracle: Gravelord Sword Dance
            120,         // 5110 - Miracle: Gravelord Greatsword Dance
            0,           // 5120
            0,           // 5130
            0,           // 5140
            0,           // 5150
            0,           // 5160
            0,           // 5170
            0,           // 5180
            0,           // 5190
            3,           // 5200 - Miracle: Escape Death
            3,           // 5210 - Miracle: Homeward
            0,           // 5220
            0,           // 5230
            0,           // 5240
            0,           // 5250
            0,           // 5260
            0,           // 5270
            0,           // 5280
            0,           // 5290
            63,          // 5300 - Miracle: Force
            9,           // 5310 - Miracle: Wrath of the Gods
            18,          // 5320 - Miracle: Emit Force
            0,           // 5330
            0,           // 5340
            0,           // 5350
            0,           // 5360
            0,           // 5370
            0,           // 5380
            0,           // 5390
            15,          // 5400 - Miracle: Seek Guidance
            0,           // 5410
            0,           // 5420
            0,           // 5430
            0,           // 5440
            0,           // 5450
            0,           // 5460
            0,           // 5470
            0,           // 5480
            0,           // 5490
            30,          // 5500 - Miracle: Lightning Spear
            30,          // 5510 - Miracle: Great Lightning Spear
            15,          // 5520 - Miracle: Sunlight Spear
            0,           // 5530
            0,           // 5540
            0,           // 5550
            0,           // 5560
            0,           // 5570
            0,           // 5580
            0,           // 5590
            12,          // 5600 - Miracle: Magic Barrier
            6,           // 5610 - Miracle: Great Magic Barrier
            0,           // 5620
            0,           // 5630
            0,           // 5640
            0,           // 5650
            0,           // 5660
            0,           // 5670
            0,           // 5680
            0,           // 5690
            12,          // 5700 - Miracle: Karmic Justice
            0,           // 5710
            0,           // 5720
            0,           // 5730
            0,           // 5740
            0,           // 5750
            0,           // 5760
            0,           // 5770
            0,           // 5780
            0,           // 5790
            15,          // 5800 - Miracle: Tranquil Walk of Peace
            6,           // 5810 - Miracle: Vow of Silence
            0,           // 5820
            0,           // 5830
            0,           // 5840
            0,           // 5850
            0,           // 5860
            0,           // 5870
            0,           // 5880
            0,           // 5890
            3,           // 5900 - Miracle: Sunlight Blade
            3,           // 5910 - Miracle: Darkmoon Blade

        };

        uint64_t SlotBase = *((uint64_t*)(BaseB + 0x10));
        SlotBase = *((uint64_t*)(SlotBase + 0x418));
        SlotBase = SlotBase + 0x18;

        // For each of the ten attunement slots...
        for (int i = 0; i < 11; i++) {

            // Read the spell ID
            uint32_t spellID = *(uint32_t*)SlotBase;
            ConsoleWrite("spellID = 0x%X", spellID);

            // Check for unused slot
            if (spellID == -1 || spellID == 0) {
                SlotBase += 8;
                continue;
            }

            // Lookup spell quantity
            uint32_t spellQuantity = spells[(spellID / 10) - 300];

            // Replenish spells
            *(uint32_t*)(SlotBase+4) = spellQuantity;

            // Next slot
            SlotBase += 8;

        }
    }
}

void reviveChar() {
    ConsoleWriteDebug("%s -reviveChar: entered", Mod::output_prefix);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheatsASMReviveCharWrapper, 0, 0, 0);
}

void hollowChar() {

    ConsoleWriteDebug("%s -hollowChar: entered", Mod::output_prefix);

    byte CharType_hollow[1] = { 0x08 };
    byte TeamType_hollow[1] = { 0x04 };

    uint64_t TeamType = CheatsASMFollow(BaseX + 0x68) + 0xD8;
    uint64_t CharType = CheatsASMFollow(BaseX + 0x68) + 0xD4;

    if (BaseX &&
        *(byte*)TeamType == 0x01 &&
        *(byte*)CharType == 0x00)
    {

        memcpy((void*)TeamType, TeamType_hollow, 1);
        memcpy((void*)CharType, CharType_hollow, 1);
    }

}

void warp() {

    // TODO Check character is loaded here?

    ConsoleWriteDebug("-warp: entered\n");

    struct SimpleClassHomewardWrapperArguments {
        uint64_t _BaseB;
        uint64_t _Homeward;
    };

    SimpleClassHomewardWrapperArguments* args = (SimpleClassHomewardWrapperArguments*)malloc(sizeof(SimpleClassHomewardWrapperArguments));
    args->_BaseB = BaseB;
    args->_Homeward = Homeward;

    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheatsASMHomewardWrapper, args, 0, 0);
}

void kick(short player) {

    ConsoleWriteDebug("%s kick(%d): entered", Mod::output_prefix, player);

    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheatsASMKickPlayerWrapper, &player, 0, 0 );
}

void Cheats::teleport(COORDINATES coordinates) {

    if (BaseX) {

        // Disable gravity & enable noDead
        noGravitySet(true);
        noDeadSet(true);

        //Optimise
        uint64_t BaseX_f68 = CheatsASMFollow(BaseX + 0x68);
        uint64_t BaseX_f18 = CheatsASMFollow(BaseX_f68 + 0x18);
        uint64_t BaseX_f28 = CheatsASMFollow(BaseX_f18 + 0x28);
        uint64_t BaseX_f50 = CheatsASMFollow(BaseX_f28 + 0x50);
        uint64_t BaseX_f20 = CheatsASMFollow(BaseX_f50 + 0x20);
        uint64_t X = BaseX_f20 + 0x120;
        uint64_t Z = BaseX_f20 + 0x124;
        uint64_t Y = BaseX_f20 + 0x128;

        // Change coordinates
        *(uint64_t*)X = coordinates.x;
        *(uint64_t*)Z = coordinates.z;
        *(uint64_t*)Y = coordinates.y;

        // Enable gravity & disable noDead
        noGravitySet(true);
        noDeadSet(true);        // Proably should return noDead to the previous state
    }
}

void stopDurabilityDamage() {

    ConsoleWriteDebug("-stopDurabilityDamage: entered");
    uint64_t InfDur1AOB = (uint64_t)sp::mem::aob_scan("45 89 4B 14 44 3B 51 20");
    //ConsoleWrite("%s InfDur1AOB = %X", Mod::output_prefix, InfDur1AOB);

    // A conditional (and therefore relative) jump was overwritten by inject_jmp_14b. Calculate absolute address of the jump target
    // This allows the stop_durability_damage_hook procedure to correctly replicate the instructions overwritten by the trampoline
    stop_durability_damage_original_jump = InfDur1AOB + 0x1F;

    // Apply the injection
    sp::mem::code::x64::inject_jmp_14b((void*)InfDur1AOB, &stop_durability_damage_injection_return, 0, &stop_durability_damage_hook);

    /*

    Before injection:

    InfDura1AOB:
    .text:000000014074BB07 45 89 4B 14                                mov     [r11+14h], r9d
    .text:000000014074BB0B 44 3B 51 20                                cmp     r10d, [rcx+20h]
    .text:000000014074BB0F 7D 15                                      jge     short loc_14074BB26
    .text:000000014074BB11 48 8B 41 30                                mov     rax, [rcx+30h]
                           ^^^^^^^^^^^ these 14 bytes are patched with a long jump to CheatsASM_InfDur1AOB

    After injections:
    .text:000000014074BB07 ff 25 00 00 00 00 dd f3 1c 4a fc 7f 00 00  jmp stop_durability_damage_hook

    Injection Code:
        cmp [r11+14h], r9d
        jle originalcode
        mov r9d, dword ptr[r11+14h]                                   ; set the new durability value to be the current value

        originalcode:
            mov dword ptr[r11+14h], r9d
            cmp r10d, dword ptr[rcx+20h]
            jge hopper
            mov rax, qword ptr[rcx+20h]
            jmp stop_durability_damage_injection_return

        hopper:
            jmp stop_durability_damage_original_jump                  ; another function somewhere

    Returning from injection:
    .text:000000014074BB15 44 89 44 02 18                             mov     [rdx+rax+18h], r8d
    .text:000000014074BB1A 48 8D 0C 02                                lea     rcx, [rdx+rax]
    .text:000000014074BB1E B0 01                                      mov     al, 1
    .text:000000014074BB20 48 8B 5C 24 08                             mov     rbx, [rsp+arg_0]
    .text:000000014074BB25 C3                                         retn
   
    */

    ConsoleWriteDebug("-stopDurabilityDamage: completed\n");
}

bool delayedVariableUpdateWrapper(void* unused) {
    if (Game::playerchar_is_loaded() && variablesUpdated == false) {
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)delayedVariableUpdate, 0, 0, 0);
        return false;
    }
    return true;
}

void delayedVariableUpdate() {

    ConsoleWriteDebug("-delayedVariableUpdate: entered");

    // Initialise debug_flagsOffset
    debug_flagsOffset = (uint64_t)sp::mem::aob_scan("4C 8D 05 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 41 B1 01 4C 8D 05 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8B 8F 10 01 00 00 E8");
    ConsoleWriteDebug("--delayedVariableUpdate: debug_flagsOffset = 0x%X", debug_flagsOffset);

    // debug_flags
    debug_flags = (uint64_t)debug_flagsOffset + *(uint32_t*)((uint64_t)debug_flagsOffset + 3) + 7;
    ConsoleWriteDebug("--delayedVariableUpdate: debug_flags = 0x%X", debug_flags);

    // One-time initialisation
    P0 = Character::initialise(P0, PlayerBase, 0);
    P1 = Character::initialise(P0, PlayerBase, 1);
    P2 = Character::initialise(P0, PlayerBase, 2);
    P3 = Character::initialise(P0, PlayerBase, 3);
    P4 = Character::initialise(P0, PlayerBase, 4);
    P5 = Character::initialise(P0, PlayerBase, 5);
    ConsoleWriteDebug("--delayedVariableUpdate: one-time Character initialisation completed", debug_flags);

    if (Mod::enable_qol_cheats) {

        Cheats::noGoodsConsume.enabled = true;
        Cheats::noArrowConsume.enabled = true;
        Cheats::noMagicConsume.enabled = true;
        Cheats::noUpdateAI.enabled = true;

        RedEyeOrb();
        EyeOfDeath();
        GreenBlossom();
        RedSignSoapstone();
        CrackedRedEyeOrb();
        Humanity();
        TwinHumanity();
        DivineBlessing();
        ElizabethMushroom();
        HomewardBone();
        DriedFinger();
        CharcoalPineResin();
        RottenPineResin();
        GoldPineResin();
        RepairPowder();
        EstusFlask();
        EggVermifuge();
        GoldCoin();
        PerculiarDoll();
        RingOfFavorAndProtection();
        stopDurabilityDamage();
        ConsoleWriteDebug("--delayedVariableUpdate: QoL cheats enabled");
    }

    // Set IgnoreLeaveMessages according to configuration
    if (Mod::disable_leave_messages) {
        printBytes((uint64_t)GameMan_Ptr_bIgnoreLeaveMessages(), 1);
        ConsoleWriteDebug("--delayedVariableUpdate: bIgnoreLeaveMessages = %s", GameMan_Set_bIgnoreLeaveMessages(0xFF) ? "true" : "false");
        printBytes((uint64_t)GameMan_Ptr_bIgnoreLeaveMessages(), 1);
    }

    variablesUpdated = true;
    ConsoleWriteDebug("-delayedVariableUpdate: completed\n");

    wchar_t dc_msg[300];
    swprintf(dc_msg, 300, L"Delayed Variable Update Completed");
    Game::show_popup_message(dc_msg);
}

void updateBase() {

    ConsoleWriteDebug("-updateBase: entered");
    // TODO Confirm that the following pointers NEED updates after each reload

    // For information TODO: add remaining pointers from GameData.cpp
    ConsoleWriteDebug("--updateBase: ds1_base   = 0x%X", Game::ds1_base);

    // BaseX
    BaseX = CheatsASMFollow((uint64_t)BaseXOffset + *(uint32_t*)((uint64_t)BaseXOffset + 3) + 7);
    ConsoleWriteDebug("--updateBase: BaseX      = 0x%X", BaseX);

    // BaseB
    BaseB = CheatsASMFollow((uint64_t)BaseBOffset + *(uint32_t*)((uint64_t)BaseBOffset + 3) + 7);
    ConsoleWriteDebug("--updateBase: BaseB      = 0x%X", BaseB);

    // BaseP
    BaseP = CheatsASMFollow(BasePOffset);
    ConsoleWriteDebug("--updateBase: BaseP      = 0x%X", BaseP);

    // PlayerBase
    PlayerBase = CheatsASMFollow(BaseX + 0x68);
    PlayerBase = CheatsASMFollow(PlayerBase + 0x18);
    ConsoleWriteDebug("--updateBase: PlayerBase = 0x%X", PlayerBase);

    ConsoleWriteDebug("-updateBase: completed\n");
}

bool monitorCharacters(void* unused) {

    if (BaseX != 0x00) {
        if (Game::playerchar_is_loaded()) {
            if (prev_playerchar_is_loaded == false) {
                if (variablesUpdated) {

                    ConsoleWriteDebug("-monitorCharacters: entered");

                    updateBase();
                    ConsoleWriteDebug("--monitorCharacters: returned from updateBase()");

                    Cheats::applyCheats();
                    ConsoleWriteDebug("--monitorCharacters: returned from Cheats::applyCheats()");

                    P0.update(PlayerBase);
                    P1.update(PlayerBase);
                    P2.update(PlayerBase);
                    P3.update(PlayerBase);
                    P4.update(PlayerBase);
                    P5.update(PlayerBase);
                    ConsoleWriteDebug("--monitorCharacters: returned from P[012345].update(PlayerBase)");

                    //ConsoleWriteDebug("%s --monitorCharacters: Loaded '%ls'", Mod::output_prefix, P0.name());

                    prev_playerchar_is_loaded = true;
                    ConsoleWriteDebug("-monitorCharacters: completed\n");

                }
            }
        }   else {
            prev_playerchar_is_loaded = false;
        }
    }
    return true;
}

void printBytes(uint64_t pointer, short rows_of_eight) {

    uint64_t tmp = pointer;
    //ConsoleWrite("%s printBytes(%X, %d)", Mod::output_prefix, pointer, rows_of_eight);
    char buffer[1] = { 0x00 };
    for (int r = 0; r < rows_of_eight; r++) {
        ConsoleWriteNLF("\n%p: ", pointer + r * 8);
        for (int b = 0; b < 8; b++) {
            tmp = (uint64_t)pointer + (r * 8) + b;
            memcpy(buffer, (void*)tmp, 1);
            ConsoleWriteNLF(" %02hhx", buffer[0]);
        }
    }
    ConsoleWriteNLF("\n");
}

void printByte(uint64_t pointer) {
    char buffer[1] = { 0x00 };
    memcpy(buffer, (void*)pointer, 1);
    ConsoleWriteNLF("\n%p: %02hhx", pointer, (unsigned char)buffer[0]);

}

void printByteRaw(uint64_t pointer) {
    char buffer[1] = { 0x00 };
    memcpy(buffer, (void*)pointer, 1);
    ConsoleWriteNLF("%02u", buffer[0]);
}

byte returnByteRaw(uint64_t pointer) {
    char buffer[1] = { 0x00 };
    memcpy(buffer, (void*)pointer, 1);
    return buffer[1];
}

inline bool bitTest(uint64_t pointer, short bit) {
    //return *((byte*)pointer) & (1 << bit) != 0;
    return _bittest64((const LONG64*)pointer, bit);
}

inline void bitmod(uint64_t ptr, short bit, bool value) {
    if (value) bitset(ptr, bit);
    else bitunset(ptr, bit);
}

inline void bitunset(uint64_t ptr, short bit) {
    *((byte*)ptr) &= ~(1 << bit);
}

inline void bitset(uint64_t ptr, short bit) {
    *((byte*)ptr) |= (1 << bit);
}

inline void bittog(uint64_t ptr, short bit) {
    *((byte*)ptr) ^= (1 << bit);
}

void printPosition() {

    // Lookup XYZ positional floats
    sp::mem::pointer X = sp::mem::pointer((void*)BaseX, { 0x68, 0x18, 0x28, 0x50, 0x20, 0x120 });
    sp::mem::pointer Z = sp::mem::pointer((void*)BaseX, { 0x68, 0x18, 0x28, 0x50, 0x20, 0x124 });
    sp::mem::pointer Y = sp::mem::pointer((void*)BaseX, { 0x68, 0x18, 0x28, 0x50, 0x20, 0x128 });

    float* fX = (float*)X.resolve();
    float* fZ = (float*)Z.resolve();
    float* fY = (float*)Y.resolve();

    // Print and check the addresses?

    ConsoleWrite("X = %.1f", fX);
    ConsoleWrite("Z = %.1f", fZ);
    ConsoleWrite("Y = %.1f", fY);

}

void printPreferences() {

    ConsoleWrite("DisableLowFpsDisconnect = %d", Mod::disable_low_fps_disconnect);
    ConsoleWrite("UseSteamNames = %d", Mod::use_steam_names);
    ConsoleWrite("FixHpBarSize = %d", Mod::fix_hp_bar_size);
    ConsoleWrite("EnableQoLCheats = %d", Mod::enable_qol_cheats);
    ConsoleWrite("VerboseMessages = %d", Mod::enable_verbose_messages);
    ConsoleWrite("DisableLeaveMessages = %d", Mod::disable_leave_messages);
    ConsoleWrite("MessageFileLocation = %s\n", Mod::message_file_location);

}

void Cheats::printMessage() {

    // 9 rows max

    char buf[1000];
    wchar_t dc_msg[1000];
    DWORD dwRead = 0;

    ConsoleWrite(Mod::message_file_location);

    HANDLE hFile = CreateFile(Mod::message_file_location, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        ConsoleWriteDebug("CreateFile error");
        return;
    }

    if (!ReadFile(hFile, buf, 1000, &dwRead, NULL)) {
        ConsoleWriteDebug("ReadFile error");
        return;
    }

    CloseHandle(hFile);

    for (int position = 0; position < dwRead; position++)
        dc_msg[position] = (wchar_t)buf[position];
  
    //swprintf(dc_msg, 1000, L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVXYZ\niiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\niiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\n3\n4\n5\n6\n7\n8\n9\n10\n");
    Game::show_popup_message(dc_msg);

}

void Cheats::printPlayers() {

    if (BaseX && PlayerBase) {

        ConsoleWrite("No. | SL  | Name             | VIT | ATN | END | STR | DEX | RES | FTH | INT | Phantom Type | Time in World");
        for (int p = 0; p < 6; p++) {
            uint64_t Player = CheatsASMFollow(PlayerBase + (p * 0x38));
            if (Player) {

                // Desired Format
                /*
                No. | SL  | Name            | VIT | ATT | END | STR | DEX | RES | FTH | INT | Phantom Type | Time in World
                01  | 125 | Mich            | 50  | 12  | 41  | 28  | 45  | 11  | 10  | 9   | Host         | 1034

                Extras?
                Steam Name
                Steam Profile Link
                Poise?
                Rings?
                Weapons?
                Base Class?
                Ping?
                */
                
                float *Time_in_World = (float*)(CheatsASMFollow(Player + 0x30) + 0x20);

                ConsoleWriteNLF(" %d  |", p);
                ConsoleWriteNLF(" %3u |", (unsigned)*(unsigned char*)(CheatsASMFollow(Player + 0x578) + 0x90));   // SL
                ConsoleWriteNLF(" %-16.16ls | ", CheatsASMFollow(Player + 0x578) + 0xA8);                          // Name
                ConsoleWriteNLF("%-3.2u | ", (unsigned)*(unsigned char*)(CheatsASMFollow(Player + 0x578) + 0x40));  // VIT
                ConsoleWriteNLF("%-3.2u | ", (unsigned)*(unsigned char*)(CheatsASMFollow(Player + 0x578) + 0x48));  // ATN
                ConsoleWriteNLF("%-3.2u | ", (unsigned)*(unsigned char*)(CheatsASMFollow(Player + 0x578) + 0x50));  // END
                ConsoleWriteNLF("%-3.2u | ", (unsigned)*(unsigned char*)(CheatsASMFollow(Player + 0x578) + 0x58));  // STR
                ConsoleWriteNLF("%-3.2u | ", (unsigned)*(unsigned char*)(CheatsASMFollow(Player + 0x578) + 0x60));  // DEX
                ConsoleWriteNLF("%-3.2u | ", (unsigned)*(unsigned char*)(CheatsASMFollow(Player + 0x578) + 0x88));  // RES
                ConsoleWriteNLF("%-3.2u | ", (unsigned)*(unsigned char*)(CheatsASMFollow(Player + 0x578) + 0x70));  // FTH
                ConsoleWriteNLF("%-3.2u | ", (unsigned)*(unsigned char*)(CheatsASMFollow(Player + 0x578) + 0x78));  // INT

                ConsoleWriteNLF(" %u | ", (unsigned)*(unsigned char*)(CheatsASMFollow(Player + 0x578) + 0xA4));   // SummonType
                // Maybe a LUT or enum for this?
                ConsoleWriteNLF(" %.0fs |\n", *(float*)(CheatsASMFollow(Player + 0x30) + 0x20));

            }
            else {
                //ConsoleWrite("%s Player %d is not populated", Mod::output_prefix, p);
            }
        }
    }
}

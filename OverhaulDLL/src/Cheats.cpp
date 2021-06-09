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

void printBytes(uint64_t pointer, short rows_of_eight);
void printByte(uint64_t pointer);

inline bool bitTest(uint64_t pointer, short bit);
inline void bitmod(uint64_t ptr, short bit, bool value);
inline void bitunset(uint64_t ptr, short bit);
inline void bitset(uint64_t ptr, short bit);
inline void bittog(uint64_t ptr, short bit);

bool monitorCharacters(void* unused);
bool delayedVariableUpdateWrapper(void* unused);
void delayedVariableUpdate();
void printPreferences();

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

Cheats::Cheats()
{
}

Cheats::~Cheats()
{
}

void Cheats::start() {

    ConsoleWrite("%s -Cheats::start: entered", Mod::output_prefix);

    // Initialise BaseXOffset
    BaseXOffset = (uint64_t)sp::mem::aob_scan("48 8B 05 ?? ?? ?? ?? 48 39 48 68 0F 94 C0 C3");
    ConsoleWrite("%s --Cheats::start: BaseXOffset = 0x%X", Mod::output_prefix, BaseXOffset);

    // Initialise BaseBOffset
    BaseBOffset = (uint64_t)sp::mem::aob_scan("48 8B 05 ?? ?? ?? ?? 45 33 ED 48 8B F1 48 85 C0");
    ConsoleWrite("%s --Cheats::start: BaseBOffset = 0x%X", Mod::output_prefix, BaseBOffset);

    // Initialise BasePOffset
    BasePOffset = (uint64_t)sp::mem::aob_scan("4C 8B 05 ?? ?? ?? ?? 48 63 C9 48 8D 04 C9");
    ConsoleWrite("%s --Cheats::start: BasePOffset = 0x%X", Mod::output_prefix, BasePOffset);

    // Initialise Homeward
    Homeward = (uint64_t)sp::mem::aob_scan("48 89 5C 24 08 57 48 83 EC 20 48 8B D9 8B FA 48 8B 49 08 48 85 C9 0F 84 ? ? ? ? E8 ? ? ? ? 48 8B 4B 08");
    ConsoleWrite("%s --Cheats::start: Homeward = 0x%X", Mod::output_prefix, Homeward);

    // Runs until a character is loaded, then updates a couple of pointers and never runs again
    MainLoop::setup_mainloop_callback(delayedVariableUpdateWrapper, NULL, "delayedVariableUpdate");

    // Runs continiously and calls other functions when a character is loaded
    MainLoop::setup_mainloop_callback(monitorCharacters, NULL, "monitorCharacters");

    ConsoleWrite("%s -Cheats::start: completed");
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

    ConsoleWriteDebug("%s -RedEyeOrb = 0x%X", Mod::output_prefix, RedEyeOrb);

    ConsoleWriteDebug("%s --RedEyeOrb: unrestrict_patch     = 0x%X", Mod::output_prefix, RedEyeOrb + 0x42);
    ConsoleWriteDebug("%s --RedEyeOrb: modify_use_animation = 0x%X", Mod::output_prefix, RedEyeOrb + 0x3E);
    ConsoleWriteDebug("%s --RedEyeOrb: opmeMenuType_patch   = 0x%X", Mod::output_prefix, RedEyeOrb + 0x3F);

    memcpy((void*)(RedEyeOrb + 0x42), unrestrict_patch, 3);
    memcpy((void*)(RedEyeOrb + 0x3E), modify_use_animation, 1);
    memcpy((void*)(RedEyeOrb + 0x3F), opmeMenuType_patch, 1);   // Could be combined with the "modify_use_animation" patch
    ConsoleWriteDebug("%s --RedEyeOrb: no menu, no restrictions, different animation", Mod::output_prefix);
}

void EyeOfDeath() {

    // ID = 109; Offset = F18
    byte unrestrict_patch[3] = { 0xff, 0xff, 0x63 };// Allow use while hollow
    byte modify_use_animation[1] = { 0x0E };        // Silver Pendant animation
    byte opmeMenuType_patch[1] = { 0x00 };          // Disable dialog on use

    uint64_t EyeOfDeath = CheatsASMFollow(BaseP + 0xF0);
    EyeOfDeath = CheatsASMFollow(EyeOfDeath + 0x38);
    EyeOfDeath = EyeOfDeath + 0xF18;

    ConsoleWriteDebug("%s -EyeOfDeath = 0x%X", Mod::output_prefix, EyeOfDeath);

    ConsoleWriteDebug("%s --EyeOfDeath: unrestrict_patch     = 0x%X", Mod::output_prefix, EyeOfDeath + 0x42);
    ConsoleWriteDebug("%s --EyeOfDeath: modify_use_animation = 0x%X", Mod::output_prefix, EyeOfDeath + 0x3E);
    ConsoleWriteDebug("%s --EyeOfDeath: opmeMenuType_patch   = 0x%X", Mod::output_prefix, EyeOfDeath + 0x3F);

    memcpy((void*)(EyeOfDeath + 0x42), unrestrict_patch, 3);
    memcpy((void*)(EyeOfDeath + 0x3E), modify_use_animation, 1);
    memcpy((void*)(EyeOfDeath + 0x3F), opmeMenuType_patch, 1);
    ConsoleWriteDebug("%s --EyeOfDeath: no menu, no restrictions, different animation", Mod::output_prefix);
}

void GreenBlossom() {

    // ID = 260; Offset = 1EE8
    byte unrestrict_use_limit[1] = { 0x00 };

    uint64_t GreenBlossom = CheatsASMFollow(BaseP + 0xF0);
    GreenBlossom = CheatsASMFollow(GreenBlossom + 0x38);
    GreenBlossom = GreenBlossom + 0x1EE8;

    ConsoleWriteDebug("%s -GreenBlossom = 0x%X", Mod::output_prefix, GreenBlossom);

    ConsoleWriteDebug("%s --GreenBlossom: unrestrict_use_limit = 0x%X", Mod::output_prefix, GreenBlossom + 0x3B);

    memcpy((void*)(GreenBlossom + 0x3B), unrestrict_use_limit, 1);
    ConsoleWriteDebug("%s --GreenBlossom: no restrictions", Mod::output_prefix);
}

void RedSignSoapstone() {

    //ID = 101; Offset = D4C
    byte unrestrict_patch[3] = { 0xff, 0xff, 0x63 };

    uint64_t RedSignSoapstone = CheatsASMFollow(BaseP + 0xF0);
    RedSignSoapstone = CheatsASMFollow(RedSignSoapstone + 0x38);
    RedSignSoapstone = RedSignSoapstone + 0xD4C;

    ConsoleWriteDebug("%s -RedSignSoapstone = 0x%X", Mod::output_prefix, RedSignSoapstone);

    ConsoleWriteDebug("%s --RedSignSoapstone: unrestrict_patch = 0x%X", Mod::output_prefix, RedSignSoapstone + 0x42);

    memcpy((void*)(RedSignSoapstone + 0x42), unrestrict_patch, 3);
    ConsoleWriteDebug("%s --RedSignSoapstone: no restrictions", Mod::output_prefix);
}

void CrackedRedEyeOrb() {

    // ID = 111; Offset = F74
    byte unrestrict_patch[3] = { 0xff, 0xff, 0x63 };
    byte opmeMenuType_patch[1] = { 0x00 };
    /* first two bytes are "vowType"s
    bit                  0x63   0xC3    0xBF
    0 = enable_live        1      1       1
    1 = enable_grey        1      1       1
    2 = enable_white       0      0       1
    3 = enable_black       0      0       1
    4 = enable_multi       0      0       1
    5 = disable_offline    1      0       1
    6 = isEquip            1      1       0
    7 = isConsume          0      1       1
    */

    uint64_t CrackedRedEyeOrb = CheatsASMFollow(BaseP + 0xF0);
    CrackedRedEyeOrb = CheatsASMFollow(CrackedRedEyeOrb + 0x38);
    CrackedRedEyeOrb = CrackedRedEyeOrb + 0xF74;

    ConsoleWriteDebug("%s -CrackedRedEyeOrb = 0x%X", Mod::output_prefix, CrackedRedEyeOrb);

    ConsoleWriteDebug("%s --CrackedRedEyeOrb: unrestrict_patch     = 0x%X", Mod::output_prefix, CrackedRedEyeOrb + 0x42);
    ConsoleWriteDebug("%s --CrackedRedEyeOrb: opmeMenuType_patch   = 0x%X", Mod::output_prefix, CrackedRedEyeOrb + 0x3F);

    memcpy((void*)(CrackedRedEyeOrb + 0x42), unrestrict_patch, 3);
    memcpy((void*)(CrackedRedEyeOrb + 0x3F), opmeMenuType_patch, 1);
    ConsoleWriteDebug("%s --CrackedRedEyeOrb: no menu, no restrictions", Mod::output_prefix);
}

void Humanity() {

    // ID = 500; Offset = 313C
    byte unrestrict_patch[3] = { 0xff, 0xff, 0xbf };

    uint64_t Humanity = CheatsASMFollow(BaseP + 0xF0);
    Humanity = CheatsASMFollow(Humanity + 0x38);
    Humanity = Humanity + 0x313C;

    ConsoleWriteDebug("%s -Humanity = 0x%X", Mod::output_prefix, Humanity);

    ConsoleWriteDebug("%s --Humanity: unrestrict_patch = 0x%X", Mod::output_prefix, Humanity + 0x42);
   
    memcpy((void*)(Humanity + 0x42), unrestrict_patch, 3);
    ConsoleWriteDebug("%s --Humanity: no restrictions", Mod::output_prefix);
}

void TwinHumanity() {

    // ID = 501; Offset = 3198
    byte unrestrict_patch[3] = { 0xff, 0xff, 0xbf };

    uint64_t TwinHumanity = CheatsASMFollow(BaseP + 0xF0);
    TwinHumanity = CheatsASMFollow(TwinHumanity + 0x38);
    TwinHumanity = TwinHumanity + 0x3198;

    ConsoleWriteDebug("%s -TwinHumanity = 0x%X", Mod::output_prefix, TwinHumanity);

    ConsoleWriteDebug("%s --TwinHumanity: unrestrict_patch = 0x%X", Mod::output_prefix, TwinHumanity + 0x42);

    memcpy((void*)(TwinHumanity + 0x42), unrestrict_patch, 3);

    ConsoleWriteDebug("%s --TwinHumanity: no restrictions", Mod::output_prefix);
}

void DivineBlessing() {

    // ID = 240; Offset = 1E8C
    byte unrestrict_patch[3] = { 0xff, 0xff, 0xbf };

    uint64_t DivineBlessing = CheatsASMFollow(BaseP + 0xF0);
    DivineBlessing = CheatsASMFollow(DivineBlessing + 0x38);
    DivineBlessing = DivineBlessing + 0x1E8C;

    ConsoleWriteDebug("%s -DivineBlessing = 0x%X", Mod::output_prefix, DivineBlessing);

    ConsoleWriteDebug("%s --DivineBlessing: unrestrict_patch = 0x%X", Mod::output_prefix, DivineBlessing + 0x42);

    memcpy((void*)(DivineBlessing + 0x42), unrestrict_patch, 3);
    ConsoleWriteDebug("%s --DivineBlessing: no restrictions", Mod::output_prefix);
}

void ElizabethMushroom() {

    // ID = 230; Offset = 1E30
    byte unrestrict_patch[3] = { 0xff, 0xff, 0xbf };

    uint64_t ElizabethMushroom = CheatsASMFollow(BaseP + 0xF0);
    ElizabethMushroom = CheatsASMFollow(ElizabethMushroom + 0x38);
    ElizabethMushroom = ElizabethMushroom + 0x1E30;

    ConsoleWriteDebug("%s -ElizabethMushroom = 0x%X", Mod::output_prefix, ElizabethMushroom);

    ConsoleWriteDebug("%s --ElizabethMushroom: unrestrict_patch = 0x%X", Mod::output_prefix, ElizabethMushroom + 0x42);

    memcpy((void*)(ElizabethMushroom + 0x42), unrestrict_patch, 3);
    ConsoleWriteDebug("%s --ElizabethMushroom: no restrictions", Mod::output_prefix);
}

void HomewardBone() {

    // ID = 330; Offset = 2560
    byte unrestrict_patch[3] = { 0xff, 0xff, 0xbf };// Allow use while online
    byte modify_use_animation[1] = { 0x0E };        // Silver Pendant animation
    byte opmeMenuType_patch[1] = { 0x00 };          // Disable dialog on use

    uint64_t HomewardBone = CheatsASMFollow(BaseP + 0xF0);
    HomewardBone = CheatsASMFollow(HomewardBone + 0x38);
    HomewardBone = HomewardBone + 0x2560;

    ConsoleWriteDebug("%s -HomewardBone = 0x%X", Mod::output_prefix, HomewardBone);

    ConsoleWriteDebug("%s --HomewardBone: unrestrict_patch     = 0x%X", Mod::output_prefix, HomewardBone + 0x42);
    ConsoleWriteDebug("%s --HomewardBone: modify_use_animation = 0x%X", Mod::output_prefix, HomewardBone + 0x3E);
    ConsoleWriteDebug("%s --HomewardBone: opmeMenuType_patch   = 0x%X", Mod::output_prefix, HomewardBone + 0x3F);

    memcpy((void*)(HomewardBone + 0x42), unrestrict_patch, 3);
    memcpy((void*)(HomewardBone + 0x3E), modify_use_animation, 1);
    memcpy((void*)(HomewardBone + 0x3F), opmeMenuType_patch, 1);
    ConsoleWriteDebug("%s --HomewardBone: no menu, no restrictions, different animation", Mod::output_prefix);
}

void DriedFinger() {

    // ID = 385; Offset = 2AC4;
    byte modify_use_animation[1] = { 0x0E };        // Silver Pendant animation
    byte modify_sfx_variation[4] = { 0x5C };        // Purple Cowards Crystal SFX

    uint64_t DriedFinger = CheatsASMFollow(BaseP + 0xF0);
    DriedFinger = CheatsASMFollow(DriedFinger + 0x38);
    DriedFinger = DriedFinger + 0x2AC4;
    ConsoleWriteDebug("%s -DriedFinger = 0x%X", Mod::output_prefix, DriedFinger);

    ConsoleWriteDebug("%s --DriedFinger: modify_use_animation = 0x%X", Mod::output_prefix, DriedFinger + 0x3E);
    ConsoleWriteDebug("%s --DriedFinger: modify_sfx_animation = 0x%X", Mod::output_prefix, DriedFinger + 0x04);
   
    memcpy((void*)(DriedFinger + 0x3E), modify_use_animation, 1);
    memcpy((void*)(DriedFinger + 0x04), modify_sfx_variation, 1);
    ConsoleWriteDebug("%s --DriedFinger: different animation, different SFX", Mod::output_prefix);
}

void CharcoalPineResin() {

    // ID = 310; Offset = 23F0
    byte modify_use_animation[1] = { 0x00 };        // Default use animation (Green Blossom style)

    uint64_t CharcoalPineResin = CheatsASMFollow(BaseP + 0xF0);
    CharcoalPineResin = CheatsASMFollow(CharcoalPineResin + 0x38);
    CharcoalPineResin = CharcoalPineResin + 0x23F0;
    ConsoleWriteDebug("%s -CharcoalPineResin = 0x%X", Mod::output_prefix, CharcoalPineResin);

    ConsoleWriteDebug("%s --CharcoalPineResin: modify_use_animation = 0x%X", Mod::output_prefix, CharcoalPineResin + 0x3E);

    memcpy((void*)(CharcoalPineResin + 0x3E), modify_use_animation, 1);
    ConsoleWriteDebug("%s --CharcoalPineResin: different animation", Mod::output_prefix);

}

void RottenPineResin() {

    // ID = 313; Offset = 2504
    byte modify_use_animation[1] = { 0x00 };        // Default use animation (Green Blossom style)

    uint64_t RottenPineResin = CheatsASMFollow(BaseP + 0xF0);
    RottenPineResin = CheatsASMFollow(RottenPineResin + 0x38);
    RottenPineResin = RottenPineResin + 0x2504;
    ConsoleWriteDebug("%s -RottenPineResin = 0x%X", Mod::output_prefix, RottenPineResin);

    ConsoleWriteDebug("%s --RottenPineResin: modify_use_animation = 0x%X", Mod::output_prefix, RottenPineResin + 0x3E);

    memcpy((void*)(RottenPineResin + 0x3E), modify_use_animation, 1);
    ConsoleWriteDebug("%s --RottenPineResin: different animation", Mod::output_prefix);
}

void GoldPineResin() {

    // ID = 311; Offset = 244C
    byte modify_use_animation[1] = { 0x00 };        // Default use animation (Green Blossom style)

    uint64_t GoldPineResin = CheatsASMFollow(BaseP + 0xF0);
    GoldPineResin = CheatsASMFollow(GoldPineResin + 0x38);
    GoldPineResin = GoldPineResin + 0x244C;
    ConsoleWriteDebug("%s -GoldPineResin = 0x%X", Mod::output_prefix, RottenPineResin);

    ConsoleWriteDebug("%s --GoldPineResin: modify_use_animation = 0x%X", Mod::output_prefix, GoldPineResin + 0x3E);

    memcpy((void*)(GoldPineResin + 0x3E), modify_use_animation, 1);
    ConsoleWriteDebug("%s --GoldPineResin: different animation", Mod::output_prefix);
}

// noGoodsConsume
void noGoodsConsumeToggle() {
    Cheats::noGoodsConsume.enabled = !Cheats::noGoodsConsume.enabled;
    ConsoleWrite("%s Cheats: noGoodsConsume = %s", Mod::output_prefix, noGoodsConsumeSet(Cheats::noGoodsConsume.enabled) ? "Active" : "Inactive");
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
        //ConsoleWrite("%s noGoodsConsume = %s", Mod::output_prefix, bitTest(noGoodsConsume, 0) ? "On" : "Off");
        return bitTest(noGoodsConsume, 0);
    }
    return -1;
}

// noArrowConsume
void noArrowConsumeToggle() {
    Cheats::noArrowConsume.enabled = !Cheats::noArrowConsume.enabled;
    ConsoleWrite("%s Cheats: noArrowConsume = %s", Mod::output_prefix, noArrowConsumeSet(Cheats::noArrowConsume.enabled) ? "Active" : "Inactive");
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
        //ConsoleWrite("%s noArrowConsume = %s", Mod::output_prefix, bitTest(noArrowConsume, 0) ? "On" : "Off");
        return bitTest(noArrowConsume, 0);
    }
    return -1;
}

// noMagicConsume
void noMagicConsumeToggle() {
    Cheats::noMagicConsume.enabled = !Cheats::noMagicConsume.enabled;
    ConsoleWrite("%s Cheats: noMagicConsume = %s", Mod::output_prefix, noMagicConsumeSet(Cheats::noMagicConsume.enabled) ? "Active" : "Inactive");
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
        //ConsoleWrite("%s noMagicConsume = %s", Mod::output_prefix, bitTest(noMagicConsume, 0) ? "On" : "Off");
        return bitTest(noMagicConsume, 0);
    }
    return -1;
}

// noDead
void noDeadToggle() {
    Cheats::noDead.enabled = !Cheats::noDead.enabled;
    ConsoleWrite("%s Cheats: noDead = %s", Mod::output_prefix, noDeadSet(Cheats::noDead.enabled) ? "Active" : "Inactive");
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
        //ConsoleWrite("%s noDead = %s", Mod::output_prefix, bitTest(noDead, 0) ? "On" : "Off");
        return bitTest(noDead, 0);
    }
    return -1;
}

// eventSuperArmor
void eventSuperArmorToggle() {
    Cheats::eventSuperArmor.enabled = !Cheats::eventSuperArmor.enabled;
    ConsoleWrite("%s Cheats: eventSuperArmor = %s", Mod::output_prefix, eventSuperArmorSet(Cheats::eventSuperArmor.enabled) ? "Active" : "Inactive");
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
        //ConsoleWrite("%s eventSuperArmor = %s", Mod::output_prefix, bitTest(eventSuperArmor, 0) ? "On" : "Off");
        return bitTest(eventSuperArmor, 0);
    }
    return -1;
}

// noUpdateAI
void noUpdateAIToggle() {
    Cheats::noUpdateAI.enabled = !Cheats::noUpdateAI.enabled;
    ConsoleWrite("%s Cheats: noUpdateAI = %s", Mod::output_prefix, noUpdateAISet(Cheats::noUpdateAI.enabled) ? "Active" : "Inactive");
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
        //ConsoleWrite("%s noUpdateAI = %s", Mod::output_prefix, bitTest(noUpdateAI, 0) ? "On" : "Off");
        return bitTest(noUpdateAI, 0);
    }
    return -1;
}

// noGravity
void noGravityToggle() {
    Cheats::noGravity.enabled = !Cheats::noGravity.enabled;
    ConsoleWrite("%s Cheats: noGravity = %s", Mod::output_prefix, noGravitySet(Cheats::noGravity.enabled) ? "Active" : "Inactive");
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
        //ConsoleWrite("%s noGravity = %s", Mod::output_prefix, bitTest(noGravity, 6) ? "On" : "Off");
        return bitTest(noGravity, 6);
    }
    return -1;
}

// noHUD
void noHUDToggle() {
    Cheats::noHUD.enabled = !Cheats::noHUD.enabled;
    ConsoleWrite("%s Cheats: noHUD = %s", Mod::output_prefix, noHUDSet(Cheats::noHUD.enabled) ? "Inactive" : "Active");
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
        //ConsoleWrite("%s noHUD = %s", Mod::output_prefix, bitTest(noHUD, 0) ? "On" : "Off");
        return bitTest(noHUD, 0);
    }
    return -1;
}

// No Collision
// Fly mode

void reviveChar() {
    ConsoleWrite("%s reviveChar()", Mod::output_prefix);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheatsASMReviveCharWrapper, 0, 0, 0);
}

void hollowChar() {

    ConsoleWrite("%s hollowChar()", Mod::output_prefix);

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

    ConsoleWrite("%s warp()", Mod::output_prefix);

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

    ConsoleWrite("%s kick(%d)", Mod::output_prefix, player);

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

    ConsoleWrite("%s stopDurabilityDamage()", Mod::output_prefix);

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

}

bool delayedVariableUpdateWrapper(void* unused) {
    if (Game::playerchar_is_loaded() && variablesUpdated == false) {
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)delayedVariableUpdate, 0, 0, 0);
        return false;
    }
    return true;
}

void delayedVariableUpdate() {

    ConsoleWriteDebug("%s -delayedVariableUpdate: entered", Mod::output_prefix);

    // Ensure pointers are initialised
    updateBase();

    // Initialise debug_flagsOffset
    debug_flagsOffset = (uint64_t)sp::mem::aob_scan("4C 8D 05 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 41 B1 01 4C 8D 05 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8B 8F 10 01 00 00 E8");
    ConsoleWrite("%s --delayedVariableUpdate: debug_flagsOffset = 0x%X", Mod::output_prefix, debug_flagsOffset);

    // debug_flags
    debug_flags = (uint64_t)debug_flagsOffset + *(uint32_t*)((uint64_t)debug_flagsOffset + 3) + 7;
    ConsoleWrite("%s --delayedVariableUpdate: debug_flags = 0x%X", Mod::output_prefix, debug_flags);

    // One-time initialisation
    P0 = Character::initialise(P0, PlayerBase, 0);
    P1 = Character::initialise(P0, PlayerBase, 1);
    P2 = Character::initialise(P0, PlayerBase, 2);
    P3 = Character::initialise(P0, PlayerBase, 3);
    P4 = Character::initialise(P0, PlayerBase, 4);
    P5 = Character::initialise(P0, PlayerBase, 5);


    // Problem with BaseP here, can no longer find the correct AOB
   if (!BasePOffset) {
        BasePOffset = (uint64_t)sp::mem::aob_scan("4C 8B 05 ?? ?? ?? ?? 48 63 C9 48 8D 04 C9");
    }
    ConsoleWriteDebug("%s --delayedVariableUpdate: BasePOffset = 0x%X", Mod::output_prefix, BasePOffset);
    
    BaseP = (uint64_t)BasePOffset + *(uint32_t*)((uint64_t)BasePOffset + 3) + 7;
    ConsoleWriteDebug("%s --delayedVariableUpdate: BaseP = 0x%X", Mod::output_prefix, BaseP);
    
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
        stopDurabilityDamage();
    }

    variablesUpdated = true;
    ConsoleWrite("%s -delayedVariableUpdate: completed", Mod::output_prefix);

}

void updateBase() {

    ConsoleWriteDebug("%s -updateBase: entered", Mod::output_prefix);

    // TODO Confirm that the follow pointers NEED updates after each reload

    // BaseX
    BaseX = CheatsASMFollow((uint64_t)BaseXOffset + *(uint32_t*)((uint64_t)BaseXOffset + 3) + 7);
    ConsoleWriteDebug("%s --updateBase: BaseX = 0x%X", Mod::output_prefix, BaseX);

    // BaseB
    BaseB = CheatsASMFollow((uint64_t)BaseBOffset + *(uint32_t*)((uint64_t)BaseBOffset + 3) + 7);
    ConsoleWriteDebug("%s --updateBase: BaseB = 0x%X", Mod::output_prefix, BaseB);

    // BaseP
    if (BasePOffset == 0x00) {
        BasePOffset = (uint64_t)sp::mem::aob_scan("4C 8B 05 ?? ?? ?? ?? 48 63 C9 48 8D 04 C9");
    }
    BaseP = CheatsASMFollow((uint64_t)BasePOffset + *(uint32_t*)((uint64_t)BasePOffset + 3) + 7);
    ConsoleWriteDebug("%s --updateBase: BaseP = 0x%X", Mod::output_prefix, BaseP);
    
    // PlayerBase
    PlayerBase = CheatsASMFollow(BaseX + 0x68);
    PlayerBase = CheatsASMFollow(PlayerBase + 0x18);
    ConsoleWriteDebug("%s --updateBase: PlayerBase = 0x%X", Mod::output_prefix, PlayerBase);

    // For pointers that need updating after load screens
    ConsoleWrite("%s -updateBase: completed");
}

bool monitorCharacters(void* unused) {

    if (BaseX != 0x00) {
        if (Game::playerchar_is_loaded()) {
            if (prev_playerchar_is_loaded == false) {
                if (variablesUpdated) {

                    ConsoleWrite("%s monitorCharacters: Updating pointers and applying cheats", Mod::output_prefix);

                    updateBase();
                    Cheats::applyCheats();

                    P0.update(PlayerBase);
                    P1.update(PlayerBase);
                    P2.update(PlayerBase);
                    P3.update(PlayerBase);
                    P4.update(PlayerBase);
                    P5.update(PlayerBase);

                    ConsoleWrite("%s Player Character is %ls", Mod::output_prefix, P0.name());

                    prev_playerchar_is_loaded = true;
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

void printPreferences() {

    ConsoleWrite("%s DisableLowFpsDisconnect = %d", Mod::output_prefix, Mod::disable_low_fps_disconnect);
    ConsoleWrite("%s UseSteamNames = %d", Mod::output_prefix, Mod::use_steam_names);
    ConsoleWrite("%s FixHpBarSize = %d", Mod::output_prefix, Mod::fix_hp_bar_size);
    ConsoleWrite("%s EnableQoLCheats = %d", Mod::output_prefix, Mod::enable_qol_cheats);

}

void Cheats::printPlayers() {

    if (BaseX && PlayerBase) {

        for (int p = 0; p < 6; p++) {
            uint64_t Player = CheatsASMFollow(PlayerBase + (p * 0x38));
            if (Player) {
                ConsoleWrite("%s Player %d is %ls", Mod::output_prefix, p, (CheatsASMFollow(Player + 0x578) + 0xA8));
                // Add phantom type?
                // Time in world?
                // Attributes?
                // Character class?
            }
            else {
                ConsoleWrite("%s Player %d is not populated", Mod::output_prefix, p);
            }
        }
    }
}

#pragma once

#ifndef _DS1_OVERHAUL_CHEATS_H_
#define _DS1_OVERHAUL_CHEATS_H_


typedef int(*CheatFunc)(bool state);

struct CHEAT {
    bool enabled = false;
};

struct COORDINATES {
    float x = 0.0;
    float z = 0.0;
    float y = 0.0;
};

class Cheats
{
private:

public:

    // Constructors/destructor
    Cheats();
    ~Cheats();

    // Cheats
    static void start();
    static void teleport(COORDINATES);

    // Variables
    COORDINATES Burg            = { 0.0, 0.0, 0.0 };
    COORDINATES Parish          = { 0.0, 0.0, 0.0 };
    COORDINATES SecretSens      = { 0.0, 0.0, 0.0 };
    COORDINATES Kiln            = { 0.0, 0.0, 0.0 };
    COORDINATES MoonlightForest = { 0.0, 0.0, 0.0 };
        
    static std::vector<CHEAT> CHEATS;

    // Functions
    static bool applyCheats();
    static void printPlayers();
    static void printMessage();

    // Cheats
    static CHEAT noGoodsConsume;    // ToggleGoodsConsume
    static CHEAT noArrowConsume;    // ToggleArrowConsume
    static CHEAT noMagicConsume;    // ToggleMagicConsume
    static CHEAT noDead;            // ToggleUndead
    static CHEAT eventSuperArmor;   // ToggleEventSuperArmor
    static CHEAT noUpdateAI;        // ToggleUpdateAI
    static CHEAT noGravity;         // ToggleGravity
    static CHEAT noHUD;             // ToggleHUD

};

void RedEyeOrb();
void EyeOfDeath();
void GreenBlossom();
void RedSignSoapstone();
void CrackedRedEyeOrb();
void Humanity();
void TwinHumanity();
void DivineBlessing();
void ElizabethMushroom();
void HomewardBone();
void DriedFinger();
void CharcoalPineResin();
void RottenPineResin();
void GoldPineResin();
void RepairPowder();
void EstusFlask();
void EggVermifuge();
void GoldCoin();
void PerculiarDoll();
void RingOfFavorAndProtection();
void LoadingScreenItems();

void noGoodsConsumeToggle();
void noGoodsConsumeApply();
int noGoodsConsumeSet(bool state);

void noArrowConsumeToggle();
void noArrowConsumeApply();
int noArrowConsumeSet(bool state);

void noMagicConsumeToggle();
void noMagicConsumeApply();
int noMagicConsumeSet(bool state);

void noDeadToggle();
void noDeadApply();
int  noDeadSet(bool state);

void eventSuperArmorToggle();
void eventSuperArmorApply();
int eventSuperArmorSet(bool state);

void noUpdateAIToggle();
void noUpdateAIApply();
int noUpdateAISet(bool state);

void noGravityToggle();
void noGravityApply();
int noGravitySet(bool state);

void noHUDToggle();
void noHUDApply();
int noHUDSet(bool state);

void replenishSpells();
void reviveChar();
void hollowChar();
void warp();
void stopDurabilityDamage();

void updateBase();

#endif // _DS1_OVERHAUL_CHEATS_H_

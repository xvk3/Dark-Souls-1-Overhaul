#include "Character.h"
#include "DarkSoulsOverhaulMod.h"

extern "C" {
    uint64_t CheatsASMFollow(uint64_t pointer);
}

Character::Character(int foo) {

}

Character::~Character() {

}

Character Character::initialise(Character Char, uint64_t PlayerBase, short index) {

    ConsoleWrite("%s initialising character with index %d", Mod::output_prefix, index);

    Char._rawBase = PlayerBase;
    Char._base    = CheatsASMFollow(PlayerBase + (index * character_index_to_offset_multiplier));
    Char._index   = index;

    return Char;
}


Character* Character::update(uint64_t PlayerBase) {

    //ConsoleWrite("%s update Character with index %d", Mod::output_prefix, this->_index);

    this->_base = CheatsASMFollow(this->_rawBase + (this->_index * character_index_to_offset_multiplier));

    return this;
}

char* Character::name() {
    return (char*)CheatsASMFollow(this->_base + 0x578) + 0xAB;
}

char* name(uint64_t Player) {
    return (char*)CheatsASMFollow(Player + 0x578) + 0xA8;
}

char* Character::steam_name() {
    return (char*)CheatsASMFollow(this->_base + 0x590) + 0x58;
}

int Character::maxHP() {
    return this->_maxHP = *(int*)CheatsASMFollow(this->_base + 0x578) + 0x18;
}

int Character::curHP() {
    return this->_curHP = *(int*)CheatsASMFollow(this->_base + 0x578) + 0x14;
}
#pragma once

#ifndef CHAR_DSR_CHARACTER_CLASS_H_
#define CHAR_DSR_CHARACTER_CLASS_H_

#include <stdint.h>

struct ATTRIBUTES {
    char VIT;
    char ATN;
    char END;
    char STR;
    char DEX;
    char RES;
    char INT;
    char FTH;
};

class Character
{

public:

    uint64_t _rawBase = NULL;
    uint64_t _base    = NULL;
    short    _index   = 0x00;
    char*    _name    = NULL;

    ATTRIBUTES _attributes = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    int      _maxHP   = 0x00;
    int      _curHP   = 0x00;
    int      _SL      = 0x00;

    static const short character_index_to_offset_multiplier = 0x38;

    Character(int foo);
    ~Character();

    static Character initialise(Character Char, uint64_t PlayerBase, short index);
    Character* update(uint64_t PlayerBase);

    // Accessors
    uint64_t base() { return this->_base; }
    char index() { return this->_index; }

    int SL() { return this->_SL; }
    char VIT() { return this->_attributes.VIT; }
    char ATN() { return this->_attributes.ATN; }
    char END() { return this->_attributes.END; }
    char STR() { return this->_attributes.STR; }
    char DEX() { return this->_attributes.DEX; }
    char RES() { return this->_attributes.RES; }
    char INT() { return this->_attributes.INT; }
    char FTH() { return this->_attributes.FTH; }

    int maxHP();
    int curHP();

    char* name();

};


#endif // CHAR_DSR_CHARACTER_CLASS_H_

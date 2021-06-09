_DATA SEGMENT

sub_140BCEDB0   dq  140BCEDB0h
set_value_in_MemberFlags_array1   dq  140BA8400h
set_value_in_MemberFlags_array2   dq  140ba80d0h
set_value_in_MemberFlags_array4   dq  140ba9dc0h
set_value_in_MemberFlags_array5   dq  140BA8730h
const0_float    dd  0.0
const0    dd  0
defaultPyroEquipLoad    dd  51.0
pyro_normal_defs    dd  20, 20, 20, 20, 13, 21, 16
pyro_normal_resists dd  44, 42, 30
max_covenant_levels dd  100, 100, 100, 100, 100, 100, 100
New_Name_110_const  db  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
New_Name_111_const  dd  0, 0, 0, 0, 0, 0, 0

_DATA ENDS

_TEXT SEGMENT

extern game_write_playerdata_to_flatbuffer_injection_helper: PROC
extern game_write_playerdata_to_flatbuffer_injection_return: qword

PUBLIC game_write_playerdata_to_flatbuffer_injection
game_write_playerdata_to_flatbuffer_injection PROC

;save temp registers
sub     rsp, 10h
movdqu  [rsp], xmm0
sub     rsp, 10h
movdqu  [rsp], xmm1
sub     rsp, 10h
movdqu  [rsp], xmm2
sub     rsp, 10h
movdqu  [rsp], xmm3
push    rax
push    rcx
push    rdx
push    r8
;push    r9
push    r10
push    r11
sub     rsp, 20h

mov     rdx, r9 ;array length
mov     rcx, r8 ;array start
call    game_write_playerdata_to_flatbuffer_injection_helper
mov     r9, rax ;save over the length

add     rsp, 20h
pop     r11
pop     r10
;pop     r9
pop     r8
pop     rdx
pop     rcx
pop     rax
movdqu  xmm3, [rsp]
add     rsp, 10h
movdqu  xmm2, [rsp]
add     rsp, 10h
movdqu  xmm1, [rsp]
add     rsp, 10h
movdqu  xmm0, [rsp]
add     rsp, 10h

;original code
lea     rdx, [rsp+24h]
mov     rcx, rbx
call    qword ptr [sub_140BCEDB0]

jmp     game_write_playerdata_to_flatbuffer_injection_return

game_write_playerdata_to_flatbuffer_injection ENDP


extern PlayerStat_ClearCount_injection_return: qword
PUBLIC PlayerStat_ClearCount_injection
PlayerStat_ClearCount_injection PROC
;set the clear count to 1
;so players can have all NG+ stuff without being detected as banned
mov     eax, 1
;original code
mov     r8d, eax
mov     edx, 1
mov     rcx, rdi
jmp     PlayerStat_ClearCount_injection_return
PlayerStat_ClearCount_injection ENDP

extern PlayerStat_New_Name_5_injection_return: qword
PUBLIC PlayerStat_New_Name_5_injection
PlayerStat_New_Name_5_injection PROC
;unknown. 0 seems to be normal
mov     r8d, 0
;original code
lea     edx, [r12+3]
mov     rcx, rdi
jmp     PlayerStat_New_Name_5_injection_return
PlayerStat_New_Name_5_injection ENDP

extern PlayerStat_New_Name_7_injection_return: qword
PUBLIC PlayerStat_New_Name_7_injection
PlayerStat_New_Name_7_injection PROC
;unknown. 0xFDE80 seems to be normal
mov     eax, 0FDE80h
;original code
mov     r8d, eax
lea     edx, [r12+5]
mov     rcx, rdi
jmp     PlayerStat_New_Name_7_injection_return
PlayerStat_New_Name_7_injection ENDP

extern PlayerStat_SoulCount_injection_return: qword
PUBLIC PlayerStat_SoulCount_injection
PlayerStat_SoulCount_injection PROC
;a healthy 2 billion (enough to level up to max)
;this fits into an int32_t, just to be safe
mov     r8d, 2000000000
;original code
lea     edx, [r12+7]
mov     rcx, rdi
jmp     PlayerStat_SoulCount_injection_return
PlayerStat_SoulCount_injection ENDP

extern PlayerStat_SoulMemory_injection_return: qword
PUBLIC PlayerStat_SoulMemory_injection
PlayerStat_SoulMemory_injection PROC
;keep it the same value as current soul count
mov     r8d, 2000000000
;original code
lea     edx, [r12+8]
mov     rcx, rdi
jmp     PlayerStat_SoulMemory_injection_return
PlayerStat_SoulMemory_injection ENDP

extern PlayerStat_Archetype_injection_return: qword
PUBLIC PlayerStat_Archetype_injection
PlayerStat_Archetype_injection PROC
;hardcode as pyrocmancer (since they can start at sl1)
;this is ok because we're just going to hardcode all the other stat levels as well
mov     r8d, 7
;original code
lea     edx, [r12+9]
mov     rcx, rdi
jmp     PlayerStat_Archetype_injection_return
PlayerStat_Archetype_injection ENDP

extern PlayerStat_HP_injection_return: qword
PUBLIC PlayerStat_HP_injection
PlayerStat_HP_injection PROC
;default pyromancer hp
mov     r8d, 573
;original code
lea     edx, [r12+0Ah]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_HP_injection_return
PlayerStat_HP_injection ENDP

extern PlayerStat_MaxHp_injection_return: qword
PUBLIC PlayerStat_MaxHp_injection
PlayerStat_MaxHp_injection PROC
;default pyromancer hp
mov     r8d, 573
;original code
lea     edx, [r12+0Bh]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_MaxHp_injection_return
PlayerStat_MaxHp_injection ENDP

extern PlayerStat_BaseMaxHp_1_injection_return: qword
PUBLIC PlayerStat_BaseMaxHp_1_injection
PlayerStat_BaseMaxHp_1_injection PROC
;default pyromancer hp
mov     r8d, 573
;original code
lea     edx, [r12+0Ch]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_BaseMaxHp_1_injection_return
PlayerStat_BaseMaxHp_1_injection ENDP

extern PlayerStat_Mp_injection_return: qword
PUBLIC PlayerStat_Mp_injection
PlayerStat_Mp_injection PROC
;default pyromancer mp
mov     r8d, 81
;original code
lea     edx, [r12+0Dh]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Mp_injection_return
PlayerStat_Mp_injection ENDP

extern PlayerStat_MaxMp_injection_return: qword
PUBLIC PlayerStat_MaxMp_injection
PlayerStat_MaxMp_injection PROC
;default pyromancer mp
mov     r8d, 81
;original code
lea     edx, [r12+0Eh]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_MaxMp_injection_return
PlayerStat_MaxMp_injection ENDP

extern PlayerStat_BaseMaxHp_2_injection_return: qword
PUBLIC PlayerStat_BaseMaxHp_2_injection
PlayerStat_BaseMaxHp_2_injection PROC
;default pyromancer hp
mov     r8d, 573
;original code
lea     edx, [r12+0Fh]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_BaseMaxHp_2_injection_return
PlayerStat_BaseMaxHp_2_injection ENDP

extern PlayerStat_Vitality_injection_return: qword
PUBLIC PlayerStat_Vitality_injection
PlayerStat_Vitality_injection PROC
;default pyromancer vit
mov     r8d, 10
;original code
lea     edx, [r12+10h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Vitality_injection_return
PlayerStat_Vitality_injection ENDP

extern PlayerStat_Attunement_injection_return: qword
PUBLIC PlayerStat_Attunement_injection
PlayerStat_Attunement_injection PROC
;default pyromancer att
mov     r8d, 12
;original code
lea     edx, [r12+11h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Attunement_injection_return
PlayerStat_Attunement_injection ENDP

extern PlayerStat_Endurance_injection_return: qword
PUBLIC PlayerStat_Endurance_injection
PlayerStat_Endurance_injection PROC
;default pyromancer end
mov     r8d, 11
;original code
lea     edx, [r12+12h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Endurance_injection_return
PlayerStat_Endurance_injection ENDP

extern PlayerStat_Strength_injection_return: qword
PUBLIC PlayerStat_Strength_injection
PlayerStat_Strength_injection PROC
;default pyromancer str
mov     r8d, 12
;original code
lea     edx, [r12+13h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Strength_injection_return
PlayerStat_Strength_injection ENDP

extern PlayerStat_Dexterity_injection_return: qword
PUBLIC PlayerStat_Dexterity_injection
PlayerStat_Dexterity_injection PROC
;default pyromancer dex
mov     r8d, 9
;original code
lea     edx, [r12+14h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Dexterity_injection_return
PlayerStat_Dexterity_injection ENDP

extern PlayerStat_Resistance_injection_return: qword
PUBLIC PlayerStat_Resistance_injection
PlayerStat_Resistance_injection PROC
;default pyromancer res
mov     r8d, 12
;original code
lea     edx, [r12+15h]
mov     rcx, rdi
jmp     PlayerStat_Resistance_injection_return
PlayerStat_Resistance_injection ENDP

extern PlayerStat_Intelligence_injection_return: qword
PUBLIC PlayerStat_Intelligence_injection
PlayerStat_Intelligence_injection PROC
;default pyromancer int
mov     r8d, 10
;original code
lea     edx, [r12+16h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Intelligence_injection_return
PlayerStat_Intelligence_injection ENDP

extern PlayerStat_Force_injection_return: qword
PUBLIC PlayerStat_Force_injection
PlayerStat_Force_injection PROC
;default pyromancer faith
mov     r8d, 8
;original code
lea     edx, [r12+17h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Force_injection_return
PlayerStat_Force_injection ENDP

extern PlayerStat_ItemDiscoveryRate_injection_return: qword
PUBLIC PlayerStat_ItemDiscoveryRate_injection
PlayerStat_ItemDiscoveryRate_injection PROC
;default item discovery
mov     r8d, 100
;original code
lea     edx, [r12+18h]
mov     rcx, rdi
jmp     PlayerStat_ItemDiscoveryRate_injection_return
PlayerStat_ItemDiscoveryRate_injection ENDP

extern PlayerStat_HumanityCount_injection_return: qword
PUBLIC PlayerStat_HumanityCount_injection
PlayerStat_HumanityCount_injection PROC
;0 humanity
mov     r8d, 0
;original code
lea     edx, [r12+21h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_HumanityCount_injection_return
PlayerStat_HumanityCount_injection ENDP

extern PlayerStat_attackR1_injection_return: qword
PUBLIC PlayerStat_attackR1_injection
PlayerStat_attackR1_injection PROC
;bare fists
mov     r8d, 20 
;original code
lea     edx, [r12+19h]
mov     rcx, rdi
jmp     PlayerStat_attackR1_injection_return
PlayerStat_attackR1_injection ENDP

extern PlayerStat_attackR2_injection_return: qword
PUBLIC PlayerStat_attackR2_injection
PlayerStat_attackR2_injection PROC
;bare fists
mov     r8d, 20
;original code
lea     edx, [r12+1Ah]
mov     rcx, rdi
jmp     PlayerStat_attackR2_injection_return
PlayerStat_attackR2_injection ENDP

extern PlayerStat_attackL1_injection_return: qword
PUBLIC PlayerStat_attackL1_injection
PlayerStat_attackL1_injection PROC
;bare fists
mov     r8d, 20
;original code
lea     edx, [r12+1Bh]
mov     rcx, rdi
jmp     PlayerStat_attackL1_injection_return
PlayerStat_attackL1_injection ENDP

extern PlayerStat_attackL2_injection_return: qword
PUBLIC PlayerStat_attackL2_injection
PlayerStat_attackL2_injection PROC
;bare fists
mov     r8d, 20
;original code
lea     edx, [r12+1Ch]
mov     rcx, rdi
jmp     PlayerStat_attackL2_injection_return
PlayerStat_attackL2_injection ENDP

extern PlayerStat_EstusLevel_injection_return: qword
PUBLIC PlayerStat_EstusLevel_injection
PlayerStat_EstusLevel_injection PROC
;no upgrade
mov     r8d, 0
;original code
lea     edx, [r12+1Eh]
mov     rcx, rdi
jmp     PlayerStat_EstusLevel_injection_return
PlayerStat_EstusLevel_injection ENDP

extern PlayerStat_Left_Hand_1_injection_return: qword
PUBLIC PlayerStat_Left_Hand_1_injection
PlayerStat_Left_Hand_1_injection PROC
;bare fist
mov     r8d, 5
;original code
lea     edx, [r12+22h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Left_Hand_1_injection_return
PlayerStat_Left_Hand_1_injection ENDP

extern PlayerStat_Left_Hand_2_injection_return: qword
PUBLIC PlayerStat_Left_Hand_2_injection
PlayerStat_Left_Hand_2_injection PROC
;bare fist
mov     r8d, 5
;original code
lea     edx, [r12+23h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Left_Hand_2_injection_return
PlayerStat_Left_Hand_2_injection ENDP

extern PlayerStat_Right_Hand_1_injection_return: qword
PUBLIC PlayerStat_Right_Hand_1_injection
PlayerStat_Right_Hand_1_injection PROC
;bare fist
mov     r8d, 5
;original code
lea     edx, [r12+24h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Right_Hand_1_injection_return
PlayerStat_Right_Hand_1_injection ENDP

extern PlayerStat_Right_Hand_2_injection_return: qword
PUBLIC PlayerStat_Right_Hand_2_injection
PlayerStat_Right_Hand_2_injection PROC
;bare fist
mov     r8d, 5
;original code
lea     edx, [r12+25h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_Right_Hand_2_injection_return
PlayerStat_Right_Hand_2_injection ENDP

extern PlayerStat_ArmorHead_injection_return: qword
PUBLIC PlayerStat_ArmorHead_injection
PlayerStat_ArmorHead_injection PROC
;no armor
mov     r8d, 0
;original code
lea     edx, [r12+26h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_ArmorHead_injection_return
PlayerStat_ArmorHead_injection ENDP

extern PlayerStat_ArmorBody_injection_return: qword
PUBLIC PlayerStat_ArmorBody_injection
PlayerStat_ArmorBody_injection PROC
;no armor
mov     r8d, 0
;original code
lea     edx, [r12+27h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_ArmorBody_injection_return
PlayerStat_ArmorBody_injection ENDP

extern PlayerStat_ArmorArms_injection_return: qword
PUBLIC PlayerStat_ArmorArms_injection
PlayerStat_ArmorArms_injection PROC
;no armor
mov     r8d, 0
;original code
lea     edx, [r12+28h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_ArmorArms_injection_return
PlayerStat_ArmorArms_injection ENDP

extern PlayerStat_ArmorLegs_injection_return: qword
PUBLIC PlayerStat_ArmorLegs_injection
PlayerStat_ArmorLegs_injection PROC
;no armor
mov     r8d, 0
;original code
lea     edx, [r12+29h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array1]
jmp     PlayerStat_ArmorLegs_injection_return
PlayerStat_ArmorLegs_injection ENDP

extern PlayerStat_Stamina_injection_return: qword
PUBLIC PlayerStat_Stamina_injection
PlayerStat_Stamina_injection PROC
;base pyro stamina
mov     r8d, 93
;original code
lea     edx, [r12+2Ch]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_Stamina_injection_return
PlayerStat_Stamina_injection ENDP

extern PlayerStat_MaxStamina_injection_return: qword
PUBLIC PlayerStat_MaxStamina_injection
PlayerStat_MaxStamina_injection PROC
;base pyro stamina
mov     r8d, 93
;original code
lea     edx, [r12+2Dh]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_MaxStamina_injection_return
PlayerStat_MaxStamina_injection ENDP

extern PlayerStat_BaseMaxStamina_injection_return: qword
PUBLIC PlayerStat_BaseMaxStamina_injection
PlayerStat_BaseMaxStamina_injection PROC
;base pyro stamina
mov     r8d, 93
;original code
lea     edx, [r12+2Eh]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_BaseMaxStamina_injection_return
PlayerStat_BaseMaxStamina_injection ENDP

extern PlayerStat_WeaponinSlot0_injection_return: qword
PUBLIC PlayerStat_WeaponinSlot0_injection
PlayerStat_WeaponinSlot0_injection PROC
;no weapon
mov     r8d, 900000
;original code
lea     edx, [r12+2Fh]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_WeaponinSlot0_injection_return
PlayerStat_WeaponinSlot0_injection ENDP

extern PlayerStat_WeaponinSlot2_injection_return: qword
PUBLIC PlayerStat_WeaponinSlot2_injection
PlayerStat_WeaponinSlot2_injection PROC
;no weapon
mov     r8d, 900000
;original code
lea     edx, [r12+30h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_WeaponinSlot2_injection_return
PlayerStat_WeaponinSlot2_injection ENDP

extern PlayerStat_WeaponinSlot1_injection_return: qword
PUBLIC PlayerStat_WeaponinSlot1_injection
PlayerStat_WeaponinSlot1_injection PROC
;no weapon
mov     r8d, 900000
;original code
lea     edx, [r12+31h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_WeaponinSlot1_injection_return
PlayerStat_WeaponinSlot1_injection ENDP

extern PlayerStat_WeaponinSlot3_injection_return: qword
PUBLIC PlayerStat_WeaponinSlot3_injection
PlayerStat_WeaponinSlot3_injection PROC
;no weapon
mov     r8d, 900000
;original code
lea     edx, [r12+32h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_WeaponinSlot3_injection_return
PlayerStat_WeaponinSlot3_injection ENDP

extern PlayerStat_ArmorinSlot0_injection_return: qword
PUBLIC PlayerStat_ArmorinSlot0_injection
PlayerStat_ArmorinSlot0_injection PROC
;no armor
mov     r8d, 900000
;original code
lea     edx, [r12+33h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_ArmorinSlot0_injection_return
PlayerStat_ArmorinSlot0_injection ENDP

extern PlayerStat_ArmorinSlot1_injection_return: qword
PUBLIC PlayerStat_ArmorinSlot1_injection
PlayerStat_ArmorinSlot1_injection PROC
;no armor
mov     r8d, 901000
;original code
lea     edx, [r12+34h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_ArmorinSlot1_injection_return
PlayerStat_ArmorinSlot1_injection ENDP

extern PlayerStat_ArmorinSlot2_injection_return: qword
PUBLIC PlayerStat_ArmorinSlot2_injection
PlayerStat_ArmorinSlot2_injection PROC
;no armor
mov     r8d, 902000
;original code
lea     edx, [r12+35h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_ArmorinSlot2_injection_return
PlayerStat_ArmorinSlot2_injection ENDP

extern PlayerStat_ArmorinSlot3_injection_return: qword
PUBLIC PlayerStat_ArmorinSlot3_injection
PlayerStat_ArmorinSlot3_injection PROC
;no armor
mov     r8d, 903000
;original code
lea     edx, [r12+36h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_ArmorinSlot3_injection_return
PlayerStat_ArmorinSlot3_injection ENDP

extern PlayerStat_RinginSlot0_injection_return: qword
PUBLIC PlayerStat_RinginSlot0_injection
PlayerStat_RinginSlot0_injection PROC
;no ring
mov     r8d, 0FFFFFFFFh
;original code
lea     edx, [r12+37h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_RinginSlot0_injection_return
PlayerStat_RinginSlot0_injection ENDP

extern PlayerStat_RinginSlot1_injection_return: qword
PUBLIC PlayerStat_RinginSlot1_injection
PlayerStat_RinginSlot1_injection PROC
;no ring
mov     r8d, 0FFFFFFFFh
;original code
lea     edx, [r12+38h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_RinginSlot1_injection_return
PlayerStat_RinginSlot1_injection ENDP

extern PlayerStat_IteminQuickbar0_injection_return: qword
PUBLIC PlayerStat_IteminQuickbar0_injection
PlayerStat_IteminQuickbar0_injection PROC
;no item
mov     r8d, 0FFFFFFFFh
;original code
lea     edx, [r12+39h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_IteminQuickbar0_injection_return
PlayerStat_IteminQuickbar0_injection ENDP

extern PlayerStat_IteminQuickbar1_injection_return: qword
PUBLIC PlayerStat_IteminQuickbar1_injection
PlayerStat_IteminQuickbar1_injection PROC
;no item
mov     r8d, 0FFFFFFFFh
;original code
lea     edx, [r12+3Ah]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_IteminQuickbar1_injection_return
PlayerStat_IteminQuickbar1_injection ENDP

extern PlayerStat_IteminQuickbar2_injection_return: qword
PUBLIC PlayerStat_IteminQuickbar2_injection
PlayerStat_IteminQuickbar2_injection PROC
;no item
mov     r8d, 0FFFFFFFFh
;original code
lea     edx, [r12+3Bh]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_IteminQuickbar2_injection_return
PlayerStat_IteminQuickbar2_injection ENDP

extern PlayerStat_IteminQuickbar3_injection_return: qword
PUBLIC PlayerStat_IteminQuickbar3_injection
PlayerStat_IteminQuickbar3_injection PROC
;no item
mov     r8d, 0FFFFFFFFh
;original code
lea     edx, [r12+3Ch]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_IteminQuickbar3_injection_return
PlayerStat_IteminQuickbar3_injection ENDP

extern PlayerStat_IteminQuickbar4_injection_return: qword
PUBLIC PlayerStat_IteminQuickbar4_injection
PlayerStat_IteminQuickbar4_injection PROC
;no item
mov     r8d, 0FFFFFFFFh
;original code
lea     edx, [r12+3Dh]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_IteminQuickbar4_injection_return
PlayerStat_IteminQuickbar4_injection ENDP

extern PlayerStat_IteminArrowBoltSlot0_injection_return: qword
PUBLIC PlayerStat_IteminArrowBoltSlot0_injection
PlayerStat_IteminArrowBoltSlot0_injection PROC
;no item
mov     r8d, 0FFFFFFFFh
;original code
lea     edx, [r12+3Eh]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_IteminArrowBoltSlot0_injection_return
PlayerStat_IteminArrowBoltSlot0_injection ENDP

extern PlayerStat_IteminArrowBoltSlot1_injection_return: qword
PUBLIC PlayerStat_IteminArrowBoltSlot1_injection
PlayerStat_IteminArrowBoltSlot1_injection PROC
;no item
mov     r8d, 0FFFFFFFFh
;original code
lea     edx, [r12+3Fh]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_IteminArrowBoltSlot1_injection_return
PlayerStat_IteminArrowBoltSlot1_injection ENDP

extern PlayerStat_IteminArrowBoltSlot2_injection_return: qword
PUBLIC PlayerStat_IteminArrowBoltSlot2_injection
PlayerStat_IteminArrowBoltSlot2_injection PROC
;no item
mov     r8d, 0FFFFFFFFh
;original code
lea     edx, [r12+40h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_IteminArrowBoltSlot2_injection_return
PlayerStat_IteminArrowBoltSlot2_injection ENDP

extern PlayerStat_IteminArrowBoltSlot3_injection_return: qword
PUBLIC PlayerStat_IteminArrowBoltSlot3_injection
PlayerStat_IteminArrowBoltSlot3_injection PROC
;no item
mov     r8d, 0FFFFFFFFh
;original code
lea     edx, [r12+41h]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_IteminArrowBoltSlot3_injection_return
PlayerStat_IteminArrowBoltSlot3_injection ENDP

extern PlayerStat_EquippedSpell1_injection_return: qword
PUBLIC PlayerStat_EquippedSpell1_injection
PlayerStat_EquippedSpell1_injection PROC
;no spell
mov     r8d, 0FFFFFFFFh
;original code
mov     edx, [r15]
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array2]
jmp     PlayerStat_EquippedSpell1_injection_return
PlayerStat_EquippedSpell1_injection ENDP

extern PlayerStat_New_Name_86_injection_return: qword
PUBLIC PlayerStat_New_Name_86_injection
PlayerStat_New_Name_86_injection PROC
;this seems to be human/not-human?
mov     r8d, 1
;original code
mov     edx, 56h
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array4]
cmp     byte ptr [rsi+3C0h], 0
jmp     PlayerStat_New_Name_86_injection_return
PlayerStat_New_Name_86_injection ENDP

extern PlayerStat_New_Name_87_injection_return: qword
PUBLIC PlayerStat_New_Name_87_injection
PlayerStat_New_Name_87_injection PROC
;
mov     r8d, 0
;original code
mov     edx, 57h
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array4]
cmp     byte ptr [rsi+3C0h], 0
jmp     PlayerStat_New_Name_87_injection_return
PlayerStat_New_Name_87_injection ENDP

extern PlayerStat_New_Name_88_injection_return: qword
PUBLIC PlayerStat_New_Name_88_injection
PlayerStat_New_Name_88_injection PROC
;
mov     r8d, 0
;original code
mov     edx, 58h
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array4]
cmp     byte ptr [rsi+3C0h], 0
jmp     PlayerStat_New_Name_88_injection_return
PlayerStat_New_Name_88_injection ENDP

extern PlayerStat_New_Name_89_injection_return: qword
PUBLIC PlayerStat_New_Name_89_injection
PlayerStat_New_Name_89_injection PROC
;
mov     r8d, 0
;original code
mov     edx, 59h
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array4]
mov     edx, 1Ch
jmp     PlayerStat_New_Name_89_injection_return
PlayerStat_New_Name_89_injection ENDP

extern PlayerStat_New_Name_93_injection_return: qword
PUBLIC PlayerStat_New_Name_93_injection
PlayerStat_New_Name_93_injection PROC
;
movss   xmm2, dword ptr [const0_float]
;original code
mov     edx, 5Dh
mov     rcx, rdi
call    qword ptr [set_value_in_MemberFlags_array5]
jmp     PlayerStat_New_Name_93_injection_return
PlayerStat_New_Name_93_injection ENDP

extern PlayerStat_defSAToughnessTotal_injection_return: qword
PUBLIC PlayerStat_defSAToughnessTotal_injection
PlayerStat_defSAToughnessTotal_injection PROC
;0 poise
movd   xmm2, dword ptr [const0]
;original code
cvtdq2ps xmm2, xmm2
mov     edx, 5Eh
jmp     PlayerStat_defSAToughnessTotal_injection_return
PlayerStat_defSAToughnessTotal_injection ENDP

extern PlayerStat_MaxEquipLoad_injection_return: qword
PUBLIC PlayerStat_MaxEquipLoad_injection
PlayerStat_MaxEquipLoad_injection PROC
;use the default Pyro Equip Load
movss   xmm2, dword ptr [defaultPyroEquipLoad]
;original code
mov     edx, 5Fh
jmp     PlayerStat_MaxEquipLoad_injection_return
PlayerStat_MaxEquipLoad_injection ENDP

extern PlayerStat_NormalDefenses_injection_return: qword
PUBLIC PlayerStat_NormalDefenses_injection
PlayerStat_NormalDefenses_injection PROC
;original code
mov     [rbp-1], eax
;use the default pyro naked defs
lea     r8, [pyro_normal_defs]
;original code
mov     edx, 6Ah
mov     rcx, rdi
jmp     PlayerStat_NormalDefenses_injection_return
PlayerStat_NormalDefenses_injection ENDP

extern PlayerStat_NormalResists_injection_return: qword
PUBLIC PlayerStat_NormalResists_injection
PlayerStat_NormalResists_injection PROC
;original code
mov     [rbp-71h], ecx
;use the default pyro naked resists
lea     r8, [pyro_normal_resists]
;original code
mov     edx, 6Bh
mov     rcx, rdi
jmp     PlayerStat_NormalResists_injection_return
PlayerStat_NormalResists_injection ENDP

extern PlayerStat_CovenantLevel_injection_return: qword
PUBLIC PlayerStat_CovenantLevel_injection
PlayerStat_CovenantLevel_injection PROC
;original code
mov     [rbp-1], ebx
;max levels in every covenant
lea     r8, [max_covenant_levels]
;original code
mov     edx, 6Ch
mov     rcx, rdi
jmp     PlayerStat_CovenantLevel_injection_return
PlayerStat_CovenantLevel_injection ENDP

extern PlayerStat_New_Name_110_injection_return: qword
PUBLIC PlayerStat_New_Name_110_injection
PlayerStat_New_Name_110_injection PROC
;original code
mov     [rbp-6Fh], al
;
lea     r8, [New_Name_110_const]
;original code
mov     edx, 6Eh
mov     rcx, rdi
jmp     PlayerStat_New_Name_110_injection_return
PlayerStat_New_Name_110_injection ENDP

extern PlayerStat_New_Name_111_injection_return: qword
PUBLIC PlayerStat_New_Name_111_injection
PlayerStat_New_Name_111_injection PROC
;original code
mov     [rbp-1], eax
;
lea     r8, [New_Name_111_const]
;original code
mov     edx, 6Fh
mov     rcx, rdi
jmp     PlayerStat_New_Name_111_injection_return
PlayerStat_New_Name_111_injection ENDP

_TEXT    ENDS
END

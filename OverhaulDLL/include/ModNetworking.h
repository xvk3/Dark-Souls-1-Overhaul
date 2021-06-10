#pragma once

#ifndef ModNetworking_H
#define ModNetworking_H

#include <cstdint>
#include <unordered_map>

class TimerClientInfo
{
public:
    TimerClientInfo()
    {
        timeOfLastResync = 0;
        timePingPacketSent = 0;
        packetDelay = 0;
        waitingForPingResponse = false;
        lastAidCount = 0xff;
    };
    uint64_t timeOfLastResync;
    uint64_t timePingPacketSent;
    uint64_t packetDelay;
    bool waitingForPingResponse;
    //state indicator for what animation we've received (the most sig 3 bits in the type1 packet)
    uint8_t lastAidCount;
};

class ModNetworking
{
public:
    static void start();

    //configuration options for who can connect if you're host/how to change settings if you're guest
    static bool allow_connect_with_non_mod_host;
    static bool allow_connect_with_legacy_mod_host;
    static bool allow_connect_with_overhaul_mod_host;
    static bool allow_connect_with_non_mod_guest;

    //info about the current host/guest
    static bool host_mod_installed;
    static bool host_legacy_enabled;
    static bool guest_mod_installed;
    static bool guest_legacy_enabled;

    static int64_t timer_offset;
    //key: steamid
    static std::unordered_map<uint64_t, TimerClientInfo> hostTimerSyncronizationData;
private:
    static const uint64_t sendPacket_injection_offset = 0x5096dc; //injection is before the send call so we can modify the packet
    static const uint64_t getNetMessage_injection_offset = 0x509606;
    static const uint64_t GetSteamData_Packet_injection_offset = 0x108806a;
    static const uint64_t SendRawP2PPacket_injection_offset = 0x10ba232;
    static const uint64_t ParseRawP2PPacketType_injection_offset = 0x10b5340;
    static const uint64_t type1_32byte_p2pPacket_parsing_rollback_injection_offset = 0x5034ab;
    static const uint64_t type1_40byte_p2pPacket_parsing_rollback_injection_offset = 0x5034e6;
    static const uint64_t type1_p2pPacket_sending_rollback_injection_offset = 0x503223;
};

#endif

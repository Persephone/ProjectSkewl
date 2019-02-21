#pragma once

#ifndef DEF_LASTMANSTANDING_H
#define DEF_LASTMANSTANDING_H

#include <map>

enum LastManStandingEnums : int32
{
    LastManStanding_CheckMatch = 0,
    LastManStanding_CheckQueue, // This must always be 1!
    LastManStanding_MainMenu,
    LastManStanding_AddPlayerToQueue,
    LastManStanding_RemovePlayerFromQueue,
    LastManStanding_BeginMatch,

    LMS_STATE_TIMER_ACTIVE                 = 5020,
    LMS_STATE_TIMER                        = 5021,
    LMS_STATE_PLAYERS_ALIVE_ACTIVE         = 5022,
    LMS_STATE_PLAYERS_ALIVE                = 5023,
    LMS_STATE_STORM_REDUCTION_TIMER_ACTIVE = 5024,
    LMS_STATE_STORM_REDUCTION_TIMER        = 5025
};

struct LastManStandingData
{
    uint32 Phase;
    Creature* center;
    int8 DomeReductionTime;

    time_t FinishTime;
    std::unordered_map<ObjectGuid, bool /*warning*/> m_Players;

    bool Started;

    const uint32 GetAlivePlayers();
    void PhasePlayers();
    void CheckPlayerPositions();
    bool SpawnCenter();
    void SendMessageToAll(std::string msg);
    void UpdateWorldState(uint32 state, uint32 value);
    void SendInitWorldStates(bool apply);
};

class LastManStanding
{
public:
    static LastManStanding* instance();

    const uint32 GetCurrentPhase() { return m_Phase; }
    const uint32 GetNextPhase();
    void TeleportPlayers(LastManStandingData* data);

    void EndEvent(uint32 phase);
    const bool IsInQueue(ObjectGuid guid);
    LastManStandingData* GetMatchData(Player* player);
    void RemoveFromQueue(ObjectGuid guid);
    std::pair<uint32, uint8> AddToQueue(ObjectGuid guid);
    LastManStandingData* BeginMatch(uint32 phase);
    void RemoveFromMatch(uint32 phase, ObjectGuid guid);

    std::string GetNameLink(Player* player);

    std::vector<ObjectGuid> GetActiveQueue();
    std::map<uint32 /* Phase */, LastManStandingData> &GetActiveEvents() { return m_ActiveEvents; }

    EventMap Events;

private:
    LastManStanding() { };
    ~LastManStanding() { };

    std::map<uint32 /* Phase */, std::vector<ObjectGuid>> m_Queue;
    std::map<uint32 /* Phase */, LastManStandingData> m_ActiveEvents;

    uint32 m_Phase;
};

#define sLastManStandingMgr LastManStanding::instance()

#endif // ! LastManStanding_H

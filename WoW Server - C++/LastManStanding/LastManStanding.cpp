#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "GameTime.h"
#include "Pet.h"
#include "LastManStanding.h"
#include "Log.h"
#include "Map.h"
#include "MapManager.h"
#include "World.h"
#include "WorldSession.h"
#include "Chat.h"

constexpr uint32 MapId = 0;
constexpr uint32 AreaId = 279;
constexpr uint32 MinPlayers = 2;
constexpr uint32 MaxDuration = 5 * MINUTE;
constexpr uint32 TimeBeforeQueuePop = 10; //3 * MINUTE;
constexpr uint32 UpdateTimeInterval = 1000; // 1 seconds
constexpr uint32 RootSpellId = 75215;
constexpr uint32 CenterNPC = 75009;
constexpr float  DistanceToCenter = 6.3f;
constexpr float  CenterStartSize = 20.0f;
constexpr float  DomeReductionValue = 1.0f;
constexpr uint32 DomeReductionTime = 15; // seconds
const Position   CenterPosition(288.966755f, 376.836731f, -66.191811f, 4.608738f); // center of event
const WorldLocation ReturnLocation(603, 1665.815063f, 13.970251f, 427.309967f, 2.370627f); // Where they get teleported after event or on death

std::vector<Position> TeleportLocations = {
    { 323.733307f, 379.481293f, -58.769131f, 3.226680f },
    { 306.251343f, 336.747437f, -52.887299f, 1.829995f },
    { 234.100937f, 357.213959f, -61.442867f, 0.666821f },
    { 235.916351f, 390.990997f, -52.143265f, 5.907002f },
    { 275.665192f, 415.372925f, -67.074966f, 5.008115f },
};

const uint32 MaxPlayers = TeleportLocations.size();

int32 TimeLeftBeforeQueuePop = 0;

namespace LastManStandingEvent
{
    class Countdown : BasicEvent
    {
    public:
        Countdown(Player* player, int32 count) : BasicEvent(), player(player), count(count)
        {
            player->GetSession()->SendAreaTriggerMessage("Get ready!");
            player->RemoveAura(RootSpellId);
            Schedule();
        }
        ~Countdown()
        {
        }

        bool Execute(uint64, uint32)
        {
            if (!player || !player->IsInWorld() || player->GetAreaId() != AreaId)
            {
                ScheduleAbort();
                return true;
            }

            if (--count <= 0)
            {
                player->GetSession()->SendAreaTriggerMessage("GO!");
                player->RemoveAura(RootSpellId);
                return true;
            }

            player->AddAura(RootSpellId, player);
            player->GetSession()->SendAreaTriggerMessage("%i!", count);
            Schedule();
            return false;
        }

        void Schedule()
        {
            player->m_Events.AddEvent(this, player->m_Events.CalculateTime(IN_MILLISECONDS));
        }

        int32 count;
        Player* player;
    };

    class Warning : BasicEvent
    {
    public:
        Warning(Player* player, Creature* center, LastManStandingData* data, int32 count) : BasicEvent(), player(player), center(center), data(data), count(count)
        {
            player->GetSession()->SendAreaTriggerMessage("Warning! You are leaving the event!");
            Schedule();
        }
        ~Warning()
        {
        }

        bool Execute(uint64, uint32)
        {
            if (!player || !player->IsInWorld() || player->GetAreaId() != AreaId)
            {
                data->m_Players[player->GetGUID()] = false;
                ScheduleAbort();
                return true;
            }

            // Player is inside again so cancel exit warning
            if (player->GetExactDist(center->GetPosition()) <= DistanceToCenter * center->GetObjectScale())
            {
                data->m_Players[player->GetGUID()] = false;
                ScheduleAbort();
                return true;
            }

            if (--count <= 0)
            {
                sLastManStandingMgr->RemoveFromMatch(player->GetPhaseMask(), player->GetGUID());

                // Phasing him out will remove him at next update
                player->SetPhaseMask(PHASEMASK_NORMAL, true);
                if (Pet* pet = player->GetPet())
                    pet->SetPhaseMask(PHASEMASK_NORMAL, true);

                data->UpdateWorldState(LMS_STATE_PLAYERS_ALIVE, data->GetAlivePlayers());
                data->SendMessageToAll(Trinity::StringFormat("[|cffFFEvent Message|r]: %s has been removed from the event.", sLastManStandingMgr->GetNameLink(player)));
                ChatHandler(player->GetSession()).SendSysMessage("[|cffFF0000Warning|r]: You have been removed from the event.");
                return true;
            }

            player->GetSession()->SendAreaTriggerMessage("Warning! You are leaving the event!");
            ChatHandler(player->GetSession()).PSendSysMessage("[|cffFF0000Warning|r]: You are leaving the event area and will be removed from the event in %u seconds.", count);
            Schedule();
            return false;
        }

        void Schedule()
        {
            player->m_Events.AddEvent(this, player->m_Events.CalculateTime(IN_MILLISECONDS));
        }

        int32 count;
        Player* player;
        Creature* center;
        LastManStandingData* data;
    };
}

/* LastManStanding Functions */
void LastManStandingData::CheckPlayerPositions()
{
    for (auto& member : m_Players)
    {
        if (Player* player = ObjectAccessor::FindPlayer(member.first))
        {
            if (player->GetExactDist(center->GetPosition()) >= DistanceToCenter * center->GetObjectScale())
            {
                if (!member.second)
                {
                    // Prevent spamming tons of events
                    member.second = true;
                    new LastManStandingEvent::Warning(player, center, this, 4);
                }
            }
        }
    }
}

bool LastManStandingData::SpawnCenter()
{
    Map* map = sMapMgr->CreateBaseMap(MapId);
    if (TempSummon* summon = map->SummonCreature(CenterNPC, CenterPosition, nullptr, MaxDuration * IN_MILLISECONDS))
    {
        summon->SetTempSummonType(TEMPSUMMON_TIMED_DESPAWN);
        summon->SetPhaseMask(Phase, true);
        summon->SetObjectScale(CenterStartSize);
        center = summon->ToCreature();
        return true;
    }

    return false;
}

void LastManStandingData::SendMessageToAll(std::string msg)
{
    for (auto& member : m_Players)
        if (Player* player = ObjectAccessor::FindPlayer(member.first))
            ChatHandler(player->GetSession()).PSendSysMessage("%s", msg);
}

void LastManStandingData::UpdateWorldState(uint32 state, uint32 value)
{
    for (auto& member : m_Players)
        if (Player* player = ObjectAccessor::FindPlayer(member.first))
            player->SendUpdateWorldState(state, value);
}

void LastManStandingData::SendInitWorldStates(bool apply)
{
    for (auto& member : m_Players)
        if (Player* player = ObjectAccessor::FindPlayer(member.first))
        {
            player->SendUpdateWorldState(LMS_STATE_TIMER_ACTIVE, apply ? 1 : 0);
            player->SendUpdateWorldState(LMS_STATE_PLAYERS_ALIVE_ACTIVE, apply ? 1 : 0);
            player->SendUpdateWorldState(LMS_STATE_STORM_REDUCTION_TIMER_ACTIVE, apply ? 1 : 0);
        }

}

const uint32 LastManStanding::GetNextPhase()
{
    m_Phase = m_Phase << 1;
    if (m_Phase == 0)
        m_Phase = 2;

    return m_Phase;
}

void LastManStanding::TeleportPlayers(LastManStandingData* data)
{
    std::vector<Position> locations = TeleportLocations;

    for (auto const& member : data->m_Players)
    {
        if (Player* player = ObjectAccessor::FindPlayer(member.first))
        {
            // Reduce Event Energy
            player->ReduceEventEnergy();

            // Update Phasemask
            player->SetPhaseMask(data->Phase, true);
            if (Pet* pet = player->GetPet())
                pet->SetPhaseMask(data->Phase, true);

            uint8 index = urand(0, TeleportLocations.size() - 1);
            Position pos = locations[index];
            player->TeleportTo(MapId, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), TELE_TO_NOT_UNSUMMON_PET);
            locations.erase(locations.begin() + index);

            new LastManStandingEvent::Countdown(player, 15);
        }
    }
}

void LastManStandingData::PhasePlayers()
{
    for (auto const& member : m_Players)
    {
        if (Player* player = ObjectAccessor::FindPlayer(member.first))
        {
            if (player->IsAlive() && player->GetAreaId() == AreaId)
            {
                player->SetPhaseMask(Phase, true);
                if (Pet* pet = player->GetPet())
                    pet->SetPhaseMask(Phase, true);
            }
        }
    }
}

void LastManStanding::EndEvent(uint32 phase)
{
    auto event = m_ActiveEvents.find(phase);
    if (event != m_ActiveEvents.end())
    {
        // Remove Dome
        event->second.center->DespawnOrUnsummon();

        // 1 player left - reward him
        if (event->second.GetAlivePlayers() == 1)
        {
            for (auto const& member : event->second.m_Players)
                if (Player* player = ObjectAccessor::FindPlayer(member.first))
                {
                    sWorld->SendGlobalText(Trinity::StringFormat("[Dalaran Crater Royale]: %s has won the event!", GetNameLink(player)).c_str(), NULL);
                    player->SetPhaseMask(PHASEMASK_NORMAL, true);
                    if (Pet* pet = player->GetPet())
                        pet->SetPhaseMask(PHASEMASK_NORMAL, true);
                    player->ResurrectPlayer(1.0f);
                    player->TeleportTo(ReturnLocation, TELE_TO_NOT_UNSUMMON_PET);

                    // Rewards
                    player->AddItem(6354, 2);
                    player->GiveXP(1000, nullptr);
                    ChatHandler(player->GetSession()).SendSysMessage("[Dalaran Crater Royale]: You got an extra 1000 XP for winning the event!");
                }
        }
        else // More than 1 man standing after 5 minutes - no winnersri
        {
            for (auto const& member : event->second.m_Players)
                if (Player* player = ObjectAccessor::FindPlayer(member.first))
                {
                    player->ResurrectPlayer(1.0f);
                    ChatHandler(player->GetSession()).SendSysMessage("[Dalaran Crater Royale]: Time is up! There were no winners of this match.");
                }
        }

        event->second.SendInitWorldStates(false);
        event->second.center = nullptr;
    }

    m_ActiveEvents.erase(phase);
}

const bool LastManStanding::IsInQueue(ObjectGuid guid)
{
    auto const& itr = std::find_if(m_Queue.begin(), m_Queue.end(),
        [&guid](std::pair<uint32, std::vector<ObjectGuid>> const& data)
    {
        return std::find(data.second.begin(), data.second.end(), guid) != data.second.end();
    });

    return itr != m_Queue.end();
}

LastManStandingData* LastManStanding::GetMatchData(Player* player)
{
    auto const& data = m_ActiveEvents.find(player->GetPhaseMask());
    if (data == m_ActiveEvents.end())
        return nullptr;

    auto const& playerData = data->second.m_Players.find(player->GetGUID());
    if (playerData == data->second.m_Players.end())
        return nullptr;

    return &data->second;
}

void LastManStanding::RemoveFromQueue(ObjectGuid guid)
{
    auto const& itr = std::find_if(m_Queue.begin(), m_Queue.end(),
        [&guid](std::pair<uint32, std::vector<ObjectGuid>> const& data)
    {
        return std::find(data.second.begin(), data.second.end(), guid) != data.second.end();
    });

    if (itr != m_Queue.end())
        m_Queue.erase(itr);
}

std::pair<uint32, uint8> LastManStanding::AddToQueue(ObjectGuid guid)
{
    std::pair<uint32 /*phase*/, uint8 /* players in queue*/> result;
    result.first = PHASEMASK_NORMAL;

    // Check current phase
    auto const& queue = m_Queue.find(GetCurrentPhase());
    if (queue != m_Queue.end())
    {
        // Found existing queue with this phase
        if (queue->second.size() < MaxPlayers)
        {
            queue->second.push_back(guid);
            if (queue->second.size() >= MaxPlayers) // queue is full
            {
                result.first = GetCurrentPhase();
                return result; // Maximum size has been reached, lets begin
            }
            // Queue is not full yet
            result.second = MaxPlayers - queue->second.size();
            return result;
        }
    }

    // Generate new queue
    uint32 nextPhase = GetNextPhase();
    // Failsafe - Make sure we arent overwriting any ongoing match
    auto const& event = m_ActiveEvents.find(nextPhase);
    if (event != m_ActiveEvents.end())
    {
        result.second = MaxPlayers + 1;
        return result;
    }

    // Add him to the new queue
    m_Queue[nextPhase].push_back(guid);
    result.second = MinPlayers - 1;
    return result;
}

LastManStandingData* LastManStanding::BeginMatch(uint32 phase)
{
    LastManStandingData data;
    data.Phase = phase;
    data.Started = false;
    data.DomeReductionTime = DomeReductionTime;
    data.FinishTime = GameTime::GetGameTime() + MaxDuration;

    // Add players to the event
    for (ObjectGuid const& guid : m_Queue[phase])
        data.m_Players[guid] = false;

    // Clear queue at this phase
    m_Queue.erase(phase);
    m_ActiveEvents.insert(std::make_pair(phase, data));
    TimeLeftBeforeQueuePop = 0;
    return Trinity::Containers::MapGetValuePtr(m_ActiveEvents, phase);
}

void LastManStanding::RemoveFromMatch(uint32 phase, ObjectGuid guid)
{
    m_ActiveEvents[phase].m_Players.erase(guid);
}

std::vector<ObjectGuid> LastManStanding::GetActiveQueue()
{
    auto itr = m_Queue.find(m_Phase);
    if (itr != m_Queue.end())
        return itr->second;

    return { };
}

LastManStanding* LastManStanding::instance()
{
    static LastManStanding instance;
    return &instance;
}

const uint32 LastManStandingData::GetAlivePlayers()
{
    uint32 count = 0;
    for (auto const& member : m_Players)
    {
        if (Player* player = ObjectAccessor::FindPlayer(member.first))
            if (player->GetAreaId() == AreaId && player->IsAlive() && player->GetPhaseMask() == Phase)
                ++count;
    }
    return count;
}

std::string LastManStanding::GetNameLink(Player* player)
{
    std::string name = player->GetName();
    std::string color = "";
    switch (player->getClass())
    {
    case CLASS_DEATH_KNIGHT:
        color = "|cffC41F3B";
        break;
    case CLASS_DRUID:
        color = "|cffFF7D0A";
        break;
    case CLASS_HUNTER:
        color = "|cffABD473";
        break;
    case CLASS_MAGE:
        color = "|cff69CCF0";
        break;
    case CLASS_PALADIN:
        color = "|cffF58CBA";
        break;
    case CLASS_PRIEST:
        color = "|cffFFFFFF";
        break;
    case CLASS_ROGUE:
        color = "|cffFFF569";
        break;
    case CLASS_SHAMAN:
        color = "|cff0070DE";
        break;
    case CLASS_WARLOCK:
        color = "|cff9482C9";
        break;
    case CLASS_WARRIOR:
        color = "|cffC79C6E";
        break;
    }
    return "|Hplayer:" + name + "|h|cffFFFFFF[" + color + name + "|cffFFFFFF]|h|r";
}

class LastManStanding_Master : public CreatureScript
{
public:
    LastManStanding_Master() : CreatureScript("LastManStanding_Master") { }

    static bool OnGossipHello(Player* player, Creature* creature)
    {
        player->PlayerTalkClass->ClearMenus();

        if (sLastManStandingMgr->IsInQueue(player->GetGUID()))
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "Leave Queue", LastManStanding_RemovePlayerFromQueue, 0, "Do you want to leave the Queue?\n\nClick accept to confirm your actions", 0, false);
        else
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "Join Queue", LastManStanding_AddPlayerToQueue, 0, "Do you want to join the Queue?\n\nClick accept to confirm your actions", 0, false);
        AddGossipItemFor(player, GOSSIP_ICON_DOT, "----------------------------------", LastManStanding_MainMenu, 0);

        std::string info = TimeLeftBeforeQueuePop > 0 ?
            Trinity::StringFormat("Status:\n\nPlayers in queue: %u.\nTime until next match: %s", sLastManStandingMgr->GetActiveQueue().size(), secsToTimeString(TimeLeftBeforeQueuePop)) :
            Trinity::StringFormat("Status:\n\nPlayers in queue: %u.", sLastManStandingMgr->GetActiveQueue().size());

        SendGossipMenuFor(player, info, creature->GetGUID());
        return true;
    }

    static bool OnGossipSelect(Player* player, Creature* creature, int32 sender, int32 ItemSlot, std::any anything = 0)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (sender)
        {
            case LastManStanding_MainMenu:
                OnGossipHello(player, creature);
                break;
            case LastManStanding_AddPlayerToQueue:
            {
                if (!player->GetEventEnergy())
                {
                    creature->TalkToTarget("Sorry, you do not have enough energy to enter this event.", CHAT_MSG_MONSTER_SAY, player);
                    break;
                }

                // Engage Start
                auto const& result = sLastManStandingMgr->AddToQueue(player->GetGUID());
                if (result.first != PHASEMASK_NORMAL)
                {
                    creature->GetAI()->DoAction(result.first);
                    creature->TalkToTarget("The match is about to begin!", CHAT_MSG_MONSTER_SAY, player);
                }
                else if (result.second == MaxPlayers + 1) // No available matches
                {
                    creature->TalkToTarget("There are no available Battleground Phases to fight in, you will have to wait till a match is over.", CHAT_MSG_MONSTER_SAY, player);
                }
                else if (sLastManStandingMgr->GetActiveQueue().size() >= MinPlayers)
                {
                    if (TimeLeftBeforeQueuePop <= 0)
                    {
                        creature->GetAI()->DoAction(LastManStanding_CheckQueue);
                        TimeLeftBeforeQueuePop = TimeBeforeQueuePop;
                    }

                    creature->TalkToTarget(Trinity::StringFormat("You have been added to the queue, %u more is required for the battle to begin instantly. Else it will begin within %s.",
                        result.second, secsToTimeString(TimeLeftBeforeQueuePop)), CHAT_MSG_MONSTER_SAY, player);
                }
                else
                {
                    creature->TalkToTarget(Trinity::StringFormat("You have been added to the queue, %u more is required for the battle to begin starting process.", result.second), CHAT_MSG_MONSTER_SAY, player);
                }
                CloseGossipMenuFor(player);
            }
            break;
            case LastManStanding_RemovePlayerFromQueue:
            {
                sLastManStandingMgr->RemoveFromQueue(player->GetGUID());
                creature->TalkToTarget("You have been removed from the queue", CHAT_MSG_MONSTER_SAY, player);
                CloseGossipMenuFor(player);
            }
            break;
        }

        return true;
    }

    struct LastManStanding_MasterAI : public ScriptedAI
    {
    public:
        LastManStanding_MasterAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void BeginMatch(uint32 phase)
        {
            if (LastManStandingData* data = sLastManStandingMgr->BeginMatch(phase))
            {
                // Spawn Center
                data->SpawnCenter();
                sLastManStandingMgr->TeleportPlayers(data);
                sLastManStandingMgr->Events.ScheduleEvent(phase, 15000);
            }
            else
                TC_LOG_ERROR("server.loading", "KOTH: Unable to start match with phase id %u.", phase);
        }

        void DoAction(int32 phase) override
        {
            // Begin Queue Countdown
            if (phase == LastManStanding_CheckQueue)
                Events.ScheduleEvent(LastManStanding_CheckQueue, 1000);
            else
                BeginMatch(phase);
        }

        void UpdateAI(uint32 diff) override
        {
            Events.Update(diff);

            while (uint32 eventId = Events.ExecuteEvent())
            {
                if (eventId == LastManStanding_CheckQueue)
                {
                    if (sLastManStandingMgr->GetActiveQueue().size() < MinPlayers)
                    {
                        TimeLeftBeforeQueuePop = 0;
                        me->Say("Not enough players are in the queue.", LANG_UNIVERSAL);
                        break;
                    }

                    --TimeLeftBeforeQueuePop;
                    if (TimeLeftBeforeQueuePop <= 0)
                    {
                        // We are not at maximum players, but we start anyway as we are above minplayers
                        BeginMatch(sLastManStandingMgr->GetCurrentPhase());
                    }
                    else
                        Events.ScheduleEvent(LastManStanding_CheckQueue, 1000);
                }
            }
        }

    private:
        bool GossipHello(Player* player) override
        {
            return OnGossipHello(player, me);
        }

        bool GossipSelect(Player* player, uint32 menu_id, uint32 gossipListId) override
        {
            int32 sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            int32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            std::any anything = player->PlayerTalkClass->GetGossipOptionAny(gossipListId);
            return OnGossipSelect(player, me, sender, action, anything);
        }

        EventMap Events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new LastManStanding_MasterAI(creature);
    }
};

class LastManStanding_EventHandler : public WorldScript
{
public:
    LastManStanding_EventHandler() : WorldScript("LastManStanding_EventHandler")
    {
    }

    void OnUpdate(uint32 diff) override
    {
        if (sLastManStandingMgr->GetActiveEvents().empty())
            return;

        sLastManStandingMgr->Events.Update(diff);

        while (uint32 phase = sLastManStandingMgr->Events.ExecuteEvent())
        {
            auto const& event = sLastManStandingMgr->GetActiveEvents().find(phase);
            if (event != sLastManStandingMgr->GetActiveEvents().end())
            {
                if (event->second.FinishTime <= GameTime::GetGameTime() || event->second.GetAlivePlayers() == 1)
                    sLastManStandingMgr->EndEvent(phase);
                else
                {
                    // Send initial worldstates
                    if (!event->second.Started)
                    {
                        event->second.Started = true;
                        event->second.SendInitWorldStates(true);
                        event->second.UpdateWorldState(LMS_STATE_TIMER, uint32(event->second.FinishTime - GameTime::GetGameTime()));
                        event->second.UpdateWorldState(LMS_STATE_PLAYERS_ALIVE, event->second.GetAlivePlayers());
                    }

                    // Reduce Dome Size
                    if (event->second.center->GetObjectScale() > 1.0f)
                    {
                        if (--event->second.DomeReductionTime <= 0)
                        {
                            float newSize = std::max(1.0f, event->second.center->GetObjectScale() - DomeReductionValue);
                            event->second.center->SetObjectScale(newSize);
                            event->second.DomeReductionTime = DomeReductionTime;
                            event->second.SendMessageToAll("[|cffFF0000Event Message|r]: Dome size has been reduced!");
                        }
                        event->second.UpdateWorldState(LMS_STATE_STORM_REDUCTION_TIMER, event->second.DomeReductionTime);
                    }

                    // Update Timers
                    event->second.UpdateWorldState(LMS_STATE_TIMER, std::max(uint32(0), uint32(event->second.FinishTime - GameTime::GetGameTime())));

                    //event->second.PhasePlayers();
                    event->second.CheckPlayerPositions();
                    sLastManStandingMgr->Events.ScheduleEvent(phase, UpdateTimeInterval);
                }
            }
        }
    }
};

class LastManStanding_PvPHandler : public PlayerScript
{
public:
    LastManStanding_PvPHandler() : PlayerScript("LastManStanding_PvPHandler") {}

    void OnPlayerKilledByCreature(Creature* killer, Player* victim) override
    {
        if (sLastManStandingMgr->GetActiveEvents().empty())
            return;

        if (killer->GetAreaId() == AreaId)
        {
            uint32 phase = killer->GetPhaseMask();
            auto event = sLastManStandingMgr->GetActiveEvents().find(phase);
            if (event != sLastManStandingMgr->GetActiveEvents().end())
            {
                sLastManStandingMgr->RemoveFromMatch(phase, victim->GetGUID());

                victim->SetPhaseMask(PHASEMASK_NORMAL, true);
                if (Pet* pet = victim->GetPet())
                    pet->SetPhaseMask(PHASEMASK_NORMAL, true);

                if (Unit* owner = killer->GetOwner())
                    if (Player* player = owner->ToPlayer())
                        victim->GetSession()->SendNotification("You have been eliminated by %s!", killer->GetName().c_str());

                event->second.UpdateWorldState(LMS_STATE_PLAYERS_ALIVE, event->second.GetAlivePlayers());

                victim->SendUpdateWorldState(LMS_STATE_TIMER_ACTIVE, 0);
                victim->SendUpdateWorldState(LMS_STATE_PLAYERS_ALIVE_ACTIVE, 0);
                victim->SendUpdateWorldState(LMS_STATE_STORM_REDUCTION_TIMER_ACTIVE, 0);

                victim->ResurrectPlayer(1.0f);
                victim->TeleportTo(ReturnLocation, TELE_TO_NOT_UNSUMMON_PET);

                if (event->second.GetAlivePlayers() == 1)
                    sLastManStandingMgr->EndEvent(phase);
                else
                    event->second.SendMessageToAll(Trinity::StringFormat("[|cffFF0000Event Message|r]: There are |cffFF0000%u|r players alive!", event->second.GetAlivePlayers()));
            }
        }
    }

    void OnPVPKill(Player* killer, Player* victim) override
    {
        if (sLastManStandingMgr->GetActiveEvents().empty())
            return;

        if (killer->GetAreaId() == AreaId)
        {
            uint32 phase = killer->GetPhaseMask();
            auto event = sLastManStandingMgr->GetActiveEvents().find(phase);
            if (event != sLastManStandingMgr->GetActiveEvents().end())
            {
                sLastManStandingMgr->RemoveFromMatch(phase, victim->GetGUID());

                victim->SetPhaseMask(PHASEMASK_NORMAL, true);
                if (Pet* pet = victim->GetPet())
                    pet->SetPhaseMask(PHASEMASK_NORMAL, true);

                victim->GetSession()->SendNotification("You have been eliminated by %s!", killer->GetName().c_str());

                event->second.UpdateWorldState(LMS_STATE_PLAYERS_ALIVE, event->second.GetAlivePlayers());

                victim->SendUpdateWorldState(LMS_STATE_TIMER_ACTIVE, 0);
                victim->SendUpdateWorldState(LMS_STATE_PLAYERS_ALIVE_ACTIVE, 0);
                victim->SendUpdateWorldState(LMS_STATE_STORM_REDUCTION_TIMER_ACTIVE, 0);

                victim->ResurrectPlayer(1.0f);
                victim->TeleportTo(ReturnLocation, TELE_TO_NOT_UNSUMMON_PET);

                if (event->second.GetAlivePlayers() == 1)
                    sLastManStandingMgr->EndEvent(phase);
                else
                    event->second.SendMessageToAll(Trinity::StringFormat("[|cffFF0000Event Message|r]: There are |cffFF0000%u|r players alive!", event->second.GetAlivePlayers()));
            }
        }
    }

    void OnLogout(Player* player) override
    {
        ObjectGuid guid = player->GetGUID();

        if (sLastManStandingMgr->IsInQueue(guid))
        {
            sLastManStandingMgr->RemoveFromQueue(guid);
            return;
        }

        if (auto const& event = sLastManStandingMgr->GetMatchData(player))
        {
            sLastManStandingMgr->RemoveFromMatch(event->Phase, guid);

            if (event->GetAlivePlayers() == 1)
                sLastManStandingMgr->EndEvent(event->Phase);
            else
                event->SendMessageToAll(Trinity::StringFormat("[|cffFF0000Event Message|r]: There are |cffFF0000%u|r players alive!", event->GetAlivePlayers()));

            return;
        }
    }
};

void AddSC_LastManStanding()
{
    new LastManStanding_Master();
    new LastManStanding_EventHandler();
    new LastManStanding_PvPHandler();
}

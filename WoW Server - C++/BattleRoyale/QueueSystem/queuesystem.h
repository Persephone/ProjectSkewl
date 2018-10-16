#ifndef QUEUESYSTEM_H
#define QUEUESYSTEM_H

#include "Define.h"
#include "Player.h"

#include <optional>
#include <numeric>
#include <memory>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include "GameMode.h"

class Lobby;

namespace BattleRoyale
{
    class QueueSystem
    {
    public:
        static QueueSystem* instance();
        bool InQueue(Lobby* lobby) const;
        std::pair<bool, std::optional<std::string>> Enqueue(Lobby* lobby, MatchType type);
        std::pair<bool, std::optional<std::string>> Dequeue(Lobby* lobby);

    private:
        void OnEnqueue(Lobby* lobby);
        void UpdateQueue(MatchType type);

    private:
        std::unordered_map<MatchType, std::vector<Lobby*>> m_queue;
    };

    #define sQueueSystem QueueSystem::instance()
}

#endif 

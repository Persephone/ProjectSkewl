#include "QueueSystem.h"
#include "Lobby.h"

BattleRoyale::QueueSystem* BattleRoyale::QueueSystem::instance()
{
    static QueueSystem instance;
    return &instance;
}

bool BattleRoyale::QueueSystem::InQueue(Lobby* lobby) const
{
    for (auto it = m_queue.begin(); it != m_queue.end(); it++)
    {
        for (auto lobbyit = it->second.begin(); lobbyit != it->second.end(); lobbyit++)
        {
            Lobby* innerLobby = *lobbyit;
            if (lobby == innerLobby)
                return true;
        }
    }

    return false;
}

std::pair<bool, std::optional<std::string>> BattleRoyale::QueueSystem::Enqueue(Lobby* lobby, MatchType type)
{
    std::pair<bool, std::optional<std::string>> result;
    result.first = false;

    if (InQueue(lobby))
    {
        result.second = "Already in a queue!";
        return result;
    }

    GameMode* mode = sGameModeHandler->GetGameMode(type);
    if (!mode->IsEligible(lobby))
    {
        result.second = "Not eligible";
        return result;
    }

    m_queue[type].push_back(lobby);
    OnEnqueue(lobby);

    result.first = true;
    return result;
}

std::pair<bool, std::optional<std::string>> BattleRoyale::QueueSystem::Dequeue(Lobby* lobby)
{
    std::pair<bool, std::optional<std::string>> result;
    result.first = false;

    if (!InQueue(lobby))
    {
        result.second = "Not in a queue!";
        return result;
    }

    for (auto it = m_queue.begin(); it != m_queue.end(); it++)
    {
        if (it->second.empty())
            continue;

        auto& vec = it->second;
        auto rit = std::remove(vec.begin(), vec.end(), lobby);

        if (rit != vec.end())
            vec.erase(rit, vec.end());
    }

    result.first = true;
    return result;
}

void BattleRoyale::QueueSystem::OnEnqueue(Lobby* lobby) 
{
    UpdateQueue(lobby->GetMode()->GetType());
}

void BattleRoyale::QueueSystem::UpdateQueue(MatchType type)
{
    std::vector<Lobby*> lobbies = m_queue[type];

    auto sumPlayerCount = [](int sum, Lobby* lobby) { return sum + lobby->GetPlayers().size(); };
    int playersInQueue = std::accumulate(lobbies.begin(), lobbies.end(), 0, sumPlayerCount);

    GameMode* mode = sGameModeHandler->GetGameMode(type);
    if (playersInQueue > mode->GetMinPlayers() * mode->GetQueuePopTreshold())
    {
        // Queue pop
    }
}

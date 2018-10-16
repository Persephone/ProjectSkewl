#include "GameMode.h"
#include "Lobby.h"

BattleRoyale::GameMode::GameMode(MatchType type, uint32 maxLobbySize)
    : m_type(type), m_maxLobbySize(maxLobbySize)
{
    m_minPlayers = 1;
    m_maxPlayers = 20;
    m_queuePopTreshHold = 60;
}

BattleRoyale::MatchType BattleRoyale::GameMode::GetType() const
{
    return m_type;
}

uint32 BattleRoyale::GameMode::GetMaxLobbySize() const
{
    return m_maxLobbySize;
}

uint32 BattleRoyale::GameMode::GetMinPlayers() const
{
    return m_minPlayers;
}

uint32 BattleRoyale::GameMode::GetMaxPlayers() const
{
    return m_maxPlayers;
}

float BattleRoyale::GameMode::GetQueuePopTreshold() const
{
    return m_queuePopTreshHold;
}

bool BattleRoyale::GameMode::IsEligible(Lobby* lobby) const
{
    return lobby->GetPlayers().size() <= GetMaxLobbySize();
}

void BattleRoyale::GameMode::SetMinPlayers(uint32 minPlayers)
{
    m_minPlayers = minPlayers;
}

void BattleRoyale::GameMode::SetMaxPlayers(uint32 maxPlayers)
{
    m_maxPlayers = maxPlayers;
}

void BattleRoyale::GameMode::SetQueuePopTreshold(float queuePopTreshold)
{
    m_queuePopTreshHold = queuePopTreshold;
}

void BattleRoyale::GameModeHandler::AddGameMode(MatchType type, std::shared_ptr<GameMode> mode)
{
    m_gameModes[type] = mode;
}

BattleRoyale::GameModeHandler::GameModeHandler()
{
    std::shared_ptr<GameMode> solo = std::make_shared<GameMode>(MatchType::Solo, 1);
    solo->SetMinPlayers(10);
    solo->SetMaxPlayers(20);
    solo->SetQueuePopTreshold(0.7f);

    std::shared_ptr<GameMode> duo = std::make_shared<GameMode>(MatchType::Duo, 2);
    duo->SetMinPlayers(20);
    duo->SetMaxPlayers(40);
    duo->SetQueuePopTreshold(0.7f);

    std::shared_ptr<GameMode> quad = std::make_shared<GameMode>(MatchType::Quad, 4);
    quad->SetMinPlayers(40);
    quad->SetMaxPlayers(80);
    quad->SetQueuePopTreshold(0.7f);

    AddGameMode(MatchType::Solo, solo);
    AddGameMode(MatchType::Duo, duo);
    AddGameMode(MatchType::Quad, quad);
}

BattleRoyale::GameMode* BattleRoyale::GameModeHandler::GetGameMode(MatchType type) const
{
    return m_gameModes.at(type).get();
}

BattleRoyale::GameModeHandler * BattleRoyale::GameModeHandler::instance()
{
    static GameModeHandler instance;
    return &instance;
}

#include "Lobby.h"
#include "ScriptMgr.h"
#include "Chat.h"

uint32 BattleRoyale::Lobby::Count = 0;

BattleRoyale::Lobby::Lobby(Player* player)
    :  m_mode(sGameModeHandler->GetGameMode(MatchType::Solo)),
    m_maxPlayerCount(4)
{
    m_id = Count;
    Count++;
    m_owner = player;
}

bool BattleRoyale::Lobby::operator==(const Lobby& other) const
{
    return GetId() == other.GetId();
}

bool BattleRoyale::Lobby::AddPlayer(Player* player)
{
    if (IsMember(player) || GetPlayers().size() + 1 > GetMaxPlayerCount())
        return false;

    m_members.push_back(player);
    m_readyStatus[player] = false;

    player->SetLobby(shared_from_this());
    sScriptMgr->OnBattleRoyaleAddPlayer(player);
    return true;
}

bool BattleRoyale::Lobby::KickPlayer(Player* player)
{
    if (!IsMember(player))
        return false;

    m_members.erase(std::remove(m_members.begin(), m_members.end(), player), m_members.end());
    m_readyStatus.erase(player);

    if (IsOwner(player))
        ChooseNewOwner();

    player->SetPhaseMask(PHASEMASK_NORMAL, true); // has to be changed to unique phasing.

    OnMembersChange();
    return true;
}

bool BattleRoyale::Lobby::IsMember(Player* player) const
{
    std::vector<Player*> players = GetPlayers();
    return std::find(players.begin(), players.end(), player) != players.end();
}

bool BattleRoyale::Lobby::IsOwner(Player* player) const
{
    return m_owner == player;
}

Player* BattleRoyale::Lobby::GetOwner() const
{
    return m_owner;
}

std::vector<Player*> BattleRoyale::Lobby::GetPlayers() const
{
    return m_members;
}

bool BattleRoyale::Lobby::IsReady() const
{
    for (Player* const& player : GetPlayers())
        if (!IsReady(player))
            return false;

    return true;
}

bool BattleRoyale::Lobby::IsReady(Player* player) const
{
    return m_readyStatus.at(player);
}

void BattleRoyale::Lobby::SetReady(Player* player, bool status)
{
    m_readyStatus[player] = status;
}

const BattleRoyale::GameMode* BattleRoyale::Lobby::GetMode() const
{
    return m_mode;
}

bool BattleRoyale::Lobby::IsFull() const
{
    return GetMemberCount() == GetMaxPlayerCount();
}

uint32 BattleRoyale::Lobby::GetMaxPlayerCount() const
{
    return m_maxPlayerCount;
}

uint8 BattleRoyale::Lobby::GetMemberCount() const
{
    return m_members.size();
}

uint32 BattleRoyale::Lobby::GetId() const
{
    return m_id;
}

void BattleRoyale::Lobby::BroadcastMessage(char const* fmt)
{
    for (Player* const& player : GetPlayers())
        if (player)
            ChatHandler(player->GetSession()).SendSysMessage(fmt);
}

void BattleRoyale::Lobby::ChooseNewOwner()
{
    if (GetPlayers().size() < 1)
        return;

    m_owner = m_members[0];
}

void BattleRoyale::Lobby::OnMembersChange()
{
    if (GetPlayers().size() == 0)
        sQueueSystem->Dequeue(this);
}

bool BattleRoyale::Lobby::SetMode(GameMode* mode)
{
    if (!mode->IsEligible(this))
        return false;

    m_mode = mode;
    return true;
}

#ifndef LOBBY_H
#define LOBBY_H

#include "Define.h" 
#include <memory>
#include <optional>

#include "GameMode.h"
#include "QueueSystem.h"

namespace BattleRoyale
{
    class Lobby : public std::enable_shared_from_this<Lobby>
    {
    public:
        Lobby(Player* player);
        bool operator==(const Lobby& other) const;

        bool AddPlayer(Player* player);
        bool KickPlayer(Player* player);
        bool IsMember(Player* player) const;
        bool IsOwner(Player* player) const;
        bool IsReady() const;
        bool IsReady(Player* player) const;
        void SetReady(Player* player, bool status = true);
        bool SetMode(GameMode* mode);
        bool IsFull() const;

        void BroadcastMessage(char const* fmt);
        
        template<typename... Args>
        void BroadcastMessage(char const* fmt, Args&&... args)
        {
            BroadcastMessage(Trinity::StringFormat(fmt, std::forward<Args>(args)...).c_str());
        }

        Player* GetOwner() const;
        std::vector<Player*> GetPlayers() const; 
        const GameMode* GetMode() const;
        uint32 GetMaxPlayerCount() const;
        uint32 GetId() const;
        uint8  GetMemberCount() const;

    private:
        void ChooseNewOwner();

        void OnMembersChange();

        uint32 m_id;
        Player* m_owner;
        uint32 m_maxPlayerCount;
        std::vector<Player*> m_members; 
        GameMode* m_mode;
        std::unordered_map<Player*, bool> m_readyStatus;

        static uint32 Count;
    };
}

#endif

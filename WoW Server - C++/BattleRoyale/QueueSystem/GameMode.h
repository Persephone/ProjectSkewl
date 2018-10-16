#ifndef GAMEMODE_H
#define GAMEMODE_H

#include <memory>
#include <unordered_map>

namespace BattleRoyale
{
    class Lobby;

    enum MatchType
    {
        Solo,
        Duo,
        Quad
    };

    class GameMode
    {
    public:
        GameMode(MatchType type, uint32 maxLobbySize);
        virtual bool IsEligible(Lobby* lobby) const;

        void SetMinPlayers(uint32 minPlayers);
        void SetMaxPlayers(uint32 maxPlayers);

        // A value of 50% with a MinPlayers count to 10 would make queue pop at 5 but not start before 20 (or wait time has run out).
        void SetQueuePopTreshold(float queuePopTreshold); 

        MatchType GetType() const;
        uint32 GetMaxLobbySize() const;
        uint32 GetMinPlayers() const;
        uint32 GetMaxPlayers() const;
        float GetQueuePopTreshold() const;

    private:
        MatchType m_type;
        uint32 m_maxLobbySize;
        uint32 m_minPlayers;
        uint32 m_maxPlayers;
        float m_queuePopTreshHold;

        friend class GameModeHandler;
    };

    class GameModeHandler
    {
    public:
        GameModeHandler();
        GameMode* GetGameMode(MatchType type) const;
        static GameModeHandler* instance();

    private:
        void AddGameMode(MatchType type, std::shared_ptr<GameMode> mode);
        std::unordered_map<MatchType, std::shared_ptr<GameMode>> m_gameModes;
    };

    #define sGameModeHandler GameModeHandler::instance()
}

#endif

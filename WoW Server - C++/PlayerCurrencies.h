#ifndef PLAYER_CURRENCIES_H
#define PLAYER_CURRENCIES_H

#include <memory>
#include <unordered_map>

struct CharacterCurrency
{
    int8   WinStreak;
    int8   TopWinStreak;
    float  RankPoints;
    float  TopRankPoints;
    time_t DailyQuestTimer;
    uint8  DailyQuestAmount;
    int32  KillingSpree;
    uint16 TotalUpgradeRank;

    void ModifyRankPoints(float value, Player* player = nullptr);
    void ModifyKillingSpree(int32 points);
    void ModifyTotalUpgradeRank(int32 amount);
    void ModifyWinStreak(Player* player, int32 points);
    void HandleDailyQuests(Player* player);
    void ReduceDailyAmount();
};

struct AccountCurrency
{
    uint32 Account;
    int32  VP;
    int32  DP;
    uint32 ExtraMask;

    void ModifyVP(int value);
    void ModifyDP(int value);
    bool HasExtraMask(uint32 mask);
    void SetExtraMask(uint32 mask);
    uint32 GetDP() const { return DP; }
    uint32 GetVP() const { return VP; }
    void SaveData();
};

class TC_GAME_API CurrencyHandler
{
public:
    CurrencyHandler();
    static CurrencyHandler* instance();

    CharacterCurrency* GetCharacterCurrency(ObjectGuid guid);
    void LoadCharacterCurrencies();
    void AddCharacterCurrency(ObjectGuid guid);
    void SaveCharacterCurrencies(ObjectGuid guid);
    const std::unordered_map<ObjectGuid, CharacterCurrency> &GetCharacterCurrencies() const { return m_CharacterCurrencies; }

    AccountCurrency* GetAccountCurrency(uint32 account);
    void LoadAccountCurrencies();
    void AddAccountCurrencyData(uint32 account);

private:
    std::unordered_map<ObjectGuid, CharacterCurrency> m_CharacterCurrencies;
    std::unordered_map<uint32, AccountCurrency> m_AccountCurrencies;
};

#define sCurrencyHandler CurrencyHandler::instance()

#endif

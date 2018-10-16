#include "PlayerCurrencies.h"
#include "CustomMgr.h"
#include "Containers.h"
#include "Chat.h"
#include "GameTime.h"

CharacterCurrency* CurrencyHandler::GetCharacterCurrency(ObjectGuid guid)
{
    return Trinity::Containers::MapGetValuePtr(m_CharacterCurrencies, guid);
}

AccountCurrency* CurrencyHandler::GetAccountCurrency(uint32 account)
{
    return Trinity::Containers::MapGetValuePtr(m_AccountCurrencies, account);
}

void CurrencyHandler::AddCharacterCurrency(ObjectGuid guid)
{
    CharacterCurrency currency;
    currency.WinStreak        = 0;
    currency.TopWinStreak     = 0;
    currency.RankPoints       = 0.0f;
    currency.TopRankPoints    = 0.0f;
    currency.DailyQuestTimer  = 0;
    currency.DailyQuestAmount = 0;
    currency.KillingSpree     = 0;
    currency.TotalUpgradeRank = 0;
    m_CharacterCurrencies[guid] = currency;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CUSTOM_CURRENCIES);
    stmt->setUInt32(0, guid.GetCounter());
    CharacterDatabase.Execute(stmt);
}

void CurrencyHandler::SaveCharacterCurrencies(ObjectGuid guid)
{
    if (CharacterCurrency* val = GetCharacterCurrency(guid))
    {
        SQLTransaction trans = CharacterDatabase.BeginTransaction();
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CUSTOM_CURRENCIES);
        stmt->setUInt8 (0, val->WinStreak);
        stmt->setUInt8 (1, val->TopWinStreak);
        stmt->setFloat (2, val->RankPoints);
        stmt->setFloat (3, val->TopRankPoints);
        stmt->setUInt32(4, uint32(val->DailyQuestTimer));
        stmt->setUInt8 (5, val->DailyQuestAmount);
        stmt->setInt32 (6, val->KillingSpree);
        stmt->setInt16 (7, val->TotalUpgradeRank);
        stmt->setUInt32(8, guid.GetCounter());
        trans->Append(stmt);
        CharacterDatabase.CommitTransaction(trans);
    }
}

inline void GiveRandomQuest(Player* player)
{
    std::vector<uint32> quests;
    for (uint32 const id : sCustomMgr->DailyQuests) // loop through the quests
        if (!player->IsActiveQuest(id))
            quests.push_back(id); // those quests the player doesnt have add them to available list

    if (!quests.empty())
    {
        Quest const* questInfo = sObjectMgr->GetQuestTemplate(quests[urand(0, quests.size())]); // get a random result from the vector
        if (!questInfo)
            return;
        if (player->CanAddQuest(questInfo, true))
        {
            player->AddQuestAndCheckCompletion(questInfo, NULL);
            player->GetSession()->SendAreaTriggerMessage("You have gotten a new Daily Quest!");
        }
    }
}

void CharacterCurrency::HandleDailyQuests(Player* player)
{
    time_t now = GameTime::GetGameTime();

    if (DailyQuestTimer < now && DailyQuestAmount < 3)
    {
        DailyQuestTimer = now + DAY;
        DailyQuestAmount += 1;
        GiveRandomQuest(player);
    }
}

void CharacterCurrency::ReduceDailyAmount()
{
    if (DailyQuestAmount > 0)
        DailyQuestAmount = DailyQuestAmount -= 1;
    else
        DailyQuestAmount = 0;
}

void CharacterCurrency::ModifyRankPoints(float value, Player* player)
{
    float currentRank = RankPoints;
    float newAmount = currentRank + value;

    if (newAmount < 0.0f) {
        newAmount = 0.0f;
    }

    if (newAmount > 50000) {
        newAmount = 50000.0f;
    }

    uint16 iNewAmount = uint16(newAmount);

    if (player)
    {
        sCustomMgr->AssignTitle(player, iNewAmount);
        ChatHandler(player->GetSession()).PSendSysMessage("|cff00FFFFYour ranking is now at:|r |cffFF0000%u|r |cff00FFFFRank Points.|r", iNewAmount);
    }
    RankPoints = newAmount;
    if (newAmount > TopRankPoints)
    {
        TopRankPoints = newAmount;
        if (player)
            ChatHandler(player->GetSession()).PSendSysMessage("|cff00FFFFYour top ranking is now at:|r |cffFF0000%u|r |cff00FFFFRank Points.|r", iNewAmount);
    }
}

void CharacterCurrency::ModifyWinStreak(Player* player, int32 points)
{
    int newAmount = WinStreak + points;

    // Hard limit people to 1000DP
    if (newAmount > 100) {
        newAmount = 100;
    }
    if (newAmount < 0)
        newAmount = 0;

    WinStreak = newAmount;
    if (newAmount > TopWinStreak)
    {
        TopWinStreak = newAmount;
        ChatHandler(player->GetSession()).PSendSysMessage("|cff00FFFFYour top Win Streak is now at:|r |cffFF0000%u|r", uint32(newAmount));
    }
}

void CharacterCurrency::ModifyKillingSpree(int32 points)
{
    int newAmount = KillingSpree + points;

    // Hard limit people to 1000DP
    if (newAmount > 1000)
        newAmount = 1000;

    if (newAmount < 0)
        newAmount = 0;

    KillingSpree = newAmount;
}

void CharacterCurrency::ModifyTotalUpgradeRank(int32 amount)
{
    int newAmount = TotalUpgradeRank + amount;

    if (newAmount > 500)
        newAmount = 500;

    if (newAmount < 0)
        newAmount = 0;

    TotalUpgradeRank = newAmount;
}

void CurrencyHandler::LoadCharacterCurrencies()
{
    // Clean up non-existing data
    CharacterDatabase.DirectExecute("DELETE FROM character_currencies WHERE guid NOT IN (SELECT guid FROM characters)");

    uint32 oldMSTime = getMSTime();

    QueryResult result = CharacterDatabase.Query("SELECT guid, WinStreak, TopWinStreak, RankPoints, TopRankPoints, DailyQuestTimer, DailyQuestAmount, KillingSpree, TotalUpgradeRank FROM character_currencies");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 character currencies. DB table `character_currencies` is empty.");
        return;
    }
    else
    {
        uint32 count = 0;
        do
        {
            Field *fields = result->Fetch();

            CharacterCurrency currency;
            ObjectGuid guid           = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
            currency.WinStreak        = fields[1].GetInt8();
            currency.TopWinStreak     = fields[2].GetInt8();
            currency.RankPoints       = fields[3].GetFloat();
            currency.TopRankPoints    = fields[4].GetFloat();
            currency.DailyQuestTimer  = time_t(fields[5].GetUInt64());
            currency.DailyQuestAmount = fields[6].GetUInt8();
            currency.KillingSpree     = fields[7].GetInt32();
            currency.TotalUpgradeRank = fields[8].GetInt16();

            // fix any corrupt data
            if (currency.WinStreak < 0)
                currency.WinStreak = 0;
            if (currency.RankPoints < 0.0f)
                currency.RankPoints = 0.0f;
            if (currency.KillingSpree < 0)
                currency.KillingSpree = 0;
            if (currency.DailyQuestAmount > 3)
                currency.DailyQuestAmount = 3;
            if (currency.TotalUpgradeRank < 0)
                currency.TotalUpgradeRank = 0;

            m_CharacterCurrencies[guid] = currency;
            ++count;

        } while (result->NextRow());

        TC_LOG_INFO("server.loading", ">> Loaded %u chararacter currency data", count);
    }
}
#pragma endregion CharacterCurrency

#pragma region AccountCurrency
void CurrencyHandler::AddAccountCurrencyData(uint32 account)
{
    AccountCurrency acurrency;
    acurrency.Account = account;
    acurrency.VP = 0;
    acurrency.DP = 0;
    acurrency.ExtraMask = 0;
    m_AccountCurrencies[account] = acurrency;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CUSTOM_ACCOUNT_CURRENCIES);
    stmt->setUInt32(0, acurrency.Account);
    stmt->setInt32(1, acurrency.VP);
    stmt->setInt32(2, acurrency.DP);
    stmt->setUInt32(3, acurrency.ExtraMask);
    CharacterDatabase.Execute(stmt);
}

void AccountCurrency::ModifyDP(int value)
{
    int newAmount = DP + value;

    // Hard limit people to 1000DP
    if (newAmount > 1000) {
        newAmount = 1000;
    }
    if (newAmount < 0)
        newAmount = 0;

    DP = newAmount;
    SaveData();
}

void AccountCurrency::ModifyVP(int value)
{
    int newAmount = VP + value;

    // Hard limit people to 1000VP
    if (newAmount > 1000) {
        newAmount = 1000;
    }
    if (newAmount < 0)
        newAmount = 0;

    VP = newAmount;
    SaveData();
}

bool AccountCurrency::HasExtraMask(uint32 mask)
{
    if ((ExtraMask & mask) != 0)
        return true;

    return false;
}

void AccountCurrency::SetExtraMask(uint32 mask)
{
    ExtraMask |= mask;
    SaveData();
}

void AccountCurrency::SaveData()
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CUSTOM_ACCOUNT_CURRENCIES);
    stmt->setInt32 (0, VP);
    stmt->setInt32 (1, DP);
    stmt->setUInt32(2, ExtraMask);
    stmt->setUInt32(3, Account);
    CharacterDatabase.Execute(stmt);
}

void CurrencyHandler::LoadAccountCurrencies()
{
    // Get the time before we started loading.
    uint32 oldMSTime = getMSTime();

    QueryResult result = CharacterDatabase.Query("SELECT id, vp, dp, ExtraMask FROM website.account_data");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 account currencies. DB table `account_currencies` is empty.");
        return;
    }
    else
    {
        uint32 count = 0;
        do
        {
            Field *fields = result->Fetch();

            AccountCurrency currency;
            currency.Account    = fields[0].GetUInt32();
            currency.VP         = fields[1].GetInt32();
            currency.DP         = fields[2].GetInt32();
            currency.ExtraMask  = fields[3].GetUInt32();

            // fix any corrupt data
            if (currency.VP < 0)
                currency.VP = 0;
            if (currency.DP < 0)
                currency.DP = 0;

            m_AccountCurrencies[currency.Account] = currency;
            ++count;

        } while (result->NextRow());

        TC_LOG_INFO("server.loading", ">> Loaded %u account currency data", count);
    }
}
#pragma endregion

CurrencyHandler::CurrencyHandler()
{
}

CurrencyHandler* CurrencyHandler::instance()
{
    static CurrencyHandler instance;
    return &instance;
}

#include "CustomMgr.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "Battleground.h"
#include "Creature.h"
#include "Containers.h"
#include "PlayerCurrencies.h"
#include "Item.h"
#include "Log.h"
#include "Language.h"
#include "World.h"
#include "GameTime.h"
#include "Guild.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "SocialMgr.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellAuras.h"
#include "StringFormat.h"
#include "Random.h"
#include "Config.h"
#include "CharacterCache.h"
#include "Chat.h"
#include "Battleground.h"
#include "WorldSession.h"
#include "Player.h"
#include "Lobby.h"
#include <regex>
#include <algorithm>

CustomMgr::CustomMgr()
{
    m_SelectedEventBG = BATTLEGROUND_TYPE_NONE;
    m_ExtraTalentBG = BATTLEGROUND_TYPE_NONE;
    HighScoreGuid = ObjectGuid::Empty;
    WorldChatStatus = true;
    EventRunning = false;
    m_MallLocationA = WorldLocation(603, 1525.103638f, 119.818726f, 427.316956f, 0.011676f);
    m_MallLocationH = WorldLocation(603, 1525.103638f, 119.818726f, 427.316956f, 0.011676f);
}

#pragma region RankData

VQRank CustomMgr::GetRankDataByPoints(uint16 points, ObjectGuid guid /*= ObjectGuid::Empty*/)
{
    if (guid == HighScoreGuid)
        return VQRank(points, "Grand Marshal", "High Warlord", "00FF00", "Achievement_PVP_A_14", "Achievement_PVP_H_14");

    VQRank rank = VQRank();
    for(VQRankListType::iterator it = VQRankList.begin(); it != VQRankList.end(); ++it)
    {
        if (it->pointsNeeded <= points)
            rank = *it;
        else
            break;
    }
    return rank;
}

VQRank CustomMgr::GetRankDataByName(std::string name)
{
    for(VQRankListType::iterator it = VQRankList.begin(); it != VQRankList.end(); ++it)
    {
        if(it->nameA == name)
            return *it;
    }
    return VQRank();
}

VQRank CustomMgr::GetRankDataByColor(std::string color)
{
    for(VQRankListType::iterator it = VQRankList.begin(); it != VQRankList.end(); ++it)
    {
        if(it->color == color)
            return *it;
    }
    return VQRank();
}

void CustomMgr::AddRank(uint16 pointsNeeded, std::string nameA, std::string nameH, std::string color, std::string iconA, std::string iconH)
{
    if(nameA.empty() || color.empty() || iconA.empty())
        return;
    
    for(VQRankListType::iterator it = VQRankList.begin(); it != VQRankList.end(); ++it)
        if(it->pointsNeeded >= pointsNeeded)
            return;

    VQRankList.push_back(VQRank(pointsNeeded, nameA, nameH, color, iconA, iconH));
}

void CustomMgr::LoadRanks()
{
    // Get the time before we started loading.
    uint32 oldMSTime = getMSTime();

    //                                                      0             1          2
    QueryResult result = WorldDatabase.Query("SELECT rankPointsNeeded, rankNameA, rankNameH, rankColor, rankIconA, rankIconH FROM custom_ranks ORDER BY rankPointsNeeded ASC");

    //If no rows found
    if (!result)
    {
        //Display something
        TC_LOG_INFO("server.loading", ">> Loaded 0 VQ ranks. DB table `custom_ranks` is empty.");
    }
    else
    {
        //Continue loading.

        //Reset row counter
        uint32 count = 0;

        //Loop through the results
        do
        {
            // Get the fields
            Field *fields = result->Fetch();
            AddRank(fields[0].GetUInt16(), fields[1].GetString(), fields[2].GetString(), fields[3].GetString(), fields[4].GetString(), fields[5].GetString());

            //Increase row counter
            ++count;
        } while (result->NextRow());

        // Log that we loaded everything.
        TC_LOG_INFO("server.loading", ">> Loaded %u VQ", count);
    }
}
#pragma endregion

#pragma region GuildBonuses

GuildLevelBonuses* CustomMgr::GetGuildBonusByLevel(uint8 level)
{
    for (auto it = GuildBonusInfo.begin(); it != GuildBonusInfo.end(); it++)
        if (it->Level == level)
            return &(*it);

    return nullptr;
}

float CustomMgr::GetGuildBonus(uint8 level, GuildBonus bonus)
{
    for (auto const& it : GuildBonusInfo)
        if (it.Level == level && it.Bonus == bonus)
           return it.Multiplier;

    return 0.0f;
}

std::string CustomMgr::GetGuildBonusName(uint8 bonus) const
{
    std::string bonusName = "";
    switch (bonus)
    {
        case 0:  bonusName = "|TInterface\\icons\\INV_Misc_Coin_01:28:28:-22:0|tGold Loot Bonus"; break;
        case 1:  bonusName = "|TInterface\\icons\\Achievement_Arena_2v2_7:28:28:-22:0|tHonor Bonus"; break;
        case 2:  bonusName = "|TInterface\\icons\\Ability_Paladin_BeaconofLight:28:28:-22:0|tVendor Discount Bonus"; break;
        case 3:  bonusName = "|TInterface\\icons\\achievement_reputation_05:28:28:-22:0|tReputation Bonus"; break;
        case 4:  bonusName = "|TInterface\\icons\\INV_Misc_Rune_07:28:28:-22:0|tMarks Bonus"; break;
        case 5:  bonusName = "|TInterface\\icons\\Achievement_Arena_3v3_7:28:28:-22:0|tRank Points Booster."; break;
        default: bonusName = "Error"; break;
    }
    return bonusName;
}

std::string CustomMgr::GetCondName(uint32 cond) const
{
    std::string condName = "";
    switch (cond)
    {
    case 1:  condName = "|TInterface/InventoryItems/WoWUnknownItem01:28:28:-22:0|tItem Requirement"; break;
    case 2:  condName = "|TInterface/icons\\Ability_Paladin_BeaconofLight:28:28:-22:0|tTitle Requirement"; break;
    case 3:  condName = "|TInterface/icons\\INV_Misc_Coin_01:28:28:-22:0|tGold Requirement"; break;
    case 4:  condName = "|TInterface/icons\\Achievement_Arena_2v2_7:28:28:-22:0|tHonor Requirement"; break;
    case 5:  condName = "|TInterface/icons\\achievement_reputation_05:28:28:-22:0|tArena Requirement"; break;
    case 6:  condName = "|TInterface/icons\\achievement_reputation_05:28:28:-22:0|tRankpoints Requirement"; break;
    case 7:  condName = "|TInterface/icons\\INV_Misc_Rune_07:28:28:-22:0|tVP Requirement"; break;
    case 8:  condName = "|TInterface/icons\\Achievement_Arena_3v3_7:28:28:-22:0|tDP Requirement"; break;
    default: condName = "Error"; break;
    }
    return condName;
}

void CustomMgr::ListGuildBonusesForGossip(Player* player, Guild* guild, int32 sender, int32 action)
{
    bool found = false;
    if (guild)
    {
        for (auto const &val : GuildBonusInfo)
        {
            if (val.Level == guild->GetLevel())
            {
                std::string info = Trinity::StringFormat("%s : +%u%s", GetGuildBonusName(val.Bonus), val.Multiplier < 1.0f ? uint8(val.Multiplier * 100) : uint8(val.Multiplier), val.Bonus == 4 ? " Marks / Win." : "%");
                AddGossipItemFor(player, GOSSIP_ICON_DOT, info, sender, action);
                found = true;
            }
        }
    }

    if (!found)
        AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface\\icons\\Achievement_worldevent_lunar:27:28:-22:0|tNo Bonuses.", sender, action);
}

void CustomMgr::LoadGuildBonusInfo()
{
    if (!GuildBonusInfo.empty())
        GuildBonusInfo.clear();

    // Get the time before we started loading.
    uint32 oldMSTime = getMSTime();

    //                                                      0             1          2
    QueryResult result = CharacterDatabase.Query("SELECT Level, Bonus, Multiplier FROM guild_bonus_info");

    //If no rows found
    if (!result)
    {
        //Display something
        TC_LOG_INFO("server.loading", ">> Loaded 0 guild bonuses. DB table `guild_bonus_info` is empty.");
        return;
    }
    else
    {
        //Continue loading.

        //Reset row counter
        uint32 count = 0;

        //Loop through the results
        do
        {
            // Get the fields
            Field *fields = result->Fetch();

            GuildLevelBonuses info;
            info.Level      = fields[0].GetUInt8();
            info.Bonus      = GuildBonus(fields[1].GetUInt8());
            info.Multiplier = fields[2].GetFloat();

            GuildBonusInfo.push_back(info);
            //Increase row counter
            ++count;

        } while (result->NextRow());

        // Log that we loaded everything.
        TC_LOG_INFO("server.loading", ">> Loaded %u Guild Bonuses", count);
    }
}

#pragma endregion

void CustomMgr::RemoveKillStreakData(ObjectGuid guid)
{
    KillStreakData::const_iterator itr = KillStreakContainer.find(guid);
    if (!KillStreakContainer.empty() && itr != KillStreakContainer.end())
        KillStreakContainer.erase(guid);
}

void CustomMgr::HandleBGKill(Battleground* bg, Player* killer, Player* victim)
{
    // Don't reward credit for killing ourselves, like fall damage of hellfire (warlock)
    if (killer == victim)
        return;

    if (killer->GetSession()->GetRemoteAddress() == victim->GetSession()->GetRemoteAddress())
    {
        ChatHandler(killer->GetSession()).PSendSysMessage("|cffFF0000No rewards given for killing someone with same IP-Address as you.|r");
        return;
    }

    if (victim->GetGUID() == KillStreakContainer[killer->GetGUID()].lastkill)
    {
        ChatHandler(killer->GetSession()).PSendSysMessage("|cffFF0000You recently killed this player, no rewards added.|r");
        return;
    }

    if (victim->IsActiveQuest(50707))
        victim->KilledMonsterCredit(7);

    if (killer->IsActiveQuest(50509))
        killer->KilledMonsterCredit(10);

    if (killer->IsActiveQuest(50514))
        killer->KilledMonsterCredit(10);

    if (killer->IsActiveQuest(50519))
        killer->KilledMonsterCredit(10);

    if (killer->IsActiveQuest(50523))
        killer->KilledMonsterCredit(10);

    if (victim->IsActiveQuest(50525))
        victim->KilledMonsterCredit(10);

    if (victim->IsActiveQuest(50532))
        victim->KilledMonsterCredit(10);

    if (victim->IsActiveQuest(50540))
        victim->KilledMonsterCredit(10);

    ObjectGuid KillerGUID = killer->GetGUID();
    ObjectGuid VictimGUID = victim->GetGUID();
    bool isVip = IsVIP(killer->GetSession()->GetAccountId());

    uint32 victimKillStreak = KillStreakContainer[VictimGUID].killstreak;

    KillStreakContainer[KillerGUID].killstreak++;
    KillStreakContainer[KillerGUID].lastkill = VictimGUID;
    KillStreakContainer[KillerGUID].deathstreak = 0;
    KillStreakContainer[VictimGUID].killstreak = 0;
    KillStreakContainer[VictimGUID].lastkill = ObjectGuid::Empty;
    KillStreakContainer[VictimGUID].deathstreak++;

    int32 killerKillStreak = KillStreakContainer[KillerGUID].killstreak;

    uint32 money = (urand(30, 60) * SILVER) + (isVip ? 20 * SILVER : 0)          // extra money if the killer is a vip
        - (KillStreakContainer[VictimGUID].deathstreak * 5 * SILVER)  // less money given if the target has died a lot, to prevent farming
        + (KillStreakContainer[VictimGUID].killstreak * 10 * SILVER); // extra money given if the target had a high killstreak

    if (money < SILVER) // minimum given will be 1 silver.
        money = SILVER;

    killer->ModifyMoney(money);

    if (victimKillStreak >= 3)
    {
        std::ostringstream ss;
        ss << "|cffFF0000[PvP System]|r:|cffFF0000 ";
        ss << killer->GetName();
        ss << "|r has ended the killing spree of|cffFF0000 ";
        ss << victim->GetName();
        ss << "|r and received|cffFF0000 ";
        ss << MoneyToMoneyString(money);
        bg->PSendMessageToAll(SERVER_MSG_STRING, CHAT_MSG_SYSTEM, NULL, ss.str().c_str());
    }

    if (killerKillStreak >= 3)
    {
        std::ostringstream ss;
        ss << "|cffFF0000[PvP System]|r: |cffFF0000";
        ss << killer->GetName();
        ss << "|r has killed |cffFF0000";
        ss << victim->GetName();
        ss << " |rand ";
        ss << GetSpreeString(KillStreakContainer[KillerGUID].killstreak, killer);
        bg->PSendMessageToAll(SERVER_MSG_STRING, CHAT_MSG_SYSTEM, NULL, ss.str().c_str());
        killer->AddAura(64328, killer);

        if (CharacterCurrency* currency = killer->GetCurrency())
            if (currency->KillingSpree < killerKillStreak)
                currency->KillingSpree = killerKillStreak;
    }

    switch (killerKillStreak) // killstreak specific events
    {
    case 3:
        if (killer->IsActiveQuest(50052))
            killer->KilledMonsterCredit(105000);
        break;
    case 5:
    {
        if (Aura* aura = killer->AddAura(48279, killer))
            aura->SetDuration(5000);
    }
    break;
    case 6:
        if (killer->IsActiveQuest(50053))
            killer->KilledMonsterCredit(105001);
        break;
    case 9:
        if (killer->IsActiveQuest(50054))
            killer->KilledMonsterCredit(105002);
        break;
    case 10:
    {
        if (Aura* aura = killer->AddAura(61427, killer))
            aura->SetDuration(10000);
    }
    break;
    case 12:
        if (killer->IsActiveQuest(50055))
            killer->KilledMonsterCredit(105003);
        break;
    case 15:
        if (killer->IsActiveQuest(50056))
            killer->KilledMonsterCredit(105004);
        break;
    case 20:
    {
        if (killer->IsActiveQuest(50057))
            killer->KilledMonsterCredit(105005);

        killer->AddAura(64328, killer); // visual 
        killer->AddAura(2983, killer); // speed
        killer->AddAura(24378, killer); // berserk
        killer->GetCurrency()->ModifyRankPoints(20, killer);
        ChatHandler(killer->GetSession()).PSendSysMessage("|cffFF0000[PvP System]|r: You have been awarded |cffFF000020|r Rank Points for achieving 20 kills in a row!");
    }
    break;
    case 30:
    {
        killer->AddAura(64328, killer); // visual 
        killer->AddAura(2983, killer); // speed
        killer->AddAura(24378, killer); // berserk
        killer->SetTitle(sCharTitlesStore.LookupEntry(141));
        ChatHandler(killer->GetSession()).PSendSysMessage("|cffFF0000[PvP System]|r: You have been awarded with a new title for achieving 30 kills in a row!");
    }
    break;
    }

    killer->GetSession()->SendAreaTriggerMessage("|cffFF0000[PvP System]|r: You got |cffFF0000%u|r killstreak, keep going!", killerKillStreak);
}

std::string CustomMgr::GetSpreeString(int value, Player* killer)
{
    std::string SpreeString = "";
    switch (value)
    {
    case 3:
        SpreeString = "is on a killing spree!";
        break;
    case 4:
        SpreeString = "is on a rampage!";
        break;
    case 5:
        SpreeString = "is dominating!";
        break;
    case 6:
        SpreeString = "is unstoppable!";
        break;
    case 7:
        SpreeString = "is godlike!";
        break;
    case 8:
        SpreeString = "is Impressive!";
        break;
    case 9:
        SpreeString = "just made a MONSTER KILL!";
        break;
    case 10:
        SpreeString = "just landed a ludicrouskill!";
        break;
    case 11:
        SpreeString = "is unbelievable!";
        break;
    case 12:
        SpreeString = "is breaking the boundaries!";
        break;
    case 13:
        SpreeString = "just scored an ACE!";
        break;
    case 14:
        SpreeString = "is INCREDIBLE!";
        break;
    case 15:
        SpreeString = "is WICKED SICK!";
        break;
    case 16:
        SpreeString = "is a Combo Whore!";
        break;
    case 17:
        SpreeString = "is BEYOND IMAGINARY LEVEL!!";
        break;
    case 18:
        SpreeString = "JUST NUKED THE SCOREBOARD!";
        break;
    case 19:
        SpreeString = "just landed an ULTRA KILL!";
        break;
    case 20:
        SpreeString = "JUST MADE 20 KILLS WITHOUT DIENG, THIS IS MADNESS!!!";
        break;
    default:
        SpreeString = "is on a killing spree!";
        break;
    }
    return SpreeString;
}

void CustomMgr::AssignTitle(Player* player, uint16 points)
{
    static const std::vector<uint32> titleReqs = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1250, 1500, 2000 };
    uint32 titleStart = player->GetTeam() == HORDE ? 15 : 1;
    bool assigned = false;
    // Loop titles from top requirements to bottom
    for (int i = titleReqs.size() - 1; i >= 0; --i)
    {
        if (CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(titleStart + i))
        {
            if (!assigned && titleReqs[i] <= points)
            {
                if (!player->HasTitle(title))
                {
                    player->SetTitle(title);
                    player->SetUInt32Value(PLAYER_CHOSEN_TITLE, title->ID);
                }
                assigned = true;
            }
            else
            {
                if (player->HasTitle(title))
                {
                    player->SetTitle(title, true);
                    if (player->GetUInt32Value(PLAYER_CHOSEN_TITLE) == title->ID)
                        player->SetUInt32Value(PLAYER_CHOSEN_TITLE, 0);
                }
            }
        }
    }
}

std::string CustomMgr::MoneyToMoneyString(uint32 money)
{
    return std::to_string((money / GOLD)) + "g" + std::to_string((money % GOLD) / SILVER) + "s" + std::to_string(money % SILVER) + "c";
}

#pragma region CustomConditions

CustomConditions const* CustomMgr::GetConditionDataById(uint32 id) const
{
    return Trinity::Containers::MapGetValuePtr(CustomCondContainer, id);
}

CustomConditions const* CustomMgr::GetConditionDataByValue(uint8 type, uint32 value) const
{
    for (auto &val : CustomCondContainer)
    {
        if (val.second.CondReqType == CustomCondReqType(type) && val.second.CondReqValue == value)
            return &val.second;
    }
    return nullptr;
}

bool CustomConditions::Meets(Player* player, Creature* creature, uint32 itemId, uint32 itemCount) const
{
    if (!player)
        return false;

    uint32 posValue = abs(CondReqValue) * itemCount;
    std::string msg = "";
    bool conditionMet = false;
    switch (CondReqType)
    {
    case CUSTOM_CONDITION_REQ_TITLE:
    {
        uint32 minTitle = player->GetTeam() == HORDE ? 15 : 1;
        uint32 maxTitle = player->GetTeam() == HORDE ? 28 : 14;
        if (CondReqEntry <= maxTitle)
        {
            uint8 found = 0;
            for (uint8 i = minTitle; i <= maxTitle; i++)
            {
                if (player->HasTitle(sCharTitlesStore.LookupEntry(i)))
                    found = i;
            }
            if (found >= CondReqEntry)
                conditionMet = true;
            else
                msg = Trinity::StringFormat("Sorry you do not meet the conditions. You need the title: |cff00FFFF%s|r", CondReqName);
        }
        else if (player->HasTitle(sCharTitlesStore.LookupEntry(CondReqEntry)))
            conditionMet = true;
        else
            msg = Trinity::StringFormat("Sorry you do not meet the conditions. You need the title: |cff00FFFF%s|r", CondReqName);
    }
    break;
    case CUSTOM_CONDITION_REQ_ITEM:
    {
        if (player->HasItemCount(CondReqEntry, posValue, true))
        {
            conditionMet = true;
            if (CondReqValue < 0)
                player->DestroyItemCount(CondReqEntry, posValue, true);
        }
        else
        {
            if (ItemTemplate const* temp = sObjectMgr->GetItemTemplate(CondReqEntry))
                msg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u %s.", posValue, sCustomMgr->GetItemLink(player, temp));
        }
    }
    break;
    case CUSTOM_CONDITION_REQ_MONEY:
    {
        if (player->GetMoney() >= posValue)
        {
            conditionMet = true;
            if (CondReqValue < 0)
                player->ModifyMoney(-static_cast<int32>(posValue));
        }
        else
            msg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %s.", sCustomMgr->MoneyToMoneyString(posValue));
    }
    break;
    case CUSTOM_CONDITION_REQ_HONOR:
    {
        if (player->GetHonorPoints() >= posValue)
        {
            conditionMet = true;
            if (CondReqValue < 0)
                player->ModifyHonorPoints(-static_cast<int32>(posValue));
        }
        else
            msg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u Honor Points.", posValue);
    }
    break;
    case CUSTOM_CONDITION_REQ_ARENA:
    {
        if (player->GetArenaPoints() >= posValue)
        {
            conditionMet = true;
            if (CondReqValue < 0)
                player->ModifyArenaPoints(-static_cast<int32>(posValue));
        }
        else
            msg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u Arena Points.", posValue);
    }
    break;
    case CUSTOM_CONDITION_REQ_RANKPOINTS:
    {
        if (CharacterCurrency* curr = player->GetCurrency())
        {
            if (curr->RankPoints >= posValue)
            {
                conditionMet = true;
                if (CondReqValue < 0)
                    curr->ModifyRankPoints(-static_cast<int32>(posValue), player);
            }
            else
                msg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u Rank Points.", posValue);
        }
    }
    break;
    case CUSTOM_CONDITION_REQ_VP:
    {
        if (AccountCurrency* curr = sCurrencyHandler->GetAccountCurrency(player->GetSession()->GetAccountId()))
        {
            if (curr->GetVP() >= int32(posValue))
            {
                conditionMet = true;
                if (CondReqValue < 0)
                    curr->ModifyVP(-static_cast<int32>(posValue));
            }
            else
                msg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u Vote Points.", posValue);
        }
    }
    break;
    case CUSTOM_CONDITION_REQ_DP:
    {
        if (AccountCurrency* curr = sCurrencyHandler->GetAccountCurrency(player->GetSession()->GetAccountId()))
        {
            if (curr->GetDP() >= int32(posValue))
            {
                conditionMet = true;
                if (CondReqValue < 0)
                {
                    curr->ModifyDP(-static_cast<int32>(posValue));
                    if (itemId > 0)
                    {
                        if (ItemTemplate const* pItem = sObjectMgr->GetItemTemplate(itemId))
                            WorldDatabase.PExecute("INSERT INTO `check_donation_purchases` (`charName`, `accId`, `itemName`, `itemId`, `price`, `amount`) VALUES ('%s', '%u', \"%s\", '%u', '%u', '%u')",
                                player->GetName().c_str(), player->GetSession()->GetAccountId(), pItem->Name1, itemId, posValue, itemCount);
                    }
                }
            }
            else
                msg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u Donation Points.", posValue);
        }
    }
    break;
    case CUSTOM_CONDITION_REQ_GUILD_LEVEL:
    {
        if (Guild* guild = player->GetGuild())
        {
            if (guild->GetLevel() >= CondReqValue)
                conditionMet = true;
            else
                msg = Trinity::StringFormat("Sorry you do not meed the conditions. Your guild have to be level %u.", CondReqValue);
        }
        else
            msg = "You have to be in a guild.";
    }
    break;
    default:
    break;
    }

    if (!conditionMet)
    {
        if (creature)
            creature->TalkToTarget(msg, CHAT_MSG_MONSTER_SAY, player);
        else
            ChatHandler(player->GetSession()).PSendSysMessage(msg.c_str());
    }

    return conditionMet;
}

std::string CustomConditions::ListCond(uint32 count) const
{
    std::string msg = "";
    uint32 posValue = abs(CondReqValue) * count;

    switch (CondReqType)
    {
    case CUSTOM_CONDITION_REQ_TITLE:
    {
        msg = Trinity::StringFormat("Requires Title:\n|cff00FFFF%s|r", CondReqName);
    }
    break;
    case CUSTOM_CONDITION_REQ_ITEM:
    {
        if (CondReqValue < 0)
            msg = Trinity::StringFormat("Requires: %u |cff00FFFF%s|r.\nCondition |cffFF0000will|r remove requirement(s).", posValue, CondReqName);
        else
            msg = Trinity::StringFormat("Requires: %u |cff00FFFF%s|r.\nCondition |cffFF0000will not|r remove requirement(s).", posValue, CondReqName);
    }
    break;
    case CUSTOM_CONDITION_REQ_MONEY:
    {
        if (CondReqValue < 0)
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r.\nCondition |cffFF0000will|r remove requirement(s).", sCustomMgr->MoneyToMoneyString(posValue));
        else
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r.\nCondition |cffFF0000will not|r remove requirement(s).", sCustomMgr->MoneyToMoneyString(posValue));
    }
    break;
    case CUSTOM_CONDITION_REQ_HONOR:
    {
        if (CondReqValue < 0)
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Honor Points.\nCondition |cffFF0000will|r remove requirement(s).", posValue);
        else
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Honor Points.\nCondition |cffFF0000will not|r remove requirement(s).", posValue);
    }
    break;
    case CUSTOM_CONDITION_REQ_ARENA:
    {
        if (CondReqValue < 0)
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Arena Points.\nCondition |cffFF0000will|r remove requirement(s).", posValue);
        else
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Arena Points.\nCondition |cffFF0000will not|r remove requirement(s).",posValue);
    }
    break;
    case CUSTOM_CONDITION_REQ_RANKPOINTS:
    {
        if (CondReqValue < 0)
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Rank Points.\nCondition |cffFF0000will|r remove requirement(s).", posValue);
        else
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Rank Points.\nCondition |cffFF0000will not|r remove requirement(s).", posValue);
    }
    break;
    case CUSTOM_CONDITION_REQ_VP:
    {
        if (CondReqValue < 0)
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Vote Points.\nCondition |cffFF0000will|r remove requirement(s).", posValue);
        else
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Vote Points.\nCondition |cffFF0000will not|r remove requirement(s).", posValue);
    }
    break;
    case CUSTOM_CONDITION_REQ_DP:
    {
        if (CondReqValue < 0)
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Donation Points.\nCondition |cffFF0000will|r remove requirement(s).", posValue);
        else
            msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Donation Points.\nCondition |cffFF0000will not|r remove requirement(s).", posValue);
    }
    break;
    case CUSTOM_CONDITION_REQ_GUILD_LEVEL:
    {
        msg = Trinity::StringFormat("Requires: Guild Level |cff00FFFF%s|r.", posValue);
    }
    break;
    default:
        break;
    }

    return msg;
}

void removeCharsFromString(std::string &str, char const* charsToRemove) {
    for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
        str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
    }
}

void CustomMgr::AddCondition(uint32 id, uint8 type, uint32 entry, int32 value, std::string name)
{
    CustomConditions cond;
    cond.CondId = id;
    cond.CondReqEntry = entry;
    cond.CondReqName = name;
    cond.CondReqType = CustomCondReqType(type);
    cond.CondReqValue = value;
    CustomCondContainer[id] = cond;
}

void CustomMgr::LoadCustomConditions()
{
    // Get the time before we started loading.
    uint32 oldMSTime = getMSTime();

    //                                                     0       1         2
    QueryResult result = WorldDatabase.Query("SELECT id, CondReqType, CondReqEntry, CondReqValue FROM custom_conditions");

    //If no rows found
    if (!result)
    {
        //Display something
        TC_LOG_INFO("server.loading", ">> Loaded 0 custom conditions. DB table `custom_conditions` is empty.");
        return;
    }
    else
    {
        //Continue loading.

        //Reset row counter
        uint32 count = 0;

        //Loop through the results
        do
        {
            // Get the fields
            Field *fields = result->Fetch();

            CustomConditions cond;
            cond.CondId            = fields[0].GetUInt32();
            cond.CondReqType       = CustomCondReqType(fields[1].GetUInt32());
            cond.CondReqEntry      = fields[2].GetUInt32();
            cond.CondReqValue      = fields[3].GetInt32();

            if (cond.CondReqType >= CUSTOM_CONDITION_MAX)
            {
                TC_LOG_ERROR("server.loading", "Condition ID %u has invalid CondReqType %u, skipped.", cond.CondId, uint32(cond.CondReqType));
                continue;
            }

            switch (cond.CondReqType)
            {
                case CUSTOM_CONDITION_REQ_ITEM:
                {
                    if (ItemTemplate const* item = sObjectMgr->GetItemTemplate(cond.CondReqEntry))
                    {
                        cond.CondReqName = item->Name1;
                    }
                    else
                    {
                        TC_LOG_ERROR("server.loading", "Condition ID %u With Item Req has invalid ReqEntry %u, skipped.", cond.CondId, cond.CondReqEntry);
                        continue;
                    }
                }
                break;
                case CUSTOM_CONDITION_REQ_TITLE:
                {
                    if (CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(cond.CondReqEntry))
                    {
                        std::string name = titleEntry->nameMale[LOCALE_enUS];
                        std::regex pattern("%s");
                        name = std::regex_replace(name, pattern, "");
                        removeCharsFromString(name, ",");
                        cond.CondReqName = name;
                    }
                    else
                    {
                        TC_LOG_ERROR("server.loading", "Condition ID %u With Title Req has invalid ReqEntry %u, skipped.", cond.CondId, cond.CondReqEntry);
                        continue;
                    }
                    if (cond.CondReqValue > 0)
                        cond.CondReqValue = 0;
                }
                break;
                case CUSTOM_CONDITION_REQ_MONEY:
                {
                    if (uint32(abs(cond.CondReqValue)) > MAX_MONEY_AMOUNT)
                    {
                        TC_LOG_ERROR("server.loading", "Condition ID %u With Money Req has invalid ReqAmount, skipped.", cond.CondId);
                        continue;
                    }
                }
                break;
                case CUSTOM_CONDITION_REQ_HONOR:
                {
                    if (uint32(abs(cond.CondReqValue)) > sWorld->getIntConfig(CONFIG_MAX_HONOR_POINTS))
                    {
                        TC_LOG_ERROR("server.loading", "Condition ID %u With Honor Req has invalid ReqAmount, skipped.", cond.CondId);
                        continue;
                    }
                }
                break;
                case CUSTOM_CONDITION_REQ_ARENA:
                {
                    if (uint32(abs(cond.CondReqValue)) > sWorld->getIntConfig(CONFIG_MAX_ARENA_POINTS))
                    {
                        TC_LOG_ERROR("server.loading", "Condition ID %u With Arena Req has invalid ReqAmount, skipped.", cond.CondId);
                        continue;
                    }
                }
                break;
                case CUSTOM_CONDITION_REQ_GUILD_LEVEL:
                {
                    if (cond.CondReqValue < 0)
                    {
                        TC_LOG_ERROR("server.loading", "Condition ID %u With Guild Level Req has negative ReqAmount, skipped.", cond.CondId);
                        continue;
                    }
                }
                break;
                case CUSTOM_CONDITION_REQ_VP:
                case CUSTOM_CONDITION_REQ_DP:
                case CUSTOM_CONDITION_REQ_RANKPOINTS:
                break;
                default:
                break;
            }

            CustomCondContainer[cond.CondId] = cond;
            ++count;

        } while (result->NextRow());

        // Log that we loaded everything.
        TC_LOG_INFO("server.loading", ">> Loaded %u custom condition data", count);
    }
}

#pragma endregion

#pragma region AreaCode

bool CustomMgr::IsFFAPvP(uint32 areaID)
{
    auto itr = m_FFAContainer.find(areaID);
    if (itr != m_FFAContainer.end())
	{
		if (areaID == itr->first)
		{
			if (itr->second == 0 || time_t(itr->second) > GameTime::GetGameTime())
				return true;
			else
                m_FFAContainer.erase(itr);
		}
	}

	return false;
}

void CustomMgr::AddFFAPvP(uint32 areaID, uint64 duration)
{
	m_FFAContainer[areaID] = duration;
}

void CustomMgr::DeleteFFAPvP(uint32 areaID)
{
    m_FFAContainer.erase(areaID);
}

bool CustomMgr::IsSanctuary(uint32 areaID)
{
    auto itr = m_SanctuaryContainer.find(areaID);
    if (itr != m_SanctuaryContainer.end())
	{
		if (areaID == itr->first)
		{
			if (itr->second == 0 || time_t(itr->second) > GameTime::GetGameTime())
				return true;
			else
                m_SanctuaryContainer.erase(itr);
		}
	}

	return false;
}

void CustomMgr::AddSanctuary(uint32 areaID, uint64 duration)
{
	m_SanctuaryContainer[areaID] = duration;
}

void CustomMgr::DeleteSanctuary(uint32 areaID)
{
    m_SanctuaryContainer.erase(areaID);
}

void CustomMgr::LoadNoFlyAreas()
{
    if (!TitanNoFlyAreaSet.empty())
        TitanNoFlyAreaSet.clear();

    // Get the time before we started loading.
    uint32 oldMSTime = getMSTime();

    //                                                      0             1          2
    QueryResult result = WorldDatabase.Query("SELECT areaId FROM custom_noflyzone");

    //If no rows found
    if (!result)
    {
        //Display something
        TC_LOG_INFO("server.loading", ">> Loaded 0 Titan No Fly Zone area ids. DB table `custom_noflyzone` is empty.");
        return;
    }
    else
    {
        //Continue loading.

        //Reset row counter
        uint32 count = 0;

        //Loop through the results
        do
        {
            // Get the fields
            Field *fields = result->Fetch();
            uint32 areaID = fields[0].GetUInt32();

            TitanNoFlyAreaSet.insert(areaID);
            //Increase row counter
            ++count;

        } while (result->NextRow());

        // Log that we loaded everything.
        TC_LOG_INFO("server.loading", ">> Loaded %u VQ No Fly Zone area ids", count);
    }
}

void CustomMgr::LoadSanctuaryAreas()
{
    if (!m_SanctuaryContainer.empty())
        m_SanctuaryContainer.clear();

    // Get the time before we started loading.
    uint32 oldMSTime = getMSTime();

    //                                                      0             1          2
    QueryResult result = WorldDatabase.Query("SELECT areaId, duration FROM custom_sanctuary");

    //If no rows found
    if (!result)
    {
        //Display something
        TC_LOG_INFO("server.loading", ">> Loaded 0 Titan Sanctuary area ids. DB table `custom_sanctuary` is empty.");
        return;
    }
    else
    {
        //Continue loading.

        //Reset row counter
        uint32 count = 0;

        //Loop through the results
        do
        {
            // Get the fields
            Field *fields = result->Fetch();
            uint32 areaID = fields[0].GetUInt32();
            uint64 duration = fields[1].GetUInt64();

            if (duration != 0 && (time_t(duration) < GameTime::GetGameTime()))
            {
                WorldDatabase.DirectPExecute("DELETE FROM custom_sanctuary WHERE areaId = %u", areaID);
                continue;
            }

            m_SanctuaryContainer[areaID] = duration;
            //Increase row counter
            ++count;

        } while (result->NextRow());

        // Log that we loaded everything.
        TC_LOG_INFO("server.loading", ">> Loaded %u VQ Sanctuary area ids", count);
    }
}

bool CustomMgr::IsVIP(uint32 account)
{
    auto itr = m_VIPContainer.find(account);
    if (itr != m_VIPContainer.end())
    {
        if (account == itr->first)
        {
            if (itr->second == 0 || time_t(itr->second) > GameTime::GetGameTime())
                return true;
            else
                m_VIPContainer.erase(itr);
        }
    }

    return false;
}

uint64 CustomMgr::GetVIPDuration(uint32 account)
{
    for (auto const &val : m_VIPContainer)
        if (val.first == account)
            if (val.second != 0)
                return val.second - uint64(GameTime::GetGameTime());

    return 0;
}

bool CustomMgr::AddVIP(uint32 account, uint32 duration)
{
    try
    {
        uint64 timer = duration == 0 ? 0 : GameTime::GetGameTime() + duration;
        m_VIPContainer[account] = timer; // add new VIP member

        PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT_PREMIUM);
        stmt->setUInt32(0, account);
        stmt->setUInt64(1, timer);
        LoginDatabase.Execute(stmt);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

void CustomMgr::LoadVIPAccounts()
{
    if (!m_VIPContainer.empty())
        m_VIPContainer.clear();

    // Get the time before we started loading.
    uint32 oldMSTime = getMSTime();

    //                                                      0             1          2
    QueryResult result = LoginDatabase.Query("SELECT account, duration FROM account_premium");

    //If no rows found
    if (!result)
    {
        //Display something
        TC_LOG_INFO("server.loading", ">> Loaded 0 VIP accounts. DB table `account_premium` is empty.");
        return;
    }
    else
    {
        //Continue loading.

        //Reset row counter
        uint32 count = 0;

        //Loop through the results
        do
        {
            // Get the fields
            Field *fields = result->Fetch();
            uint32 account = fields[0].GetUInt32();
            uint64 duration = fields[1].GetUInt64();

            if (duration != 0 && (time_t(duration) < GameTime::GetGameTime()))
            {
                LoginDatabase.DirectPExecute("DELETE FROM account_premium WHERE account = %u", account);
                continue;
            }

            m_VIPContainer[account] = duration; // Load VIP Member to container
            //Increase row counter
            ++count;

        } while (result->NextRow());

        // Log that we loaded everything.
        TC_LOG_INFO("server.loading", ">> Loaded %u VIP Accounts ", count);
    }
}

void CustomMgr::LoadFFAPvPAreas()
{
    if (!m_FFAContainer.empty())
        m_FFAContainer.clear();

    // Get the time before we started loading.
    uint32 oldMSTime = getMSTime();

    //                                                      0             1          2
    QueryResult result = WorldDatabase.Query("SELECT areaId, duration FROM custom_ffapvp");

    //If no rows found
    if (!result)
    {
        //Display something
        TC_LOG_INFO("server.loading", ">> Loaded 0 Titan FFA PvP area ids. DB table `custom_ffapvp` is empty.");
        return;
    }
    else
    {
        //Continue loading.

        //Reset row counter
        uint32 count = 0;

        //Loop through the results
        do
        {
            // Get the fields
            Field *fields = result->Fetch();
            uint32 areaID = fields[0].GetUInt32();
            uint64 duration = fields[1].GetUInt64();

            if (duration != 0 && (time_t(duration) < GameTime::GetGameTime()))
            {
                WorldDatabase.DirectPExecute("DELETE FROM custom_ffapvp WHERE areaId = %u", areaID);
                continue;
            }

            m_FFAContainer[areaID] = duration;
            //Increase row counter
            ++count;

        } while (result->NextRow());

        // Log that we loaded everything.
        TC_LOG_INFO("server.loading", ">> Loaded %u VQ FFA PvP area ids ", count);
    }
}

void CustomMgr::LoadNoGroupAreas()
{
    if (!TitanNoGroupAreaSet.empty())
        TitanNoGroupAreaSet.clear();

    // Get the time before we started loading.
    uint32 oldMSTime = getMSTime();

    //                                                      0             1          2
    QueryResult result = WorldDatabase.Query("SELECT areaId FROM custom_nogroupareas");

    //If no rows found
    if (!result)
    {
        //Display something
        TC_LOG_INFO("server.loading", ">> Loaded 0 VQ Restricted Grouping area ids. DB table `custom_nogroupareas` is empty.");
        return;
    }
    else
    {
        //Continue loading.

        //Reset row counter
        uint32 count = 0;

        //Loop through the results
        do
        {
            // Get the fields
            Field *fields = result->Fetch();
            uint32 areaID = fields[0].GetUInt32();

            TitanNoGroupAreaSet.insert(areaID);
            //Increase row counter
            ++count;

        } while (result->NextRow());

        // Log that we loaded everything.
        TC_LOG_INFO("server.loading", ">> Loaded %u VQ Restricted Grouping area ids", count);
    }
}
#pragma endregion

#pragma region DuelBetting

void CustomMgr::HandleSelectDuelBetting(Player* player, Creature* creature, int32 item, int32 count)
{
    AddGossipItemFor(player, 10, "yep", item, count);
    SendGossipMenuFor(player, "bla bla", creature->GetGUID());
}

#pragma endregion

#pragma region Boosters

void CustomMgr::LoadBoosters()
{
    m_BoostContainer.clear(); // Clear the booster map and reload it, this way we will clear expired boosters.
    CharacterDatabase.DirectExecute("DELETE FROM character_boost WHERE unsetdate < UNIX_TIMESTAMP() AND unsetdate <> 0"); // Remove expired boosters from DB.
                                                                                                                          // Load all boosters to m_Boosters.
    QueryResult rboost = CharacterDatabase.Query("SELECT guid, unsetdate, boost_type, boost_modifier FROM character_boost");
    if (rboost)
    {
        do
        {
            if (Field* fboost = rboost->Fetch())
            {
                ObjectGuid guid = ObjectGuid(HighGuid::Player, fboost[0].GetUInt32());
                uint64 unsetdate = fboost[1].GetUInt64();
                Boosters boost_type = Boosters(fboost[2].GetUInt32());
                float boost_modifier = fboost[3].GetFloat();

                if (time_t(unsetdate) < GameTime::GetGameTime())
                {
                    CharacterDatabase.DirectPExecute("DELETE FROM character_boost WHERE guid = %u AND boost_type = %u", guid, boost_type);
                    continue;
                }
                AddBooster(guid, unsetdate, boost_type, boost_modifier);
            }
        } while (rboost->NextRow());
    }
}

void CustomMgr::SaveBooster(ObjectGuid guid)
{
    for (auto const &val : m_BoostContainer)
        if (val.ownerguid == guid)
            CharacterDatabase.PExecute("REPLACE INTO character_boost (guid, boost_type, unsetdate, boost_modifier) VALUES ('%u', '%u', 'UNIX_TIMESTAMP()+%u', '%f')", val.ownerguid, val.boost_type, val.unsetdate, val.boost_modifier);
}

CustomMgr* CustomMgr::instance()
{
    // Thread safe in C++11 standard
    static CustomMgr instance;
    return &instance;
}

void CustomMgr::SaveBoosters()
{
    for (BoostContainer::iterator itr = m_BoostContainer.begin(); itr != m_BoostContainer.end(); ++itr)
    {
        // Check that there are no booster with this value.
        QueryResult rboost1 = CharacterDatabase.PQuery("SELECT 1 FROM character_boost WHERE guid = %u AND boost_type = %u", itr->ownerguid.GetCounter(), itr->boost_type);
        if (!rboost1)
        {
            // There are no booster with this value, but there might be one with the same type.
            QueryResult rboost2 = CharacterDatabase.PQuery("SELECT 1 FROM character_boost WHERE guid = %u AND boost_type = %u", itr->ownerguid.GetCounter(), itr->boost_type);
            if (!rboost2) // There were none with the same type, insert a new one.
                CharacterDatabase.PExecute("REPLACE INTO character_boost (guid, boost_type, unsetdate, boost_modifier) VALUES (%u,%u,UNIX_TIMESTAMP()+%u,%f)", itr->ownerguid.GetCounter(), itr->boost_type, itr->unsetdate, itr->boost_modifier);
            else
                CharacterDatabase.PExecute("UPDATE character_boost SET boost_modifier = %f WHERE guid = %u AND boost_type = %u", itr->boost_modifier, itr->ownerguid.GetCounter(), itr->boost_type);
        }
    }
}

bool CustomMgr::CheckBooster(ObjectGuid guid, Boosters boost_type)
{
    for (auto const &itr : m_BoostContainer)
        if (itr.ownerguid == guid && itr.boost_type == boost_type)
            return true;

    return false;
}

void CustomMgr::AddBooster(ObjectGuid guid, uint64 unsetdate, Boosters boost_type, float boost_modifier)
{
    m_BoostContainer.push_back(BoostInfo(guid, unsetdate, boost_type, boost_modifier));
}

float CustomMgr::GetBoosterMod(ObjectGuid guid, Boosters boost_type)
{
    for (auto const &itr : m_BoostContainer)
        if (itr.ownerguid == guid && itr.boost_type == boost_type)
            return itr.boost_modifier;

    return 1.0f;
}

std::string CustomMgr::GetBoosterName(uint32 boost_type)
{
    std::string boostname;
    switch (boost_type)
    {
    case 5:  boostname = "|TInterface\\icons\\Ability_Paladin_BeaconofLight:22:22:-22:0|tReputation Booster."; break;
    case 1:  boostname = "|TInterface\\icons\\Achievement_Arena_2v2_7:22:22:-22:0|tHonor Booster."; break;
    case 3:  boostname = "|TInterface\\icons\\INV_Misc_Rune_07:22:22:-22:0|tWSG Marks Booster."; break;
    case 4:  boostname = "|TInterface\\icons\\INV_Misc_Coin_01:22:22:-22:0|tGold Booster."; break;
    case 7:  boostname = "|TInterface\\icons\\Achievement_Arena_5v5_7:22:22:-22:0|tArena Booster."; break;
    case 2:  boostname = "|TInterface\\icons\\Achievement_Arena_3v3_7:22:22:-22:0|tRank Points Booster."; break;
    default: boostname = "Never."; break;
    }

    return boostname;
}
#pragma endregion

#pragma region Battleground

float Battleground::GetTeamTotalRankPoints(uint32 teamID) const
{
    float total = 1.0f; // to prevent division by 0
    for (auto itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* player = _GetPlayerForTeam(teamID, itr, "GetTeamTotalItemLevel"))
            if (CharacterCurrency* curr = player->GetCurrency())
                total += curr->RankPoints;
    return total;
}

void Battleground::SendArenaReadyCheck(Player* player) const
{
    if (GetStatus() != STATUS_WAIT_JOIN) // dont send anything
        return;

    if (m_ReadyCheck) // already done manually via command so dont send anything
        return;

    WorldPacket packet_0x1(0x051, 0x17);
    packet_0x1.appendPackGUID(0xFFFFFFFF);
    packet_0x1 << uint8(0x0);
    packet_0x1 << std::string("Arena");
    packet_0x1 << uint8(0x0);
    packet_0x1 << uint8(0xa);
    packet_0x1 << uint8(0x0);
    packet_0x1 << uint8(0x4);
    packet_0x1 << uint8(0x0);

    WorldPacket packet_0x2(0x322, 0x8);
    packet_0x2 << uint64(0xFFFFFFFF);

    if (player)
    {
        player->GetSession()->SendPacket(&packet_0x1);
        player->GetSession()->SendPacket(&packet_0x2);
    }
    else
    {
        for (auto const itr : m_Players)
        {
            if (Player *player = ObjectAccessor::FindPlayer(itr.first))
            {
                player->GetSession()->SendPacket(&packet_0x1);
                player->GetSession()->SendPacket(&packet_0x2);
            }
        }
    }
}

float Battleground::GetPoints(uint32 team, float allyPoints, float hordePoints, bool victory)
{
    float points = 0;
    float TeamPoints = 0;
    float EnemyPoints = 0;
    float diffPercent = allyPoints > hordePoints ? (((allyPoints - hordePoints) / allyPoints) + 1.1) :
        (((hordePoints - allyPoints) / hordePoints) + 1.1);

    if (team == ALLIANCE)
    {
        TeamPoints = allyPoints;
        EnemyPoints = hordePoints;
    }
    else
    {
        TeamPoints = hordePoints;
        EnemyPoints = allyPoints;
    }

    if (victory)
    {
        points = frand(22.0f, 26.0f);
        if (TeamPoints < EnemyPoints)
            points *= diffPercent;
    }
    else
    {
        points = frand(12.0f, 16.0f);
        if (TeamPoints > EnemyPoints)
            points *= diffPercent;
    }

    return points;
}

void CustomMgr::HandleBGQuests(Player* player, bool isArena)
{
    for (BattlegroundQuestsList::iterator itr = BGQuests.begin(); itr != BGQuests.end(); ++itr)
    {
        if (player->GetMapId() == itr->bgId || itr->bgId == 0)
        {
            if (player->IsActiveQuest(itr->quest))
            {
                if (isArena)
                    player->KilledMonsterCredit(itr->npcArena);
                else
                    player->KilledMonsterCredit(itr->npc);
            }
        }
    }
}

void CustomMgr::HandleDeserter(ObjectGuid guid, std::string ip)
{
    if (ip.empty())
        return;

    DesertInfo* dInfo = GetDesertInfo(ip);
    if (dInfo)
        ApplyDeserter(guid, ip, true);
    else
        AddDeserter(guid, ip);
}

void CustomMgr::ApplyDeserter(ObjectGuid guid, std::string ip, bool increase)
{
    if (DesertInfo* dInfo = GetDesertInfo(ip))
    {
        if (increase)
        {
            dInfo->Count += 1;
            dInfo->TimeLeft = GameTime::GetGameTime() + 900;
            if (CharacterCurrency* currency = sCurrencyHandler->GetCharacterCurrency(guid))
            {
                currency->WinStreak = 0;
                currency->ModifyRankPoints(-20);
            }
        }
            
        if (Player* player = ObjectAccessor::FindPlayer(guid))
        {
            if (!player->HasAura(26013)) // add deserter aura
            {
                if (increase)
                    player->GetSession()->SendAreaTriggerMessage("Win Streak Bonus Reset");

                if (dInfo->TimeLeft > GameTime::GetGameTime())
                {
                    uint32 desertercount = dInfo->Count;
                    if (desertercount > 4)
                        desertercount = 4;

                    Aura* aura = player->AddAura(26013, player);
                    if (aura)
                        aura->SetDuration(uint32(dInfo->TimeLeft - GameTime::GetGameTime()) * desertercount * IN_MILLISECONDS);
                    ChatHandler(player->GetSession()).PSendSysMessage("[|cffFF0000DESERTER WARNING|r] Your IP-Address has been flagged as a leaver for |cffFF0000%u|r active battlegrounds, and because of that you have been given a penalty!", dInfo->Count);
                }
            }
        }
    }
}

DesertInfo* CustomMgr::GetDesertInfo(std::string ip)
{
    return Trinity::Containers::MapGetValuePtr(m_Deserters, ip);
}

void CustomMgr::AddDeserter(ObjectGuid guid, std::string ip)
{
    DesertInfo dInfo;
    dInfo.Count = 0;
    dInfo.TimeLeft = GameTime::GetGameTime() + 900;
    m_Deserters[ip] = dInfo;
    ApplyDeserter(guid, ip, true);
}

std::shared_ptr<BattleRoyale::Lobby>& Player::GetLobby()
{
    return m_lobby;
}

void Player::SetLobby(std::shared_ptr<BattleRoyale::Lobby> lobby)
{
    m_lobby = lobby;
}

bool Player::LeaveLobby()
{
    if (!GetLobby())
        return false;

    GetLobby()->KickPlayer(this);
    m_lobby.reset();
    return true;
}

std::string CustomMgr::GetSpellIcon(uint32 entry, uint32 width, uint32 height, int x, int y) const
{
    std::ostringstream ss;
    ss << "|T";
    const SpellInfo* temp = sSpellMgr->GetSpellInfo(entry);
    const SpellIconEntry* dispInfo = NULL;
    if (temp)
    {
        dispInfo = sSpellIconStore.LookupEntry(temp->SpellIconID);
        if (dispInfo)
            ss << dispInfo->SpellIcon;
    }
    if (!dispInfo)
        ss << "Interface\\Icons\\Ability_Temp";
    ss << ":" << width << ":" << height << ":" << x << ":" << y << "|t";
    return ss.str();
}

void CustomMgr::LoadArenaDebuff()
{
    if (!DebuffClassMasks.empty())
        DebuffClassMasks.clear();
    QueryResult result = WorldDatabase.Query("SELECT classMask FROM custom_arena_debuff");

    //If no rows found
    if (!result)
    {
        //Display something
        TC_LOG_INFO("server.loading", ">> Loaded 0 custom arena debuff ids. DB table `custom_arena_debuff` is empty.");
        return;
    }
    else
    {
        //Reset row counter
        uint32 count = 0;

        //Loop through the results
        do
        {
            // Get the fields
            Field *fields = result->Fetch();
            uint32 classMask = fields[0].GetUInt32();

            DebuffClassMasks.push_back(classMask);
            //Increase row counter
            ++count;

        } while (result->NextRow());

        // Log that we loaded everything.
        TC_LOG_INFO("server.loading", ">> Loaded %u custom arena debuff ids", count);
    }
}

void CustomMgr::LoadBattlegroundQuests()
{
    if (!BGQuests.empty())
        BGQuests.clear();
    QueryResult result = WorldDatabase.Query("SELECT bgMapId, questId, npcId, npcIdArena FROM custom_battlegroundquests");

    //If no rows found
    if (!result)
    {
        //Display something
        TC_LOG_INFO("server.loading", ">> Loaded 0 suffix ids. DB table `custom_battlegroundquests` is empty.");
        return;
    }

    uint32 count = 0;
    do {
        Field *fields = result->Fetch();
        uint32 bgMap = fields[0].GetUInt32();
        uint32 questId = fields[1].GetUInt32();
        uint32 npcId = fields[2].GetUInt32();
        uint32 npcIdArena = fields[3].GetUInt32();

        BGQuests.push_back(CBattlegroundQuests(bgMap, questId, npcId, npcIdArena));
        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u custom_battlegroundquests ids", count);
}
#pragma endregion

#pragma region Random

void Unit::TalkToTarget(std::string message, ChatMsg msgType, Player const* target)
{
    WorldPacket packet;
    ChatHandler::BuildChatPacket(packet, msgType, LANG_UNIVERSAL, this, target, message);
    target->GetSession()->SendPacket(&packet);
}

std::string CustomMgr::GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y)
{
    TC_LOG_DEBUG("custom.transmog", "Transmogrification::GetItemIcon");

    std::ostringstream ss;
    ss << "|TInterface";
    const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
    const ItemDisplayInfoEntry* dispInfo = NULL;
    if (temp)
    {
        dispInfo = sItemDisplayInfoStore.LookupEntry(temp->DisplayInfoID);
        if (dispInfo)
            ss << "/ICONS/" << dispInfo->inventoryIcon;
    }
    if (!dispInfo)
        ss << "/InventoryItems/WoWUnknownItem01";
    ss << ":" << width << ":" << height << ":" << x << ":" << y << "|t";
    return ss.str();
}

std::string CustomMgr::GetItemName(uint32 entry, LocaleConstant localeConstant)
{
    const ItemTemplate* itemTemplate = sObjectMgr->GetItemTemplate(entry);
    std::string name = itemTemplate->Name1;
    if (localeConstant >= 0)
        if (ItemLocale const* il = sObjectMgr->GetItemLocale(itemTemplate->ItemId))
            sObjectMgr->GetLocaleString(il->Name, localeConstant, name);
    return name;
}

std::string CustomMgr::GetItemLink(Player* player, const ItemTemplate* temp)
{
    std::ostringstream oss;
    oss << "|c" << std::hex << ItemQualityColors[temp->Quality] << std::dec <<
        "|Hitem:" << temp->ItemId << ":0:0:0:0:0:0:0:0:0|h[" << GetItemName(temp->ItemId, player->GetSession()->GetSessionDbcLocale()) << "]|h|r";
    return oss.str();
}

#pragma endregion

void CustomMgr::LoadDailyQuests()
{
    if (!DailyQuests.empty())
        DailyQuests.clear();

	QueryResult result = WorldDatabase.Query("SELECT questId FROM dailyquests");

	//If no rows found
	if (!result)
	{
		//Display something
		TC_LOG_INFO("server.loading", ">> Loaded 0 Daily quests ids. DB table `dailyquests` is empty.");
		return;
	}
	else
	{
		//Reset row counter
		uint32 count = 0;

		//Loop through the results
		do
		{
			// Get the fields
			Field *fields = result->Fetch();
			uint32 questId = fields[0].GetUInt32();

			DailyQuests.push_back(questId);
			//Increase row counter
			++count;

		} while (result->NextRow());

		// Log that we loaded everything.
		TC_LOG_INFO("server.loading", ">> Loaded %u Daily Quest ids", count);
	}
}

void CustomMgr::SaveCharGuildXp(ObjectGuid guid)
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_PLAYER_GIVEN_XP);
    stmt->setUInt32(0, guid.GetCounter());
    stmt->setUInt32(1, GetCharGuildXp(guid));
    CharacterDatabase.Execute(stmt);
}

void CustomMgr::IncreaseCharGuildXp(ObjectGuid guid, uint32 val)
{
    CharGuildXp[guid] += val;
}

void CustomMgr::SetCharGuildXp(ObjectGuid guid, uint32 val)
{
    if (val == 0)
        CharGuildXp.erase(guid);
    else
        CharGuildXp[guid] = val;
}

uint32 CustomMgr::GetCharGuildXp(ObjectGuid guid)
{
    return CharGuildXp[guid];
}

void CustomMgr::LoadCharGuildXp()
{
    uint32 count = 0;
    uint32 msTime = getMSTime();
    CharacterDatabase.DirectExecute("DELETE FROM character_guild_xp_given WHERE xp = 0");
    QueryResult result = CharacterDatabase.Query("SELECT guid, xp FROM character_guild_xp_given");
    if (!result)
        return;
    do
    {
        Field* fields = result->Fetch();
        CharGuildXp[ObjectGuid(HighGuid::Player, fields[0].GetUInt32())] = fields[1].GetUInt32();
        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Char Guild Xp Info %u MS.", count, GetMSTimeDiffToNow(msTime));
}

void CustomMgr::LoadAll()
{
    LoadCharGuildXp();
    LoadGuildBonusInfo();
    LoadVIPAccounts();
    LoadBoosters();
    LoadArenaDebuff();
    LoadBattlegroundQuests();
    LoadDailyQuests();
    LoadSanctuaryAreas();
    LoadFFAPvPAreas();
    LoadNoGroupAreas();
    LoadNoFlyAreas();
    LoadRanks();
}

CustomMgr::~CustomMgr() 
{
    VQRankList.clear();
}

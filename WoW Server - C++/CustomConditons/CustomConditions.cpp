#include "CustomConditions.h"
#include "Containers.h"
#include "CustomMgr.h"
#include "PlayerCurrencies.h"
#include "Guild.h"
#include "Chat.h"
#include "Log.h"
#include "WorldSession.h"
#include <regex>
#include "CustomConditions.h"

using namespace CustomConditions;

std::unique_ptr<CustomCondition> const* CustomConditionsMgr::GetConditionById(uint32 id) const
{
    return Trinity::Containers::MapGetValuePtr(m_CustomConditions, id);
}

std::unique_ptr<CustomCondition> const* CustomConditionsMgr::GetConditionByValue(uint8 type, uint32 value) const
{
    for (auto const &val : m_CustomConditions)
    {
        if (val.second->GetType() == CustomCondReqType(type) && val.second->GetValue() == value)
            return &val.second;
    }
    return nullptr;
}

void CustomCondition::AnnounceFail(Player* player, Creature* creature)
{
    if (creature)
        creature->TalkToTarget(AnnounceFailMsg, CHAT_MSG_MONSTER_SAY, player);
    else
        ChatHandler(player->GetSession()).SendSysMessage(AnnounceFailMsg.c_str());
}

void CustomConditionsMgr::LoadCustomConditions()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT id, CondReqType, CondReqEntry, CondReqValue FROM custom_conditions");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 custom conditions. DB table `custom_conditions` is empty.");
        return;
    }
    else
    {
        uint32 count = 0;

        do
        {
            Field *fields = result->Fetch();

            uint32 CondId       = fields[0].GetUInt32();
            CustomCondReqType CondReqType  = CustomCondReqType(fields[1].GetUInt32());
            uint32 CondReqEntry = fields[2].GetUInt32();
            int32  CondReqValue = fields[3].GetInt32();

            if (CondReqType >= CUSTOM_CONDITIONS_MAX)
            {
                TC_LOG_ERROR("server.loading", "Condition ID %u has invalid CondReqType %u, skipped.", CondId, CondReqEntry);
                continue;
            }

            switch (CondReqType)
            {
            case CUSTOM_CONDITION_REQ_ITEM:
            {
                if (ItemTemplate const* item = sObjectMgr->GetItemTemplate(CondReqEntry))
                {
                    std::string name = item->Name1;
                    m_CustomConditions.insert(std::make_pair(CondId, std::make_unique<RequireItem>(CondReqEntry, CondReqValue, name)));
                }
                else
                {
                    TC_LOG_ERROR("server.loading", "Condition ID %u With Item Req has invalid ReqEntry %u, skipped.", CondId, CondReqEntry);
                    continue;
                }
            }
            break;
            case CUSTOM_CONDITION_REQ_TITLE:
            {
                if (CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(CondReqEntry))
                {
                    auto removeCharsFromString = [](std::string &str, char const* charsToRemove) -> void
                    {
                        for (unsigned int i = 0; i < strlen(charsToRemove); ++i)
                        {
                            str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
                        }
                    };

                    std::string name = titleEntry->nameMale[LOCALE_enUS];
                    std::regex pattern("%s");
                    name = std::regex_replace(name, pattern, "");
                    removeCharsFromString(name, ",");
                    m_CustomConditions.insert(std::make_pair(CondId, std::make_unique<RequireTitle>(CondReqEntry, name)));
                }
                else
                {
                    TC_LOG_ERROR("server.loading", "Condition ID %u With Title Req has invalid ReqEntry %u, skipped.", CondId, CondReqEntry);
                    continue;
                }
            }
            break;
            case CUSTOM_CONDITION_REQ_MONEY:
            {
                if (uint32(abs(CondReqValue)) > MAX_MONEY_AMOUNT)
                {
                    TC_LOG_ERROR("server.loading", "Condition ID %u With Money Req has invalid ReqAmount, skipped.", CondId);
                    continue;
                }

                m_CustomConditions.insert(std::make_pair(CondId, std::make_unique<RequireMoney>(CondReqValue)));
            }
            break;
            case CUSTOM_CONDITION_REQ_HONORPOINTS:
            {
                if (uint32(abs(CondReqValue)) > sWorld->getIntConfig(CONFIG_MAX_HONOR_POINTS))
                {
                    TC_LOG_ERROR("server.loading", "Condition ID %u With Honor Req has invalid ReqAmount, skipped.", CondId);
                    continue;
                }

                m_CustomConditions.insert(std::make_pair(CondId, std::make_unique<RequireHonorPoints>(CondReqValue)));
            }
            break;
            case CUSTOM_CONDITION_REQ_ARENAPOINTS:
            {
                if (uint32(abs(CondReqValue)) > sWorld->getIntConfig(CONFIG_MAX_ARENA_POINTS))
                {
                    TC_LOG_ERROR("server.loading", "Condition ID %u With Arena Req has invalid ReqAmount, skipped.", CondId);
                    continue;
                }

                m_CustomConditions.insert(std::make_pair(CondId, std::make_unique<RequireArenaPoints>(CondReqValue)));
            }
            break;
            case CUSTOM_CONDITION_REQ_GUILD_LEVEL:
            {
                if (CondReqValue < 0)
                {
                    TC_LOG_ERROR("server.loading", "Condition ID %u With Guild Level Req has negative ReqAmount, skipped.", CondId);
                    continue;
                }

                m_CustomConditions.insert(std::make_pair(CondId, std::make_unique<RequireGuildLevel>(CondReqValue)));
            }
            break;
            case CUSTOM_CONDITION_REQ_VP:
                m_CustomConditions.insert(std::make_pair(CondId, std::make_unique<RequireVotePoints>(CondReqValue)));
                break;
            case CUSTOM_CONDITION_REQ_DP:
                m_CustomConditions.insert(std::make_pair(CondId, std::make_unique<RequireDonorPoints>(CondReqValue)));
                break;
            case CUSTOM_CONDITION_REQ_RANKPOINTS:
                m_CustomConditions.insert(std::make_pair(CondId, std::make_unique<RequireRankPoints>(CondReqValue)));
                break;
            default:
                break;
            }

            ++count;

        } while (result->NextRow());

        TC_LOG_INFO("server.loading", ">> Loaded %u custom condition data", count);
    }
}

CustomConditionsMgr* CustomConditionsMgr::instance()
{
    static CustomConditionsMgr instance;
    return &instance;
}

const bool CustomConditions::RequireTitle::Meets(Player* player, uint32 /*buyCount*/)
{
    bool conditionMet = false;
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
            AnnounceFailMsg = Trinity::StringFormat("Sorry you do not meet the conditions. You need the title: |cff00FFFF%s|r", CondReqName);
    }
    else if (player->HasTitle(sCharTitlesStore.LookupEntry(CondReqEntry)))
        conditionMet = true;
    else
        AnnounceFailMsg = Trinity::StringFormat("Sorry you do not meet the conditions. You need the title: |cff00FFFF%s|r", CondReqName);

    return conditionMet;
}

const std::string CustomConditions::RequireTitle::ListCond(uint32 buycount)
{
    return Trinity::StringFormat("Requires Title:\n|cff00FFFF%s|r", CondReqName);
}

const bool CustomConditions::RequireItem::Meets(Player* player, uint32 buyCount)
{
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);
    bool conditionMet = false;

    if (player->HasItemCount(CondReqEntry, posValue, true))
    {
        conditionMet = true;
        if (CondReqValue < 0)
            player->DestroyItemCount(CondReqEntry, posValue, true);
    }
    else
    {
        if (ItemTemplate const* temp = sObjectMgr->GetItemTemplate(CondReqEntry))
            AnnounceFailMsg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u %s.", posValue, sCustomMgr->GetItemLink(player, temp));
    }

    return conditionMet;
}

const std::string CustomConditions::RequireItem::ListCond(uint32 buyCount)
{
    std::string msg = "";
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (CondReqValue < 0)
        msg = Trinity::StringFormat("Requires: %u |cff00FFFF%s|r.\nCondition |cffFF0000will|r remove requirement(s).", posValue, CondReqName);
    else
        msg = Trinity::StringFormat("Requires: %u |cff00FFFF%s|r.\nCondition |cffFF0000will not|r remove requirement(s).", posValue, CondReqName);

    return msg;
}

const bool CustomConditions::RequireMoney::Meets(Player* player, uint32 buyCount)
{
    bool conditionMet = false;
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (player->GetMoney() >= posValue)
    {
        conditionMet = true;
        if (CondReqValue < 0)
            player->ModifyMoney(-static_cast<int32>(posValue));
    }
    else
        AnnounceFailMsg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %s.", sCustomMgr->MoneyToMoneyString(posValue));

    return conditionMet;
}

const std::string CustomConditions::RequireMoney::ListCond(uint32 buyCount)
{
    std::string msg = "";
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (CondReqValue < 0)
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r.\nCondition |cffFF0000will|r remove requirement(s).", sCustomMgr->MoneyToMoneyString(posValue));
    else
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r.\nCondition |cffFF0000will not|r remove requirement(s).", sCustomMgr->MoneyToMoneyString(posValue));

    return msg;
}

const bool CustomConditions::RequireHonorPoints::Meets(Player* player, uint32 buyCount)
{
    bool conditionMet = false;
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (player->GetHonorPoints() >= posValue)
    {
        conditionMet = true;
        if (CondReqValue < 0)
            player->ModifyHonorPoints(-static_cast<int32>(posValue));
    }
    else
        AnnounceFailMsg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u Honor Points.", posValue);

    return conditionMet;
}

const std::string CustomConditions::RequireHonorPoints::ListCond(uint32 buyCount)
{
    std::string msg = "";
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (CondReqValue < 0)
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Honor Points.\nCondition |cffFF0000will|r remove requirement(s).", posValue);
    else
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Honor Points.\nCondition |cffFF0000will not|r remove requirement(s).", posValue);

    return msg;
}

const bool CustomConditions::RequireArenaPoints::Meets(Player * player, uint32 buyCount)
{
    bool conditionMet = false;
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (player->GetArenaPoints() >= posValue)
    {
        conditionMet = true;
        if (CondReqValue < 0)
            player->ModifyArenaPoints(-static_cast<int32>(posValue));
    }
    else
        AnnounceFailMsg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u Arena Points.", posValue);

    return conditionMet;
}

const std::string CustomConditions::RequireArenaPoints::ListCond(uint32 buyCount)
{
    std::string msg = "";
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (CondReqValue < 0)
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Arena Points.\nCondition |cffFF0000will|r remove requirement(s).", posValue);
    else
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Arena Points.\nCondition |cffFF0000will not|r remove requirement(s).", posValue);

    return msg;
}

const bool CustomConditions::RequireRankPoints::Meets(Player * player, uint32 buyCount)
{
    bool conditionMet = false;
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (CharacterCurrency* currency = player->GetCurrency())
    {
        if (currency->RankPoints >= posValue)
        {
            conditionMet = true;
            if (CondReqValue < 0)
                currency->ModifyRankPoints(-static_cast<int32>(posValue), player);
        }
        else
            AnnounceFailMsg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u Rank Points.", posValue);
    }

    return conditionMet;
}

const std::string CustomConditions::RequireRankPoints::ListCond(uint32 buyCount)
{
    std::string msg = "";
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (CondReqValue < 0)
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Rank Points.\nCondition |cffFF0000will|r remove requirement(s).", posValue);
    else
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Rank Points.\nCondition |cffFF0000will not|r remove requirement(s).", posValue);

    return msg;
}

const bool CustomConditions::RequireVotePoints::Meets(Player * player, uint32 buyCount)
{
    bool conditionMet = false;
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (AccountCurrency* currency = sCurrencyHandler->GetAccountCurrency(player->GetSession()->GetAccountId()))
    {
        if (currency->GetVP() >= int32(posValue))
        {
            conditionMet = true;
            if (CondReqValue < 0)
                currency->ModifyVP(-static_cast<int32>(posValue));
        }
        else
            AnnounceFailMsg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u Vote Points.", posValue);
    }

    return conditionMet;
}

const std::string CustomConditions::RequireVotePoints::ListCond(uint32 buyCount)
{
    std::string msg = "";
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (CondReqValue < 0)
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Vote Points.\nCondition |cffFF0000will|r remove requirement(s).", posValue);
    else
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Vote Points.\nCondition |cffFF0000will not|r remove requirement(s).", posValue);

    return msg;
}

const bool CustomConditions::RequireDonorPoints::Meets(Player * player, uint32 buyCount)
{
    bool conditionMet = false;
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (AccountCurrency* currency = sCurrencyHandler->GetAccountCurrency(player->GetSession()->GetAccountId()))
    {
        if (currency->GetDP() >= int32(posValue))
        {
            conditionMet = true;
            if (CondReqValue < 0)
                currency->ModifyDP(-static_cast<int32>(posValue));
        }
        else
            AnnounceFailMsg = Trinity::StringFormat("Sorry you do not meet the conditions. You need %u Donation Points.", posValue);
    }

    return conditionMet;
}

const std::string CustomConditions::RequireDonorPoints::ListCond(uint32 buyCount)
{
    std::string msg = "";
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (CondReqValue < 0)
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Donation Points.\nCondition |cffFF0000will|r remove requirement(s).", posValue);
    else
        msg = Trinity::StringFormat("Requires: |cff00FFFF%s|r Donation Points.\nCondition |cffFF0000will not|r remove requirement(s).", posValue);

    return msg;
}

const bool CustomConditions::RequireGuildLevel::Meets(Player * player, uint32 buyCount)
{
    bool conditionMet = false;
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);

    if (Guild* guild = player->GetGuild())
    {
        if (guild->GetLevel() >= CondReqValue)
            conditionMet = true;
        else
            AnnounceFailMsg = Trinity::StringFormat("Sorry you do not meed the conditions. Your guild have to be level %u.", CondReqValue);
    }
    else
        AnnounceFailMsg = "You have to be in a guild.";

    return conditionMet;
}

const std::string CustomConditions::RequireGuildLevel::ListCond(uint32 buyCount)
{
    uint32 posValue = GetAbsoluteValue(CondReqValue, buyCount);
    return Trinity::StringFormat("Requires: Guild Level |cff00FFFF%s|r.", posValue);;
}

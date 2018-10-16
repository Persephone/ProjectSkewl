/*#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "Battleground.h"
#include "Creature.h"
#include "Containers.h"
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
#include <regex>
#include <algorithm>
#include "CustomConditions.h"
#include "CustomMgr.h"

void removeCharsFromString(std::string &str, char const* charsToRemove) {
    for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
        str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
    }
}

bool TitleReqCondition::Evaluate(Player * player) // it wont until the system is changed wherever its used xD hm
{
    uint32 minTitle = player->GetTeam() == HORDE ? 15 : 1;
    uint32 maxTitle = player->GetTeam() == HORDE ? 28 : 14;

    if (m_reqEntry <= maxTitle)
    {
        uint8 found = 0;
        for (uint8 i = minTitle; i <= maxTitle; i++)
        {
            if (player->HasTitle(sCharTitlesStore.LookupEntry(i)))
                found = i;
        }
        if (found < m_reqEntry)
            return false;
    }
    else if (!player->HasTitle(sCharTitlesStore.LookupEntry(m_reqEntry)))
        return false;

    return true;
}

bool TitleReqCondition::IsValid()
{
    if (CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(m_reqEntry))
        return true;

    TC_LOG_ERROR("server.loading", "Condition ID %u With Title Req has invalid ReqEntry %u, skipped.", GetId(), m_reqEntry);
    return false;
}

bool ItemReqCondition::Evaluate(Player* player)
{
    uint32 posValue = abs(m_reqCount) * m_reqCount;

    if (player->HasItemCount(m_reqEntry, posValue, true))
    {
        if (m_reqCount < 0)
            player->DestroyItemCount(m_reqEntry, posValue, true);

        return true;
    }

    return false;
}

bool ItemReqCondition::IsValid()
{
    if (CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(m_reqEntry))
        return true;

    TC_LOG_ERROR("server.loading", "Condition ID %u With Title Req has invalid ReqEntry %u, skipped.", GetId(), m_reqEntry);
    return false;
}

std::string ItemReqCondition::ListCond()
{
    return Trinity::StringFormat(m_listCond, m_reqEntry, m_reqCount);
}

std::shared_ptr<CustomCondition> CreateNewConditon(uint32 id, CustomCondReqType type, uint32 condReqEntry, int32 condReqValue)
{
    switch (type)
    {
    case CUSTOM_CONDITION_REQ_ITEM:
    {
        return std::make_shared<CustomCondition>(new ItemReqCondition(id, std::make_tuple(condReqEntry, condReqValue)));
        break;
    }
    break;
    case CUSTOM_CONDITION_REQ_TITLE:
        return std::make_shared<CustomCondition>(new TitleReqCondition(id, std::make_tuple(condReqEntry, condReqValue)));
        break;
    case CUSTOM_CONDITION_REQ_MONEY:
    {

    }
    break;
    case CUSTOM_CONDITION_REQ_HONOR:
    {

    }
    break;
    case CUSTOM_CONDITION_REQ_ARENA:

    }
    break;
    case CUSTOM_CONDITION_REQ_GUILD_LEVEL:
    {

    }
    break;
    case CUSTOM_CONDITION_REQ_VP:
    case CUSTOM_CONDITION_REQ_DP:
    case CUSTOM_CONDITION_REQ_RANKPOINTS:
        break;
    default:
        break;
    }

    return nullptr;
}

void CustomConditionsHandler::LoadConditions()
{
    // Get the time before we started loading.
    uint32 oldMSTime = getMSTime();
    QueryResult result = WorldDatabase.Query("SELECT id, CondReqType, CondReqEntry, CondReqValue FROM custom_conditions");

    //If no rows found
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 custom conditions. DB table `custom_conditions` is empty.");
        return;
    }

    uint32 count = 0;

    //Loop through the results
    do
    {
        // Get the fields
        Field *fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();
        CustomCondReqType type = CustomCondReqType(fields[1].GetUInt32());
        uint32 condReqEntry = fields[2].GetUInt32();
        uint32 condReqValue = fields[3].GetInt32();
        std::string condReqName = "";

        if (type >= CUSTOM_CONDITION_MAX)
        {
            TC_LOG_ERROR("server.loading", "Condition ID %u has invalid CondReqType %u, skipped.", id, uint32(type));
            continue;
        }

        switch (type)
        {
        case CUSTOM_CONDITION_REQ_ITEM:
        {
            AddCondition(std::make_shared<CustomCondition>(new ItemReqCondition(id, std::make_tuple(condReqEntry, condReqValue))));
            break;
                //TC_LOG_ERROR("server.loading", "Condition ID %u With Item Req has invalid ReqEntry %u, skipped.", id, condReqEntry);
        }
        break;
        case CUSTOM_CONDITION_REQ_TITLE: // so the validation should happend here and this is where it should get the title info from dbcs, atleast from how i see it :P 
            AddCondition(std::make_shared<CustomCondition>(new TitleReqCondition(id, std::make_tuple(condReqEntry, condReqValue))));
            break;
        case CUSTOM_CONDITION_REQ_MONEY:
        {

        }
        break;
        case CUSTOM_CONDITION_REQ_HONOR:
        {

        }
        break;
        case CUSTOM_CONDITION_REQ_ARENA:
        {

        }
        break;
        case CUSTOM_CONDITION_REQ_GUILD_LEVEL:
        {

        }
        break;
        case CUSTOM_CONDITION_REQ_VP:
        case CUSTOM_CONDITION_REQ_DP:
        case CUSTOM_CONDITION_REQ_RANKPOINTS:
            break;
        default:
            break;
        }

        ++count;

    } while (result->NextRow());
}

CustomConditionsHandler* CustomConditionsHandler::instance()
{
    static CustomConditionsHandler instance;
    return &instance;
}

CustomConditionsHandler::~CustomConditionsHandler()
{

}

CustomCondition& CustomCondition::Multiply(int multiplier)
{
    CustomCondition& cpy(*this); // now it makes sense to me xD ^^ I hope this works but if this tuple thingie contains the entry and the value? or what does it contain?
    std::get<1>(cpy.m_values) *= multiplier;
    return cpy;
}*/

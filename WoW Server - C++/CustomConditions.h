/*#ifndef _CUSTOMCONDITIONSMGR_H
#define _CUSTOMCONDITIONSMGR_H

#include "Define.h" 
#include <memory>
#include <optional>
#include <utility>

enum CustomCondReqType
{
    CUSTOM_CONDITION_REQ_ITEM = 1,
    CUSTOM_CONDITION_REQ_TITLE,
    CUSTOM_CONDITION_REQ_MONEY,
    CUSTOM_CONDITION_REQ_HONOR,
    CUSTOM_CONDITION_REQ_ARENA,
    CUSTOM_CONDITION_REQ_RANKPOINTS,
    CUSTOM_CONDITION_REQ_VP,
    CUSTOM_CONDITION_REQ_DP,
    CUSTOM_CONDITION_REQ_GUILD_LEVEL,

    CUSTOM_CONDITION_MAX
};

class CustomCondition
{
public:
    CustomCondition(uint32 id, std::tuple<uint32, int32> values) : m_id(id), m_values(values) { }

    virtual bool Evaluate(Player* player) = 0;
    virtual std::string FailedResponseMsg() { return m_FailedMessage; }

    template<typename Args...>
    std::string ListCond(Args&&... args)
    {
        return fmt::sprintf(std::forward<Format>(m_listCond), std::forward<Args>(args)...);
    }

    virtual std::string ListCond() { return m_listCond; }
    virtual bool IsValid() = 0;

    CustomCondition& Multiply(int multiplier);
    uint32 GetId() { return m_id; }
    std::tuple<uint32, int32> GetValues() { return m_values; }
    CustomCondReqType GetType() { return m_type; }

protected:
    CustomCondReqType m_type;
    std::tuple<uint32, int32> m_values; 
    std::string m_FailedMessage;
    std::string m_listCond;
    uint32 m_id;
};

class TitleReqCondition : public CustomCondition
{
public:
    TitleReqCondition(int id, std::tuple<uint32, int32> values)
        : CustomCondition(id, values), m_reqEntry(std::get<0>(m_values))
    {
        m_FailedMessage = Trinity::StringFormat("Sorry you do not meet the conditions. You need the title: |cff00FFFF%s|r", m_reqName);
        m_listCond = Trinity::StringFormat("Requires Title:\n|cff00FFFF%s|r", m_reqName);
        m_type = CustomCondReqType::CUSTOM_CONDITION_REQ_TITLE;
    }

    bool Evaluate(Player* player) override;
    bool IsValid() override;

private:
    uint32& m_reqEntry;
    std::string m_reqName;
};

class ItemReqCondition : public CustomCondition
{
public:
    ItemReqCondition(int id, std::tuple<uint32, int32> values)
        : CustomCondition(id, values), m_reqEntry(std::get<0>(m_values)), m_reqCount(std::get<1>(m_values))
    {
        m_listCond = "Requires: %u |cff00FFFF%s|r.\nCondition |cffFF0000will not|r remove requirement(s).";
        m_type = CustomCondReqType::CUSTOM_CONDITION_REQ_ITEM;
    }

    bool Evaluate(Player* player) override;
    bool IsValid() override;

    std::string ListCond() override;

    uint32 GetReqEntry() { return m_reqEntry; }
    uint32 GetReqCount() { return m_reqCount; }

private:
    uint32& m_reqEntry;
    int32& m_reqCount;
    std::string m_reqName;
};

// Handler
class TC_GAME_API CustomConditionsHandler
{
public:
    CustomConditionsHandler();
    CustomConditionsHandler()
    {
        LoadConditions();
    }

    ~CustomConditionsHandler();

    void AddCondition(std::shared_ptr<CustomCondition> condition)
    {
        if (!condition->IsValid())
            return;

        // Might want to add failchecks here (to check that index is not occupied)
        m_conditions[condition->GetId()] = condition;
    }

    std::optional<std::shared_ptr<CustomCondition>> GetCondition(int id)
    {
        std::optional<std::shared_ptr<CustomCondition>> opt;
        auto itr = m_conditions.find(id);
        if (itr != m_conditions.end())
            opt = itr->second;
        return opt;
    }

    std::optional<std::shared_ptr<CustomCondition>> GetConditionByValue(CustomCondReqType type, int32 value)
    {
        std::optional<std::shared_ptr<CustomCondition>> opt;

        auto itr = std::find(m_conditions.begin(), m_conditions.end(), [&type, &value](std::pair<int, std::shared_ptr<CustomCondition>> pair) {
            return pair.second->GetType() == type && std::get<1>(pair.second->GetValues()) == value;
        });

        if (itr != m_conditions.end())
            opt = itr->second;

        return opt;
    }

    std::shared_ptr<CustomCondition> GetMaxIdCondition()
    {
        auto itr = std::max_element(m_conditions.begin(), m_conditions.end(), [](std::shared_ptr<CustomCondition> one, std::shared_ptr<CustomCondition> two) {
            return one->GetId() > two->GetId();
        });

        return itr->second;
    }

    std::shared_ptr<CustomCondition> CreateCustomCondition(uint32 id, CustomCondReqType type, uint32 entry, int32 value);

    std::unordered_map<int, std::shared_ptr<CustomCondition>> GetConditions() { return m_conditions; }

    static CustomConditionsHandler* instance();

private:

    void LoadConditions();

private:
    std::unordered_map<int, std::shared_ptr<CustomCondition>> m_conditions;
};

//Assign the class
#define sCustomConditionsMgr CustomConditionsHandler::instance()

#endif
*/

#ifndef DEF_CUSTOMCONDITIONS_H
#define DEF_CUSTOMCONDITIONS_H

class Player;
class Creature;

#include "Define.h"
#include <unordered_map>
#include <memory>

namespace CustomConditions
{
    enum CustomCondReqType : uint8
    {
        CUSTOM_CONDITIONS_TYPE_NONE = 0,

        CUSTOM_CONDITION_REQ_ITEM,
        CUSTOM_CONDITION_REQ_TITLE,
        CUSTOM_CONDITION_REQ_MONEY,
        CUSTOM_CONDITION_REQ_HONORPOINTS,
        CUSTOM_CONDITION_REQ_ARENAPOINTS,
        CUSTOM_CONDITION_REQ_RANKPOINTS,
        CUSTOM_CONDITION_REQ_VP,
        CUSTOM_CONDITION_REQ_DP,
        CUSTOM_CONDITION_REQ_GUILD_LEVEL,

        CUSTOM_CONDITIONS_MAX
    };

    class CustomCondition
    {
    protected:
        CustomCondReqType CondReqType = CUSTOM_CONDITIONS_TYPE_NONE;
        uint32 CondReqEntry           = 0; 
        int32  CondReqValue           = 0;
        std::string CondReqName       = "";
        std::string AnnounceFailMsg   = "";
        const uint32 GetAbsoluteValue(int32 value, int32 mod = 1) { return abs(value) * mod; }

    public:
        explicit CustomCondition(CustomCondReqType type, uint32 entry, int32 value, std::string name) : CondReqType(type), CondReqEntry(entry), CondReqValue(value), CondReqName(name) { }
        explicit CustomCondition(CustomCondReqType type, uint32 entry, std::string name) : CondReqType(type), CondReqEntry(entry), CondReqName(name) { }
        explicit CustomCondition(CustomCondReqType type, uint32 entry, int32 value) : CondReqType(type), CondReqEntry(entry), CondReqValue(value) { }
        explicit CustomCondition(CustomCondReqType type, int32 value) : CondReqType(type), CondReqValue(value) { }

        virtual const bool Meets(Player* player, uint32 buyCount = 1) = 0;
        virtual const std::string ListCond(uint32 buyCount = 0) = 0;

        void AnnounceFail(Player* player, Creature* creature = nullptr);
        const CustomCondReqType GetType() { return CondReqType; }
        int32 GetValue() const { return CondReqValue; }
        uint32 GetEntry() const { return CondReqEntry; }

        virtual ~CustomCondition() = default;
    };

    class RequireItem : public CustomCondition
    {
    public:
        RequireItem(uint32 entry, int32 value, std::string name) : CustomCondition(CUSTOM_CONDITION_REQ_ITEM, entry, value, name) { }

        const bool Meets(Player* player, uint32 buyCount = 1) override;
        const std::string ListCond(uint32 buyCount) override;
    };

    class RequireTitle : public CustomCondition
    {
    public:
        RequireTitle(uint32 entry, std::string name) : CustomCondition(CUSTOM_CONDITION_REQ_TITLE, entry, name) { }

        const bool Meets(Player* player, uint32 buyCount = 1) override;
        const std::string ListCond(uint32 buyCount) override;
    };

    class RequireMoney : public CustomCondition
    {
    public:
        RequireMoney(int32 value) : CustomCondition(CUSTOM_CONDITION_REQ_MONEY, value) { }

        const bool Meets(Player* player, uint32 buyCount = 1) override;
        const std::string ListCond(uint32 buyCount) override;
    };

    class RequireHonorPoints : public CustomCondition
    {
    public:
        RequireHonorPoints(int32 value) : CustomCondition(CUSTOM_CONDITION_REQ_HONORPOINTS, value) { }

        const bool Meets(Player* player, uint32 buyCount = 1) override;
        const std::string ListCond(uint32 buyCount) override;
    };

    class RequireArenaPoints : public CustomCondition
    {
    public:
        RequireArenaPoints(int32 value) : CustomCondition(CUSTOM_CONDITION_REQ_ARENAPOINTS, value) { }

        const bool Meets(Player* player, uint32 buyCount = 1) override;
        const std::string ListCond(uint32 buyCount) override;
    };

    class RequireRankPoints : public CustomCondition
    {
    public:
        RequireRankPoints(int32 value) : CustomCondition(CUSTOM_CONDITION_REQ_RANKPOINTS, value) { }

        const bool Meets(Player* player, uint32 buyCount = 1) override;
        const std::string ListCond(uint32 buyCount) override;
    };

    class RequireVotePoints : public CustomCondition
    {
    public:
        RequireVotePoints(int32 value) : CustomCondition(CUSTOM_CONDITION_REQ_VP, value) { }

        const bool Meets(Player* player, uint32 buyCount = 1) override;
        const std::string ListCond(uint32 buyCount) override;
    };

    class RequireDonorPoints : public CustomCondition
    {
    public:
        RequireDonorPoints(int32 value) : CustomCondition(CUSTOM_CONDITION_REQ_DP, value) { }

        const bool Meets(Player* player, uint32 buyCount = 1) override;
        const std::string ListCond(uint32 buyCount) override;
    };

    class RequireGuildLevel : public CustomCondition
    {
    public:
        RequireGuildLevel(int32 value) : CustomCondition(CUSTOM_CONDITION_REQ_GUILD_LEVEL, value) { }

        const bool Meets(Player* player, uint32 buyCount = 1) override;
        const std::string ListCond(uint32 buyCount) override;
    };

    class TC_GAME_API CustomConditionsMgr
    {
    public:
        static CustomConditionsMgr* instance();

        std::unique_ptr<CustomCondition> const* GetConditionById(uint32 id) const;
        std::unique_ptr<CustomCondition> const* GetConditionByValue(uint8 type, uint32 value) const;
        void LoadCustomConditions();

    private:
        CustomConditionsMgr() { };
        ~CustomConditionsMgr() { };

        std::unordered_map<uint32, std::unique_ptr<CustomCondition>> m_CustomConditions;
    };
};

#define sCustomConditionsMgr CustomConditions::CustomConditionsMgr::instance()

#endif

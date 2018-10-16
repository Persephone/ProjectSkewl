#ifndef _VQMGR_H
#define _VQMGR_H

#include "Define.h"

class Player;
class Guild;
class Battleground;
struct ItemTemplate;

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

struct CustomConditions
{
    uint32 CondId;
    CustomCondReqType CondReqType;
    uint32 CondReqEntry;
    int32 CondReqValue;
    std::string CondReqName;

    bool Meets(Player* player, Creature* creature = nullptr, uint32 itemId = 0, uint32 itemCount = 1) const;
    std::string ListCond(uint32 count = 1) const;
};

struct RandomBossInfo
{
	uint32 Entry;
    float x;
    float y;
    float z;
    uint32 MapId;
    uint32 BossMapId;
	std::string Hint;
    uint32 NextBoss;
    std::string WormholeInfo;
    bool Enabled;
};

struct VQRank
{
    uint16 pointsNeeded;
    std::string nameA;
    std::string nameH;
    std::string color;
	std::string iconA;
    std::string iconH;

    VQRank(uint16 _pointsNeeded = 0, std::string _nameA = "Unranked", std::string _nameH = "Unranked", std::string _color = "A4A4A4", std::string _iconA = "U", std::string _iconH = "U") :
    pointsNeeded(_pointsNeeded), nameA(_nameA), nameH(_nameH), color(_color), iconA(_iconA), iconH(_iconH) { }
};

struct CBattlegroundQuests
{
	CBattlegroundQuests(uint32 bgMap, uint32 questId, uint32 reward, uint32 rewardArena) : bgId(bgMap), quest(questId), npc(reward), npcArena(rewardArena) { }

	uint32 bgId;
	uint32 quest;
	uint32 npc;
	uint32 npcArena;
};

struct KillStreakInfo
{
    uint32 killstreak;
    uint32 deathstreak;
    ObjectGuid lastkill;
};

enum ExtraMaskInfo
{
    EXTRAMASK_FREE_VIP      = 0x00000001,
    EXTRAMASK_CINEMATIC     = 0x00000002,
    EXTRAMASK_SML	        = 0x00000004,
	EXTRAMASK_SMS		    = 0x00000008,
	EXTRAMASK_SPHERE		= 0x00000010,
	EXTRAMASK_SHELL         = 0x00000020,
	EXTRAMASK_BECON	        = 0x00000040,
	EXTRAMASK_ANVEENA	    = 0x00000080,
	EXTRAMASK_STARFALL      = 0x00000100,
	EXTRAMASK_FORCE         = 0x00000200,
	EXTRAMASK_BALL          = 0x00000400,
	EXTRAMASK_PURPLE        = 0x00000800
};

enum Boosters
{
    BOOSTER_GOLD            = 0,    // done
    BOOSTER_HONOR           = 1,    // done
    BOOSTER_RANK_POINTS     = 2,    // done
    BOOSTER_WSG_MARKS       = 3,    // done
    BOOSTER_MONEY           = 4,    // done
    BOOSTER_REP             = 5,    // done
    BOOSTER_REPAIR_COST     = 6,    // done
    BOOSTER_ARENA_POINTS    = 7,    // done
};

struct BoostInfo
{
    BoostInfo(ObjectGuid guid, uint64 date, uint32 type, float modifier) : ownerguid(guid), unsetdate(date), boost_type(type), boost_modifier(modifier) { }

    ObjectGuid ownerguid;
    uint64 unsetdate;
    uint32 boost_type;
    float  boost_modifier;
};

enum GuildBonus : uint8
{
    GUILD_BONUS_LOOT_GOLD       = 0,
    GUILD_BONUS_HONOR           = 1,
    GUILD_BONUS_VENDOR_DISCOUNT = 2,
    GUILD_BONUS_REPUTATION      = 3,
    GUILD_BONUS_MARKS           = 4
};

struct GuildLevelBonuses
{
    uint8 Level;
    GuildBonus Bonus;
    float Multiplier;
};

struct DesertInfo
{
    uint32 Count;
    time_t TimeLeft;
};

class TC_GAME_API CustomMgr
{
private:
    // VIP CONTAINER
    typedef std::unordered_map<uint32, uint64> VIPContainer;
    VIPContainer m_VIPContainer;

	typedef std::unordered_map<uint32, uint64> FFAContainer;
    FFAContainer m_FFAContainer;

	typedef std::unordered_map<uint32, uint64> SanctuaryContainer;
    SanctuaryContainer m_SanctuaryContainer;

    /* GUILD LEVEL BONUSES */
    typedef std::vector<GuildLevelBonuses> GuildBonusContainer;
    GuildBonusContainer GuildBonusInfo;

	std::set<uint32> TitanNoGroupAreaSet;
	std::set<uint32> TitanNoFlyAreaSet;
	typedef std::unordered_map<std::string, DesertInfo> DeserterContainer;
    DeserterContainer m_Deserters;

	typedef std::vector<CBattlegroundQuests> BattlegroundQuestsList;
	BattlegroundQuestsList BGQuests;

	typedef std::vector<VQRank> VQRankListType;
	VQRankListType VQRankList;

    typedef std::unordered_map<ObjectGuid, KillStreakInfo> KillStreakData;
    KillStreakData KillStreakContainer;

    typedef std::map<uint32, CustomConditions> CustomCondList;
    CustomCondList CustomCondContainer;

    typedef std::unordered_map<ObjectGuid, uint32> CharGuildXPContainer;
    CharGuildXPContainer CharGuildXp;

    /* BOOSTERS */
    typedef std::vector<BoostInfo> BoostContainer;

    void SaveBoosters();
    void AddBooster(ObjectGuid guid, uint64 unsetdate, Boosters boost_type, float boost_modifier);

    void LoadBoosters();
    void LoadDailyQuests();
    void LoadSanctuaryAreas();
    void LoadFFAPvPAreas();
    void LoadNoGroupAreas();
    void LoadNoFlyAreas();
    void LoadGuildBonusInfo();
    void LoadCharGuildXp();

public:
    CustomMgr();
    static CustomMgr* instance();

    BattlegroundTypeId m_ExtraTalentBG;

    WorldLocation m_MallLocationA;
    WorldLocation m_MallLocationH;
    ObjectGuid HighScoreGuid;
    BattlegroundTypeId m_SelectedEventBG;

    /* BOOST SYSTEM*/
    void SaveBooster(ObjectGuid guid);
    bool CheckBooster(ObjectGuid guid, Boosters boost_type);
    float GetBoosterMod(ObjectGuid guid, Boosters boost_type);
    std::string GetBoosterName(uint32 boost_type);
    time_t GetRemainingBoostTime(uint64 start_date, uint64 unsetdate) const
    {
        double diff = difftime(unsetdate - start_date, time(NULL));
        if (diff <= 0)
            return 0;
        return diff;
    }
    BoostContainer m_BoostContainer;

    // GUILD LEVEL SYSTEM
    GuildLevelBonuses* GetGuildBonusByLevel(uint8 level);
    float GetGuildBonus(uint8 level, GuildBonus bonus);
    void ListGuildBonusesForGossip(Player* player, Guild* guild, int32 sender, int32 action);
    std::string GetGuildBonusName(uint8 bonus) const;

    // BG KILLINGSPREE FUNCTIONS
    void RemoveKillStreakData(ObjectGuid guid);
    void HandleBGKill(Battleground* bg, Player* killer, Player* victim);
    std::string GetSpreeString(int value, Player* killer);

    void AddRank(uint16 pointsNeeded, std::string nameA, std::string nameH, std::string color, std::string iconA, std::string iconH);
    void LoadRanks();
	std::vector<std::string> bossIps;

    std::string MoneyToMoneyString(uint32 money);

    CustomConditions const* GetConditionDataById(uint32 id) const;
    CustomConditions const* GetConditionDataByValue(uint8 type, uint32 value) const;
    void AddCondition(uint32 id, uint8 type, uint32 entry, int32 value, std::string name);

    void IncreaseCharGuildXp(ObjectGuid guid, uint32 val);
    uint32 GetCharGuildXp(ObjectGuid guid);
    void SaveCharGuildXp(ObjectGuid guid);
    void SetCharGuildXp(ObjectGuid guid, uint32 val);

	std::vector<uint32> DailyQuests;
	std::vector<uint32> DebuffClassMasks;
	std::vector<RandomBossInfo> RandomBossStore;

	VQRank GetRankDataByPoints(uint16 points, ObjectGuid guid = ObjectGuid::Empty);
    VQRank GetRankDataByName(std::string name);
    VQRank GetRankDataByColor(std::string color);

    const CustomCondList &get_Conditions() const { return CustomCondContainer; }

    uint64 GetVIPDuration(uint32 account);

    void LoadAll();
    void LoadArenaDebuff();
    void LoadBattlegroundQuests();
    void LoadCustomConditions();
    void LoadVIPAccounts();

    DesertInfo* GetDesertInfo(std::string ip);
    void AddDeserter(ObjectGuid guid, std::string ip);
    void ApplyDeserter(ObjectGuid guid, std::string ip, bool increase);
    void HandleDeserter(ObjectGuid, std::string ip);
	void ClearDeserters() { m_Deserters.clear(); }
    void AssignTitle(Player* player, uint16 points);

    std::string GetSpellIcon(uint32 entry, uint32 width, uint32 height, int x, int y) const;

	bool WorldChatStatus;
	bool IsSanctuary(uint32 areaID);
	void AddSanctuary(uint32 areaID, uint64 duration);
	bool IsFFAPvP(uint32 areaID);
    void DeleteFFAPvP(uint32 areaID);
    void DeleteSanctuary(uint32 areaID);
    bool IsVIP(uint32 account);
    bool AddVIP(uint32 account, uint32 duration);
	void AddFFAPvP(uint32 areaID, uint64 duration);
	bool NoGroupArea(uint32 areaID) const { return TitanNoGroupAreaSet.find(areaID) != TitanNoGroupAreaSet.end(); };
	bool NoFlyArea(uint32 areaID) const { return TitanNoFlyAreaSet.find(areaID) != TitanNoFlyAreaSet.end(); }
    void HandleSelectDuelBetting(Player* player, Creature* creature, int32 item, int32 count);
	void HandleBGQuests(Player* player, bool isArena);
	bool EventRunning;

    std::string GetRaceIcon(uint16 race_id, uint8 gender, uint32 width, uint32 height, int x, int y)
    {
        std::string icon = "";
        switch (race_id)
        {
        case RACE_HUMAN: icon = gender == GENDER_MALE ? "achievement_character_human_male" : "achievement_character_human_female";
            break;
        case RACE_ORC: icon = gender == GENDER_MALE ? "achievement_character_orc_male" : "achievement_character_orc_female";
            break;
        case RACE_DWARF: icon = gender == GENDER_MALE ? "achievement_character_dwarf_male" : "achievement_character_dwarf_female";;
            break;
        case RACE_GNOME: icon = gender == GENDER_MALE ? "achievement_character_gnome_male" : "achievement_character_gnome_female";
            break;
        case RACE_DRAENEI: icon = gender == GENDER_MALE ? "achievement_character_draenei_male" : "achievement_character_draenei_female";
            break;
        case RACE_TROLL: icon = gender == GENDER_MALE ? "achievement_character_troll_male" : "achievement_character_troll_female";
            break;
        case RACE_TAUREN: icon = gender == GENDER_MALE ? "achievement_character_tauren_male" : "achievement_character_tauren_female";
            break;
        case RACE_BLOODELF: icon = gender == GENDER_MALE ? "achievement_character_bloodelf_male" : "achievement_character_bloodelf_female";
            break;
        case RACE_NIGHTELF: icon = gender == GENDER_MALE ? "achievement_character_nightelf_male" : "achievement_character_nightelf_female";
            break;
        case RACE_UNDEAD_PLAYER: icon = gender == GENDER_MALE ? "achievement_character_undead_male" : "achievement_character_undead_female";
            break;
        default: icon = "Inv_misc_questionmark";
            break;
        }

        return Trinity::StringFormat("|TInterface\\ICONS\\%s:%u:%u:%i:%i|t|t", icon, width, height, x, y);
    }

	std::string GetClassIcon(uint16 class_id, uint32 width, uint32 height, int x, int y)
	{
        std::string icon = "";
		switch (class_id)
		{
		case CLASS_WARRIOR: icon = "inv_sword_27";
			break;
		case CLASS_PALADIN: icon = "ability_thunderbolt";
			break;
		case CLASS_HUNTER: icon = "inv_weapon_bow_07";
			break;
		case CLASS_ROGUE: icon = "inv_throwingknife_04";
			break;
		case CLASS_PRIEST: icon = "inv_staff_30";
			break;
		case CLASS_DEATH_KNIGHT: icon = "spell_deathknight_classicon";
			break;
		case CLASS_SHAMAN: icon = "spell_nature_bloodlust";
			break;
		case CLASS_MAGE: icon = "inv_staff_13.jpg";
			break;
		case CLASS_WARLOCK: icon = "spell_nature_drowsy";
			break;
		case CLASS_DRUID: icon = "Ability_Druid_Maul";
			break;
		default: icon = "Inv_misc_questionmark";
			break;
		}

		return Trinity::StringFormat("|TInterface\\ICONS\\%s:%u:%u:%i:%i|t|t", icon, width, height, x, y);
	}

	std::string killstorank(uint8 race, uint32 kills)
	{
		const char* rankStr = NULL;
		// alliance icons [blue]
		if ((race == RACE_HUMAN) || (race == RACE_DWARF) || (race == RACE_NIGHTELF) || (race == RACE_GNOME) || (race == RACE_DRAENEI))
		{
			if (kills < 250) // rank under 250 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_01:20:20:0:0|t";
			else if (kills >= 250 && kills < 500) // rank at 250 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_02:20:20:0:0|t";
			else if (kills >= 500 && kills < 750) // rank at 500 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_03:20:20:0:0|t";
			else if (kills >= 750 && kills < 1000) // rank at 750 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_04:20:20:0:0|t";
			else if (kills >= 1000 && kills < 1250) // rank at 1000 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_05:20:20:0:0|t";
			else if (kills >= 1250 && kills < 1500) // rank at 1250 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_06:20:20:0:0|t";
			else if (kills >= 1500 && kills < 1750) // rank at 1500 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_07:20:20:0:0|t";
			else if (kills >= 1750 && kills < 2000) // rank at 1750 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_08:20:20:0:0|t";
			else if (kills >= 2000 && kills < 2250) // rank at 2000 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_09:20:20:0:0|t";
			else if (kills >= 2250 && kills < 2500) // rank at 2250 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_10:20:20:0:0|t";
			else if (kills >= 2500 && kills < 2750) // rank at 2500 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_11:20:20:0:0|t";
			else if (kills >= 2750 && kills < 3000) // rank at 2750 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_12:20:20:0:0|t";
			else if (kills >= 3000 && kills < 4000) // rank at 3000 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_13:20:20:0:0|t";
			else if (kills >= 4000 && kills < 7000) // rank at 4000 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_14:20:20:0:0|t";
			else if (kills >= 7000 && kills < 10000) // rank at 7000 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_A_15:20:20:0:0|t";
			else if (kills >= 10000) // rank at 10000+ kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_P_250K:20:20:0:0|t";
			else
				rankStr = "|TInterface\\ICONS\\Inv_misc_questionmark:20:20:0:0|t";
		}
		else // horde icons [red]
		{
			if (kills < 250) // rank under 250 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_01:20:20:0:0|t";
			else if (kills >= 250 && kills < 500) // rank at 250 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_02:20:20:0:0|t";
			else if (kills >= 500 && kills < 750) // rank at 500 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_03:20:20:0:0|t";
			else if (kills >= 750 && kills < 1000) // rank at 750 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_04:20:20:0:0|t";
			else if (kills >= 1000 && kills < 1250) // rank at 1000 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_05:20:20:0:0|t";
			else if (kills >= 1250 && kills < 1500) // rank at 1250 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_06:20:20:0:0|t";
			else if (kills >= 1500 && kills < 1750) // rank at 1500 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_07:20:20:0:0|t";
			else if (kills >= 1750 && kills < 2000) // rank at 1750 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_08:20:20:0:0|t";
			else if (kills >= 2000 && kills < 2250) // rank at 2000 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_09:20:20:0:0|t";
			else if (kills >= 2250 && kills < 2500) // rank at 2250 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_10:20:20:0:0|t";
			else if (kills >= 2500 && kills < 2750) // rank at 2500 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_11:20:20:0:0|t";
			else if (kills >= 2750 && kills < 3000) // rank at 2750 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_12:20:20:0:0|t";
			else if (kills >= 3000 && kills < 4000) // rank at 3000 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_13:20:20:0:0|t";
			else if (kills >= 4000 && kills < 7000) // rank at 4000 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_14:20:20:0:0|t";
			else if (kills >= 7000 && kills < 10000) // rank at 7000 kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_H_15:20:20:0:0|t";
			else if (kills >= 10000) // rank at 10000+ kills
				rankStr = "|TInterface\\ICONS\\Achievement_PVP_P_250K:20:20:0:0|t";
			else
				rankStr = "|TInterface\\ICONS\\Inv_misc_questionmark:20:20:0:0|t";
		}
		return rankStr;
	}

	std::string ratingtorank(uint32 rating, uint8 type)
	{
		const char* rankStr = NULL;
		if (type == 2)
		{
			if (rating < 1700) // if rating under 1700
				rankStr = "|TInterface\\ICONS\\Achievement_Arena_2v2_3:25:25:-22:0|t";
			else if (rating >= 1700 && rating < 1900) // if rating at 1700+
				rankStr = "|TInterface\\ICONS\\Achievement_Arena_2v2_5:25:25:-22:0|t";
			else if (rating >= 1900 && rating < 2100) // if rating at 1900+
				rankStr = "|TInterface\\ICONS\\Achievement_Arena_2v2_7:25:25:-22:0|t";
			else if (rating >= 2100) // if rating at 2100+
				rankStr = "|TInterface\\ICONS\\Achievement_FeatsOfStrength_Gladiator_02:25:25:-22:0|t";
			else
				rankStr = "|TInterface\\ICONS\\Inv_misc_questionmark:25:25:-22:0|t";
		}
		if (type == 3)
		{
			if (rating < 1700) // if rating under 1700
				rankStr = "|TInterface\\ICONS\\Achievement_Arena_3v3_1:25:25:-22:0|t";
			else if (rating >= 1700 && rating < 1900) // if rating at 1700+
				rankStr = "|TInterface\\ICONS\\Achievement_Arena_3v3_6:25:25:-22:0|t";
			else if (rating >= 1900 && rating < 2100) // if rating at 1900+
				rankStr = "|TInterface\\ICONS\\Achievement_Arena_3v3_7:25:25:-22:0|t";
			else if (rating >= 2100) // if rating at 2100+
				rankStr = "|TInterface\\ICONS\\Achievement_FeatsOfStrength_Gladiator_09:25:25:-22:0|t";
			else
				rankStr = "|TInterface\\ICONS\\Inv_misc_questionmark:25:25:-22:0|t";
		}
		if (type == 5)
		{
			if (rating < 1700) // if rating under 1700
				rankStr = "|TInterface\\ICONS\\Achievement_Arena_5v5_1:25:25:-22:0|t";
			else if (rating >= 1700 && rating < 1900) // if rating at 1700+
				rankStr = "|TInterface\\ICONS\\Achievement_Arena_5v5_5:25:25:-22:0|t";
			else if (rating >= 1900 && rating < 2100) // if rating at 1900+
				rankStr = "|TInterface\\ICONS\\Achievement_Arena_5v5_7:25:25:-22:0|t";
			else if (rating >= 2100) // if rating at 2100+
				rankStr = "|TInterface\\ICONS\\Achievement_FeatsOfStrength_Gladiator_04:25:25:-22:0|t";
			else
				rankStr = "|TInterface\\ICONS\\Inv_misc_questionmark:25:25:-22:0|t";
		}
		return rankStr;
	}

	std::string TimeToStamp(time_t t)
	{
		tm aTm;
		localtime_r(&t, &aTm);
		//       YYYY   year
		//       MM     month (2 digits 01-12)
		//       DD     day (2 digits 01-31)
		//       HH     hour (2 digits 00-23)
		//       MM     minutes (2 digits 00-59)
		//       SS     seconds (2 digits 00-59)
		char buf[20];
		snprintf(buf, 20, "%02d-%02d-%04d", aTm.tm_mday, aTm.tm_mon + 1, aTm.tm_year + 1900);
		return std::string(buf);
	}

    std::string GetCondName(uint32 cond) const;

    std::string GetItemName(uint32 entry, LocaleConstant localeConstant);

    std::string GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y);

    std::string GetItemLink(Player* player, const ItemTemplate* temp);

    std::string RaceIdToString(uint8 race_id)
    {
        const char* race_s = NULL;
        switch (race_id)
        {
        case RACE_HUMAN:            race_s = "Human";       break;
        case RACE_ORC:              race_s = "Orc";         break;
        case RACE_DWARF:            race_s = "Dwarf";       break;
        case RACE_NIGHTELF:         race_s = "Night Elf";   break;
        case RACE_UNDEAD_PLAYER:    race_s = "Undead";      break;
        case RACE_TAUREN:           race_s = "Tauren";      break;
        case RACE_GNOME:            race_s = "Gnome";       break;
        case RACE_TROLL:            race_s = "Troll";       break;
        case RACE_BLOODELF:         race_s = "Blood Elf";   break;
        case RACE_DRAENEI:          race_s = "Draenei";     break;
        default:                    race_s = "unknown";
        }
        return race_s;
    }

	std::string ClassIdToString(uint8 class_id)
	{
		const char* classStr = NULL;
		switch (class_id)
		{
			//warrior
		case CLASS_WARRIOR: classStr = "Warrior";
			break;
			//paladin
		case CLASS_PALADIN: classStr = "Paladin";
			break;
			//hunter
		case CLASS_HUNTER: classStr = "Hunter";
			break;
			//rogue
		case CLASS_ROGUE: classStr = "Rogue";
			break;
			//priest
		case CLASS_PRIEST: classStr = "Priest";
			break;
			//Deathknight
		case CLASS_DEATH_KNIGHT: classStr = "Death Knight";
			break;
			//Shaman
		case CLASS_SHAMAN: classStr = "Shaman";
			break;
			//mage
		case CLASS_MAGE: classStr = "Mage";
			break;
			//Warlock
		case CLASS_WARLOCK: classStr = "Warlock";
			break;
			//Druid
		case CLASS_DRUID: classStr = "Druid";
			break;
		default: classStr = "|TInterface\\ICONS\\Inv_misc_questionmark:20:20:0:0|t";
			break;
		}
		return classStr;
	}

    std::string GMColor(uint8 rank)
    {
        std::string color = "";
        switch (rank)
        {
        case 1:
            color = "|cff00BFFF[GM]";
            break;
        case 2:
            color = "|cff00BFFF[GM]";
            break;
        case 3:
            color = "|cff800000[Admin]";
            break;
        }
        return color;
    }

    std::string GetNameLink(Player* player)
    {
        std::string name = player->GetName();
        std::string color;
        switch (player->getClass())
        {
        case CLASS_DEATH_KNIGHT:
            color = "|cffC41F3B";
            break;
        case CLASS_DRUID:
            color = "|cffFF7D0A";
            break;
        case CLASS_HUNTER:
            color = "|cffABD473";
            break;
        case CLASS_MAGE:
            color = "|cff69CCF0";
            break;
        case CLASS_PALADIN:
            color = "|cffF58CBA";
            break;
        case CLASS_PRIEST:
            color = "|cffFFFFFF";
            break;
        case CLASS_ROGUE:
            color = "|cffFFF569";
            break;
        case CLASS_SHAMAN:
            color = "|cff0070DE";
            break;
        case CLASS_WARLOCK:
            color = "|cff9482C9";
            break;
        case CLASS_WARRIOR:
            color = "|cffC79C6E";
            break;
        }
        return "|Hplayer:" + name + "|h|cffFFFFFF[" + color + name + "|cffFFFFFF]|h|r";
    }

    ~CustomMgr();
};

//Assign the class
#define sCustomMgr CustomMgr::instance()

#endif

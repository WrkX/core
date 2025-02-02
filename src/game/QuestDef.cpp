/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) 2011-2016 Nostalrius <https://nostalrius.org>
 * Copyright (C) 2016-2017 Elysium Project <https://github.com/elysium-project>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "QuestDef.h"
#include "Player.h"
#include "World.h"

Quest::Quest(Field* questRecord)
{
    QuestId = questRecord[0].GetUInt32();
    QuestMethod = questRecord[1].GetUInt32();
    ZoneOrSort = questRecord[2].GetInt32();
    MinLevel = questRecord[3].GetUInt32();
    MaxLevel = questRecord[125].GetUInt32();
    QuestLevel = questRecord[4].GetUInt32();
    Type = questRecord[5].GetUInt32();
    RequiredClasses = questRecord[6].GetUInt32();
    RequiredRaces = questRecord[7].GetUInt32();
    RequiredSkill = questRecord[8].GetUInt32();
    RequiredSkillValue = questRecord[9].GetUInt32();
    RequiredCondition = questRecord[128].GetUInt32();
    RepObjectiveFaction = questRecord[10].GetUInt32();
    RepObjectiveValue = questRecord[11].GetInt32();
    RequiredMinRepFaction = questRecord[12].GetUInt32();
    RequiredMinRepValue = questRecord[13].GetInt32();
    RequiredMaxRepFaction = questRecord[14].GetUInt32();
    RequiredMaxRepValue = questRecord[15].GetInt32();
    SuggestedPlayers = questRecord[16].GetUInt32();
    LimitTime = questRecord[17].GetUInt32();
    m_QuestFlags = questRecord[18].GetUInt16();
    m_SpecialFlags = questRecord[19].GetUInt16();
    PrevQuestId = questRecord[20].GetInt32();
    NextQuestId = questRecord[21].GetInt32();
    ExclusiveGroup = questRecord[22].GetInt32();
    BreadcrumbForQuestId = questRecord[129].GetInt32();
    NextQuestInChain = questRecord[23].GetUInt32();
    SrcItemId = questRecord[24].GetUInt32();
    SrcItemCount = questRecord[25].GetUInt32();
    SrcSpell = questRecord[26].GetUInt32();
    Title = questRecord[27].GetCppString();
    Details = questRecord[28].GetCppString();
    Objectives = questRecord[29].GetCppString();
    OfferRewardText = questRecord[30].GetCppString();
    RequestItemsText = questRecord[31].GetCppString();
    EndText = questRecord[32].GetCppString();

    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        ObjectiveText[i] = questRecord[33 + i].GetCppString();

    for (int i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; ++i)
        ReqItemId[i] = questRecord[37 + i].GetUInt32();

    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        ReqItemCount[i] = questRecord[41 + i].GetUInt32();

    for (int i = 0; i < QUEST_SOURCE_ITEM_IDS_COUNT; ++i)
        ReqSourceId[i] = questRecord[45 + i].GetUInt32();

    for (int i = 0; i < QUEST_SOURCE_ITEM_IDS_COUNT; ++i)
        ReqSourceCount[i] = questRecord[49 + i].GetUInt32();

    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        ReqCreatureOrGOId[i] = questRecord[53 + i].GetInt32();

    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        ReqCreatureOrGOCount[i] = questRecord[57 + i].GetUInt32();

    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        ReqSpell[i] = questRecord[61 + i].GetUInt32();

    for (int i = 0; i < QUEST_REWARD_CHOICES_COUNT; ++i)
        RewChoiceItemId[i] = questRecord[65 + i].GetUInt32();

    for (int i = 0; i < QUEST_REWARD_CHOICES_COUNT; ++i)
        RewChoiceItemCount[i] = questRecord[71 + i].GetUInt32();

    for (int i = 0; i < QUEST_REWARDS_COUNT; ++i)
        RewItemId[i] = questRecord[77 + i].GetUInt32();

    for (int i = 0; i < QUEST_REWARDS_COUNT; ++i)
        RewItemCount[i] = questRecord[81 + i].GetUInt32();

    for (int i = 0; i < QUEST_REPUTATIONS_COUNT; ++i)
        RewRepFaction[i] = questRecord[85 + i].GetUInt32();

    for (int i = 0; i < QUEST_REPUTATIONS_COUNT; ++i)
        RewRepValue[i] = questRecord[90 + i].GetInt32();

    RewXP = questRecord[127].GetUInt32();
    RewOrReqMoney = questRecord[95].GetInt32();
    RewMoneyMaxLevel = questRecord[96].GetUInt32();
    RewSpell = questRecord[97].GetUInt32();
    RewSpellCast = questRecord[98].GetUInt32();
    RewMailTemplateId = questRecord[99].GetInt32();
    RewMailDelaySecs = questRecord[100].GetUInt32();
    RewMailMoney = questRecord[126].GetUInt32();
    PointMapId = questRecord[101].GetUInt32();
    PointX = questRecord[102].GetFloat();
    PointY = questRecord[103].GetFloat();
    PointOpt = questRecord[104].GetUInt32();

    for (int i = 0; i < QUEST_EMOTE_COUNT; ++i)
        DetailsEmote[i] = questRecord[105 + i].GetUInt32();

    for (int i = 0; i < QUEST_EMOTE_COUNT; ++i)
        DetailsEmoteDelay[i] = questRecord[109 + i].GetUInt32();

    IncompleteEmote = questRecord[113].GetUInt32();
    CompleteEmote = questRecord[114].GetUInt32();

    for (int i = 0; i < QUEST_EMOTE_COUNT; ++i)
        OfferRewardEmote[i] = questRecord[115 + i].GetInt32();

    for (int i = 0; i < QUEST_EMOTE_COUNT; ++i)
        OfferRewardEmoteDelay[i] = questRecord[119 + i].GetInt32();

    QuestStartScript = questRecord[123].GetUInt32();
    QuestCompleteScript = questRecord[124].GetUInt32();

    m_isActive = true;

    if (QuestMethod & QUEST_METHOD_DISABLED)
    {
        // Leave invalid entries to be caught by ObjectMgr
        if (QuestMethod <= QUEST_METHOD_LIMIT)
            QuestMethod = QUEST_METHOD_DISABLED;

        m_isActive = false;
    }

    m_reqitemscount = 0;
    m_reqCreatureOrGOcount = 0;
    m_rewitemscount = 0;
    m_rewchoiceitemscount = 0;

    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
    {
        if (ReqItemId[i])
            ++m_reqitemscount;
        if (ReqCreatureOrGOId[i])
            ++m_reqCreatureOrGOcount;
    }

    for (uint32 i : RewItemId)
    {
        if (i)
            ++m_rewitemscount;
    }

    for (uint32 i : RewChoiceItemId)
    {
        if (i)
            ++m_rewchoiceitemscount;
    }
}

uint32 Quest::XPValue(Player* pPlayer) const
{
    if (pPlayer)
    {
        if (RewXP > 0)
        {
            uint32 pLevel = pPlayer->GetLevel();
            uint32 qLevel = QuestLevel;
            float fullxp = RewXP;

            if (pLevel <= qLevel +  5)
                return uint32(ceilf(fullxp));
            else if (pLevel <= qLevel + 8)
                return uint32(ceilf(fullxp * 0.8f));
            else if (pLevel <= qLevel + 10)
                return uint32(ceilf(fullxp * 0.6f));
            else if (pLevel <= qLevel +  12)
                return uint32(ceilf(fullxp * 0.4f));
            else if (pLevel <= qLevel +  15)
                return uint32(ceilf(fullxp * 0.2f));
            else
                return uint32(ceilf(fullxp * 0.1f));
        }
    }
    return 0;
}

int32  Quest::GetRewOrReqMoney() const
{
    if (RewOrReqMoney <= 0)
        return RewOrReqMoney;

    return int32(RewOrReqMoney * sWorld.getConfig(CONFIG_FLOAT_RATE_DROP_MONEY));
}

int32 Quest::GetRewMoneyMaxLevelAtComplete() const
{
    // World of Warcraft Client Patch 1.10.0 (2006-03-28)
    //  - Quest Experience to Gold Conversion at Level 60
    //    Previously, quest experience was wasted if one completed a quest at
    //    level 60. In this patch, any quests done at maximum level will have
    //    their experience reward converted to a healthy amount of gold, thus
    //    adding additional incentive to completing those quests in your log
    //    once you hit 60.
    if (sWorld.getConfig(CONFIG_BOOL_NO_QUEST_XP_TO_GOLD) && (sWorld.GetWowPatch() < WOW_PATCH_110))
        return 0;

    return int32(GetRewMoneyMaxLevel() * sWorld.getConfig(CONFIG_FLOAT_RATE_DROP_MONEY));
}

bool Quest::IsAllowedInRaid() const
{
    if (Type == QUEST_TYPE_RAID)
        return true;
    if (m_QuestFlags & QUEST_FLAGS_RAID)
        return true;

    return sWorld.getConfig(CONFIG_BOOL_QUEST_IGNORE_RAID);
}

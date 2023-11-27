#include "PartyBotAI.h"
#include "PartyBotAI.h"
#include "custom_raid_strats.h"

enum
{
    MAGMADAR_FRENZY = 19451
};


bool PartyBotAI::partybotIsInRaid(Player* player)
{
    if (player->GetMapId() == 409)
    {
        return true;
    }
        return false;

}

void PartyBotAI::moltenCoreTactics(Player* bot)
{
    if (Unit* pVictim = me->GetVictim())
    {
        // Lava Bomb
       // if (bot->HasAura(19428))
       // {
       //     sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "Has conflag");
       //     bot->GetMotionMaster()->MoveDistance(pVictim,-1.0f);
       // }

        // MAGMADAR
        if (pVictim->GetGUIDLow() == 56683)
        {
            // HUNTER
            if (bot->GetClass() == CLASS_HUNTER)
            {
                if (pVictim->HasAura(MAGMADAR_FRENZY))
                {
                    if (m_spells.hunter.pTranquilizingShot
                        && CanTryToCastSpell(me, m_spells.hunter.pTranquilizingShot))
                    {
                        if (DoCastSpell(pVictim, m_spells.hunter.pTranquilizingShot) == SPELL_CAST_OK)
                        return;
                    }
                }
            }
            // WARRIOR
            if (bot->GetClass() == CLASS_WARRIOR)
            {
                if (me->GetShapeshiftForm() != FORM_BERSERKERSTANCE &&
                    m_spells.warrior.pBerserkerStance &&
                    CanTryToCastSpell(me, m_spells.warrior.pBerserkerStance))
                {
                    DoCastSpell(me, m_spells.warrior.pBerserkerStance);
                    return;
                }
            }
        }
    }
}
#include "scriptPCH.h"
#include "custom.h"
#include "ScriptedAI.h"
#include <ctime>
#include "PlayerBotMgr.h"
#include "Chat.h"
#include <WorldSession.h>
#include <PartyBotAI.h>
#include <AccountMgr.h>
#include "Companions.h"

std::vector<uint32> guids (20);


bool OnGossipHello_CompanionNPC(Player* player, Creature* creature)
{
    uint32 calculatedCosts = getCompanionCosts(player);
    uint32 greetingsText = 21000;
    std::string costText = "Recruit new Companions for " + formatPrice(calculatedCosts);
    const char* recruitText = costText.c_str();
    const char* joinText = "Request Companions to join";
    const char* cancelText = "Cancel Companion contracts";
    player->ADD_GOSSIP_ITEM(5, recruitText, GOSSIP_SENDER_MAIN, 1);
    player->ADD_GOSSIP_ITEM(5, joinText, GOSSIP_SENDER_MAIN, 200);
    player->ADD_GOSSIP_ITEM(5, cancelText, GOSSIP_SENDER_MAIN, 300);
    player->SEND_GOSSIP_MENU(greetingsText, creature->GetGUID());
    return true;
}

void SendDefaultMenu_CompanionNPC(Player* player, Creature* creature, uint32 action)
{
    uint32 calculatedCosts = getCompanionCosts(player);
    uint32 greetingsText = 21000;
    uint32 classText = 21001;
    uint32 specText = 21002;
    uint32 notEnoughMoneyText = 21003;
    uint32 loadCompanionsText = 21004;
    uint32 cancelContractText = 21005;
    uint32 cancelConfirmText = 21006;
    uint32 tooManyCompanionsText = 21007;
    std::string costText = "Recruit new Companions for " + formatPrice(calculatedCosts);
    const char* recruitText = costText.c_str();
    const char* joinText = "Request Companions to join";
    const char* cancelText = "Cancel Companion contracts";
    uint8 companionClass;
    CombatBotRoles companionRoles;
    uint32 playerguid;
    QueryResult* result;
    
    uint8 iterator = 1;
    switch (action)
    {
    case 1:
        player->ADD_GOSSIP_ITEM(0, "Druid", GOSSIP_SENDER_MAIN, 10);
        player->ADD_GOSSIP_ITEM(0, "Priest", GOSSIP_SENDER_MAIN, 20);
        player->ADD_GOSSIP_ITEM(0, "Warrior", GOSSIP_SENDER_MAIN, 30);
        if (player->GetTeam() == HORDE) {
            player->ADD_GOSSIP_ITEM(0, "Shaman", GOSSIP_SENDER_MAIN, 40);
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "Paladin", GOSSIP_SENDER_MAIN, 50);
        }
        player->ADD_GOSSIP_ITEM(0, "Warlock", GOSSIP_SENDER_MAIN, 60);
        player->ADD_GOSSIP_ITEM(0, "Hunter", GOSSIP_SENDER_MAIN, 70);
        player->ADD_GOSSIP_ITEM(0, "Mage", GOSSIP_SENDER_MAIN, 80);
        player->ADD_GOSSIP_ITEM(0, "Rogue", GOSSIP_SENDER_MAIN, 90);
        player->ADD_GOSSIP_ITEM(5, "<-[back]", GOSSIP_SENDER_MAIN, 100);
        player->SEND_GOSSIP_MENU(classText, creature->GetGUID());
        break;
        // Classes
        // Druid
    case 10:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Tank", GOSSIP_SENDER_MAIN, 11);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Healer", GOSSIP_SENDER_MAIN, 12);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Ranged DPS", GOSSIP_SENDER_MAIN, 13);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Meele DPS", GOSSIP_SENDER_MAIN, 14);
        player->ADD_GOSSIP_ITEM(5, "<-[back]", GOSSIP_SENDER_MAIN, 1);
        player->SEND_GOSSIP_MENU(specText, creature->GetGUID());
        break;
        // Priest
    case 20:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Healer", GOSSIP_SENDER_MAIN, 21);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Ranged DPS", GOSSIP_SENDER_MAIN, 22);
        player->ADD_GOSSIP_ITEM(5, "<-[back]", GOSSIP_SENDER_MAIN, 1);
        player->SEND_GOSSIP_MENU(specText, creature->GetGUID());
        break;
        // Warrior
    case 30:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Tank", GOSSIP_SENDER_MAIN, 31);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Meele DPS", GOSSIP_SENDER_MAIN, 32);
        player->ADD_GOSSIP_ITEM(5, "<-[back]", GOSSIP_SENDER_MAIN, 1);
        player->SEND_GOSSIP_MENU(specText, creature->GetGUID());
        break;
        // Shaman
    case 40:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Healer", GOSSIP_SENDER_MAIN, 41);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Ranged DPS", GOSSIP_SENDER_MAIN, 42);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Meele DPS", GOSSIP_SENDER_MAIN, 43);
        player->ADD_GOSSIP_ITEM(5, "<-[back]", GOSSIP_SENDER_MAIN, 1);
        player->SEND_GOSSIP_MENU(specText, creature->GetGUID());
        break;
        // Paladin
    case 50:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Tank", GOSSIP_SENDER_MAIN, 51);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Healer", GOSSIP_SENDER_MAIN, 52);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Meele DPS", GOSSIP_SENDER_MAIN, 53);
        player->ADD_GOSSIP_ITEM(5, "<-[back]", GOSSIP_SENDER_MAIN, 1);
        player->SEND_GOSSIP_MENU(specText, creature->GetGUID());
        break;
        // Warlock
    case 60:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Ranged DPS", GOSSIP_SENDER_MAIN, 61);
        player->ADD_GOSSIP_ITEM(5, "<-[back]", GOSSIP_SENDER_MAIN, 1);
        player->SEND_GOSSIP_MENU(specText, creature->GetGUID());
        break;
        // Hunter
    case 70:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Ranged DPS", GOSSIP_SENDER_MAIN, 71);
        player->ADD_GOSSIP_ITEM(5, "<-[back]", GOSSIP_SENDER_MAIN, 1);
        player->SEND_GOSSIP_MENU(specText, creature->GetGUID());
        break;
        // Mage
    case 80:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Ranged DPS", GOSSIP_SENDER_MAIN, 81);
        player->ADD_GOSSIP_ITEM(5, "<-[back]", GOSSIP_SENDER_MAIN, 1);
        player->SEND_GOSSIP_MENU(specText, creature->GetGUID());
        break;
        // Rogue
    case 90:
        player->ADD_GOSSIP_ITEM(0, "Meele DPS", GOSSIP_SENDER_MAIN, 91);
        player->ADD_GOSSIP_ITEM(5, "<-[back]", GOSSIP_SENDER_MAIN, 1);
        player->SEND_GOSSIP_MENU(specText, creature->GetGUID());
        break;
    case 11:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_DRUID;
            companionRoles = ROLE_TANK;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 12:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_DRUID;
            companionRoles = ROLE_HEALER;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 13:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_DRUID;
            companionRoles = ROLE_RANGE_DPS;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 14:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_DRUID;
            companionRoles = ROLE_MELEE_DPS;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 21:

        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_PRIEST;
            companionRoles = ROLE_HEALER;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 22:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_PRIEST;
            companionRoles = ROLE_RANGE_DPS;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 31:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_WARRIOR;
            companionRoles = ROLE_TANK;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 32:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_WARRIOR;
            companionRoles = ROLE_MELEE_DPS;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 41:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_SHAMAN;
            companionRoles = ROLE_HEALER;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 42:

        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_SHAMAN;
            companionRoles = ROLE_RANGE_DPS;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 43:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_SHAMAN;
            companionRoles = ROLE_MELEE_DPS;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 51:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_PALADIN;
            companionRoles = ROLE_TANK;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 52:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            
            companionClass = CLASS_PALADIN;
            companionRoles = ROLE_HEALER;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 53:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_PALADIN;
            companionRoles = ROLE_MELEE_DPS;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 61:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_WARLOCK;
            companionRoles = ROLE_RANGE_DPS;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        }
    case 71:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_HUNTER;
            companionRoles = ROLE_RANGE_DPS;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        };
    case 81:
        if (player->GetMoney() > calculatedCosts)
        {
            player->CLOSE_GOSSIP_MENU();
            companionClass = CLASS_MAGE;
            companionRoles = ROLE_RANGE_DPS;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        };
    case 91:
        if (player->GetMoney() > calculatedCosts)
        {
            
            companionClass = CLASS_ROGUE;
            companionRoles = ROLE_MELEE_DPS;
            if (!buyNewCompanionCheck(player, companionRoles, companionClass, calculatedCosts))
            {
                player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
                player->SEND_GOSSIP_MENU(tooManyCompanionsText, creature->GetGUID());
            }
            player->CLOSE_GOSSIP_MENU();
            break;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 100);
            player->SEND_GOSSIP_MENU(notEnoughMoneyText, creature->GetGUID());
            break;
        };
    case 100:
        player->ADD_GOSSIP_ITEM(5, recruitText, GOSSIP_SENDER_MAIN, 1);
        player->ADD_GOSSIP_ITEM(5, joinText, GOSSIP_SENDER_MAIN, 200);
        player->ADD_GOSSIP_ITEM(5, cancelText, GOSSIP_SENDER_MAIN, 300);
        player->SEND_GOSSIP_MENU(greetingsText, creature->GetGUID());
        break;

        // Load companion cases
    case 200:
        playerguid = player->GetGUIDLow();
        result = CharacterDatabase.PQuery("SELECT comp.guid, comp.name, comp.class, cc.role, comp.level FROM characters_companions cc INNER JOIN characters c ON c.guid = cc.characters_guid INNER JOIN characters comp ON comp.guid = cc.companion_characters_guid WHERE c.guid = %u", playerguid);
        if (result)
        {
            do
            {
                uint32 compGuid;
                uint8 compClassInt, compRoleInt, compLevel;

                std::string name, compClass, compRole;
                Field* cc = result->Fetch();
                compGuid = cc[0].GetUInt32();
                name = cc[1].GetString();
                compClassInt = cc[2].GetUInt8();
                compRoleInt = cc[3].GetUInt8();
                compLevel = cc[4].GetUInt8();

                switch (compClassInt)
                {
                case 1:
                    compClass = "Warrior";
                    break;
                case 2:
                    compClass = "Paladin";
                    break;
                case 3:
                    compClass = "Hunter";
                    break;
                case 4:
                    compClass = "Rogue";
                    break;
                case 5:
                    compClass = "Priest";
                    break;
                case 7:
                    compClass = "Shaman";
                    break;
                case 8:
                    compClass = "Mage";
                    break;
                case 9:
                    compClass = "Warlock";
                    break;
                case 11:
                    compClass = "Druid";
                    break;
                }
                switch (compRoleInt)
                {
                case 1:
                    compRole = "Meele DPS";
                    break;
                case 2:
                    compRole = "Range DPS";
                    break;
                case 3:
                    compRole = "Tank";
                    break;
                case 4:
                    compRole = "Healer";
                    break;
                }

                std::string concatString = name + " - Level " + std::to_string(compLevel) + " " + compClass + " - " + compRole;
                const char* CompanionString = concatString.c_str();
                player->ADD_GOSSIP_ITEM(0, CompanionString, GOSSIP_SENDER_MAIN, 200 + iterator);
                //sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "GUID %u: %u", iterator, compGuid);
                guids[iterator - 1] = compGuid;

                iterator++;
            } while (result->NextRow());
        }

        delete result;
        player->SEND_GOSSIP_MENU(loadCompanionsText, creature->GetGUID());
        break;
    case 201:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[0]);
        break;

    case 202:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[1]);
        break;
    case 203:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[2]);
        break;
    case 204:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[3]);
        break;
    case 205:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[4]);
        break;
    case 206:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[5]);
        break;
    case 207:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[6]);
        break;
    case 208:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[7]);
        break;
    case 209:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[8]);
        break;
    case 210:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[9]);
        break;
    case 211:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[10]);
        break;
    case 212:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[11]);
        break;
    case 213:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[12]);
        break;
    case 214:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[13]);
        break;
    case 215:
        player->CLOSE_GOSSIP_MENU();
        loadBotFromMenu(player, guids[14]);
        break;
        // Cancel Contract cases
    case 300:
        playerguid = player->GetGUIDLow();
        result = CharacterDatabase.PQuery("SELECT comp.guid, comp.name, comp.class, cc.role, comp.level FROM characters_companions cc INNER JOIN characters c ON c.guid = cc.characters_guid INNER JOIN characters comp ON comp.guid = cc.companion_characters_guid WHERE c.guid = %u", playerguid);
        if (result)
        {
            do
            {
                uint32 compGuid;
                uint8 compClassInt, compRoleInt, compLevel;

                std::string name, compClass, compRole;
                Field* cc = result->Fetch();
                compGuid = cc[0].GetUInt32();
                name = cc[1].GetString();
                compClassInt = cc[2].GetUInt8();
                compRoleInt = cc[3].GetUInt8();
                compLevel = cc[4].GetUInt8();

                switch (compClassInt)
                {
                case 1:
                    compClass = "Warrior";
                    break;
                case 2:
                    compClass = "Paladin";
                    break;
                case 3:
                    compClass = "Hunter";
                    break;
                case 4:
                    compClass = "Rogue";
                    break;
                case 5:
                    compClass = "Priest";
                    break;
                case 7:
                    compClass = "Shaman";
                    break;
                case 8:
                    compClass = "Mage";
                    break;
                case 9:
                    compClass = "Warlock";
                    break;
                case 11:
                    compClass = "Druid";
                    break;
                }
                switch (compRoleInt)
                {
                case 1:
                    compRole = "Meele DPS";
                    break;
                case 2:
                    compRole = "Range DPS";
                    break;
                case 3:
                    compRole = "Tank";
                    break;
                case 4:
                    compRole = "Healer";
                    break;
                }

                std::string concatString = name + " - Level " + std::to_string(compLevel) + " " + compClass + " - " + compRole;
                const char* CompanionString = concatString.c_str();
                player->ADD_GOSSIP_ITEM(0, CompanionString, GOSSIP_SENDER_MAIN, 300 + iterator);
                //sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "GUID %u: %u", iterator, compGuid);
                guids[iterator - 1] = compGuid;

                iterator++;
            } while (result->NextRow());
        }

        delete result;
        player->SEND_GOSSIP_MENU(cancelContractText, creature->GetGUID());
        break;
    case 301:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 302:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 303:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 304:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 305:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 306:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 307:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 308:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 309:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 310:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 311:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 312:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 313:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 314:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 315:
        player->ADD_GOSSIP_ITEM(0, "Yes, I want to let him go!", GOSSIP_SENDER_MAIN, action + 15);
        player->SEND_GOSSIP_MENU(cancelConfirmText, creature->GetGUID());
        break;
    case 316:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[0]);
        break;
    case 317:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[1]);
        break;
    case 318:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[2]);
        break;
    case 319:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[3]);
        break;
    case 320:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[4]);
        break;
    case 321:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[5]);
        break;
    case 322:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[6]);
        break;
    case 323:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[7]);
        break;
    case 324:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[8]);
        break;
    case 325:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[9]);
        break;
    case 326:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[10]);
        break;
    case 327:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[11]);
        break;
    case 328:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[12]);
        break;
    case 329:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[13]);
        break;
    case 330:
        player->CLOSE_GOSSIP_MENU();
        cancelCompanionContract(player, guids[14]);
        break;
    }
}
void CancelCompanionContractMenu_CompanionNPC(Player* player, Creature* creature, uint32 action)
{
    uint32 playerguid;
    QueryResult* result;

    switch (action)
    {
        playerguid = player->GetGUIDLow();
        result = CharacterDatabase.PQuery("SELECT comp.guid, comp.name, comp.class, cc.role, comp.level FROM characters_companions cc INNER JOIN characters c ON c.guid = cc.characters_guid INNER JOIN characters comp ON comp.guid = cc.companion_characters_guid WHERE c.guid = %u", playerguid);
        player->ADD_GOSSIP_ITEM(0, "I will come back later", GOSSIP_SENDER_MAIN, 1);
        player->SEND_GOSSIP_MENU(0, creature->GetGUID());
    }
}


bool GossipSelect_CompanionNPC(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    // Main menu
    if (sender == GOSSIP_SENDER_MAIN)
        SendDefaultMenu_CompanionNPC(player, creature, action);
    return true;
}

void AddSC_custom_companions()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "npc_companions";
    newscript->pGossipHello = &OnGossipHello_CompanionNPC;
    newscript->pGossipSelect = &GossipSelect_CompanionNPC;
    newscript->RegisterSelf(false);
}
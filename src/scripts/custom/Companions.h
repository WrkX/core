#ifndef DEF_Companions_H
#define DEF_Companions_H

#include "PlayerBotMgr.h"
#include "Chat.h"
#include <WorldSession.h>
#include <PartyBotAI.h>
#include <AccountMgr.h>
#include "ObjectMgr.h"

class WorldSession;
class Player;


bool loadBotFromMenu(Player* player, uint32 ccGuid);
bool loadExistingBot(Player* player, CombatBotRoles companionRole, uint8 companionClass);
bool cancelCompanionContract(Player* player, uint32 companionGuid);
bool buyNewCompanionCheck(Player* player, CombatBotRoles companionRole, uint8 companionClass, uint32 companionCosts);
void findExistingCompanion(const uint32& ccGuid, const uint8& ccClass, CombatBotRoles role, Player* player);
bool createNewCompanionAccount(uint8 botClass, Player* pPlayer, uint32 playerCharGuid, CombatBotRoles role);
std::string formatPrice(uint32 copper);
uint32 getCompanionCosts(Player* player);
bool loadCompanion(std::string companionname, bool newCompanion, CombatBotRoles role, Player* player);
bool createBotCharacter(uint8 botClass, Player* pPlayer, uint32 accountid, uint32 playerCharGuid, CombatBotRoles botRole);
uint8 SelectRandomRaceForClassCompanion(uint8 playerClass, Team playerTeam);
std::string CreateRandomBotName(uint8 gender);
bool canGetNewCompanion(uint32 charguid);

#endif
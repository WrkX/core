#include "Chat.h"
#include "PartyBotAI.h"
#include "Chat.h"
#include "Companions.h"
#include "Bag.h"
#include "PlayerBotMgr.h"

WorldSession* m_session;


bool loadBotFromMenu(Player* player,uint32 ccGuid)
{
    QueryResult* botresult = CharacterDatabase.PQuery("SELECT name, role FROM characters c inner join characters_companions cc on c.guid = cc.companion_characters_guid WHERE guid = %u", ccGuid);
    Field* bot = botresult->Fetch();
    std::string ccName = bot[0].GetString();
    uint8 ccRole = bot[1].GetUInt8();
    CombatBotRoles role = static_cast<CombatBotRoles>(ccRole);

    loadCompanion(ccName, false, role, player);
    return true;
}

bool cancelCompanionContract(Player* pPlayer, uint32 companionGuid)
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT account from characters where guid = %u", companionGuid);
    Field* toDeletePlayer = result->Fetch();
    uint32 toDeletePlayerAccountId = toDeletePlayer[0].GetUInt32();
    CharacterDatabase.BeginTransaction();
    CharacterDatabase.PExecute("DELETE FROM characters_companions WHERE characters_guid = %u AND companion_characters_guid = %u",
        pPlayer->GetGUIDLow(),
        companionGuid);
    

    WorldSession* session = new WorldSession(toDeletePlayerAccountId, nullptr, sAccountMgr.GetSecurity(toDeletePlayerAccountId), 0, LOCALE_enUS);

    Player* player = new Player(session);
    player->DeleteFromDB(companionGuid, toDeletePlayerAccountId);
    CharacterDatabase.CommitTransaction();
    delete player;
    delete session;
    return true;
}

bool buyNewCompanionCheck(Player* player, CombatBotRoles companionRole, uint8 companionClass, uint32 companionCosts)
{
    const char* playername = player->GetName();

    uint32 charGuid = sObjectMgr.GetPlayerGuidByName(playername);

    if (!charGuid)
    {
        return false;
    }
    
    if (canGetNewCompanion(charGuid))
    {
        player->ModifyMoney(-companionCosts);
        createNewCompanionAccount(companionClass, player, charGuid, companionRole);
        return true;
    }
    else
    {
        WorldSession* session = player->GetSession();
        session->SendAreaTriggerMessage("You already have too many companions, cancel their contracts if you want more!");
        return false;
    }
    
    }

bool createNewCompanionAccount(uint8 botClass, Player* pPlayer, uint32 playerCharGuid, CombatBotRoles role)
{
    QueryResult* result = LoginDatabase.Query("SELECT max(id)+1 FROM account");
    Field* field = result->Fetch();
    uint32 accid = field[0].GetUInt32();

    std::string accountData = "Companion";
    accountData += std::to_string(accid);
    AccountOpResult accountCreation = sAccountMgr.CreateAccount(accountData, accountData);

    if (accountCreation == AOR_OK)
    {
        sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "Account: %s was created", accountData);
        createBotCharacter(botClass, pPlayer, accid, playerCharGuid, role);
        return true;
    }
    else
    {
        return false;
    }
}

uint32 getCompanionCosts(Player* player)
{
    uint8 playerLevel = player->GetLevel();
    if (playerLevel >= 10 && playerLevel < 40)
        return 2000 + ((playerLevel * playerLevel) * 2);
    else if (playerLevel >= 40 && playerLevel < 50)
        return 8000 + ((playerLevel * playerLevel) * 4);
    else if (playerLevel >= 50 && playerLevel < 55)
        return 80000 + ((playerLevel * playerLevel) * 10);
    else if (playerLevel >= 55 && playerLevel < 59)
        return 200000 + ((playerLevel * playerLevel) * 15);
    else
        return 310000 + ((playerLevel * playerLevel) * 25);
}

std::string formatPrice(uint32 copper) 
{
    std::ostringstream out;
    if (!copper)
    {
        out << "0";
        return out.str();
    }

    uint32 gold = uint32(copper / 10000);
    copper -= (gold * 10000);
    uint32 silver = uint32(copper / 100);
    copper -= (silver * 100);

    bool space = false;
    if (gold > 0)
    {
        out << gold << "g";
        space = true;
    }

    if (silver > 0 && gold < 50)
    {
        if (space) out << " ";
        out << silver << "s";
        space = true;
    }

    if (copper > 0 && gold < 10)
    {
        if (space) out << " ";
        out << copper << "c";
    }

    return out.str();
}

bool loadCompanion(std::string companionname, bool newCompanion, CombatBotRoles role, Player* pPlayer)
{
    PartyBotAI* pAI;

    if (!pPlayer)
        return false;
    uint8 playerLevel = pPlayer->GetLevel();
    std::string name = companionname;

    if (name.empty())
    {

        return false;
    }

    ObjectGuid guid = sObjectMgr.GetPlayerGuidByName(name).GetCounter();
    if (!guid)
    {

        return false;
    }

    if (sObjectAccessor.FindPlayerNotInWorld(guid))
    {

        return false;
    }

    float x, y, z;
    pPlayer->GetNearPoint(pPlayer, x, y, z, 0, 5.0f, frand(0.0f, 6.0f));
    if (newCompanion == false)
        pAI = new PartyBotAI(pPlayer, pPlayer->GetMapId(), role, pPlayer->GetMap()->GetInstanceId(), x, y, z, pPlayer->GetOrientation());
    else
        pAI = new PartyBotAI(pPlayer, pPlayer->GetMapId(), role, playerLevel, pPlayer->GetMap()->GetInstanceId(), x, y, z, pPlayer->GetOrientation());

    if (!sPlayerBotMgr.AddBot(guid, false, pAI))
    {
        delete pAI;

        return false;
    }
    return true;
}

bool createBotCharacter(uint8 botClass, Player* pPlayer, uint32 accountid, uint32 playerCharGuid, CombatBotRoles botRole)
{
    Team faction = pPlayer->GetTeam();
    uint8 race = SelectRandomRaceForClassCompanion(botClass, faction);
    uint8 gender = rand() % 2;
    std::string name = CreateRandomBotName(gender);
    WorldSession* session = new WorldSession(accountid, nullptr, sAccountMgr.GetSecurity(accountid), 0, LOCALE_enUS);

    Player* player = new Player(session);

    uint32 maxguid = sObjectMgr.GeneratePlayerLowGuid();
    sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "maxguid %u", maxguid);

    if (!player || !session)
    {
        sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "BOTS: Unable to create session or player for random acc %d - name: \"%s\"; race: %u; class: %u", accountid, name.c_str(), race, botClass);
        return false;
    }

    if (!player->SaveNewPlayer(session, maxguid, name, race, botClass, gender, 0, 0, 0, 0, 0))
    {
        player->DeleteFromDB(player->GetObjectGuid(), accountid, true, true);
        delete session;
        delete player;
        sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "Unable to create random bot for account %d - name: \"%s\"; race: %u; class: %u",
            accountid, name.c_str(), race, botClass);
        return false;
    }

    sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "Random bot created for account %d - name: \"%s\"; race: %u; class: %u",
        accountid, name.c_str(), race, botClass);

    uint32 roleInt = static_cast<uint32>(botRole);

    CharacterDatabase.PExecute("INSERT INTO characters_companions (characters_guid, companion_characters_guid,class, role) VALUES (%u, %u, %u, %u)",
        playerCharGuid,
        maxguid,
        botClass,
        roleInt);

    char* companionName = &*name.begin();
    loadCompanion(companionName, true, botRole, pPlayer);
    return true;
}



uint8 SelectRandomRaceForClassCompanion(uint8 playerClass, Team playerTeam)
{
    std::vector<uint32> validRaces;
    for (uint32 raceId = 1; raceId < MAX_RACES; ++raceId)
    {
        if (playerTeam == ALLIANCE)
        {
            if (!((1 << (raceId - 1)) & RACEMASK_ALLIANCE))
                continue;
        }
        else if (playerTeam == HORDE)
        {
            if (!((1 << (raceId - 1)) & RACEMASK_HORDE))
                continue;
        }

        if (sObjectMgr.GetPlayerInfo(raceId, playerClass))
            validRaces.push_back(raceId);
    }

    if (validRaces.empty())
        return 0;

    return SelectRandomContainerElement(validRaces);
}

std::string CreateRandomBotName(uint8 gender)
{
    QueryResult* result = CharacterDatabase.Query("SELECT MAX(name_id) FROM companion_names");
    if (!result)
    {
        sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "No more names left for random bots");
        return "";
    }

    Field* fields = result->Fetch();
    uint32 maxId = fields[0].GetUInt32();
    delete result;

    result = CharacterDatabase.PQuery("SELECT n.name FROM companion_names n LEFT OUTER JOIN characters e ON e.name = n.name WHERE e.guid IS NULL and n.gender = '%u' order by rand() limit 1", gender);
    if (!result)
    {
        sLog.Out(LOG_BASIC, LOG_LVL_ERROR, ("No more names left for random bots"));
        return "";
    }

    fields = result->Fetch();
    std::string cname = fields[0].GetString();
    delete result;
    return cname;
}

bool canGetNewCompanion(uint32 charguid)
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM characters_companions WHERE characters_guid = %u", charguid);
    Field* field = result->Fetch();
    uint32 companionAmount = field[0].GetUInt32();

    if (companionAmount < 15)
        return true;
    return false;
}

bool ChatHandler::IsTargetCompanion(Player* pTarget)
{
    if (!pTarget)
    {
        SendSysMessage("No target selected.");
        SetSentErrorMessage(true);
        return false;
    }

    if (!pTarget->AI())
    {
        SendSysMessage("Target is no Companion.");
        SetSentErrorMessage(true);
        return false;
    }
    return true;
}

bool isPlayerBotOwner(Player* pPlayer, Player* bot = nullptr, uint32 botGuid = 0)
{
    uint32 playerguid = pPlayer->GetGUIDLow();
    if (botGuid == 0)
        botGuid = bot->GetGUIDLow();
    QueryResult* result = CharacterDatabase.PQuery("SELECT characters_guid FROM characters_companions WHERE characters_guid = %u AND companion_characters_guid = %u", playerguid, botGuid);
    if (result)
        return true;
    return false;
}

void PartyBotAI::findItemsInInv(std::list<uint32>& itemIdSearchList, std::list<Item*>& foundItemList) const
{

    // look for items in main bag
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; itemIdSearchList.size() > 0 && slot < INVENTORY_SLOT_ITEM_END; ++slot)
    {
        Item* const pItem = me->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        for (std::list<uint32>::iterator it = itemIdSearchList.begin(); it != itemIdSearchList.end(); ++it)
        {
            if (pItem->GetProto()->ItemId != *it)
                continue;

            if (me->GetTrader() && me->GetTradeData()->HasItem(pItem->GetObjectGuid()))
                continue;

            foundItemList.push_back(pItem);
            itemIdSearchList.erase(it);
            break;
        }
    }

    // for all for items in other bags
    for (uint8 bag = INVENTORY_SLOT_BAG_START; itemIdSearchList.size() > 0 && bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        Bag* const pBag = (Bag*)me->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (!pBag)
            continue;

        for (uint8 slot = 0; itemIdSearchList.size() > 0 && slot < pBag->GetBagSize(); ++slot)
        {
            Item* const pItem = me->GetItemByPos(bag, slot);
            if (!pItem)
                continue;

            for (std::list<uint32>::iterator it = itemIdSearchList.begin(); it != itemIdSearchList.end(); ++it)
            {
                if (pItem->GetProto()->ItemId != *it)
                    continue;

                if (me->GetTrader() && me->GetTradeData()->HasItem(pItem->GetObjectGuid()))
                    continue;

                foundItemList.push_back(pItem);
                itemIdSearchList.erase(it);
                break;
            }
        }
    }
}

void PartyBotAI::EquipItem(Item* src_Item)
{
    uint8 src_bagIndex = src_Item->GetBagSlot();
    uint8 src_slot = src_Item->GetSlot();

    //DEBUG_LOG("PlayerbotAI::EquipItem: %s in srcbag = %u, srcslot = %u", src_Item->GetProto()->Name1, src_bagIndex, src_slot);

    uint16 dest;
    InventoryResult msg = me->CanEquipItem(NULL_SLOT, dest, src_Item, !src_Item->IsBag());
    if (msg != EQUIP_ERR_OK)
    {
        me->SendEquipError(msg, src_Item, nullptr);
        return;
    }

    uint16 src = src_Item->GetPos();
    if (dest == src)                                        // prevent equip in same slot, only at cheat
        return;

    Item* dest_Item = me->GetItemByPos(dest);
    if (!dest_Item)                                          // empty slot, simple case
    {
        me->RemoveItem(src_bagIndex, src_slot, true);
        me->EquipItem(dest, src_Item, true);
        me->AutoUnequipOffhandIfNeed();
    }
    else                                                    // have currently equipped item, not simple case
    {
        uint8 dest_bagIndex = dest_Item->GetBagSlot();
        uint8 dest_slot = dest_Item->GetSlot();

        msg = me->CanUnequipItem(dest, false);
        if (msg != EQUIP_ERR_OK)
        {
            me->SendEquipError(msg, dest_Item, nullptr);
            return;
        }

        // check dest->src move possibility
        ItemPosCountVec sSrc;
        if (me->IsInventoryPos(src))
        {
            msg = me->CanStoreItem(src_bagIndex, src_slot, sSrc, dest_Item, true);
            if (msg != EQUIP_ERR_OK)
                msg = me->CanStoreItem(src_bagIndex, NULL_SLOT, sSrc, dest_Item, true);
            if (msg != EQUIP_ERR_OK)
                msg = me->CanStoreItem(NULL_BAG, NULL_SLOT, sSrc, dest_Item, true);
        }

        if (msg != EQUIP_ERR_OK)
        {
            me->SendEquipError(msg, dest_Item, src_Item);
            return;
        }

        // now do moves, remove...
        me->RemoveItem(dest_bagIndex, dest_slot, false);
        me->RemoveItem(src_bagIndex, src_slot, false);

        // add to dest
        me->EquipItem(dest, src_Item, true);

        // add to src
        if (me->IsInventoryPos(src))
            me->StoreItem(sSrc, dest_Item, true);

        me->AutoUnequipOffhandIfNeed();
    }
}

bool ChatHandler::HandleCompanionEquipCommand(char* args)
{
    if (!args)
        return false;

    char* cId = ExtractKeyFromLink(&args, "Hitem");

    if (!cId)
        return false;

    uint32 itemId = 0;
    if (!ExtractUInt32(&cId, itemId))
    {
        std::string itemName = cId;
        WorldDatabase.escape_string(itemName);
        QueryResult* result = WorldDatabase.PQuery("SELECT `entry` FROM `item_template` WHERE `name` = '%s'", itemName.c_str());
        if (!result)
        {
            PSendSysMessage("Item not found in db.");
            SetSentErrorMessage(true);
            return false;
        }
        itemId = result->Fetch()->GetUInt16();
        delete result;
    }
    Player* pPlayer = GetSession()->GetPlayer();
    Player* pTarget = GetSelectedPlayer();
    if (!IsTargetCompanion(pTarget))
        return false;
    
    if (!isPlayerBotOwner(pPlayer, pTarget))
    {
        SendSysMessage("Your are not the Bots owner.");
        return false;
    }
        

    if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(pTarget->AI()))
    {
        std::list<uint32> itemIds;
        std::list<Item*> itemList;

        itemIds.push_back(itemId);
        pAI->findItemsInInv(itemIds, itemList);
        for (std::list<Item*>::iterator it = itemList.begin(); it != itemList.end(); ++it)
            pAI->EquipItem(*it);
    }
    return false;
}
   
void PartyBotAI::SendCompanionEquipList()
{
    // find all unequipped items and put them in
    // a vector of dynamically created lists where the vector index is from 0-18
    // and the list contains Item* that can be equipped to that slot
    // Note: each dynamically created list in the vector must be deleted at end
    // so NO EARLY RETURNS!
    // see enum EquipmentSlots in Player.h to see what equipment slot each index in vector
    // is assigned to. (The first is EQUIPMENT_SLOT_HEAD=0, and last is EQUIPMENT_SLOT_TABARD=18)
    std::list<Item*>* equip[19];
    for (uint8 i = 0; i < 19; ++i)
        equip[i] = nullptr;

    // list out items in main backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        Item* const pItem = me->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!pItem)
            continue;

        uint16 dest;
        uint8 msg = me->CanEquipItem(NULL_SLOT, dest, pItem, !pItem->IsBag());
        if (msg != EQUIP_ERR_OK)
            continue;

        // the dest looks like it includes the old loc in the 8 higher bits
        // so casting it to a uint8 strips them
        int8 equipSlot = uint8(dest);
        if (!(equipSlot >= 0 && equipSlot < 19))
            continue;

        // create a list if one doesn't already exist
        if (equip[equipSlot] == nullptr)
            equip[equipSlot] = new std::list<Item*>;

        std::list<Item*>* itemListForEqSlot = equip[equipSlot];
        itemListForEqSlot->push_back(pItem);
    }

    // list out items in other removable backpacks
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*)me->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = me->GetItemByPos(bag, slot);
                if (!pItem)
                    continue;

                uint16 dest;
                uint8 msg = me->CanEquipItem(NULL_SLOT, dest, pItem, !pItem->IsBag());
                if (msg != EQUIP_ERR_OK)
                    continue;

                int8 equipSlot = uint8(dest);
                if (!(equipSlot >= 0 && equipSlot < 19))
                    continue;

                // create a list if one doesn't already exist
                if (equip[equipSlot] == nullptr)
                    equip[equipSlot] = new std::list<Item*>;

                std::list<Item*>* itemListForEqSlot = equip[equipSlot];
                itemListForEqSlot->push_back(pItem);
            }
    }

    ChatHandler ch(GetPartyLeader());

    const std::string descr[] = { "head", "neck", "shoulders", "body", "chest",
                                  "waist", "legs", "feet", "wrists", "hands", "finger1", "finger2",
                                  "trinket1", "trinket2", "back", "mainhand", "offhand", "ranged",
                                  "tabard"
    };

    // now send client all items that can be equipped by slot
    for (uint8 equipSlot = 0; equipSlot < 19; ++equipSlot)
    {
        if (equip[equipSlot] == nullptr)
            continue;
        std::list<Item*>* itemListForEqSlot = equip[equipSlot];
        std::ostringstream out;
        out << descr[equipSlot] << ": ";
        for (std::list<Item*>::iterator it = itemListForEqSlot->begin(); it != itemListForEqSlot->end(); ++it)
        {
            const ItemPrototype* const pItemProto = (*it)->GetProto();

            std::string itemName = pItemProto->Name1;

            out << " |cffffffff|Hitem:" << pItemProto->ItemId
                << ":0:0:0:0:0:0:0" << "|h[" << itemName
                << "]|h|r";
        }
        ch.SendSysMessage(out.str().c_str());

        delete itemListForEqSlot; // delete list of Item*
    }
}

bool ChatHandler::HandleCompanionListEquipCommand(char* args)
{
    Player* pTarget = GetSelectedPlayer();
    if (!IsTargetCompanion(pTarget))
        return false;

    if (!isPlayerBotOwner(GetSession()->GetPlayer(), pTarget))
    {
        SendSysMessage("Your are not the Bots owner.");
        return false;
    }
    if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(pTarget->AI()))
    {
        pAI->SendCompanionEquipList();
        return true;
    }
    return false;
}

Item* PartyBotAI::FindItem(uint32 ItemId)
{
    // list out items equipped & in main backpack
    //INVENTORY_SLOT_ITEM_START = 23
    //INVENTORY_SLOT_ITEM_END = 39

    for (uint8 slot = EQUIPMENT_SLOT_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        // DEBUG_LOG ("[PlayerbotAI]: FindItem - [%s's]backpack slot = %u",m_bot->GetName(),slot); // 23 to 38 = 16
        Item* const pItem = me->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);  // 255, 23 to 38
        if (pItem)
        {
            const ItemPrototype* const pItemProto = pItem->GetProto();
            if (!pItemProto)
                continue;

            if (pItemProto->ItemId == ItemId)   // have required item
                return pItem;
        }
    }
    // list out items in other removable backpacks
    //INVENTORY_SLOT_BAG_START = 19
    //INVENTORY_SLOT_BAG_END = 23

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)  // 20 to 23 = 4
    {
        const Bag* const pBag = (Bag*)me->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);    // 255, 20 to 23
        if (pBag)
            for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
            {
                Item* const pItem = me->GetItemByPos(bag, slot); // 20 to 23, 1 to bagsize
                if (pItem)
                {
                    const ItemPrototype* const pItemProto = pItem->GetProto();
                    if (!pItemProto)
                        continue;

                    if (pItemProto->ItemId == ItemId)        // have required item
                        return pItem;
                }
            }
    }
    return nullptr;
}

bool ChatHandler::HandleCompanionDeleteItem(char* args)
{
    if (!args)
        return false;

    char* cId = ExtractKeyFromLink(&args, "Hitem");

    if (!cId)
        return false;
    
    

    uint32 itemId = 0;
    if (!ExtractUInt32(&cId, itemId))
    {
        std::string itemName = cId;
        WorldDatabase.escape_string(itemName);
        QueryResult* result = WorldDatabase.PQuery("SELECT `entry` FROM `item_template` WHERE `name` = '%s'", itemName.c_str());
        if (!result)
        {
            PSendSysMessage("Item not found in db.");
            SetSentErrorMessage(true);
            return false;
        }
        itemId = result->Fetch()->GetUInt16();
        delete result;
    }
    Player* pPlayer = GetSession()->GetPlayer();
    Player* pTarget = GetSelectedPlayer();
    if (!IsTargetCompanion(pTarget))
        return false;

    if (!isPlayerBotOwner(pPlayer, pTarget))
    {
        SendSysMessage("Your are not the Bots owner.");
        return false;
    }

    if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(pTarget->AI()))
    {

        Item* pItem = pAI->FindItem(itemId);
        if (pItem)
        {
            pTarget->DestroyItemCount(itemId, 1, true, false, true);
            pTarget->SaveInventoryAndGoldToDB();
            return true;
        }
        return false;
    }
    return false;
}

bool ChatHandler::HandleCompanionDeleteEverything(char* args)
{
    Player* pTarget = GetSelectedPlayer();
    if (!IsTargetCompanion(pTarget))
        return false;

    if (!isPlayerBotOwner(GetSession()->GetPlayer(), pTarget))
    {
        SendSysMessage("Your are not the Bots owner.");
        return false;
    }

    if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(pTarget->AI()))
    {

        for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
        {
            Item* const item = pTarget->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            if (item)
            {
                pTarget->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
                pTarget->SaveInventoryAndGoldToDB();
            }
                
        }

        for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
        {
            
            const Bag* const pBag = static_cast<Bag*>(pTarget->GetItemByPos(INVENTORY_SLOT_BAG_0, bag));
            if (pBag)
            {
                for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                {
                    Item* const item = pTarget->GetItemByPos(bag, slot);
                    if (item)
                    {
                        pTarget->DestroyItem(bag, slot, true);
                        pTarget->SaveInventoryAndGoldToDB();
                    }
                }
            }
        }
        
    }
    pTarget->SaveInventoryAndGoldToDB();
    return true;
}

bool CompanionComeToMeHelper(Player* pBot, Player* pPlayer, uint8 mode)
{
    ChatHandler ch(pPlayer);
    if (pBot->AI() && pBot->IsAlive() && pBot->IsInMap(pPlayer) && !pBot->HasUnitState(UNIT_STAT_NO_FREE_MOVE))
    {
        if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(pBot->AI()))
        {
            // mode 0 = all, 1 all but tanks, 2 all ranged + healer
            if (mode == 1)
            {
                ch.SendSysMessage("mode 1");
                if (pAI->GetRole() == ROLE_TANK)
                {
                    return false;
                }
            }
            else if (mode == 2)
            {
                ch.SendSysMessage("mode 2");
                if (pAI->GetRole() == ROLE_TANK || pAI->GetRole() == ROLE_MELEE_DPS)
                {
                    return false;
                }
            }
            if (pBot->GetVictim())
            {
                pBot->AttackStop(true);
                pBot->InterruptNonMeleeSpells(false);
                if (!pBot->IsStopped())
                    pBot->StopMoving();
                if (pBot->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
                    pBot->GetMotionMaster()->Clear();
                if (pAI->m_updateTimer.GetExpiry() < 3000)
                    pAI->m_updateTimer.Reset(3000);
            }
            if (Pet* pPet = pBot->GetPet())
            {
                if (pPet->GetVictim())
                {
                    pPet->AttackStop();
                    pPet->GetCharmInfo()->SetIsCommandAttack(false);
                    pPet->GetCharmInfo()->SetIsCommandFollow(true);
                }

            }

            if (pBot->GetStandState() != UNIT_STAND_STATE_STAND)
                pBot->SetStandState(UNIT_STAND_STATE_STAND);

            pBot->InterruptSpellsWithInterruptFlags(SPELL_INTERRUPT_FLAG_MOVEMENT);
            pBot->MonsterMove(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ());

            return true;
        }
    }

    return false;
}

bool ChatHandler::HandleCompanionComeCommand(char* args)
{
    Player* pPlayer = GetSession()->GetPlayer();
    bool success = false;

    int32 mode = 0;
     if (*args)
    {
        if (!ExtractInt32(&args, mode))
        {
            return false;
        }
    }
    if (mode > 2)
        mode = 2;

    if (Group* pGroup = pPlayer->GetGroup())
    {
        for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            if (Player* pMember = itr->getSource())
            {
                if (pMember == pPlayer)
                    continue;
                CompanionComeToMeHelper(pMember, pPlayer, mode);
                success = true;
            }
        }

        if (success && mode == 0)
            SendSysMessage("All companions are coming to your position.");
        else if (success && mode == 1)
            SendSysMessage("All meele and ranged companions are coming to your position.");
        else if (success && mode == 2)
            SendSysMessage("All ranged companions are coming to your position.");
        else
            SendSysMessage("There are no party bots in the group or they cannot move.");
        return success;
    }

    SendSysMessage("You are not in a group.");
    SetSentErrorMessage(true);
    return false;
}

bool ChatHandler::HandleCompanionJoinCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();
    Player* pTarget = GetSelectedPlayer();

    if (!IsTargetCompanion(pTarget))
        return false;
    
    if (pTarget->AI())
        {
            loadBotFromMenu(pPlayer, pTarget->GetGUIDLow());
        }

return true;
}

bool ChatHandler::HandleCompanionTellRoleCommmand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();
    Group* pGroup = pPlayer->GetGroup();

    for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        if (Player* pMember = itr->getSource())
        {
            if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(pMember->AI()))
            {
                uint8 role = static_cast<uint8>(pAI->m_role);
                std::string roleString;
                std::string botname(pMember->GetName());
                switch (role)
                {
                case 0:
                    PSendSysMessage("%s is Role: Invalid", botname);
                    break;
                case 1:
                    PSendSysMessage("%s is Role: Meele DPS", botname);
                    break;
                case 2:
                    PSendSysMessage("%s is Role: Range DPS", botname);
                    break;
                case 3:
                    PSendSysMessage("%s is Role: Tank", botname);
                    break;
                case 4:
                    PSendSysMessage("%s is Role: Healer", botname);
                    break;
                }
                SetSentErrorMessage(true);
            }
        }
    }
    return true;    
}

bool ChatHandler::HandleCompanionDPSPause(char* args)
{
    int32 duration = 0;
    if (*args)
    {
        if (!ExtractInt32(&args, duration))
        {
            return false;
        }
    }
    if (duration == 0)
        duration = 5;

    duration = duration * IN_MILLISECONDS;

    Player* pPlayer = GetSession()->GetPlayer();
    Group* pGroup = pPlayer->GetGroup();
    if (!pGroup)
    {
        SendSysMessage("You are not in a group.");
        SetSentErrorMessage(true);
        return false;
    }

    bool success = false;
    for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        if (Player* pMember = itr->getSource())
        {
            if (pMember == pPlayer)
                continue;
            if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(pMember->AI()))
            {
                if (pAI->GetRole() == ROLE_TANK || pAI->GetRole() == ROLE_HEALER)
                    continue;

                pAI->m_updateTimer.Reset(duration);

                if (duration)
                {
                    pMember->StopMoving();
                    pMember->GetMotionMaster()->MoveIdle();

                }
            }
            success = true;
        }
    }

    if (success)
    {
        PSendSysMessage("All DPS Companion paused for %u seconds.", (duration / IN_MILLISECONDS));
    }
    else
        SendSysMessage("No party bots in group.");

    return true;

bool ChatHandler::HandleCompanionPassiveCommand(char* args)
{
    Player* pPlayer = GetSession()->GetPlayer();
    Unit* pVictim = pPlayer->GetSelectedUnit();
    Group* pGroup = pPlayer->GetGroup();

    for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        if (Player* pMember = itr->getSource())
        {
            if (pMember == pPlayer)
                continue;
            
            if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(pMember->AI()))
            {
                pAI->m_isPassive = true;
            }
        }
    }
    return true;
}
bool ChatHandler::HandleCompanionActiveCommand(char* args)
{
    Player* pPlayer = GetSession()->GetPlayer();
    Unit* pVictim = pPlayer->GetSelectedUnit();
    Group* pGroup = pPlayer->GetGroup();

    for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        if (Player* pMember = itr->getSource())
        {
            if (pMember == pPlayer)
                continue;
            
            if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(pMember->AI()))
            {
                pAI->m_isPassive = false;
            }
        }
    }
    return true;
}

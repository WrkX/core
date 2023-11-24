#include "Chat.h"
#include "Companions.h"


WorldSession* m_session;


bool loadBotFromMenu(Player* player,uint32 ccGuid)
{
    QueryResult* botresult = CharacterDatabase.PQuery("SELECT name, role FROM characters c inner join characters_companions cc on c.guid = cc.companion_characters_guid WHERE guid = %u", ccGuid);
    Field* bot = botresult->Fetch();
    std::string ccName = bot[0].GetString();
    uint8 ccRole = bot[1].GetUInt8();
    CombatBotRoles role = static_cast<CombatBotRoles>(ccRole);
    //char* companionName = &*ccName.begin();

    loadCompanion(ccName, false, role, player);
    return true;
}

bool loadExistingBot(Player* player, CombatBotRoles companionRole, uint8 companionClass)
{
    const char* playername = player->GetName();

    uint32 charGuid = sObjectMgr.GetPlayerGuidByName(playername);

    if (!charGuid)
    {
        return false;
    }

    QueryResult* result = CharacterDatabase.PQuery("SELECT companion_characters_guid, class, role FROM characters_companions WHERE characters_guid = %u and class = %u", charGuid, companionClass);

    if (result)
    {
        //SendSysMessage("Companion Found");
        //SetSentErrorMessage(true);

        do
        {
            uint32 ccGuid;
            uint8 ccClass, ccRole;
            Field* cc = result->Fetch();
            ccGuid = cc[0].GetUInt32();
            ccClass = cc[1].GetUInt8();
            ccRole = cc[2].GetUInt8();

            CombatBotRoles roleCasted = static_cast<CombatBotRoles>(ccRole);
            if (ccClass == companionClass && roleCasted == companionRole)
            {
                findExistingCompanion(ccGuid, ccClass, roleCasted, player);
            }
        } while (result->NextRow());
        // needa do things
        return false;
    }
    return false;
}

bool cancelCompanionContract(Player* pPlayer, uint32 companionGuid)
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT account from characters where guid = %u", companionGuid);
    Field* toDeletePlayer = result->Fetch();
    uint32 toDeletePlayerAccountId = toDeletePlayer[0].GetUInt32();

    CharacterDatabase.PExecute("DELETE FROM characters_companions WHERE characters_guid = %u AND companion_characters_guid = %u",
        pPlayer->GetGUIDLow(),
        companionGuid);
    

    WorldSession* session = new WorldSession(toDeletePlayerAccountId, nullptr, sAccountMgr.GetSecurity(toDeletePlayerAccountId), 0, LOCALE_enUS);

    Player* player = new Player(session);
    player->DeleteFromDB(companionGuid, toDeletePlayerAccountId);
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
        //SendSysMessage("CharGuid not found");
        //SetSentErrorMessage(true);
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

void findExistingCompanion(const uint32& ccGuid, const uint8& ccClass, CombatBotRoles role, Player* player)
{
    QueryResult* botresult = CharacterDatabase.PQuery("SELECT name FROM characters WHERE guid = %u and class = %u", ccGuid, ccClass);
    Field* botname = botresult->Fetch();
    std::string ccName = botname[0].GetString();
    //char* companionName = &*ccName.begin();

    loadCompanion(ccName, false, role, player);
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
        //PSendSysMessage("Account: %s was created", accountData);
        //SetSentErrorMessage(true);
        createBotCharacter(botClass, pPlayer, accid, playerCharGuid, role);
        return true;
    }
    else
    {
        //PSendSysMessage("Account: %s was not created", accountData);
        //SetSentErrorMessage(true);
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
        pAI = new PartyBotAI(pPlayer, pPlayer->GetMapId(), pPlayer->GetMap()->GetInstanceId(), x, y, z, pPlayer->GetOrientation());
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

    UINT32 roleInt = static_cast<UINT32>(botRole);

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

    //PSendSysMessage("Companionamount: %u", companionAmount);
    //SetSentErrorMessage(true);
    // TODO: Make configurable
    if (companionAmount < 15)
        return true;
    return false;
}

bool ChatHandler::HandleCompanionEquipCommand(char* args) 
{
    Player* pPlayer = GetSession()->GetPlayer();
    Group* pGroup = pPlayer->GetGroup();
    char* arg1 = ExtractQuotedArg(&args);
    char* arg2 = ExtractQuotedArg(&args);
    std::string partyName(arg1);
    std::string item(arg2);

    QueryResult* result = WorldDatabase.PQuery("SELECT `entry`,`Inventory_Type` FROM `item_template` WHERE `name` = '%s'", item);
    if (!result)
    {
        SendSysMessage("Item not found in db.");
        SetSentErrorMessage(true);
        return false;
    }
    Field* field = result->Fetch();
    uint32 itemId = field[0].GetUInt32();
    uint8 slot = 16;//field[0].GetUInt8();
    delete result;

    PSendSysMessage("ItemID: %u", itemId);
    SetSentErrorMessage(true);

    if (!pGroup)
        return false;
    for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        if (Player* pMember = itr->getSource())
        {
            if (pMember == pPlayer)
                continue;

            PSendSysMessage("Member: %c", pMember->GetName());
            SetSentErrorMessage(true);

            if (pMember->AI())
            {
                pMember->AutoUnequipItemFromSlot(slot);
                //pMember->StoreNewItemInBestSlots(itemId, 1);
                return true;
            }
        }
    }
    PSendSysMessage("Player not found.");
    SetSentErrorMessage(true);
    return false;

    //DeleteFromInventoryDB
    /* 
 
    char* arg1 = ExtractArg(&args);
    char* arg2 = ExtractArg(&args);
    
    arg1->
    PSendSysMessage("arg1: %c", arg1);
    SetSentErrorMessage(true);
    PSendSysMessage("arg2: %c", arg2);
    SetSentErrorMessage(true);
    return true;

    // if not guild name only (in "") then player name
    ObjectGuid target_guid;
    if (!ExtractPlayerTarget(&nameStr, nullptr, &target_guid))
    {
        PSendSysMessage("Error, player not found", nameStr);
        SetSentErrorMessage(true);
        return false;
    }

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
    PSendSysMessage("Name: %c", nameStr);
    SetSentErrorMessage(true);
    PSendSysMessage("Item: %u", itemId);
    SetSentErrorMessage(true);

    
    */
}
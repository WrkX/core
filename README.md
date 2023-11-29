[![vmangos CI build](https://github.com/WrkX/core/actions/workflows/vmangos.yml/badge.svg)](https://github.com/WrkX/core/actions/workflows/vmangos.yml)

# Goals
The goal of this fork is to make Vmangos singleplayer friendly. While I try to keep the world as 'vanilla' as possible I am creating custom content to have more to do and balancing classes so even the usually weaker classes are 'playable'.

Currently these features are part of this project:
- Companion System: Similar to the companions in NycterMoon. You can recruit partybots in each city and invite them to your group in every inn. This will cost a little, but your very own bots are persistent. You will have to gear them up etc.. For this there are new commands. They are of course still the PartyBots already in Vmangos, but I am working on improving their AI. I am also working on raid strats, so you will be able to raid with our companions without them dying in every mechanic.
- Custom mounts: currently only the spectral tiger, I will work on adding more. You will need a new patch.mpq for this, will I will provide soon. These are sold by the Mad mount merchant & his twin brother, who randomly spawn in Org and SW.
- New items, like new bags that improve stats and are dropped in dungeons,
- Badges: Every high level dungeon boss and every raid boss will drop a Badge. Each raid has his own Badge. You can turn these Badges in to either get items from the raid or get rep for one of the city factions (after unlocking the Paragon System). The vendors are scattered around the world. The Dungeon & Molten Core vendors are in the citis (every city vendor sells different items), the others have to be searched for.
- Paragon System: After getting max rep with one of the city factions you can unlock the paragon system, to gain mounts that were removed in patch 1.4. You will reset your rep with the faction to revered and lose rep with other horde/alliance reps, but will get one of the mounts in return. This can be repeated until you have all mounts of a faction.
- Raid Progression: You will have to go through the raids in an order. At level 60 Thrall & Bolvar will start a quest for this. You will have to kill Ragnaros & Onyxia to unlock BWL. After killing Nefarian you will unlock AQ20 & ZG. After AQ20 (ZG is not needed), you will unlock AQ40, which will, of course, unlock Naxxramas.
- (WIP) 10 man Raids: All raids are 10 man raids.
- & more, like custom quest chains, class reworks (keeping them as vanilla as possible), new pets, etc.

(not all of this is already done)

## Custom SQL Changes (needed for most of my changes)
coming soon:tm:


# Vmangos

## Progressive Vanilla
This project is an independent continuation of the Elysium / LightsHope codebases, focused on delivering the most complete and accurate content progression system possible, including support for the patch appropriate game clients.

### Currently supported builds
- 1.12.1.5875+
- 1.11.2.5464
- 1.10.2.5302
- 1.9.4.5086
- 1.8.4.4878
- 1.7.1.4695
- 1.6.1.4544
<!--- 1.5.1.4449
- 1.4.2.4375
- 1.3.1.4297
- 1.2.4.4222-->

### Project guidelines
- Accuracy: The point of an emulator is to recreate the functionality of that which it is emulating as closely as possible. Therefore any custom behaviour should be behind a config option and off by default.
- DB Scripting: Content should be separate from the core itself, so hardcoding scripts should be avoided where possible. When database scripting functionality is insufficient, we simply expand it.
- Full progression: The ultimate goal of this project is to have complete progression starting from patch 1.2 through 1.12. This means every piece of data must be marked with the patch in which it was added or changed to it's current state.
- Tools are great: Content creation should not require programming knowledge. We hope to eventually provide tools that allow for user-friendly editing of database scripts and content, with all data presented in human-readable form.

### Downloads
- [![vmangos CI build](https://github.com/WrkX/core/actions/workflows/dev-release.yml/badge.svg)](https://github.com/WrkX/core/releases/tag/latest)  Latest development binary
- [![vmangos Development DB Dump](https://github.com/WrkX//core/actions/workflows/db_dump.yml/badge.svg)](https://github.com/WrkX//core/releases/tag/db_latest)  mysql5.6 full dump, no update needed.

### Useful Links
- [Wiki](https://github.com/vmangos/wiki)
- [Discord](https://discord.gg/x9a2jt7)
- [Script Editor](https://github.com/brotalnia/scripteditor)
- [Script Converter](https://github.com/vmangos/ScriptConverter)

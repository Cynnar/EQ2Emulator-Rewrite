#include "stdafx.h"

#include "ZoneDatabase.h"
#include "../../common/log.h"
#include "../ZoneServer/ZoneServer.h"
#include "../ZoneServer/ZoneOperator.h"
#include "../Spawns/Entity.h"
#include "../Spawns/Object.h"
#include "../Spawns/GroundSpawn.h"
#include "../Players/CharacterSheet.h"
#include "../../common/Classes.h"
#include "../ZoneServer/SpawnCamp.h"

extern ZoneOperator g_zoneOperator;
extern MasterEntityCommandList g_masterEntityCommandList;

constexpr const char* GetSpawnTableFields();
constexpr const char* GetSpawnLocationFields();
constexpr const char* GetNpcTableFields();

ZoneDatabase::ZoneDatabase() {

}

ZoneDatabase::~ZoneDatabase() {
}

bool ZoneDatabase::Start() {
	if (Connect()) {
		LogInfo(LOG_DATABASE, 0, "Connected to MySQL server at %s:%u", GetHost(), GetPort());
		return true;
	}

	return false;
}

bool ZoneDatabase::LoadZoneInfo(ZoneServer* z) {
	DatabaseResult result;

	bool success = Select(&result, "SELECT * FROM zones WHERE id = %u", z->GetID());
	if (!success)
		return success;

	if (result.Next()) {
		z->SetExpansionID(result.GetUInt8(1));
		z->SetName(result.GetString(2));
		z->SetFile(result.GetString(3));
		z->SetDescription(result.GetString(4));
		z->SetSafeX(result.GetFloat(5));
		z->SetSafeY(result.GetFloat(6));
		z->SetSafeZ(result.GetFloat(7));
		z->SetSafeHeading(result.GetFloat(8));
		z->SetUnderworld(result.GetFloat(9));
		z->SetEXPModifier(result.GetFloat(10));
		z->SetMinRecommended(result.GetUInt8(11));
		z->SetMaxRecomended(result.GetUInt8(12));
		z->SetZoneType(result.GetString(13));
		z->SetIsAlwaysLoaded(result.GetBool(14));
		z->SetCityZone(result.GetBool(15));
		z->SetWeatherAllowed(result.GetBool(16));
		z->SetMinStatus(result.GetUInt32(17));
		z->SetMinLevel(result.GetUInt32(18));
		z->SetMaxLevel(result.GetUInt32(19));
		z->SetStartingZone(result.GetUInt8(20));
		z->SetInstanceType(result.GetString(21));
		z->SetDefaultReenterTime(result.GetUInt32(22));
		z->SetDefaultResetTime(result.GetUInt32(23));
		z->SetDefaultLockoutTime(result.GetUInt32(24));
		z->SetForceGroupToZone(result.GetUInt16(25));
		z->SetScriptID(result.GetUInt32(26));
		z->SetShutdownTimer(result.GetUInt32(27));
		z->SetZoneMOTD(result.GetString(28));
		z->SetRulesetID(result.GetUInt32(29));
	}
	else
		return false;

	return true;
}

void ZoneDatabase::ProcessCharactersTableResult(DatabaseResult& result, const std::shared_ptr<Entity>& entity, CharacterSheet& charSheet) {
	entity->SetName(result.GetString(0), false);
	entity->SetRace(result.GetUInt8(1), false);
	entity->SetAdventureClass(result.GetUInt8(2), false);
	charSheet.advClass = entity->GetAdventureClass();
	charSheet.advArchetype = Classes::GetBaseClass(charSheet.advClass);
	charSheet.advBaseClass = Classes::GetSecondaryBaseClass(charSheet.advClass);
	entity->SetGender(result.GetUInt8(3), false);
	charSheet.deityID = result.GetUInt32(4);
	entity->SetBodySize(static_cast<int8_t>(result.GetFloat(5) * 127), false);
	entity->SetBodyBumpScale(static_cast<int8_t>(result.GetFloat(6) * 127), false);
	entity->SetLevel(result.GetUInt8(7), false);
	entity->SetOrigLevel(entity->GetAdventureLevel(), false);
	charSheet.tsClass = result.GetUInt32(8);
	charSheet.tsArchetype = Classes::GetTSBaseClass(charSheet.tsClass);
	charSheet.tsBaseClass = Classes::GetSecondaryTSBaseClass(charSheet.tsClass);
	charSheet.tsLevel = result.GetUInt16(9);
	// Set SOGA Wing Type
	// Set SOGA Chest Type
	// Set SOGA Legs Type
	entity->SetSogaHairType(result.GetUInt32(13), false);
	entity->SetSogaFacialHairType(result.GetUInt32(14), false);
	entity->SetSogaModelType(result.GetUInt32(15), false);
	entity->SetLegsType(result.GetUInt32(16), false);
	entity->SetChestType(result.GetUInt32(17), false);
	entity->SetWingType(result.GetUInt32(18), false);
	entity->SetHairType(result.GetUInt32(19), false);
	entity->SetFacialHairType(result.GetUInt32(20), false);
	entity->SetModelType(result.GetUInt32(21), false);
	entity->SetX(result.GetFloat(22), false);
	entity->SetY(result.GetFloat(23), false);
	entity->SetZ(result.GetFloat(24), false);
	entity->SetHeading(result.GetFloat(25), false);
	// Set Starting City
	// Set Account Age
	charSheet.zoneID = result.GetUInt32(28);
	charSheet.instanceID = result.GetUInt32(29);
	entity->SetTailType(result.GetUInt32(30), false);
}

void ZoneDatabase::ProcessCharacterDetailsResult(DatabaseResult& res, const std::shared_ptr<Entity>& entity, CharacterSheet& sheet) {
	//Skip id and char_id
	uint32_t i = 2;

	EntityAttributeSheet* attr = sheet.attributes;

	attr->hp.Initialize(res.GetInt32(i++));
	attr->power.Initialize(res.GetInt32(i++));
	attr->savagery.Initialize(res.GetInt32(i++));
	attr->dissonance.Initialize(res.GetInt32(i++));
	attr->concentration.maxValue = res.GetInt32(i++);
	//Attack?
	i++;
	attr->mitigation.Initialize(res.GetInt32(i++));
	attr->avoidance.Initialize(res.GetInt32(i++));
	//Skip some skill stats
	i += 3;
	attr->str.Initialize(res.GetInt32(i++));
	attr->sta.Initialize(res.GetInt32(i++));
	attr->agi.Initialize(res.GetInt32(i++));
	attr->wis.Initialize(res.GetInt32(i++));
	attr->intel.Initialize(res.GetInt32(i++));
	i += 7;
	//attr->heat.Initialize(res.GetInt32(i++));
	//attr->cold.Initialize(res.GetInt32(i++));
	//attr->magic.Initialize(res.GetInt32(i++));
	//attr->mental.Initialize(res.GetInt32(i++));
	//attr->divine.Initialize(res.GetInt32(i++));
	//attr->disease.Initialize(res.GetInt32(i++));
	//attr->poison.Initialize(res.GetInt32(i++));
	attr->elemental.Initialize(res.GetInt32(i++));
	attr->arcane.Initialize(res.GetInt32(i++));
	attr->noxious.Initialize(res.GetInt32(i++));
	CharacterCurrency& cur = sheet.currency;
	cur.copper = res.GetUInt32(i++);
	cur.silver = res.GetUInt32(i++);
	cur.gold = res.GetUInt32(i++);
	cur.platinum = res.GetUInt32(i++);
	sheet.petName = res.GetString(i++);
	sheet.statusPoints = res.GetUInt32(i++);
	attr->power.maxValue = res.GetInt32(i++);
	attr->hp.maxValue = res.GetInt32(i++);
	attr->savagery.maxValue = res.GetInt32(i++);
	attr->dissonance.maxValue = res.GetInt32(i++);
	
	CharacterExperience& exp = sheet.experience;
	exp.currentAdvExp = res.GetUInt32(i++);
	exp.nextAdvLevelExp = res.GetUInt32(i++);
	exp.advExpDebt = res.GetUInt32(i++);
	exp.advVitality = res.GetUInt32(i++);
	exp.currentTsExp = res.GetUInt32(i++);
	exp.nextTsLevelExp = res.GetUInt32(i++);
	//TS experience debt...?
	i++;
	exp.tsVitality = res.GetUInt32(i++);

	cur.bankCopper = res.GetUInt32(i++);
	cur.bankSilver = res.GetUInt32(i++);
	cur.bankGold = res.GetUInt32(i++);
	cur.bankPlatinum = res.GetUInt32(i++);
	
	sheet.bindZone = res.GetUInt32(i++);
	sheet.bindLocation.x = res.GetFloat(i++);
	sheet.bindLocation.y = res.GetFloat(i++);
	sheet.bindLocation.z = res.GetFloat(i++);
	sheet.bindHeading = res.GetFloat(i++);
	sheet.houseZoneId = res.GetUInt32(i++);

	entity->SetCombatVoice(res.GetUInt32(i++));
	entity->SetEmoteVoice(res.GetUInt32(i++));
	
	sheet.biography = res.GetString(i++);
	sheet.flags = res.GetUInt32(i++);
	sheet.flags2 = res.GetUInt32(i++);

	//TODO: load titles
	int32_t prefixID = res.GetInt32(i++);
	int32_t suffixID = res.GetInt32(i++);

	sheet.currentLanguage = res.GetUInt32(i++);
	
	entity->SetLastName(res.GetString(i++));
	
	//Skip aa fields
	i += 8;
}

void ZoneDatabase::SaveSingleCharacter(CharacterSheet& charSheet) {
	std::ostringstream ss;
	if (charSheet.updates.GenerateUpdate(ss)) {
		if (!Query(ss.str().c_str())) {
			LogError(LOG_DATABASE, 0, "Error saving character %s", charSheet.entity->GetName().c_str());
		}
	}
}

bool ZoneDatabase::LoadCharacter(uint32_t char_id, uint32_t account_id, std::shared_ptr<Entity> entity, CharacterSheet& charSheet) {
	charSheet.characterID = char_id;

	//First we need to set up the query
	std::ostringstream query;
	const char* characterSelect = "SELECT `name`, `race`, `class`, `gender`, `deity`, `body_size`, `body_age`, `level`, `tradeskill_class`, `tradeskill_level`, `soga_wing_type`, `soga_chest_type`, `soga_legs_type`, `soga_hair_type`, `soga_facial_hair_type`, `soga_model_type`, `legs_type`, `chest_type`, `wing_type`, `hair_type`, `facial_hair_type`, `model_type`, `x`, `y`, `z`, `heading`, `starting_city`, DATEDIFF(curdate(), `created_date`) as accage, current_zone_id, instance_id, tail_type FROM characters ";
	//`characters`
	query << characterSelect << "WHERE id = " << char_id << " AND account_id = " << account_id << " AND deleted = 0;\n";
	//`char_colors`
	query << "SELECT char_id, type, red, green, blue FROM char_colors WHERE char_id = " << char_id << ";\n";
	//`character_details`
	query << "SELECT * FROM `character_details` WHERE char_id = " << char_id << ";\n";

	DatabaseResult result;
	bool ret = Select(&result, query.str().c_str());
	if (!ret)
		return ret;

	//`characters` table
	if (result.Next()) {
		ProcessCharactersTableResult(result, entity, charSheet);
	}

	result.NextResultSet();

	//`char_colors`
	//I know it looks stupid using a map for 1 entity but this lets us reuse the same code for characters and npcs
	std::unordered_map<uint32_t, std::shared_ptr<Spawn> > colorLoadMap;
	colorLoadMap[char_id] = entity;
	ProcessEntityColors(result, colorLoadMap);

	result.NextResultSet();

	//`character_details`
	if (result.Next()) {
		ProcessCharacterDetailsResult(result, entity, charSheet);
	}

	charSheet.updates.LinkUpdateFields(charSheet);

	return ret;
}

constexpr const char* GetNpcTableFields() {
	return "npc.min_level, npc.max_level, npc.enc_level, npc.enc_level_offset, npc.class_, npc.gender, npc.min_group_size, npc.max_group_size, npc.hair_type_id, npc.facial_hair_type_id, npc.wing_type_id, npc.chest_type_id, npc.legs_type_id, npc.soga_hair_type_id, npc.soga_facial_hair_type_id, npc.action_state, npc.mood_state, npc.attack_type, npc.ai_strategy+0, npc.spell_list_id, npc.secondary_spell_list_id, npc.skill_list_id, npc.secondary_skill_list_id, npc.equipment_list_id, npc.str, npc.sta, npc.wis, npc.intel, npc.agi, npc.heat, npc.cold, npc.magic, npc.mental, npc.divine, npc.disease, npc.poison, npc.aggro_radius, npc.cast_percentage, npc.randomize, npc.soga_model_type, npc.heroic_flag, npc.alignment, npc.elemental, npc.arcane, npc.noxious, npc.hide_hood, npc.emote_state, npc.is_crouching, npc.is_sitting, npc.disable_loot, npc.weapons_equipped";
}

void ZoneDatabase::ProcessNpcResult(DatabaseResult& result, const std::shared_ptr<Entity>& npc) {
	EntityAttributeSheet& sheet = npc->attributes;

	//Base spawn data
	uint32_t i = ProcessSpawnTableFields(npc, result);

	//Default for most npcs
	npc->EnablePositionState(POS_STATE_SIMULATE_GRAVITY | ((npc->GetPosStruct()->positionState & POS_STATE_DISABLE_GRAVITY) ? 0 : POS_STATE_ON_GROUND) );

	npc->SetLevel(result.GetUInt8(i++));
	npc->SetOrigLevel(npc->GetAdventureLevel());
	npc->SetMinLevel(npc->GetAdventureLevel());
	npc->SetMaxLevel(result.GetUInt8(i++));
	npc->SetDifficulty(result.GetUInt8(i++));
	npc->SetDifficultyOffset(result.GetUInt8(i++));
	npc->SetAdventureClass(result.GetUInt8(i++));
	npc->SetGender(result.GetUInt8(i++));
	// min_group_size, no clue what this is for
	// max_group_size, no clue what this is for
	i += 2;
	npc->SetHairType(result.GetUInt32(i++));
	npc->SetFacialHairType(result.GetUInt32(i++));
	npc->SetWingType(result.GetUInt32(i++));
	npc->SetChestType(result.GetUInt32(i++));
	npc->SetLegsType(result.GetUInt32(i++));
	npc->SetSogaHairType(result.GetUInt32(i++));
	npc->SetSogaFacialHairType(result.GetUInt32(i++));
	npc->SetActionState(result.GetUInt32(i++));
	npc->SetMoodState(result.GetUInt32(i++));
	// Attack Type
	i++;
	// AI Strategy
	i++;
	// Primary Spell List
	i++;
	// Secondary Spell List
	i++;
	// Primary Skill List
	i++;
	// Secondary Skill List
	i++;
	// Equipment List ID
	i++;
	sheet.str.Initialize(result.GetInt32(i++));
	sheet.sta.Initialize(result.GetInt32(i++));
	sheet.wis.Initialize(result.GetInt32(i++));
	sheet.intel.Initialize(result.GetInt32(i++));
	sheet.agi.Initialize(result.GetInt32(i++));
	i += 7;
	//sheet.heat.Initialize(result.GetInt32(i++));
	//sheet.cold.Initialize(result.GetInt32(i++));
	//sheet.magic.Initialize(result.GetInt32(i++));
	//sheet.mental.Initialize(result.GetInt32(i++));
	//sheet.divine.Initialize(result.GetInt32(i++));
	//sheet.disease.Initialize(result.GetInt32(i++));
	//sheet.poison.Initialize(result.GetInt32(i++));
	// Aggro Radius
	i++;
	// Cast Percentage 
	i++;
	// randomize
	i++;
	npc->SetSogaModelType(result.GetUInt32(i++));
	npc->SetHeroicFlag(result.GetUInt8(i++));
	// Alignment
	i++;
	sheet.elemental.baseValue = result.GetInt32(i++);
	sheet.arcane.baseValue = result.GetInt32(i++);
	sheet.noxious.baseValue = result.GetInt32(i++);
	//Hide hood
	if (result.GetBool(i++)) {
		npc->EnableInfoVisFlags(INFO_VIS_FLAG_HIDE_HOOD);
	}
	else {
		npc->DisableInfoVisFlags(INFO_VIS_FLAG_HIDE_HOOD);
	}
	npc->SetEmoteState(result.GetUInt32(i++));
	npc->SetCrouching(result.GetBool(i++));
	npc->SetSitting(result.GetBool(i++));
	npc->SetLootDisabled(result.GetBool(i++));
	if (result.GetBool(i++)) {
		npc->EnableEntityFlags(EntityFlagWeaponsEquipped);
	}
}

bool ZoneDatabase::LoadNPCsForZone(ZoneServer* z) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT %s, %s\n"
		"FROM spawn s\n"
		"INNER JOIN spawn_npcs npc\n"
		"ON s.id = npc.spawn_id\n"
		"INNER JOIN spawn_location_entry le\n"
		"ON npc.spawn_id = le.spawn_id\n"
		"INNER JOIN spawn_location_placement lp\n"
		"ON le.spawn_location_id = lp.spawn_location_id\n"
		"WHERE lp.zone_id = %u\n",
		 GetSpawnTableFields(), GetNpcTableFields(), z->GetID());
	if (!ret)
		return ret;

	std::unordered_map<uint32_t, std::shared_ptr<Spawn> > npcs;
	npcs.reserve(result.GetNumRows());

	uint32_t count = 0;
	while (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (z->GetNPCFromMasterList(id))
			continue;

		std::shared_ptr<Entity> npc = std::make_shared<Entity>();

		ProcessNpcResult(result, npc);

		npcs[npc->GetDatabaseID()] = npc;

		count++;
		LogDebug(LOG_NPC, 5, "---Loading NPC: '%s' (%u)", npc->GetName().c_str(), id);
	}

	LogInfo(LOG_NPC, 0, "--Loaded %u NPC(s).", count);

	LogInfo(LOG_NPC, 0, "--Loading NPC Appearances.");
	result.Clear();
	ret = Select(&result,
		"SELECT na.spawn_id, na.`type`, na.red, na.green, na.blue\n"
		"FROM spawn_location_placement slp\n"
		"INNER JOIN spawn_location_entry sle ON sle.spawn_location_id = slp.spawn_location_id\n"
		"INNER JOIN npc_appearance na ON sle.spawn_id = na.spawn_id\n"
		"WHERE slp.zone_id = %u\n"
		"ORDER BY spawn_id", z->GetID());

	if (ret) {
		ProcessEntityColors(result, npcs);

		result.Clear();

		ret = Select(&result, "SELECT na.spawn_id, na.slot_id, na.equip_type, na.red, na.green, na.blue, na.highlight_red, na.highlight_green, na.highlight_blue\n"
			"FROM spawn_location_placement slp\n"
			"INNER JOIN spawn_location_entry sle ON sle.spawn_location_id = slp.spawn_location_id\n"
			"INNER JOIN npc_appearance_equip na ON sle.spawn_id = na.spawn_id\n"
			"WHERE slp.zone_id = %u\n"
			"ORDER BY na.spawn_id;", z->GetID());

		ProcessNpcAppearanceEquipment(result, npcs);
	}

	for (auto& itr : npcs) {
		itr.second->PopUpdateFlags();
		z->AddNPCToMasterList(std::static_pointer_cast<Entity>(itr.second));
	}

	return ret;
}

constexpr const char* GetObjectTableFields() {
	return "so.id, so.device_id";
}

void ZoneDatabase::ProcessObjectResult(DatabaseResult& result, const std::shared_ptr<Object>& spawn) {
	//Base spawn data
	uint32_t i = ProcessSpawnTableFields(spawn, result);

	// Object Info starts here
	spawn->SetObjectDatabaseID(result.GetUInt32(i++));
	spawn->SetDeviceID(result.GetUInt8(i++));
}

bool ZoneDatabase::LoadObjectsForZone(ZoneServer* z) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT %s, %s\n"
		"FROM spawn s\n"
		"INNER JOIN spawn_objects so\n"
		"ON s.id = so.spawn_id\n"
		"INNER JOIN spawn_location_entry le\n"
		"ON so.spawn_id = le.spawn_id\n"
		"INNER JOIN spawn_location_placement lp\n"
		"ON le.spawn_location_id = lp.spawn_location_id\n"
		"WHERE lp.zone_id = %u\n"
		"GROUP BY s.id", GetSpawnTableFields(), GetObjectTableFields(), z->GetID());

	if (!ret)
		return ret;

	uint32_t count = 0;
	while (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (z->GetNPCFromMasterList(id))
			continue;

		std::shared_ptr<Object> spawn = std::make_shared<Object>();
		ProcessObjectResult(result, spawn);
		count++;

		z->AddObjectToMasterList(spawn);
		LogDebug(LOG_NPC, 5, "---Loading Object: '%s' (%u)", spawn->GetName().c_str(), id);
	}

	LogInfo(LOG_NPC, 0, "--Loaded %u Object(s).", count);
	return ret;
}

constexpr const char* GetWidgetTableFields() {
	return "sw.id, sw.widget_id, sw.widget_x, sw.widget_y, sw.widget_z, sw.include_heading, sw.include_location, sw.type+0, sw.open_heading, sw.closed_heading, sw.open_x, sw.open_y, sw.open_z, sw.action_spawn_id, sw.open_sound_file, sw.close_sound_file, sw.open_duration, sw.close_x, sw.close_y, sw.close_z, sw.linked_spawn_id, sw.house_id";
}

void ZoneDatabase::ProcessWidgetResult(DatabaseResult& result, const std::shared_ptr<Spawn>& spawn) {
	// Base spawn data
	uint32_t index = ProcessSpawnTableFields(spawn, result);

	// Widget Info starts here
	std::unique_ptr<Widget>& wd = spawn->GetWidgetData();
	if (!wd)
		wd = std::make_unique<Widget>();

	wd->SetWidgetDatabaseID(result.GetUInt32(index++));
	wd->SetWidgetID(result.GetUInt32(index++));
	wd->SetWidgetX(result.GetFloat(index++));
	wd->SetWidgetY(result.GetFloat(index++));
	wd->SetWidgetZ(result.GetFloat(index++));
	wd->SetIncludeHeading(result.GetBool(index++));
	wd->SetIncludeLocation(result.GetBool(index++));
	wd->SetWidgetType((EWidgetType)result.GetUInt8(index++));
	wd->SetOpenHeading(result.GetFloat(index++));
	wd->SetCloseHeading(result.GetFloat(index++));
	wd->SetOpenX(result.GetFloat(index++));
	wd->SetOpenY(result.GetFloat(index++));
	wd->SetOpenZ(result.GetFloat(index++));
	wd->SetActionID(result.GetUInt32(index++));
	wd->SetOpenSoundFile(result.GetString(index++));
	wd->SetCloseSoundFile(result.GetString(index++));
	wd->SetOpenDuration(result.GetUInt32(index++));
	wd->SetCloseX(result.GetFloat(index++));
	wd->SetCloseY(result.GetFloat(index++));
	wd->SetCloseZ(result.GetFloat(index++));
	wd->SetLinkedSpawnID(result.GetUInt32(index++));
	wd->SetHouseID(result.GetUInt32(index++));
}

bool ZoneDatabase::LoadWidgetsForZone(ZoneServer* z) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT %s, %s\n"
		"FROM spawn s\n"
		"INNER JOIN spawn_widgets sw\n"
		"ON s.id = sw.spawn_id\n"
		"INNER JOIN spawn_location_entry le\n"
		"ON sw.spawn_id = le.spawn_id\n"
		"INNER JOIN spawn_location_placement lp\n"
		"ON le.spawn_location_id = lp.spawn_location_id\n"
		"WHERE lp.zone_id = %u\n"
		"GROUP BY s.id", GetSpawnTableFields(), GetWidgetTableFields(), z->GetID());

	if (!ret)
		return ret;

	uint32_t count = 0;
	while (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (z->GetWidgetFromMasterList(id))
			continue;

		std::shared_ptr<Spawn> spawn = std::make_shared<Spawn>();
		ProcessWidgetResult(result, spawn);
		count++;

		z->AddWidgetToMasterList(spawn);
		LogDebug(LOG_NPC, 5, "---Loading Widget: '%s' (%u)", spawn->GetName().c_str(), id);
	}

	LogInfo(LOG_NPC, 0, "--Loaded %u Widget(s).", count);
	return ret;
}

constexpr const char* GetSignTableFields() {
	return "ss.id, ss.type+0, ss.zone_id, ss.title, ss.description, ss.sign_distance, ss.zone_x, ss.zone_y, ss.zone_z, ss.zone_heading, ss.widget_id, ss.widget_x, ss.widget_y, ss.widget_z, ss.include_heading, ss.include_location";
}

void ZoneDatabase::ProcessSignResult(DatabaseResult& result, const std::shared_ptr<Spawn>& spawn) {
	// Spawn base info
	uint32_t index = ProcessSpawnTableFields(spawn, result);

	// Sign info starts here
	std::unique_ptr<Sign>& sign = spawn->GetSignData();
	if (!sign)
		sign = std::make_unique<Sign>();

	sign->SetSignDatabseID(result.GetUInt32(index++));
	sign->SetSignType((ESignType)result.GetUInt8(index++));
	sign->SetZoneID(result.GetUInt32(index++));
	sign->SetTitle(result.GetString(index++));
	sign->SetDescription(result.GetString(index++));
	sign->SetDistance(result.GetFloat(index++));
	sign->SetZoneX(result.GetFloat(index++));
	sign->SetZoneY(result.GetFloat(index++));
	sign->SetZoneZ(result.GetFloat(index++));
	sign->SetZoneHeading(result.GetFloat(index++));

	// Widget info starts here
	uint32_t widget_id = result.GetUInt32(index++);
	if (widget_id != 0) {
		std::unique_ptr<Widget>& widget = spawn->GetWidgetData();
		if (!widget)
			widget = std::make_unique<Widget>();

		widget->SetWidgetID(widget_id);
		widget->SetWidgetX(result.GetFloat(index++));
		widget->SetWidgetY(result.GetFloat(index++));
		widget->SetWidgetZ(result.GetFloat(index++));
		widget->SetIncludeHeading(result.GetBool(index++));
		widget->SetIncludeLocation(result.GetBool(index++));
	}
}

bool ZoneDatabase::LoadSignsForZone(ZoneServer* z) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT %s, %s\n"
		"FROM spawn s\n"
		"INNER JOIN spawn_signs ss\n"
		"ON s.id = ss.spawn_id\n"
		"INNER JOIN spawn_location_entry le\n"
		"ON ss.spawn_id = le.spawn_id\n"
		"INNER JOIN spawn_location_placement lp\n"
		"ON le.spawn_location_id = lp.spawn_location_id\n"
		"WHERE lp.zone_id = %u\n"
		"GROUP BY s.id", GetSpawnTableFields(), GetSignTableFields(), z->GetID());

	if (!ret)
		return ret;

	uint32_t count = 0;
	while (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (z->GetSignFromMasterList(id))
			continue;

		std::shared_ptr<Spawn> spawn = std::make_shared<Spawn>();
		ProcessSignResult(result, spawn);
		count++;

		z->AddSignToMasterList(spawn);
		LogDebug(LOG_NPC, 5, "---Loading Sign: '%s' (%u)", spawn->GetName().c_str(), id);
	}

	LogInfo(LOG_NPC, 0, "--Loaded %u Sign(s).", count);
	return ret;
}

constexpr const char* GetGroundSpawnTableFields() {
	return "sg.id, sg.number_harvests, sg.num_attempts_per_harvest, sg.groundspawn_id, sg.collection_skill";
}

void ZoneDatabase::ProcessGroundSpawnResult(DatabaseResult& result, const std::shared_ptr<GroundSpawn>& spawn) {
	// Spawn base info
	uint32_t index = ProcessSpawnTableFields(spawn, result);

	// GroundSpawn info starts here
	spawn->SetGroundSpawnDatabaseID(result.GetUInt32(index++));
	spawn->SetNumberOfHarvests(result.GetUInt8(index++));
	spawn->SetNumberAttemptsPerHarvets(result.GetUInt8(index++));
	spawn->SetGroundSpawnID(result.GetUInt32(index++));
	spawn->SetCollectionSkill(result.GetString(index++));
}

bool ZoneDatabase::LoadGroundSpawnsForZone(ZoneServer* z) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT %s, %s\n"
		"FROM spawn s\n"
		"INNER JOIN spawn_ground sg\n"
		"ON s.id = sg.spawn_id\n"
		"INNER JOIN spawn_location_entry le\n"
		"ON sg.spawn_id = le.spawn_id\n"
		"INNER JOIN spawn_location_placement lp\n"
		"ON le.spawn_location_id = lp.spawn_location_id\n"
		"WHERE lp.zone_id = %u\n"
		"GROUP BY s.id", GetSpawnTableFields(), GetGroundSpawnTableFields(), z->GetID());

	if (!ret)
		return ret;

	uint32_t count = 0;
	while (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (z->GetGroundSpawnFromMasterList(id))
			continue;

		std::shared_ptr<GroundSpawn> spawn = std::make_shared<GroundSpawn>();
		ProcessGroundSpawnResult(result, spawn);
		count++;

		z->AddGroundSpawnToMasterList(spawn);
		LogDebug(LOG_NPC, 5, "---Loading GroundSpawn: '%s' (%u)", spawn->GetName().c_str(), id);
	}

	LogInfo(LOG_NPC, 0, "--Loaded %u GroundSpawn(s).", count);
	return ret;
}

bool ZoneDatabase::LoadNPC(ZoneServer* z, uint32_t id) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT %s, %s\n"
		"FROM spawn s\n"
		"INNER JOIN spawn_npcs npc\n"
		"ON s.id = npc.spawn_id\n"
		"WHERE s.id = %u\n",
		GetSpawnTableFields(), GetNpcTableFields(), id);
	if (!ret)
		return ret;

	std::unordered_map<uint32_t, std::shared_ptr<Spawn> > npcs;
	npcs.reserve(result.GetNumRows());

	if (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (z->GetNPCFromMasterList(id))
			return false;

		std::shared_ptr<Entity> npc = std::make_shared<Entity>();

		ProcessNpcResult(result, npc);

		npcs[npc->GetDatabaseID()] = npc;

		LogDebug(LOG_NPC, 0, "Loaded NPC: '%s' (%u)", npc->GetName().c_str(), id);


		LogInfo(LOG_NPC, 0, "Loading NPC Appearances for npc (%u).", id);
		result.Clear();
		ret = Select(&result,
			"SELECT na.spawn_id, na.`type`, na.red, na.green, na.blue\n"
			"FROM npc_appearance na\n"
			"WHERE na.spawn_id = %u\n", id);

		if (ret) {
			ProcessEntityColors(result, npcs);

			result.Clear();

			ret = Select(&result, "SELECT na.spawn_id, na.slot_id, na.equip_type, na.red, na.green, na.blue, na.highlight_red, na.highlight_green, na.highlight_blue\n"
				"FROM npc_appearance_equip na\n"
				"WHERE na.spawn_id = %u\n", id);

			ProcessNpcAppearanceEquipment(result, npcs);
		}

		for (auto& itr : npcs) {
			itr.second->PopUpdateFlags();
			z->AddNPCToMasterList(std::static_pointer_cast<Entity>(itr.second));
		}
	}

	return ret;
}

bool ZoneDatabase::LoadObject(ZoneServer* z, uint32_t id) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT %s, %s\n"
		"FROM spawn s\n"
		"INNER JOIN spawn_objects so\n"
		"ON s.id = so.spawn_id\n"
		"WHERE s.id = %u",
		GetSpawnTableFields(), GetObjectTableFields(), id);

	if (!ret)
		return ret;

	if (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (z->GetObjectFromMasterList(id))
			return false;

		std::shared_ptr<Object> spawn = std::make_shared<Object>();
		ProcessObjectResult(result, spawn);

		z->AddObjectToMasterList(spawn);
		LogDebug(LOG_NPC, 0, "Loaded Object: '%s' (%u)", spawn->GetName().c_str(), id);
	}

	return ret;
}

bool ZoneDatabase::LoadWidget(ZoneServer* z, uint32_t id) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT %s, %s\n"
		"FROM spawn s\n"
		"INNER JOIN spawn_widgets sw\n"
		"ON s.id = sw.spawn_id\n"
		"WHERE s.id = %u",
		GetSpawnTableFields(), GetWidgetTableFields(), id);

	if (!ret)
		return ret;

	if (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (z->GetWidgetFromMasterList(id))
			return false;

		std::shared_ptr<Spawn> spawn = std::make_shared<Spawn>();
		ProcessWidgetResult(result, spawn);

		z->AddWidgetToMasterList(spawn);
		LogDebug(LOG_NPC, 0, "Loaded Widget: '%s' (%u)", spawn->GetName().c_str(), id);
	}

	return ret;
}

bool ZoneDatabase::LoadSign(ZoneServer* z, uint32_t id) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT %s, %s\n"
		"FROM spawn s\n"
		"INNER JOIN spawn_signs ss\n"
		"ON s.id = ss.spawn_id\n"
		"WHERE s.id = %u",
		GetSpawnTableFields(), GetSignTableFields(), id);

	if (!ret)
		return ret;

	if (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (z->GetSignFromMasterList(id))
			return false;

		std::shared_ptr<Spawn> spawn = std::make_shared<Spawn>();
		ProcessSignResult(result, spawn);

		z->AddSignToMasterList(spawn);
		LogDebug(LOG_NPC, 0, "Loaded Sign: '%s' (%u)", spawn->GetName().c_str(), id);
	}

	return ret;
}

bool ZoneDatabase::LoadGroundSpawn(ZoneServer* z, uint32_t id) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT %s, %s\n"
		"FROM spawn s\n"
		"INNER JOIN spawn_ground sg\n"
		"ON s.id = sg.spawn_id\n"
		"WHERE s.id = %u",
		GetSpawnTableFields(), GetGroundSpawnTableFields(), id);

	if (!ret)
		return ret;

	if (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (z->GetGroundSpawnFromMasterList(id))
			return false;

		std::shared_ptr<GroundSpawn> spawn = std::make_shared<GroundSpawn>();
		ProcessGroundSpawnResult(result, spawn);

		z->AddGroundSpawnToMasterList(spawn);
		LogDebug(LOG_NPC, 0, "Loaded GroundSpawn: '%s' (%u)", spawn->GetName().c_str(), id);
	}

	return ret;
}

bool ZoneDatabase::LoadSpawnLocationGroups(ZoneServer* z) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT slg.group_id, slg.placement_id, slp.spawn_location_id FROM spawn_location_group slg, spawn_location_placement slp WHERE slg.placement_id = slp.id and slp.zone_id = %u", z->GetID());
	if (!ret)
		return ret;

	uint32_t count = 0;
	while (result.Next()) {
		uint32_t group_id = result.GetUInt32(0);
		uint32_t placement_id = result.GetUInt32(1);
		uint32_t location_id = result.GetUInt32(2);
		z->AddSpawnGroupLocation(group_id, placement_id, location_id);
		count++;
	}

	return ret;
}

bool ZoneDatabase::LoadSpawnGroupChances(ZoneServer* z) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT slgc.group_id, slgc.percentage FROM spawn_location_group_chances slgc, spawn_location_group slg, spawn_location_placement slp where slgc.group_id = slg.group_id and slg.placement_id = slp.id and slp.zone_id = %u", z->GetID());
	if (!ret)
		return ret;

	uint32_t count = 0;
	while (result.Next()) {
		uint32_t group_id = result.GetUInt32(0);
		float percent = result.GetFloat(1);
		z->AddSpawnGroupChance(group_id, percent);
		count++;
	}

	return ret;
}

bool ZoneDatabase::LoadSpawnLocationGroupAssociations(ZoneServer* z) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT distinct slga.group_id1, slga.group_id2 FROM spawn_location_group_associations slga, spawn_location_group slg, spawn_location_placement slp where (slg.group_id = slga.group_id1 or slg.group_id = slga.group_id2) and slg.placement_id = slp.id and slp.zone_id = %u", z->GetID());
	if (!ret)
		return ret;

	uint32_t count = 0;
	while (result.Next()) {
		uint32_t group_id1 = result.GetUInt32(0);
		uint32_t group_id2 = result.GetUInt32(1);
		z->AddSpawnGroupAssociation(group_id1, group_id2);
		count++;
	}

	return ret;
}

constexpr const char* GetSpawnLocationFields() {
	return "sln.id, sln.name,\n"
		"slp.id, slp.x, slp.y, slp.z, slp.x_offset, slp.y_offset, slp.z_offset, slp.heading, slp.pitch, slp.roll, slp.respawn, slp.expire_timer, slp.expire_offset, slp.grid_id,"
		"sle.id, sle.spawn_id, sle.spawnpercentage, sle.condition, sle.script_id, sle.spawn_camp_id, sln.script_id\n";
}

uint32_t ZoneDatabase::LoadSpawnLocation(std::string query, ZoneServer* z, SpawnEntryType type) {
	DatabaseResult result;
	bool ret = Select(&result, query.c_str(), GetSpawnLocationFields(), z->GetID());

	if (!ret)
		return 0;

	uint32_t count = 0;
	uint32_t location_id = 0xFFFFFFFF;
	std::shared_ptr<SpawnLocation> spawn_location = nullptr;
	while (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (location_id == 0xFFFFFFFF || id != location_id) {
			if (spawn_location) {
				switch (type) {
				case SpawnEntryType::ENPC:
					z->AddNPCSpawnLocation(location_id, spawn_location);
					break;
				case SpawnEntryType::EOBJECT:
					z->AddObjectSpawnLocation(location_id, spawn_location);
					break;
				case SpawnEntryType::EWIDGET:
					z->AddWidgetSpawnLocation(location_id, spawn_location);
					break;
				case SpawnEntryType::ESIGN:
					z->AddSignSpawnLocation(location_id, spawn_location);
					break;
				case SpawnEntryType::EGROUNDSPAWN:
					z->AddGroundSpawnSpawnLocation(location_id, spawn_location);
					break;
				}

				count++;
			}

			spawn_location = std::make_shared<SpawnLocation>();
		}

		std::shared_ptr<SpawnEntry> entry = std::make_shared<SpawnEntry>();
		location_id = id;

		// spawn_location_name
		spawn_location->name = result.GetString(1);

		// spawn_location_placement
		spawn_location->placement_id = result.GetUInt32(2);
		spawn_location->x = result.GetFloat(3);
		spawn_location->y = result.GetFloat(4);
		spawn_location->z = result.GetFloat(5);
		spawn_location->x_offset = result.GetFloat(6);
		spawn_location->y_offset = result.GetFloat(7);
		spawn_location->z_offset = result.GetFloat(8);
		spawn_location->heading = result.GetFloat(9);
		spawn_location->pitch = result.GetFloat(10);
		spawn_location->roll = result.GetFloat(11);
		spawn_location->respawn = result.GetUInt32(12);
		// TODO: Respawn_offset
		spawn_location->expire_time = result.GetUInt32(13);
		spawn_location->expire_offset = result.GetUInt32(14);
		spawn_location->grid_id = result.GetUInt32(15);
		spawn_location->scriptID = result.GetUInt32(22);

		// spawn_location_entry
		entry->spawn_location_id = location_id;
		entry->spawn_entry_id = result.GetUInt32(16);
		entry->spawn_id = result.GetUInt32(17);
		entry->spawn_percentage = result.GetFloat(18);
		entry->condition = result.GetUInt32(19);
		entry->scriptID = result.GetUInt32(20);
		entry->spawn_camp_id = result.GetUInt32(21);
		entry->spawn_type = type;

		spawn_location->total_percentage += entry->spawn_percentage;
		spawn_location->AddEntry(entry);
	}

	if (spawn_location) {
		switch (type) {
		case SpawnEntryType::ENPC:
			z->AddNPCSpawnLocation(location_id, spawn_location);
			break;
		case SpawnEntryType::EOBJECT:
			z->AddObjectSpawnLocation(location_id, spawn_location);
			break;
		case SpawnEntryType::EWIDGET:
			z->AddWidgetSpawnLocation(location_id, spawn_location);
			break;
		case SpawnEntryType::ESIGN:
			z->AddSignSpawnLocation(location_id, spawn_location);
			break;
		case SpawnEntryType::EGROUNDSPAWN:
			z->AddGroundSpawnSpawnLocation(location_id, spawn_location);
			break;
		}

		count++;
	}

	return count;
}

bool ZoneDatabase::LoadNPCLocations(ZoneServer* z) {
	std::string query = "SELECT %s\n"
		"FROM spawn_location_placement slp\n"
		"INNER JOIN spawn_location_name sln ON slp.spawn_location_id = sln.id\n"
		"INNER JOIN spawn_location_entry sle ON sle.spawn_location_id = sln.id\n"
		"INNER JOIN spawn_npcs sn ON sn.spawn_id = sle.spawn_id\n"
		"WHERE slp.zone_id=%u ORDER BY sln.id, sle.id";

	uint32_t count = LoadSpawnLocation(query, z, SpawnEntryType::ENPC);
	LogInfo(LOG_NPC, 0, "--Loaded %u NPC spawn location(s).", count);
	return true;
}

bool ZoneDatabase::LoadObjectLocations(ZoneServer* z) {
	std::string query = "SELECT %s\n"
		"FROM spawn_location_placement slp\n"
		"INNER JOIN spawn_location_name sln ON slp.spawn_location_id = sln.id\n"
		"INNER JOIN spawn_location_entry sle ON sle.spawn_location_id = sln.id\n"
		"INNER JOIN spawn_objects so ON so.spawn_id = sle.spawn_id\n"
		"WHERE slp.zone_id=%u ORDER BY sln.id, sle.id";

	uint32_t count = LoadSpawnLocation(query, z, SpawnEntryType::EOBJECT);
	LogInfo(LOG_NPC, 0, "--Loaded %u Object spawn location(s).", count);
	return true;
}

bool ZoneDatabase::LoadWidgetLocations(ZoneServer* z) {
	std::string query = "SELECT %s\n"
		"FROM spawn_location_placement slp\n"
		"INNER JOIN spawn_location_name sln ON slp.spawn_location_id = sln.id\n"
		"INNER JOIN spawn_location_entry sle ON sle.spawn_location_id = sln.id\n"
		"INNER JOIN spawn_widgets sw ON sw.spawn_id = sle.spawn_id\n"
		"WHERE slp.zone_id=%u ORDER BY sln.id, sle.id";

	uint32_t count = LoadSpawnLocation(query, z, SpawnEntryType::EWIDGET);
	LogInfo(LOG_NPC, 0, "--Loaded %u Widget spawn location(s).", count);
	return true;
}

bool ZoneDatabase::LoadSignLocations(ZoneServer* z) {
	std::string query = "SELECT %s\n"
		"FROM spawn_location_placement slp\n"
		"INNER JOIN spawn_location_name sln ON slp.spawn_location_id = sln.id\n"
		"INNER JOIN spawn_location_entry sle ON sle.spawn_location_id = sln.id\n"
		"INNER JOIN spawn_signs ss ON ss.spawn_id = sle.spawn_id\n"
		"WHERE slp.zone_id=%u ORDER BY sln.id, sle.id";

	uint32_t count = LoadSpawnLocation(query, z, SpawnEntryType::ESIGN);
	LogInfo(LOG_NPC, 0, "--Loaded %u Sign spawn location(s).", count);
	return true;
}

bool ZoneDatabase::LoadGroundSpawnLocations(ZoneServer* z) {
	std::string query = "SELECT %s\n"
		"FROM spawn_location_placement slp\n"
		"INNER JOIN spawn_location_name sln ON slp.spawn_location_id = sln.id\n"
		"INNER JOIN spawn_location_entry sle ON sle.spawn_location_id = sln.id\n"
		"INNER JOIN spawn_ground sg ON sg.spawn_id = sle.spawn_id\n"
		"WHERE slp.zone_id=%u ORDER BY sln.id, sle.id";

	uint32_t count = LoadSpawnLocation(query, z, SpawnEntryType::EGROUNDSPAWN);
	LogInfo(LOG_NPC, 0, "--Loaded %u GroundSpawn spawn location(s).", count);
	return true;
}

constexpr const char* GetSpawnTableFields() {
	return "s.id, s.name,s.sub_title,s.prefix,s.suffix,s.last_name,s.race,s.model_type,s.size,s.size_offset,s.targetable,s.show_name,"
		"s.command_primary,s.command_secondary,s.visual_state,s.attackable,s.show_level,s.show_command_icon,s.display_hand_icon,s.faction_id,"
		"s.collision_radius,s.hp,s.power,s.savagery,s.dissonance,s.merchant_id,s.transport_id,s.merchant_type,s.script_id,s.is_solid,"
		"s.is_transport,s.face_on_hail,s.disable_gravity,s.is_global_spawn";
}

uint32_t ZoneDatabase::ProcessSpawnTableFields(const std::shared_ptr<Spawn>& spawn, DatabaseResult& res) {
	uint32_t i = 0;

	spawn->SetDatabaseID(res.GetUInt32(i++));
	spawn->SetName(res.GetString(i++));
	spawn->SetGuild(res.GetString(i++));
	spawn->SetPrefixTitle(res.GetString(i++));
	spawn->SetSuffixTitle(res.GetString(i++));
	spawn->SetLastName(res.GetString(i++));
	spawn->SetRace(res.GetUInt8(i++));
	spawn->SetModelType(res.GetUInt32(i++));
	spawn->SetSize(res.GetFloat(i++));
	spawn->SetSizeOffset(res.GetFloat(i++));
	uint32_t entityFlags = EntityIsNpc;
	//Targetable
	if (!res.GetBool(i++)) {
		entityFlags |= EntityFlagNotTargetable;
	}
	//Show name
	spawn->SetShowName(res.GetBool(i++));
	spawn->SetPrimaryCommandListID(res.GetUInt32(i++));
	spawn->SetPrimaryCommandList(g_masterEntityCommandList.GetListByID(spawn->GetPrimaryCommandListID()));
	spawn->SetSecondaryCommandListID(res.GetUInt32(i++));
	spawn->SetSecondaryCommandList(g_masterEntityCommandList.GetListByID(spawn->GetSecondaryCommandListID()));
	spawn->SetVisualState(res.GetUInt32(i++));
	//Attackable
	spawn->SetAttackable(res.GetBool(i++));
	//Show level
	spawn->SetShowLevel(res.GetBool(i++));
	//Show command icon
	spawn->SetShowCommandIcon(res.GetBool(i++));
	//Display hand icon
	if (res.GetBool(i++)) {
		entityFlags |= EntityFlagInteractable;
	}
	spawn->SetFactionID(res.GetUInt32(i++));
	spawn->SetCollisionRadius(res.GetFloat(i++));
	spawn->SetHP(res.GetUInt32(i++));
	spawn->SetPower(res.GetUInt32(i++));
	spawn->SetSavagery(res.GetUInt32(i++));
	spawn->SetDissonance(res.GetUInt32(i++));
	spawn->SetMerchantID(res.GetUInt32(i++));
	if (spawn->GetMerchantID() > 0) {
		entityFlags |= EntityFlagMerchant;
	}
	//Transport ID
	res.GetUInt32(i++);
	spawn->SetMerchantType(res.GetUInt32(i++));
	spawn->SetScriptID(res.GetUInt32(i++));
	if (res.GetBool(i++)) {
		entityFlags |= EntityFlagSolid;
	}
	if (res.GetBool(i++)) {
		entityFlags |= EntityFlagIsTransport;
	}
	spawn->SetFaceOnHail(res.GetBool(i++));
	spawn->SetGravityDisabled(res.GetBool(i++));
	spawn->SetGlobalSpawn(res.GetBool(i++));


	spawn->SetEntityFlags(entityFlags);
	spawn->PopUpdateFlags();

	return i;
}

//The input result to this function should be indexed by (id, type, red, green, blue)
void ZoneDatabase::ProcessEntityColors(DatabaseResult& result, std::unordered_map<uint32_t, std::shared_ptr<Spawn> >& outEntities) {
	uint32_t lastID = 0;
	std::shared_ptr<Spawn> entity;

	union {
		uint8_t uVal[3];
		int8_t sVal[3];
	};

	//Major hackery..
	EQ2Color& c = reinterpret_cast<EQ2Color&>(uVal[0]);
	int8_t* v = sVal;

	while (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (id != lastID) {
			lastID = id;
			entity = outEntities[id];
		}

		EmuAssert(entity);

		CStringCmpNoCopy type = result.GetString(1);

		//Union + casting trickery to reduce the code we need here
		c.Red = static_cast<uint8_t>(result.GetInt32(2));
		c.Green = static_cast<uint8_t>(result.GetInt32(3));
		c.Blue = static_cast<uint8_t>(result.GetInt32(4));

		if (type == "skin_color") {
			entity->SetSkinColor(c, false);
		}
		else if (type == "model_color") {
			entity->SetModelColor(c, false);
		}
		else if (type == "eye_color") {
			entity->SetEyeColor(c, false);
		}
		else if (type == "hair_color1") {
			entity->SetHairColor1(c, false);
		}
		else if (type == "hair_color2") {
			entity->SetHairColor2(c, false);
		}
		else if (type == "hair_highlight") {
			entity->SetHairHighlight(c, false);
		}
		else if (type == "hair_type_color") {
			entity->SetHairColor(c, false);
		}
		else if (type == "hair_type_highlight_color") {
			entity->SetHairHighlightColor(c, false);
		}
		else if (type == "hair_face_color") {
			entity->SetFacialHairColor(c);
		}
		else if (type == "hair_face_highlight_color") {
			entity->SetFacialHairHighlightColor(c, false);
		}
		else if (type == "wing_color1") {
			entity->SetWingColor(c, false);
		}
		else if (type == "wing_color2") {
			entity->SetWingHighlightColor(c, false);
		}
		else if (type == "tail_color1") {
			entity->SetTailColor(c, false);
		}
		else if (type == "tail_color2") {
			entity->SetTailHighlightColor(c, false);
		}
		else if (type == "shirt_color") {
			entity->SetChestColor(c, false);
		}
		else if (type == "unknown_chest_color") {
			entity->SetChestHighlightColor(c, false);
		}
		else if (type == "pants_color") {
			entity->SetLegsColor(c, false);
		}
		else if (type == "unknown_legs_color") {
			entity->SetLegsHighlightColor(c, false);
		}
		else if (type == "unknown9") {
			//guessing since this is right before eye type this is skull?
			entity->SetSkullType(v[0], 0, false);
			entity->SetSkullType(v[1], 1, false);
			entity->SetSkullType(v[2], 2, false);
		}
		else if (type == "eye_type") {
			entity->SetEyeType(v[0], 0, false);
			entity->SetEyeType(v[1], 1, false);
			entity->SetEyeType(v[2], 2, false);
		}
		else if (type == "ear_type") {
			entity->SetEarType(v[0], 0, false);
			entity->SetEarType(v[1], 1, false);
			entity->SetEarType(v[2], 2, false);
		}
		else if (type == "eye_brow_type") {
			entity->SetEyeBrowType(v[0], 0, false);
			entity->SetEyeBrowType(v[1], 1, false);
			entity->SetEyeBrowType(v[2], 2, false);
		}
		else if (type == "cheek_type") {
			entity->SetCheekType(v[0], 0, false);
			entity->SetCheekType(v[1], 1, false);
			entity->SetCheekType(v[2], 2, false);
		}
		else if (type == "lip_type") {
			entity->SetLipType(v[0], 0, false);
			entity->SetLipType(v[1], 1, false);
			entity->SetLipType(v[2], 2, false);
		}
		else if (type == "chin_type") {
			entity->SetChinType(v[0], 0, false);
			entity->SetChinType(v[1], 1, false);
			entity->SetChinType(v[2], 2, false);
		}
		else if (type == "nose_type") {
			entity->SetNoseType(v[0], 0, false);
			entity->SetNoseType(v[1], 1, false);
			entity->SetNoseType(v[2], 2, false);
		}
		else if (type == "body_size") {
			entity->SetBodySize(v[0], false);
		}
		else if (type == "soga_skin_color") {
			entity->SetSogaSkinColor(c, false);
		}
		else if (type == "soga_model_color") {
			entity->SetSogaModelColor(c, false);
		}
		else if (type == "soga_eye_color") {
			entity->SetSogaEyeColor(c, false);
		}
		else if (type == "soga_hair_color1") {
			entity->SetSogaHairColor1(c, false);
		}
		else if (type == "soga_hair_color2") {
			entity->SetSogaHairColor2(c, false);
		}
		else if (type == "soga_hair_highlight") {
			entity->SetSogaHairHighlight(c, false);
		}
		else if (type == "soga_hair_type_color") {
			entity->SetSogaHairColor(c, false);
		}
		else if (type == "soga_hair_type_highlight_color") {
			entity->SetSogaHairHighlightColor(c, false);
		}
		else if (type == "soga_hair_face_color") {
			entity->SetSogaFacialHairColor(c, false);
		}
		else if (type == "soga_hair_face_highlight_color") {
			entity->SetSogaFacialHairHighlightColor(c, false);
		}
		//else if (type == "soga_wing_color1") {
		//}
		//else if (type == "soga_wing_color2") {
		//}
		//else if (type == "soga_shirt_color") {
		//}
		//else if (type == "soga_unknown_chest_color") {
		//}
		//else if (type == "soga_pants_color") {
		//}
		//else if (type == "soga_unknown_legs_color") {
		//}
		else if (type == "soga_unknown13") {
			entity->SetSogaSkullType(v[0], 0, false);
			entity->SetSogaSkullType(v[1], 1, false);
			entity->SetSogaSkullType(v[2], 2, false);
		}
		else if (type == "soga_eye_type") {
			entity->SetSogaEyeType(v[0], 0, false);
			entity->SetSogaEyeType(v[1], 1, false);
			entity->SetSogaEyeType(v[2], 2, false);
		}
		else if (type == "soga_ear_type") {
			entity->SetSogaEarType(v[0], 0, false);
			entity->SetSogaEarType(v[1], 1, false);
			entity->SetSogaEarType(v[2], 2, false);
		}
		else if (type == "soga_eye_brow_type") {
			entity->SetSogaEyeBrowType(v[0], 0, false);
			entity->SetSogaEyeBrowType(v[1], 1, false);
			entity->SetSogaEyeBrowType(v[2], 2, false);
		}
		else if (type == "soga_cheek_type") {
			entity->SetSogaCheekType(v[0], 0, false);
			entity->SetSogaCheekType(v[1], 1, false);
			entity->SetSogaCheekType(v[2], 2, false);
		}
		else if (type == "soga_lip_type") {
			entity->SetSogaLipType(v[0], 0, false);
			entity->SetSogaLipType(v[1], 1, false);
			entity->SetSogaLipType(v[2], 2, false);
		}
		else if (type == "soga_chin_type") {
			entity->SetSogaChinType(v[0], 0, false);
			entity->SetSogaChinType(v[1], 1, false);
			entity->SetSogaChinType(v[2], 2, false);
		}
		else if (type == "soga_nose_type") {
			entity->SetSogaNoseType(v[0], 0, false);
			entity->SetSogaNoseType(v[1], 1, false);
			entity->SetSogaNoseType(v[2], 2, false);
		}
	}
}

//The input result to this function should be indexed by (spawn_id, slot_id, equip_type, red, green, blue, 
//highlight_red, highlight_green, highlight_blue)
void ZoneDatabase::ProcessNpcAppearanceEquipment(DatabaseResult& result, std::unordered_map<uint32_t, std::shared_ptr<Spawn> >& outEntities) {
	std::shared_ptr<Spawn> entity;
	uint32_t lastID = 0;
	EQ2EquipmentItem item;

	while (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (id != lastID) {
			entity = outEntities[id];
			lastID = id;
		}

		EmuAssert(entity);

		uint8_t slot = result.GetUInt8(1);

		item.type = result.GetUInt32(2);
		item.color.Red = result.GetUInt8(3);
		item.color.Green = result.GetUInt8(4);
		item.color.Blue = result.GetUInt8(5);
		item.highlight.Red = result.GetUInt8(6);
		item.highlight.Green = result.GetUInt8(7);
		item.highlight.Blue = result.GetUInt8(8);

		entity->SetAppearanceEquipmentItem(slot, item, false);
	}
}

bool ZoneDatabase::LoadSpawnCampsForZone(std::shared_ptr<ZoneServer> z) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT * FROM spawn_camp WHERE zone_id = %u", z->GetID());

	if (!ret)
		return ret;

	uint32_t count = 0;
	uint32_t spawn_count = 0;
	uint32_t location_count = 0;
	while (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		std::string type = result.GetString(1);
		std::shared_ptr<SpawnCamp> camp = nullptr;
		if (type == "Single")
			camp = std::make_shared<SpawnCamp>(z, result.GetFloat(4), result.GetFloat(5), result.GetFloat(6), result.GetUInt32(7));
		else if (type == "Area") {
			std::shared_ptr<SpawnCampArea> area = std::make_shared<SpawnCampArea>(z, result.GetFloat(4), result.GetFloat(5), result.GetFloat(6), result.GetUInt32(7));

			area->SetRadius(result.GetFloat(8));
			area->SetNumRadiusEncounter(result.GetUInt16(9));

			camp = area;
		}
		else if (type == "Group") {
			std::shared_ptr<SpawnCampGroup> group = std::make_shared<SpawnCampGroup>(z, result.GetFloat(4), result.GetFloat(5), result.GetFloat(6), result.GetUInt32(7));

			group->SetMinSpawns(result.GetUInt16(9));
			group->SetMaxSpawns(result.GetUInt16(10));

			camp = group;
		}
		else if (type == "Boss")
			camp = std::make_shared<SpawnCampBoss>(z, result.GetFloat(4), result.GetFloat(5), result.GetFloat(6), result.GetUInt32(7));
		else if (type == "Event")
			camp = std::make_shared<SpawnCampEvent>(z, result.GetFloat(4), result.GetFloat(5), result.GetFloat(6), result.GetUInt32(7));

		camp->SetID(id);
		camp->SetName(result.GetString(2));

		count++;
		spawn_count += LoadSpawnsForSpawnCamp(camp);
		location_count += LoadLocationsForSpawnCamp(camp);

		z->AddSpawnCamp(camp);
		LogDebug(LOG_NPC, 5, "---Loading Spawn Camp: '%s' (%u)", camp->GetName().c_str(), id);
	}

	LogInfo(LOG_NPC, 0, "--Loaded %u Spawn Camp(s).", count);
	LogInfo(LOG_NPC, 0, "---Loaded %u Spawn Camp spawns.", spawn_count);
	LogInfo(LOG_NPC, 0, "---Loaded %u Spawn Camp locations.", location_count);
	return ret;
}

uint32_t ZoneDatabase::LoadSpawnsForSpawnCamp(std::shared_ptr<SpawnCamp> camp) {
	uint32_t ret = 0;
	DatabaseResult result;
	bool select = Select(&result, "SELECT * FROM spawn_camp_spawns WHERE spawn_camp_id = %u", camp->GetID());

	if (!select)
		return ret;

	while (result.Next()) {
		std::shared_ptr<SpawnCampSpawnEntry> se = std::make_shared<SpawnCampSpawnEntry>();
		se->id = result.GetUInt32(0);
		se->spawnDBID = result.GetUInt32(2);
		se->respawn_time = result.GetUInt32(3);
		se->expire_time = result.GetUInt32(4);
		se->expire_offset = result.GetUInt32(5);
		se->minLevel = result.GetUInt8(6);
		se->maxLevel = result.GetUInt8(7);
		se->minEncounterLevel = result.GetUInt8(8);
		se->maxEncounterLevel = result.GetUInt32(9);
		se->conditional = result.GetUInt64(10);
		se->weight = result.GetUInt32(11);

		camp->AddSpawn(se);
		ret++;
	}

	return ret;
}

uint32_t ZoneDatabase::LoadLocationsForSpawnCamp(std::shared_ptr<SpawnCamp> camp) {
	uint32_t ret = 0;
	DatabaseResult result;
	bool select = Select(&result, "SELECT * FROM spawn_camp_locations WHERE spawn_camp_id = %u", camp->GetID());

	if (!select)
		return ret;

	while (result.Next()) {
		std::shared_ptr<SpawnCampLocationEntry> sl = std::make_shared<SpawnCampLocationEntry>();
		sl->id = result.GetUInt32(0);
		sl->x = result.GetFloat(3);
		sl->y = result.GetFloat(4);
		sl->z = result.GetFloat(5);
		sl->x_offset = result.GetFloat(6);
		sl->y_offset = result.GetFloat(7);
		sl->z_offset = result.GetFloat(8);
		sl->heading = result.GetFloat(9);
		sl->pitch = result.GetFloat(10);
		sl->roll = result.GetFloat(11);
		sl->grid = result.GetUInt32(12);
		sl->visual_state_override = result.GetUInt32(13);
		sl->action_state_override = result.GetUInt32(14);		

		camp->AddSpawnCampLocationEntry(sl);
		ret++;
	}

	return ret;
}

uint32_t ZoneDatabase::InsertNewSpawnCamp(std::shared_ptr<SpawnCamp> camp) {
	uint32_t ret = 0;

	std::string type = "";
	if (camp->IsAreaCamp())
		type = "Area";
	else if (camp->IsGroupCamp())
		type = "Group";
	else if (camp->IsBossCamp())
		type = "Boss";
	else if (camp->IsEventCamp())
		type = "Event";
	else
		type = "Single";

	bool result = Query("INSERT INTO spawn_camp (type, name, zone_id, x, y, z, grid) VALUES ('%s', '%s', %u, %f, %f, %f, %u)", type.c_str(), camp->GetName().c_str(), camp->GetZone()->GetID(), camp->GetX(), camp->GetY(), camp->GetZ(), camp->GetGridID());
	if (result) {
		DatabaseResult dbResult;
		bool select = Select(&dbResult, "SELECT LAST_INSERT_ID()");
		if (select && dbResult.Next()) {
			ret = dbResult.GetUInt32(0);
		}
	}

	return ret;
}

bool ZoneDatabase::SaveSpawnCamp(std::shared_ptr<SpawnCamp> camp) {
	float radius = 0.f;
	uint16_t min_spawns = 0;
	uint16_t max_spawns = 0;
	if (camp->IsAreaCamp()) {
		std::shared_ptr<SpawnCampArea> area = std::dynamic_pointer_cast<SpawnCampArea>(camp);
		if (area) {
			radius = area->GetRadius();
			min_spawns = area->GetNumRadiusEncounters();
		}
	}
	else if (camp->IsGroupCamp()) {
		std::shared_ptr<SpawnCampGroup> group = std::dynamic_pointer_cast<SpawnCampGroup>(camp);
		if (group) {
			min_spawns = group->GetMinSpawns();
			max_spawns = group->GetMaxSpawns();
		}
	}

	return Query("UPDATE spawn_camp SET name = '%s', x = %f, y = %f, z = %f, grid = %u, radius = %f, min_spawns = %u, max_spawns = %u WHERE id = %u", camp->GetName().c_str(), camp->GetX(), camp->GetY(), camp->GetZ(), camp->GetGridID(), radius, min_spawns, max_spawns, camp->GetID());
}

uint32_t ZoneDatabase::InsertNewSpawnCampLocation(uint32_t campID, std::shared_ptr<SpawnCampLocationEntry> scle) {
	uint32_t ret = 0;

	bool result = Query("INSERT INTO spawn_camp_locations (spawn_camp_id, x, y, z, x_offset, y_offset, z_offset, heading, pitch, roll, grid_id, visual_state_override, action_state_override) VALUES (%u, %f, %f, %f, %f, %f, %f, %f, %f, %f, %u, %u, %u)", campID, scle->x, scle->y, scle->z, scle->x_offset, scle->y_offset, scle->z_offset, scle->heading, scle->pitch, scle->roll, scle->grid, scle->visual_state_override, scle->action_state_override);
	if (result) {
		DatabaseResult dbResult;
		bool select = Select(&dbResult, "SELECT LAST_INSERT_ID()");
		if (select && dbResult.Next())
			ret = dbResult.GetUInt32(0);
	}

	return ret;
}

uint32_t ZoneDatabase::InsertNewSpawnCampSpawn(uint32_t campID, std::shared_ptr<SpawnCampSpawnEntry> scse) {
	uint32_t ret = 0;

	bool result = Query("INSERT INTO spawn_camp_spawns (spawn_camp_id, spawn_id, respawn_timer, expire_timer, expire_offset, min_level, max_level, min_encounter_level, max_encounter_level, `condition`, weight) VALUES (%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)", campID, scse->spawnDBID, scse->respawn_time, scse->expire_time, scse->expire_offset, scse->minLevel, scse->maxLevel, scse->minEncounterLevel, scse->maxEncounterLevel, scse->conditional, scse->weight);
	if (result) {
		DatabaseResult dbResult;
		bool select = Select(&dbResult, "SELECT LAST_INSERT_ID()");
		if (select && dbResult.Next())
			ret = dbResult.GetUInt32(0);
	}

	return ret;
}

bool ZoneDatabase::MergeSpawnIntoSpawnCamp(uint32_t spawnID, uint32_t locID, uint32_t campID) {
	return Query("UPDATE spawn_location_entry SET spawn_camp_id = %u WHERE spawn_id = %u AND spawn_location_id = %u", campID, spawnID, locID);
}

uint32_t ZoneDatabase::GetSpawnLocationCount(uint32_t locationID, std::shared_ptr<Spawn> spawn) {
	uint32_t ret = 0;
	DatabaseResult result;
	bool select = Select(&result, "SELECT count(id) FROM spawn_location_entry WHERE spawn_location_id=%u AND spawn_id=%u", locationID, spawn->GetDatabaseID());
	if (select && result.Next()) {
		ret = result.GetUInt32(0);
	}
	return ret;
}

bool ZoneDatabase::RemoveSpawnFromSpawnLocation(std::shared_ptr<Spawn> spawn) {
	bool success = false;
	uint32_t count = GetSpawnLocationCount(spawn->GetSpawnLocationID(), spawn);

	success = Query("DELETE FROM spawn_location_placement WHERE spawn_location_id=%u", spawn->GetSpawnLocationID());
	if (!success) {
		LogError(LOG_ZONE, 0, "Failed to delete spawn (%u) from location (%u)", spawn->GetDatabaseID(), spawn->GetSpawnLocationID());
		return false;
	}

	if (count == 1)
		Query("DELETE FROM spawn_location_name WHERE id=%u", spawn->GetSpawnLocationID());

	success = Query("DELETE FROM spawn_location_entry WHERE spawn_id = %u AND spawn_location_id = %u", spawn->GetDatabaseID(), spawn->GetSpawnLocationID());
	if (!success) {
		LogError(LOG_ZONE, 0, "Failed to delete spawn_location_entry for spawn (%u) and location(%u)", spawn->GetDatabaseID(), spawn->GetSpawnLocationID());
		return false;
	}
	
	return true;
}
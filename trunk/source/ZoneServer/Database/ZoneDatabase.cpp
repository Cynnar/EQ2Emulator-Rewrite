#include "stdafx.h"

#include "ZoneDatabase.h"
#include "../../common/log.h"
#include "../ZoneServer/ZoneServer.h"
#include "../ZoneServer/ZoneOperator.h"
#include "../Spawns/Entity.h"

extern ZoneOperator z;

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
		z->SetLuaScript(result.GetString(26));
		z->SetShutdownTimer(result.GetUInt32(27));
		z->SetZoneMOTD(result.GetString(28));
		z->SetRulesetID(result.GetUInt32(29));
	}
	else
		return false;

	return true;
}

bool ZoneDatabase::LoadCharacter(uint32_t char_id, uint32_t account_id, std::shared_ptr<Entity> entity) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT `name`, `race`, `class`, `gender`, `deity`, `body_size`, `body_age`, `level`, `tradeskill_class`, `tradeskill_level`, `soga_wing_type`, `soga_chest_type`, `soga_legs_type`, `soga_hair_type`, `soga_facial_hair_type`, `soga_model_type`, `legs_type`, `chest_type`, `wing_type`, `hair_type`, `facial_hair_type`, `model_type`, `x`, `y`, `z`, `heading`, `starting_city`, DATEDIFF(curdate(), `created_date`) as accage FROM characters WHERE id = %u AND account_id = %u AND deleted = 0", char_id, account_id);
	if (!ret)
		return ret;

	if (result.Next()) {
		entity->SetName(result.GetString(0), false);
		entity->SetRace(result.GetUInt8(1), false);
		entity->SetAdventureClass(result.GetUInt8(2), false);
		entity->SetGender(result.GetUInt8(3), false);
		// Set Deity
		entity->SetBodySize(result.GetUInt8(5), false);
		entity->SetBodySizeUnknown(result.GetUInt8(6), false); // Not sure if this is actually the correct field for age
		entity->SetLevel(result.GetUInt8(7), false);
		// Set Tradeskill Class
		// Set Tradeskill Level
		// Set SOGA Wing Type
		// Set SOGA Chest Type
		// Set SOGA Legs Type
		entity->SetSogaHairType(result.GetUInt16(13), false);
		entity->SetSogaFacialHairType(result.GetUInt16(14), false);
		entity->SetSogaModelType(result.GetUInt32(15), false);
		entity->SetLegsType(result.GetUInt16(16), false);
		entity->SetChestType(result.GetUInt16(17), false);
		entity->SetWingType(result.GetUInt16(18), false);
		entity->SetHairType(result.GetUInt16(19), false);
		entity->SetFacialHairType(result.GetUInt16(20), false);
		entity->SetModelType(result.GetUInt32(21), false);
		entity->SetX(result.GetFloat(22), false);
		entity->SetY(result.GetFloat(23), false);
		entity->SetZ(result.GetFloat(24), false);
		entity->SetHeading(result.GetFloat(25), false);
		// Set Starting City
		// Set Account Age

		DatabaseResult result2;
		ret = Select(&result2, "SELECT type, red, green, blue FROM char_colors WHERE char_id = %u", char_id);
		if (!ret)
			return ret;

		while (result2.Next()) {
			std::string type = result2.GetString(0);

			if (type == "skin_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetSkinColor(c, false);
			}
			else if (type == "eye_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetEyeColor(c, false);
			}
			else if (type == "hair_color1") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetHairColor1(c, false);
			}
			else if (type == "hair_color2") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetHairColor2(c, false);
			}
			else if (type == "hair_highlight") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetHairHighlight(c, false);
			}
			else if (type == "hair_type_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetHairColor(c, false);
			}
			else if (type == "hair_type_highlight_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetHairHighlightColor(c, false);
			}
			else if (type == "hair_face_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetFacialHairColor(c);
			}
			else if (type == "hair_face_highlight_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetFacialHairHighlightColor(c, false);
			}
			else if (type == "wing_color1") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetWingColor(c, false);
			}
			else if (type == "wing_color2") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetWingHighlightColor(c, false);
			}
			else if (type == "shirt_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetChestColor(c, false);
			}
			else if (type == "unknown_chest_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetChestHighlightColor(c, false);
			}
			else if (type == "pants_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetLegsColor(c, false);
			}
			else if (type == "unknown_legs_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetLegsHighlightColor(c, false);
			}
			else if (type == "unknown9") {
				//guessing since this is right before eye type this is skull?
				entity->SetSkullType(result2.GetInt8(1), 0, false);
				entity->SetSkullType(result2.GetInt8(2), 1, false);
				entity->SetSkullType(result2.GetInt8(3), 2, false);
			}
			else if (type == "eye_type") {
				entity->SetEyeType(result2.GetInt8(1), 0, false);
				entity->SetEyeType(result2.GetInt8(2), 1, false);
				entity->SetEyeType(result2.GetInt8(3), 2, false);
			}
			else if (type == "ear_type") {
				entity->SetEarType(result2.GetInt8(1), 0, false);
				entity->SetEarType(result2.GetInt8(2), 1, false);
				entity->SetEarType(result2.GetInt8(3), 2, false);
			}
			else if (type == "eye_brow_type") {
				entity->SetEyeBrowType(result2.GetInt8(1), 0, false);
				entity->SetEyeBrowType(result2.GetInt8(2), 1, false);
				entity->SetEyeBrowType(result2.GetInt8(3), 2, false);
			}
			else if (type == "cheek_type") {
				entity->SetCheekType(result2.GetInt8(1), 0, false);
				entity->SetCheekType(result2.GetInt8(2), 1, false);
				entity->SetCheekType(result2.GetInt8(3), 2, false);
			}
			else if (type == "lip_type") {
				entity->SetLipType(result2.GetInt8(1), 0, false);
				entity->SetLipType(result2.GetInt8(2), 1, false);
				entity->SetLipType(result2.GetInt8(3), 2, false);
			}
			else if (type == "chin_type") {
				entity->SetChinType(result2.GetInt8(1), 0, false);
				entity->SetChinType(result2.GetInt8(2), 1, false);
				entity->SetChinType(result2.GetInt8(3), 2, false);
			}
			else if (type == "nose_type") {
				entity->SetNoseType(result2.GetInt8(1), 0, false);
				entity->SetNoseType(result2.GetInt8(2), 1, false);
				entity->SetNoseType(result2.GetInt8(3), 2, false);
			}
			else if (type == "body_size") {
				entity->SetBodySize(result2.GetInt8(1), false);
			}
			else if (type == "soga_skin_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetSogaSkinColor(c, false);
			}
			else if (type == "soga_eye_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetSogaEyeColor(c, false);
			}
			else if (type == "soga_hair_color1") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetSogaHairColor1(c, false);
			}
			else if (type == "soga_hair_color2") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetSogaHairColor2(c, false);
			}
			else if (type == "soga_hair_highlight") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetSogaHairHighlight(c, false);
			}
			else if (type == "soga_hair_type_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetSogaHairColor(c, false);
			}
			else if (type == "soga_hair_type_highlight_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetSogaHairHighlightColor(c, false);
			}
			else if (type == "soga_hair_face_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetSogaFacialHairColor(c, false);
			}
			else if (type == "soga_hair_face_highlight_color") {
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				entity->SetSogaFacialHairHighlightColor(c, false);
			}
			else if (type == "soga_wing_color1") {
				/*
				Not in struct
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				*/
			}
			else if (type == "soga_wing_color2") {
				/*
				Not in struct
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				*/
			}
			else if (type == "soga_shirt_color") {
				/*
				Not in struct
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				*/
			}
			else if (type == "soga_unknown_chest_color") {
				/*
				Not in struct
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				*/
			}
			else if (type == "soga_pants_color") {
				/*
				Not in struct
				EQ2Color c;
				c.Red = result2.GetUInt8(1);
				c.Green = result2.GetUInt8(2);
				c.Blue = result2.GetUInt8(3);
				*/
			}
			else if (type == "soga_unknown_legs_color") {
				/*
				Not in struct
				EQ2Color c;
				c.soga_unknown_legs_color.Red = result2.GetUInt8(1);
				c.soga_unknown_legs_color.Green = result2.GetUInt8(2);
				c.soga_unknown_legs_color.Blue = result2.GetUInt8(3);
				*/
			}
			else if (type == "soga_unknown13") {
				entity->SetSogaSkullType(result2.GetInt8(1), 0, false);
				entity->SetSogaSkullType(result2.GetInt8(2), 1, false);
				entity->SetSogaSkullType(result2.GetInt8(3), 2, false);
			}
			else if (type == "soga_eye_type") {
				entity->SetSogaEyeType(result2.GetInt8(1), 0, false);
				entity->SetSogaEyeType(result2.GetInt8(2), 1, false);
				entity->SetSogaEyeType(result2.GetInt8(3), 2, false);
			}
			else if (type == "soga_ear_type") {
				entity->SetSogaEarType(result2.GetInt8(1), 0, false);
				entity->SetSogaEarType(result2.GetInt8(2), 1, false);
				entity->SetSogaEarType(result2.GetInt8(3), 2, false);
			}
			else if (type == "soga_eye_brow_type") {
				entity->SetSogaEyeBrowType(result2.GetInt8(1), 0, false);
				entity->SetSogaEyeBrowType(result2.GetInt8(2), 1, false);
				entity->SetSogaEyeBrowType(result2.GetInt8(3), 2, false);
			}
			else if (type == "soga_cheek_type") {
				entity->SetSogaCheekType(result2.GetInt8(1), 0, false);
				entity->SetSogaCheekType(result2.GetInt8(2), 1, false);
				entity->SetSogaCheekType(result2.GetInt8(3), 2, false);
			}
			else if (type == "soga_lip_type") {
				entity->SetSogaLipType(result2.GetInt8(1), 0, false);
				entity->SetSogaLipType(result2.GetInt8(2), 1, false);
				entity->SetSogaLipType(result2.GetInt8(3), 2, false);
			}
			else if (type == "soga_chin_type") {
				entity->SetSogaChinType(result2.GetInt8(1), 0, false);
				entity->SetSogaChinType(result2.GetInt8(2), 1, false);
				entity->SetSogaChinType(result2.GetInt8(3), 2, false);
			}
			else if (type == "soga_nose_type") {
				entity->SetSogaNoseType(result2.GetInt8(1), 0, false);
				entity->SetSogaNoseType(result2.GetInt8(2), 1, false);
				entity->SetSogaNoseType(result2.GetInt8(3), 2, false);
			}
		}
	}

	return ret;
}

bool ZoneDatabase::LoadNPCsForZone(ZoneServer* z) {
	DatabaseResult result;
	bool ret = Select(&result, "SELECT npc.spawn_id, s.name, npc.min_level, npc.max_level, npc.enc_level, s.race, s.model_type, npc.class_, npc.gender, s.command_primary, s.command_secondary, s.show_name, npc.min_group_size, npc.max_group_size, npc.hair_type_id, npc.facial_hair_type_id, npc.wing_type_id, npc.chest_type_id, npc.legs_type_id, npc.soga_hair_type_id, npc.soga_facial_hair_type_id, s.attackable, s.show_level, s.targetable, s.show_command_icon, s.display_hand_icon, s.hp, s.power, s.size, s.collision_radius, npc.action_state, s.visual_state, npc.mood_state, npc.initial_state, npc.activity_status, s.faction_id, s.sub_title, s.merchant_id, s.merchant_type, s.size_offset, npc.attack_type, npc.ai_strategy+0, npc.spell_list_id, npc.secondary_spell_list_id, npc.skill_list_id, npc.secondary_skill_list_id, npc.equipment_list_id, npc.str, npc.sta, npc.wis, npc.intel, npc.agi, npc.heat, npc.cold, npc.magic, npc.mental, npc.divine, npc.disease, npc.poison, npc.aggro_radius, npc.cast_percentage, npc.randomize, npc.soga_model_type, npc.heroic_flag, npc.alignment, npc.elemental, npc.arcane, npc.noxious, s.savagery, s.dissonance, npc.hide_hood, npc.emote_state, s.prefix, s.suffix, s.last_name\n"
									"FROM spawn s\n"
									"INNER JOIN spawn_npcs npc\n"
									"ON s.id = npc.spawn_id\n"
									"INNER JOIN spawn_location_entry le\n"
									"ON npc.spawn_id = le.spawn_id\n"
									"INNER JOIN spawn_location_placement lp\n"
									"ON le.spawn_location_id = lp.spawn_location_id\n"
									"WHERE lp.zone_id = %u\n"
									"GROUP BY s.id", z->GetID());
	if (!ret)
		return ret;

	uint32_t count = 0;
	while (result.Next()) {
		uint32_t id = result.GetUInt32(0);
		if (z->GetNPCFromMasterList(id))
			continue;

		std::shared_ptr<Entity> npc = std::make_shared<Entity>();
		npc->SetDatabaseID(id);
		npc->SetName(result.GetString(1));
		npc->SetLevel(result.GetUInt8(2));
		// TODO: min/max level
		//npc->SetMinLevel(result.GetUInt8(2));
		//npc->SetMaxLevel(result.GetUInt8(3));
		npc->SetDifficulty(result.GetUInt8(4));
		npc->SetRace(result.GetUInt8(5));
		npc->SetModelType(result.GetUInt32(6));
		npc->SetAdventureClass(result.GetUInt8(7));
		npc->SetGender(result.GetUInt8(8));
		// TODO: Load enity_commands table before loading spawns
		// primary commands - result.GetUint32(9);
		// secondary commands - result.GetUInt32(10);
		// Display name (vis flags in vis substruct) - result.GetUInt8(11)
		// 12 - min_group_size, no clue what this is for
		// 13 - max_group_size, no clue what this is for
		npc->SetHairType(result.GetUInt32(14));
		npc->SetFacialHairType(result.GetUInt32(15));
		npc->SetWingType(result.GetUInt32(16));
		npc->SetChestType(result.GetUInt32(17));
		npc->SetLegsType(result.GetUInt32(18));
		npc->SetSogaHairType(result.GetUInt32(19));
		npc->SetSogaFacialHairType(result.GetUInt32(20));
		// Attackable (vis flags in vis struct) - result.GetUInt8(21)
		// Show Level (vis flags in vis struct) - result.GetUInt8(22)
		// Targetable (vis flags in vis struct) - result.GetUInt8(23)
		// Show Command Icon (vis flags in vis struct) - result.GetUInt8(24)
		npc->SetHandFlag(result.GetUInt8(25));
		// Total HP - result.GetUInt32(26)
		// Total MP - result.GetUInt32(27)
		npc->SetSize(result.GetFloat(28));
		npc->SetCollisionRadius(result.GetUInt32(29));
		npc->SetActionState(result.GetUInt32(30));
		npc->SetVisualState(result.GetUInt32(31));
		npc->SetMoodState(result.GetUInt32(32));
		npc->SetState(result.GetUInt32(33));
		npc->SetEntityFlags(result.GetUInt32(34));
		// Faction ID - result.GetUInt32(35);
		if (!result.IsNull(36))
			npc->SetGuild(result.GetString(36));
		// Merchant ID - result.GetUInt32(37)
		// MerchantType - result.GetUInt8(38)
		// Size Offset (not used?) - result.GetUInt8(39)
		// Attack Type - result.GetUInt8(40)
		// AI Strategy - result.GetUInt8(41)
		// Primary Spell List - result.GetUInt32(42)
		// Secondary Spell List - result.GetUInt32(43)
		// Primary Skill List - result.GetUInt32(44)
		// Secondary Skill List - result.GetUInt32(45)
		// Equipment List ID - result.GetUInt32(46)
		// Base Str  - result.GetFloat(47)
		// Base Sta - result.GetFloat(48)
		// Base Wis - result.GetFloat(49)
		// Base Int - result.GetFloat(50)
		// Base Agi - result.GetFloat(51)
		// Base Heat - result.GetUInt32(52)
		// Base Cold - result.GetUInt32(53)
		// Base Magic - result.GetUInt32(54)
		// Base Mental  - result.GetUInt32(55)
		// Base Divine - result.GetUInt32(56)
		// Base Disease - result.GetUInt32(57)
		// Base Poison - result.GetUInt32(58)
		// Aggro Radius - result.GetFloat(59)
		// Cast Percentage - result.GetUInt8(60)
		// randomize - result.GetUint32(61)
		npc->SetSogaModelType(result.GetUInt32(62));
		npc->SetHeroicFlag(result.GetUInt8(63));
		// Alignment - result.GetUInt8(64)
		// Base Elemental - result.GetUInt32(65)
		// Base Arcane - result.GetUInt32(66)
		// Base Noxious - result.GetUInt32(67)
		// Total Savagery - result.GetUInt32(68)
		// Total Dissonance - result.GetUInt32(69)
		// Hide Hood (vis flags in info struct) - result.GetUInt8(70)
		npc->SetEmoteState(result.GetUInt32(71));
		if (!result.IsNull(72))
			npc->SetPrefixTitle(result.GetString(72));
		if (!result.IsNull(73))
			npc->SetSuffixTitle(result.GetString(73));
		if (!result.IsNull(74))
			npc->SetLastName(result.GetString(74));

		count++;

		z->AddNPCToMasterList(npc);
		LogDebug(LOG_NPC, 5, "---Loading NPC: '%s' (%u)", npc->GetName().c_str(), id);
	}

	LogInfo(LOG_NPC, 0, "--Loaded %u NPC(s).", count);

	return ret;
}
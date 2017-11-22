#pragma once
#include "Common.h"
#include "Skill.h"

ref struct Query;

ref struct Armor
{
	enum class ArmorType { HEAD = 0, BODY, ARMS, WAIST, LEGS, NumArmorTypes };
	System::String^ name;
	unsigned hr, elder_star, num_slots, defence, max_defence, rarity, difficulty, index;
	int ice_res, water_res, fire_res, thunder_res, dragon_res;
	Gender gender;
	HunterType type;
	bool torso_inc, no_skills, is_piercing, is_event;
	Ability^ danger;
	List_t< AbilityPair^ > abilities;
	List_t< MaterialComponent^ > components;

	bool IsBetterThan( Armor^ other, List_t< Ability^ >^ rel_abilities );
	int GetSkillAt( Ability^ ability );

	bool MatchesQuery( Query^ query, List_t< Ability^ >^ danger_skills, const unsigned max_slots );

	static void Load( System::String^ filename, ArmorType armor_type );
	static void LoadLanguage( System::String^ filename, ArmorType armor_type );
	static array< List_t< Armor^ >^ >^ static_armors;
	static Map_t< System::String^, Armor^ > static_armor_map;
	static Armor^ FindArmor( System::String^ name );
};

ref struct Charm
{
	List_t< AbilityPair^ > abilities;
	unsigned num_slots;
	bool custom, hacked;

	Charm() : num_slots( 0 ), custom( false ), hacked( false ) {}
	Charm( Charm^ other );
	Charm( const unsigned num_slots );

	bool StrictlyBetterThan( Charm^ other );
	bool BasicallyTheSameAs( Charm^ other );
	bool operator == ( Charm^ other );

	System::String^ GetName();
	unsigned GetHash();

	static unsigned HashFunction( const unsigned num_slots, Ability^ a1, const int p1, Ability^ a2, const int p2 );
};

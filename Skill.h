#pragma once
#include "Common.h"
#include <cliext/map>

ref struct Skill;
ref struct Query;
ref struct SkillTag;

ref struct Ability
{
	typedef System::Collections::Generic::Dictionary< int, Skill^ > SkillMap_t;
	System::String^ name;
	List_t< SkillTag^ > tags;
	bool relevant, auto_guard;
	unsigned order, static_index;
	SkillMap_t skills;

	Skill^ GetSkill( const int amount );

	static Ability^ torso_inc;
	static List_t< Ability^ > static_abilities, ordered_abilities;
	static Map_t< System::String^, Ability^ > static_ability_map;
	static Ability^ FindAbility( System::String^ name );
};

ref struct Skill
{
	System::String^ name;
	int points_required;
	Ability^ ability;

	static void Load( System::String^ filename );
	static void LoadLanguage( System::String^ filename );
	static List_t< Skill^ > static_skills, ordered_skills;
	static Map_t< System::String^, Skill^ > static_skill_map;
	static Skill^ FindSkill( System::String^ name );
};

ref struct AbilityPair
{
	Ability^ ability;
	int amount;

	AbilityPair( Ability^ ab, const int am )
		: ability( ab ), amount( am ) {}
};


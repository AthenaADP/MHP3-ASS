#pragma once
#include "Common.h"
#include "Skill.h"

ref struct Query;

ref struct Decoration
{
	System::String^ name;
	unsigned hr, elder_star, slots_required, rarity, difficulty, index;
	bool is_event;

	List_t< AbilityPair^ > abilities;
	List_t< MaterialComponent^ > components, components2;

	bool IsBetterThan( Decoration^ other, List_t< Ability^ >^ rel_abilities );
	int GetSkillAt( Ability^ ability );

	bool MatchesQuery( Query^ query );
	
	static void Load( System::String^ filename );
	static void LoadLanguage( System::String^ filename );
	static List_t< Decoration^ > static_decorations;
	static Map_t< Ability^, List_t< Decoration^ >^ > static_decoration_ability_map;
	static Map_t< System::String^, Decoration^ > static_decoration_map;
	static Decoration^ FindDecoration( System::String^ name );
	static Decoration^ GetBestDecoration( Ability^ ability, const unsigned max_slots, List_t< List_t< Decoration^ >^ >% rel_deco_map );
	static Decoration^ GetBestDecoration( Ability^ ability, const unsigned max_slots, Query^ query );
};

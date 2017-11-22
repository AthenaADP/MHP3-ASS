#pragma once
#include "Common.h"
#include <cliext/utility>

ref struct Skill;
ref struct Ability;
ref struct Armor;
ref struct Charm;
ref struct ArmorEquivalence;
ref struct Decoration;
ref struct AbilityPair;

typedef System::Collections::Generic::Dictionary< Ability^, int > AbilityMap;

ref struct Query : public System::Object
{
	List_t< Skill^ > skills;
	HunterType hunter_type;
	Gender gender;
	unsigned hr, elder_star, weapon_slots_allowed;
	bool include_piercings, allow_bad, allow_event, allow_low_rank;

	List_t< List_t< Armor^ >^ > rel_armor, inf_armor;
	List_t< Ability^ > rel_abilities;
	List_t< Decoration^ > rel_decorations, inf_decorations;
};


ref struct QueryCharmPair
{
	Query^ query;
	Charm^ charm_template;

	QueryCharmPair( Query^ q, Charm^ c ) : query( q ), charm_template( c ) {}
};

ref struct CalculationData;

ref struct Solution
{
	List_t< Armor^ > armors;
	List_t< Decoration^ > decorations, body_decorations, non_body_decorations;
	List_t< Skill^ > extra_skills, bad_skills;
	AbilityMap abilities;
	Charm^ charm;

	unsigned torso_slots_spare, torso_multiplier, total_slots_spare;
	int fire_res, ice_res, water_res, thunder_res, dragon_res;
	unsigned defence, max_defence, rarity, difficulty;
	array< unsigned >^ slots_spare;
	bool doubled;

	bool MatchesQuery( Query^ query );
	void CalculateData();

private:
	CalculationData^ data;

	bool FixBadSkill( Skill^ skill );
	bool FixBadSkills();
	bool CheckBadSkills();
	Decoration^ Count1SocketGems( Ability^ ability, int& num );
	int GetReplacable( Ability^ ability, unsigned slots );
	bool ReorderGems();
	void Replace( const unsigned index, Decoration^ decoration );
	void CalculateExtraSkills();
	void ReduceSlots();
	void ReduceSkills();
	void ReduceCharm();
	void RearrangeDecorations();
};

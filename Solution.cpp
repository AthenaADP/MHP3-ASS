#include "stdafx.h"
#include "Solution.h"
#include "Armor.h"
#include "Decoration.h"
#include "Skill.h"

using namespace System;

ref struct CalculationData
{
	Query^ query;
	Solution^ solution;
	AbilityMap need;
	Map_t< Skill^, int > relevant;
	List_t< List_t< Decoration^ >^ > rel_decoration_map;
};

bool Detrimental( Decoration^ dec, CalculationData^ data )
{
	if( dec->abilities.Count < 2 || !dec->abilities[ 1 ]->ability->relevant )
		return false;

	const int need = data->need[ dec->abilities[ 1 ]->ability ];
	const int have = data->solution->abilities[ dec->abilities[ 1 ]->ability ];
	return have + dec->abilities[ 1 ]->amount < need;
}

void AddAbilities( AbilityMap% total, List_t< AbilityPair^ >% abilities, const int multiplier )
{
	for each( AbilityPair^ ap in abilities )
	{
		if( total.ContainsKey( ap->ability ) )
			total[ ap->ability ] += ap->amount * multiplier;
		else
			total.Add( ap->ability, ap->amount * multiplier );
	}
}

bool AddDecoration( Solution^ solution, Decoration^ deco, const int multiplier, List_t< Decoration^ >% deco_list )
{
	solution->decorations.Add( deco );
	deco_list.Add( deco );
	AddAbilities( solution->abilities, deco->abilities, multiplier );
	solution->total_slots_spare -= deco->slots_required;
	return solution->abilities[ deco->abilities[ 0 ]->ability ] > -10;
}

Decoration^ GetBestAvailableDecoration( CalculationData^ data, Ability^ ability, const unsigned slots_spare )
{
	return ability->relevant
		? Decoration::GetBestDecoration( ability, slots_spare, data->rel_decoration_map )
		: Decoration::GetBestDecoration( ability, slots_spare, data->query );
}

bool Solution::FixBadSkill( Skill^ skill )
{
	while( torso_slots_spare > 0 )
	{
		Decoration^ dec = GetBestAvailableDecoration( data, skill->ability, torso_slots_spare );
		if( !dec || Detrimental( dec, data ) )
			return false;
		torso_slots_spare -= dec->slots_required;
		if( AddDecoration( data->solution, dec, torso_multiplier, body_decorations ) )
			return true;
	}

	int max_slots = 3;
	while( max_slots > 0 && !slots_spare[ max_slots ] ) max_slots--;

	if( max_slots == 0 ) return false;

	Decoration^ dec = GetBestAvailableDecoration( data, skill->ability, max_slots );
	if( !dec || Detrimental( dec, data ) )
		return false;

	while( max_slots > 0 )
	{
		if( slots_spare[ dec->slots_required ] == 0 )
		{
			if( slots_spare[ dec->slots_required + 1 ] == 0 )
			{
				slots_spare[ 3 ]--;
				slots_spare[ 2 ]++;
			}
			else
			{
				slots_spare[ dec->slots_required + 1 ]--;
				slots_spare[ 1 ]++;
			}
		}
		else slots_spare[ dec->slots_required ]--;

		if( AddDecoration( data->solution, dec, 1, non_body_decorations ) )
			return true;

		while( max_slots > 0 && !slots_spare[ max_slots ] )
			dec = GetBestAvailableDecoration( data, skill->ability, --max_slots );
	}
	return false;
}


bool Solution::FixBadSkills()
{
	for each( Skill^ skill in bad_skills )
		if( !FixBadSkill( skill ) )
			return false;
	CalculateExtraSkills();
	return true;
}

bool Solution::CheckBadSkills()
{
	return bad_skills.Count == 0 || ( total_slots_spare > 0 && FixBadSkills() ) || ReorderGems();
}

Decoration^ Solution::Count1SocketGems( Ability^ ability, int& num )
{
	num = 0;
	Map_t< Decoration^, unsigned > deco_count;
	Decoration^ best = nullptr;
	for each( Decoration^ dec in decorations )
	{
		if( dec->slots_required == 1 && dec->abilities.Count == 2 && dec->abilities[ 1 ]->ability == ability )
		{
			if( deco_count.ContainsKey( dec ) )
			{
				const int n = ++deco_count[ dec ];
				if( n > num )
				{
					num = n;
					best = dec;
				}
			}
			else
			{
				deco_count.Add( dec, 1 );
				if( num == 0 )
				{
					num = 1;
					best = dec;
				}
			}
		}
	}
	return best;
}

int Solution::GetReplacable( Ability^ ability, unsigned slots )
{
	for( int i = 0; i < decorations.Count; ++i )
		if( decorations[ i ]->slots_required == slots && decorations[ i ]->abilities.Count > 1 && decorations[ i ]->abilities[ 1 ]->ability != ability )
			return i;
	return -1;
}

bool Solution::ReorderGems()
{
	//return false;
	for each( Skill^ skill in bad_skills )
	{
		if( abilities[ skill->ability ] < -9 )
		{
			int num_1_socketed;
			Decoration^ one_slot = Count1SocketGems( skill->ability, num_1_socketed );
			if( num_1_socketed == 0 ) return false;
			
			Decoration^ bigger_slotted = GetBestAvailableDecoration( data, one_slot->abilities[ 0 ]->ability, 3 );
			const int num_required = bigger_slotted->abilities[ 0 ]->amount;
			if( num_1_socketed < num_required ) return false;
			
			const int other = GetReplacable( skill->ability, bigger_slotted->slots_required );
			Assert( other == -1 || decorations[ other ]->abilities.Count == 2, L"Failed to replace gems" );
			if( other != -1 && num_1_socketed >= decorations[ other ]->abilities[ 0 ]->amount && abilities[ decorations[ other ]->abilities[ 1 ]->ability ] > -8 )
			{
				Decoration^ other_one_slot = Decoration::static_decoration_ability_map[ decorations[ other ]->abilities[ 0 ]->ability ][ 0 ];
				Replace( other, bigger_slotted );
				int num_replaced = 0;
				int num_to_replace = decorations[ other ]->abilities[ 0 ]->amount > num_required ? decorations[ other ]->abilities[ 0 ]->amount : num_required;
				for( int i = 0; i < decorations.Count; ++i )
					if( decorations[ i ] == one_slot && num_replaced++ < num_to_replace )
						Replace( i, other_one_slot );
				if( abilities[ skill->ability ] < -9 )
					return false;
			}
			else return false;
		}
	}
	CalculateExtraSkills();
	return true;
}

void Solution::Replace( const unsigned index, Decoration^ decoration )
{
	for each( AbilityPair^ apair in decorations[ index ]->abilities )
	{
		abilities[ apair->ability ] -= apair->amount;
	}
	decorations[ index ] = decoration;
	for each( AbilityPair^ apair in decoration->abilities )
	{
		if( abilities.ContainsKey( apair->ability ) )
			abilities[ apair->ability ] += apair->amount;
		else abilities.Add( apair->ability, apair->amount );
	}
}

void AddInitialArmor( Armor^ armor, Solution^ solution, const int multiplier )
{
	if( armor )
	{
		solution->slots_spare[ armor->num_slots ] += ( armor != solution->armors[ 1 ] ) * !!armor->num_slots;
		solution->defence += armor->defence;
		solution->max_defence += armor->max_defence;
		solution->ice_res += armor->ice_res;
		solution->fire_res += armor->fire_res;
		solution->water_res += armor->water_res;
		solution->dragon_res += armor->dragon_res;
		solution->thunder_res += armor->thunder_res;
		solution->rarity += armor->rarity;
		solution->difficulty += armor->difficulty;
		AddAbilities( solution->abilities, armor->abilities, multiplier );
		solution->torso_multiplier += armor->torso_inc;
		solution->total_slots_spare += armor->num_slots;
	}
}

void GetInitialData( CalculationData^ data )
{
	for each( Skill^ skill in data->query->skills )
	{
		data->need.Add( skill->ability, skill->points_required );
		data->relevant.Add( skill, 0 );
		data->solution->abilities.Add( skill->ability, 0 );
	}
	if( data->solution->charm )
	{
		for each( AbilityPair^ ap in data->solution->charm->abilities )
		{
			if( !data->need.ContainsKey( ap->ability ) )
				data->need.Add( ap->ability, 0 );
		}
	}

	data->solution->total_slots_spare = data->query->weapon_slots_allowed;
	data->solution->torso_multiplier = 1;
	data->solution->slots_spare = gcnew array< unsigned >( 4 );
	
	data->solution->slots_spare[ data->query->weapon_slots_allowed ] += !!data->query->weapon_slots_allowed;
	
	if( data->solution->armors[ 1 ] )
		data->solution->torso_slots_spare = data->solution->armors[ 1 ]->num_slots;
	else data->solution->torso_slots_spare = 0;

	data->solution->defence = data->solution->max_defence = data->solution->fire_res = data->solution->ice_res = data->solution->water_res
		= data->solution->thunder_res = data->solution->dragon_res = data->solution->rarity = data->solution->difficulty = 0;

	AddInitialArmor( data->solution->armors[ 0 ], data->solution, 1 );
	AddInitialArmor( data->solution->armors[ 2 ], data->solution, 1 );
	AddInitialArmor( data->solution->armors[ 3 ], data->solution, 1 );
	AddInitialArmor( data->solution->armors[ 4 ], data->solution, 1 );
	AddInitialArmor( data->solution->armors[ 1 ], data->solution, data->solution->torso_multiplier );
	if( data->solution->charm )
	{
		data->solution->slots_spare[ data->solution->charm->num_slots ] += !!data->solution->charm->num_slots;
		data->solution->total_slots_spare += data->solution->charm->num_slots;
		AddAbilities( data->solution->abilities, data->solution->charm->abilities, 1 );
	}
}

template< bool allow_bad, bool allow_overkill >
void AddDecorations1( CalculationData^ data, unsigned% num_slots, const int multiplier, List_t< Decoration^ >% deco_list )
{
	List_t< Decoration^ >^ rel_decorations = data->rel_decoration_map[ 1 ];
	if( num_slots == 0 || rel_decorations->Count == 0 ) return;
	bool loop = true;
	int i = 0;
	while( loop )
	{
		Decoration^ decoration = rel_decorations[ i ];
		if( !Detrimental( decoration, data ) )
		{
			AbilityPair^ apair = decoration->abilities[ 0 ];
			Assert( apair->amount > 0, L"Decoration's first skill is negative?" );

			if( allow_bad || decoration->abilities.Count < 2 || !data->solution->abilities.ContainsKey( decoration->abilities[ 1 ]->ability ) ||
				data->solution->abilities[ decoration->abilities[ 1 ]->ability ] + decoration->abilities[ 1 ]->amount * multiplier > -10 )
			{
				const int current_amount = data->solution->abilities[ apair->ability ];
				const int decoration_amount = apair->amount * multiplier;
				const int needed_amount = data->need[ apair->ability ];

				if( decoration_amount > 0 && current_amount < needed_amount && ( allow_overkill ? ( current_amount < needed_amount ) : ( current_amount + decoration_amount <= needed_amount + apair->amount - 1 ) ) )
				{
					loop = true;
					AddDecoration( data->solution, decoration, multiplier, deco_list );
					if( --num_slots == 0 ) return;
					--i;
				}
			}
		}
		
		if( ++i == rel_decorations->Count )
		{
			loop = false;
			i = 0;
		}
	}
}

ref struct PossibleDecoration
{
	Decoration^ decoration;
	int points_given;
};

generic< typename T >
T GetLast( List_t< T >% l )
{
	return l[ l.Count - 1 ];
}

//pair( good, bad )
cliext::pair< int, int > GetScore( Decoration^ decoration, CalculationData^ data, const unsigned actual_good )
{
	static int point_score[ 6 ] = { 0, 1, 2, 3, 4, 3 };
	const int bad = decoration->abilities.Count == 1 ? 0 : ( data->solution->abilities.ContainsKey( GetLast( decoration->abilities )->ability ) ? data->solution->abilities[ GetLast( decoration->abilities )->ability ] : 0 );
	return cliext::make_pair( point_score[ actual_good ], bad );
}

PossibleDecoration^ GetBestDecoration( List_t< PossibleDecoration^ >% decorations, CalculationData^ data )
{
	unsigned best = 0;
	cliext::pair< int, int > score = GetScore( decorations[ best ]->decoration, data, decorations[ best ]->points_given );
	for( int i = 1; i < decorations.Count; ++i )
	{
		cliext::pair< int, int > curr_score = GetScore( decorations[ i ]->decoration, data, decorations[ i ]->points_given );
		if( curr_score.first >= score.first || curr_score.first == score.first && curr_score.second > score.second )
		{
			score = curr_score;
			best = i;
		}
	}
	return decorations[ best ];
}

bool IsCharmAbility( Charm^ charm, Ability^ ab )
{
	return charm && charm->abilities.Count > 0 && charm->abilities[ 0 ]->ability == ab;
}

template< bool allow_bad, bool allow_detrimental >
void AddDecorations23( CalculationData^ data, const unsigned size, unsigned& num_slots, const int multiplier, List_t< Decoration^ >% deco_list )
{
	List_t< Decoration^ >^ rel_decorations = data->rel_decoration_map[ size ];
	if( num_slots == 0 || rel_decorations->Count == 0 ) return;
	bool loop = true;
	int i = 0;
	List_t< PossibleDecoration^ > possible_decorations;
	while( loop )
	{
		Decoration^ deco = rel_decorations[ i ];

		if( allow_detrimental || !Detrimental( deco, data ) )
		{
			AbilityPair^ apair = deco->abilities[ 0 ];
			Assert( apair->amount > 0, L"Decoration's first skill is negative!?" );

			if( allow_bad || deco->abilities.Count < 2 || !data->solution->abilities.ContainsKey( deco->abilities[ 1 ]->ability ) ||
				data->solution->abilities[ deco->abilities[ 1 ]->ability ] + deco->abilities[ 1 ]->amount * multiplier > -10 )
			{
				const int current_amount = data->solution->abilities[ apair->ability ];
				const int decoration_amount = apair->amount * multiplier;
				const int needed_amount = data->need[ apair->ability ];
				if( decoration_amount > 0 && current_amount < needed_amount && current_amount + decoration_amount <= needed_amount + 1 )
				{
					loop = true;
					PossibleDecoration^ possible = gcnew PossibleDecoration;
					possible->decoration = deco;
					if( IsCharmAbility( data->solution->charm, apair->ability ) )
						possible->points_given = decoration_amount;
					else
						possible->points_given = Min( decoration_amount, needed_amount - current_amount );
					possible_decorations.Add( possible );
				}
			}
		}
		
		if( ++i == rel_decorations->Count )
		{
			loop = false;
			i = 0;
		}
	}
	if( possible_decorations.Count > 0 )
	{
		PossibleDecoration^ best = GetBestDecoration( possible_decorations, data );
		const bool good_deco = allow_bad || !Detrimental( best->decoration, data );
		Assert( good_deco, L"Best decoration isn't good?" );
		AddDecoration( data->solution, best->decoration, multiplier, deco_list );
		if( --num_slots > 0 )
			AddDecorations23< allow_bad, allow_detrimental >( data, size, num_slots, multiplier, deco_list );
	}
}

template< bool allow_bad, bool allow_detrimental >
void CalculateDecorations( CalculationData^ data )
{
	unsigned body_slots[ 4 ] = { 0, 0, 0, 0 };
	unsigned temp_slots[ 4 ] = { 0, 0, 0, 0 };
	//get initial slot configuration and sort decorations by size
	for( int i = 1; i < 4; ++i )
	{
		temp_slots[ i ] = data->solution->slots_spare[ i ];
	}
	
	body_slots[ data->solution->torso_slots_spare ] += !!data->solution->torso_slots_spare;

	//try adding decorations
	AddDecorations23< allow_bad, allow_detrimental >( data, 3, body_slots[ 3 ], data->solution->torso_multiplier, data->solution->body_decorations );
	body_slots[ 1 ] += body_slots[ 3 ];
	body_slots[ 2 ] += body_slots[ 3 ];
	body_slots[ 3 ] = 0;
	AddDecorations23< allow_bad, allow_detrimental >( data, 3, temp_slots[ 3 ], 1, data->solution->non_body_decorations );
	data->solution->slots_spare[ 3 ] -= data->solution->slots_spare[ 3 ] - temp_slots[ 3 ];
	temp_slots[ 1 ] += temp_slots[ 3 ];
	temp_slots[ 2 ] += temp_slots[ 3 ];
	temp_slots[ 3 ] = 0;

	AddDecorations23< allow_bad, allow_detrimental >( data, 2, body_slots[ 2 ], data->solution->torso_multiplier, data->solution->body_decorations );
	body_slots[ 1 ] += 2 * body_slots[ 2 ];
	body_slots[ 2 ] = 0;
	{
		const unsigned temp = temp_slots[ 2 ];
		AddDecorations23< allow_bad, allow_detrimental >( data, 2, temp_slots[ 2 ], 1, data->solution->non_body_decorations );
		const unsigned num_2slot_decorations = temp - temp_slots[ 2 ];
		if( num_2slot_decorations > data->solution->slots_spare[ 2 ] )
		{
			const unsigned to_convert = num_2slot_decorations - data->solution->slots_spare[ 2 ];
			data->solution->slots_spare[ 3 ] -= to_convert;
			data->solution->slots_spare[ 2 ] = 0;
			data->solution->slots_spare[ 1 ] += to_convert;
		}
		else data->solution->slots_spare[ 2 ] -= num_2slot_decorations;
	}
	temp_slots[ 1 ] += 2 * temp_slots[ 2 ];
	temp_slots[ 2 ] = 0;

	AddDecorations1< allow_bad, false >( data, body_slots[ 1 ], data->solution->torso_multiplier, data->solution->body_decorations );
	{
		const unsigned temp = temp_slots[ 1 ];
		AddDecorations1< allow_bad, true >( data, temp_slots[ 1 ], 1, data->solution->non_body_decorations );
		unsigned num_1slot_decorations = temp - temp_slots[ 1 ];
		if( num_1slot_decorations > data->solution->slots_spare[ 1 ] )
		{
			unsigned extra = num_1slot_decorations - data->solution->slots_spare[ 1 ];
			if( extra > data->solution->slots_spare[ 2 ] * 2 )
			{
				const unsigned extra3 = extra - data->solution->slots_spare[ 2 ] * 2;
				const unsigned num_3_convert = extra3 / 3 + !!( extra3 % 3 );
				data->solution->slots_spare[ 3 ] -= num_3_convert;
				if( num_3_convert * 3 > extra )
				{
					const unsigned super_extra = num_3_convert * 3 - extra;
					data->solution->slots_spare[ 2 ] += super_extra / 2;
					data->solution->slots_spare[ 1 ] += super_extra % 2;
					num_1slot_decorations -= extra;
					extra = 0;
				}
				else
				{
					extra -= num_3_convert * 3;
					num_1slot_decorations -= num_3_convert * 3;
				}
			}
			const unsigned num_2_slots = extra / 2 + extra % 2;
			data->solution->slots_spare[ 2 ] -= num_2_slots;
			data->solution->slots_spare[ 1 ] -= num_1slot_decorations - num_2_slots * 2;
		}
		else data->solution->slots_spare[ 1 ] -= num_1slot_decorations;
		if( body_slots[ 1 ] )
			AddDecorations1< allow_bad, true >( data, body_slots[ 1 ], data->solution->torso_multiplier ,data->solution->body_decorations );
	}
	data->solution->torso_slots_spare = body_slots[ 1 ];
}

unsigned Num1SlotDecorations( Solution^ sol, Ability^ ab )
{

	unsigned num = 0;
	for each( Decoration^ deco in sol->non_body_decorations )
	{
		if( deco->slots_required == 1 && deco->abilities[ 0 ]->ability == ab )
			num++;
	}
	if( sol->torso_multiplier == 1 )
	{
		for each( Decoration^ deco in sol->body_decorations )
		{
			if( deco->slots_required == 1 && deco->abilities[ 0 ]->ability == ab )
				num++;
		}
	}
	return num;
}

int PotentialSpareSlots( CalculationData^ data, const unsigned slot_size )
{
	int count = 0;
	for each( Armor^ arm in data->solution->armors )
	{
		count += arm->num_slots == slot_size;
	}
	count += data->query->weapon_slots_allowed == slot_size;
	count += data->solution->charm->num_slots == slot_size;
	for each( Decoration^ deco in data->solution->decorations )
	{
		count -= deco->slots_required == slot_size;
	}
	return count;
}

bool RoomFor3Slot( CalculationData^ data )
{
	const int free3 = PotentialSpareSlots( data, 3 );
	Assert( free3 >= 0, L"Not enough room for 3-slot gem(1)" );
	return free3 > 0 && free3 + PotentialSpareSlots( data, 2 ) > 0;
}

bool RoomFor2Slot( CalculationData^ data )
{
	const int free3 = PotentialSpareSlots( data, 3 );
	Assert( free3 >= 0, L"Not enough room for 3-slot gem(2)" );
	return free3 + PotentialSpareSlots( data, 2 ) > 0;
}

void SwapOut( Solution^ sol, Decoration^ deco )
{
	unsigned count = 0;
	for( int i = 0; count < deco->slots_required; ++i )
	{
		Assert( i < sol->decorations.Count, L"Overrunning decoration count" );
		if( sol->decorations[ i ]->slots_required == 1 && sol->decorations[ i ]->abilities[ 0 ]->ability == deco->abilities[ 0 ]->ability )
		{
			for each( AbilityPair^ ap in sol->decorations[ i ]->abilities )
			{
				sol->abilities[ ap->ability ] -= ap->amount; 
			}
			sol->decorations.RemoveAt( i-- );
			count++;
		}
	}
	for each( AbilityPair^ ap in deco->abilities )
	{
		sol->abilities[ ap->ability ] += ap->amount; 
	}
	sol->decorations.Add( deco );
}

void Solution::ReduceSlots()
{
	if( total_slots_spare > 0 )
	{
		unsigned i = charm->num_slots;
		while( i <= 3 && !slots_spare[ i ] ) ++i;
		if( i <= 3 )
		{
			//if( i > charm->num_slots )
			slots_spare[ i - charm->num_slots ]++;
			slots_spare[ i ]--;
			total_slots_spare -= charm->num_slots;
			charm->num_slots = 0;
		}
	}
}

void Solution::ReduceSkills()
{
	if( total_slots_spare > 0 )
	{
		for each( AbilityPair^ ap in charm->abilities )
		{
			data->need[ ap->ability ] += ap->amount;
		}
		CalculateDecorations< false, false >( data );
		for each( AbilityPair^ ap in charm->abilities )
		{
			data->need[ ap->ability ] -= ap->amount;
		}
	}
}

void Solution::RearrangeDecorations()
{
	for each( AbilityPair^ ap in charm->abilities )
	{
		unsigned num1 = Num1SlotDecorations( this, ap->ability );
		Decoration^ best3 = Decoration::GetBestDecoration( ap->ability, 3, data->rel_decoration_map );
		Decoration^ best2 = Decoration::GetBestDecoration( ap->ability, 2, data->rel_decoration_map );
		bool replaced = false;
		do 
		{
			replaced = false;
			if( num1 >= 3 )
			{
				if( best3 && best3->slots_required == 3 && RoomFor3Slot( data ) )
				{
					replaced = true;
					SwapOut( this, best3 );
					num1 -= 3;
				}
				else if( best2 && best2->slots_required == 2 && RoomFor2Slot( data ) )
				{
					replaced = true;
					SwapOut( this, best2 );
					num1 -= 2;
				}
			}
			else if( num1 == 2 )
			{
				if( best2 && best2->slots_required == 2 && RoomFor2Slot( data ) )
				{
					replaced = true;
					SwapOut( this, best2 );
					num1 -= 2;
				}
			}
		}
		while ( replaced );
	}
}

void Solution::ReduceCharm()
{
	for each( AbilityPair^ ap in charm->abilities )
	{
		const int need = data->need[ ap->ability ];
		if( need > 0 )
		{
			const int have = abilities[ ap->ability ];
			const int extra = have - need;
			Assert( extra >= 0, L"Reducing charm has caused solution to fail!" );
			const int take_off = extra >= ap->amount ? ap->amount : extra;
			ap->amount -= take_off;
			abilities[ ap->ability ] -= take_off;
		}
	}

	for( int i = 0; i < charm->abilities.Count; ++i )
	{
		if( charm->abilities[ i ]->amount == 0 )
		{
			charm->abilities.RemoveAt( i-- );
		}
	}
	if( charm->num_slots == 0 && charm->abilities.Count == 0 )
		charm = nullptr;
}

bool Solution::MatchesQuery( Query^ query )
{
	data = gcnew CalculationData;
	data->query = query;
	data->solution = this;
	for( int i = 0; i < 4; ++i )
		data->rel_decoration_map.Add( gcnew List_t< Decoration^ > );
	for each( Decoration^ deco in query->rel_decorations )
	{
		data->rel_decoration_map[ deco->slots_required ]->Add( deco );
	}
	
	GetInitialData( data );
	CalculateDecorations< true, true >( data );
	for each( Skill^ skill in query->skills )
	{
		const int have = abilities[ skill->ability ],
				  need = data->need[ skill->ability ];
		if( have < need )
			return false;
	}
	CalculateExtraSkills();

	if( !query->allow_bad && !CheckBadSkills() )
		return false;
	
	//reduce charms
	if( charm )
	{
		if( charm->custom )
			RearrangeDecorations();
		else
		{
			ReduceSlots();
			ReduceSkills();

			RearrangeDecorations();

			ReduceCharm();

			CalculateExtraSkills();
			Assert( query->allow_bad || bad_skills.Count == 0, L"A bad skill crept in?" );

#ifdef _DEBUG
			for each( Skill^ skill in query->skills )
			{
				const int have = abilities[ skill->ability ],
						  need = data->need[ skill->ability ];
				if( have < need )
				{
					System::Windows::Forms::MessageBox::Show( L"Not enough skill points!!!" );
				}
			}
#endif
		}
	}
	
	slots_spare[ torso_slots_spare ]++;
	Assert( total_slots_spare < 1000, L"Total slots spare is negative" );
	for each( Decoration^ deco in decorations )
	{
		difficulty += deco->difficulty;
	}
	return true;
}

void Solution::CalculateExtraSkills()
{
	extra_skills.Clear();
	bad_skills.Clear();
	for each( Collections::Generic::KeyValuePair< Ability^, int >^ ap in abilities )
	{
		Skill^ sk = ap->Key->GetSkill( ap->Value );
		if( sk && !data->relevant.ContainsKey( sk ) )
		{
			if( ap->Value < 0 && !data->query->allow_bad )
				bad_skills.Add( sk );
			else extra_skills.Add( sk );
		}
	}
}

void Solution::CalculateData()
{
	fire_res = ice_res = thunder_res = water_res = dragon_res = defence = rarity = difficulty = max_defence = 0;
	for each( Armor^ armor in armors )
	{
		fire_res += armor->fire_res;
		ice_res += armor->ice_res;
		thunder_res += armor->thunder_res;
		water_res += armor->water_res;
		dragon_res += armor->dragon_res;
		defence += armor->defence;
		rarity += armor->rarity;
		difficulty += armor->difficulty;
		max_defence += armor->max_defence;
	}
}

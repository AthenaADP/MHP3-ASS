#include "stdafx.h"
#include <fstream>
#include "Skill.h"
#include "Solution.h"
#include "LoadedData.h"

using namespace System;

Skill^ Ability::GetSkill( const int amount )
{
	if( amount == 0 ) return nullptr;
	int best = 0;
	SkillMap_t::Enumerator iter = skills.GetEnumerator();
	if( amount > 0 )
	{
		while( iter.MoveNext() )
			if( iter.Current.Key <= amount && iter.Current.Key > best )
				best = iter.Current.Key;
	}
	else
	{
		while( iter.MoveNext() )
			if( iter.Current.Key >= amount && iter.Current.Key < best )
				best = iter.Current.Key;
	}
	if( best == 0 ) return nullptr;
	Assert( skills.ContainsKey( best ), L"Skill dictionary doesn't contain this skill?" );
	return skills[ best ];
}
/*
bool Ability::MatchesQuery( Query^ query )
{
	SkillMap_t::Enumerator iter = skills.GetEnumerator();
	while( iter.MoveNext() )
	{
		if( iter.Current.Key > 0 && Utility::Contains( %query->skills, iter.Current.Value ) )
			return true;
	}
	return false;
}*/

Ability^ Ability::FindAbility( System::String^ name )
{
	if( name && static_ability_map.ContainsKey( name ) )
		return static_ability_map[ name ];
	return nullptr;
}

bool ContainsString( List_t< String^ >% vec, String^ item )
{
	for( int i = 0; i < vec.Count; ++i )
		if( vec[ i ] == item ) return true;
	return false;
}

int FindSkillIndex( Skill^ skill, List_t< Skill^ >^ vec )
{
	for( int i = 0; i < vec->Count; ++i )
		if( vec[ i ] == skill )
			return i;
	return -1;
}

void Skill::Load( String^ filename )
{
	IO::StreamReader fin( filename );

	Ability::static_abilities.Clear();
	Ability::static_ability_map.Clear();
	static_skills.Clear();
	static_skill_map.Clear();
	ordered_skills.Clear();
	Ability::static_abilities.Capacity = 128;
	static_skills.Capacity = 256;

	//skill,jap,ability,jap,points,type012
	String^ temp;
	while( !fin.EndOfStream && temp != L"" )
	{
		temp = fin.ReadLine();
		if( temp == L"" ) break;
		else if( temp[ 0 ] == L'#' ) continue;
		List_t< String^ > split;
		Utility::SplitString( %split, temp, L',' );
		Skill^ skill = gcnew Skill;
		skill->name = split[ 0 ];
		//skill->jname = split[ 1 ];
		if( split[ 2 ] == L"" )
		{
			Assert( !Ability::torso_inc, L"Multiple Torso Inc skills in data file" );
			Ability::torso_inc = gcnew Ability;
			Ability::torso_inc->name = split[ 0 ];
			//Ability::torso_inc->jname = split[ 1 ];
			Ability::torso_inc->static_index = Ability::static_abilities.Count;
			Ability::static_abilities.Add( Ability::torso_inc );
			Ability::static_ability_map[ Ability::torso_inc->name ] = Ability::torso_inc;
			//Ability::static_ability_map[ Ability::torso_inc->jname ] = Ability::torso_inc;
			continue;
		}
		skill->points_required = Convert::ToInt32( split[ 4 ] );
		skill->ability = Ability::FindAbility( split[ 2 ] );
		if( !skill->ability )
		{
			Ability^ ability = gcnew Ability;
			ability->name = split[ 2 ];
			ability->auto_guard = ability->name == L"Auto-Guard";
			//ability->jname = split[ 3 ];
			ability->static_index = Ability::static_abilities.Count;
			Ability::static_abilities.Add( ability );
			Ability::static_ability_map[ ability->name ] = ability;
			//Ability::static_ability_map[ ability->jname ] = ability;
			skill->ability = ability;
			if( split.Count != 8 )
				throw gcnew Exception( L"Ability " + ability->name + L" has not enough data" );
			if( split[ 6 ] != L"" )
			{
				SkillTag^ tag = SkillTag::FindTag( split[ 6 ] );
				if( !tag )
					throw gcnew Exception( L"Skill Tag '" + split[ 6 ] + L"' does not exist" );
				ability->tags.Add( tag );	
			}
			if( split[ 7 ] != L"" )
				ability->order = Convert::ToUInt32( split[ 7 ] );
			else
				throw gcnew Exception( L"Ability " + ability->name + L" is missing order number" );
		}
		skill->ability->skills[ skill->points_required ] = skill;
		static_skills.Add( skill );
		static_skill_map[ skill->name ] = skill;
		//static_skill_map[ skill->jname ] = skill;
	}
	
	fin.Close();
	static_skills.TrimExcess();
	Ability::static_abilities.TrimExcess();

	for( int i = 0; i < Ability::static_abilities.Count; ++i )
		Ability::ordered_abilities.Add( nullptr );

	for( int i = 0; i < Ability::static_abilities.Count; ++i )
		Ability::ordered_abilities[ Ability::static_abilities[ i ]->order ] = Ability::static_abilities[ i ];

	for( int i = 0; i < Ability::static_abilities.Count; ++i )
		if( Ability::ordered_abilities[ i ] == nullptr )
			throw gcnew Exception( L"Bad ordering of abilities" );

	//construct list of ordered skills
	for( int i = 1; i < Ability::ordered_abilities.Count; ++i )
	{
		Ability^ a = Ability::ordered_abilities[ i ];
		for( int j = 10; j <= 20; j += 5 )
			if( a->skills.ContainsKey( j ) )
				Skill::ordered_skills.Add( a->skills[ j ] );
		for( int j = -10; j >= -20; j -= 5 )
			if( a->skills.ContainsKey( j ) )
				Skill::ordered_skills.Add( a->skills[ j ] );
	}

	/* Ability Ordering
	00  Torso Inc
	01	Poison
	02	Paralysis
	03	Sleep
	04	Stun
	05	Mud/Snow
	06	Sense
	07	Health
	08	Rec Speed
	09	Sharpness
	10	Artisan
	11	Fencing
	12	Expert
	13	Sharpener
	14	Guard
	15	Guard Up
	16	Auto-Guard
	17	Reload Spd
	18	Recoil
	19	NormalS Up
	20	PierceS Up
	21	PelletS Up
	22	NormalSAdd
	23	PierceSAdd
	24	PelletSAdd
	25	Crag S Add
	26	ClustS Add
	27	Status
	28	Elemental
	29	BombStrUp
	30	Hunger
	31	Gluttony
	32	Attack
	33	Defense
	34	Protection
	35	HearProtct
	36	Anti-Theft
	37	Wide Area
	38	Backpackng
	39	Fire Res
	40	Water Res
	41	ThunderRes
	42	Ice Res
	43	Dragon Res
	44	Heat Res
	45	Cold Res
	46	WindPress
	47	Gathering
	48	HiSpdGathr
	49	Whim
	50	Fate
	51	Psychic
	52	Rec Level
	53	Combo Rate
	54	Shot Mix
	55	Evasion
	56	Potential
	57	LastingPwr
	58	Stamina
	59	Loading
	60	Precision
	61	Eating
	62	Carving
	63	Quake Res
	64	Evade Dist
	65	Crit Draw
	66	SpeedSetup
	67	Constitutn
	68	Rewards
	69	Perception
	70	FastCharge
	71	PunishDraw
	72	Blight Res
	73	Adversity
	74	Rapid Fire
	75	Dungmaster
	76	Antiseptic
	77	PoisonCAdd
	78	ParalyCAdd
	79	SleepCAdd
	80	PowerCAdd
	81	RazorCAdd
	82	Horn
	83	Gunnery
	84	Awaken
	85	Huntsman
	86	Fortitude
	87	Fire Atk
	88	Water Atk
	89	Thundr Atk
	90	Ice Attack
	91	Dragon Atk
	92	CutterSAdd
	93	FatigeCAdd
	94	Debilitate
	95	Slam
	96	KO
	97	FatigueAtk
	98	Sheathe
	99	Will Recov
		*/
}

Skill^ Skill::FindSkill( System::String^ name )
{
	if( static_skill_map.ContainsKey( name ) )
		return static_skill_map[ name ];
	else return nullptr;
}

void Skill::LoadLanguage( System::String^ filename )
{
	Ability::static_ability_map.Clear();
	IO::StreamReader fin( filename );
	bool ordered = false;
	if( fin.Peek() == L'#' )
	{
		String^ comment = fin.ReadLine();
		if( comment == L"#ordered" )
			ordered = true;
	}
	for( int i = 0; i < Ability::static_abilities.Count; )
	{
		String^ line = fin.ReadLine();
		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		
		if( ordered )
		{
			Ability::ordered_abilities[ i ]->name = line;
			Ability::static_ability_map.Add( line, Ability::ordered_abilities[ i ] );
		}
		else
		{
			Ability::static_abilities[ i ]->name = line;
			Ability::static_ability_map.Add( line, Ability::static_abilities[ i ] );
		}
		i++;
	}
	static_skill_map.Clear();
	for( int i = 0; i < static_skills.Count; )
	{
		String^ line = fin.ReadLine();
		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		
		if( ordered )
		{
			ordered_skills[ i ]->name = line;
			static_skill_map.Add( line, ordered_skills[ i ] );
		}
		else
		{
			static_skills[ i ]->name = line;
			static_skill_map.Add( line, static_skills[ i ] );
		}
		i++;
	}
}

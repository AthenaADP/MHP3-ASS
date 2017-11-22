#include "stdafx.h"

#include "Skill.h"
#include "Common.h"
#include "LoadedData.h"
#include "CharmDatabase.h"

using namespace System;

Skill^ LoadedData::FindSkill( const unsigned skill )
{
	Assert( int( skill ) < Skill::static_skills.Count, L"Skill index out of range" );
	return Skill::static_skills[ skill ];
}

SkillTag^ SkillTag::FindTag( String^ tag )
{
	for each( SkillTag^ st in tags )
	{
		if( st->name == tag )
			return st;
	}
	return nullptr;
}

void LoadedData::ImportTextFiles()
{
	Armor::static_armor_map.Clear();
	Armor::static_armors = gcnew array< List_t< Armor^ >^ >( int( Armor::ArmorType::NumArmorTypes ) );
	for( int i = 0; i < Armor::static_armors->Length; ++i )
		Armor::static_armors[ i ] = gcnew List_t< Armor^ >;

	Material::LoadMaterials( L"Data/components.txt" );
	SkillTag::Load( L"Data/tags.txt" );
	Skill::Load( L"Data/skills.txt" );
	Armor::Load( L"Data/head.txt",  Armor::ArmorType::HEAD );
	Armor::Load( L"Data/body.txt",  Armor::ArmorType::BODY );
	Armor::Load( L"Data/arms.txt",  Armor::ArmorType::ARMS );
	Armor::Load( L"Data/waist.txt", Armor::ArmorType::WAIST );
	Armor::Load( L"Data/legs.txt",  Armor::ArmorType::LEGS );
	Decoration::Load( L"Data/decorations.txt" );
}

void AddToList( List_t< Decoration^ >^ list, Decoration^ item, List_t< Ability^ >^ rel_abilities, List_t< Decoration^ >^ inf_decs )
{
	for( int i = 0; i < list->Count; ++i )
	{
		if( item->IsBetterThan( list[ i ], rel_abilities ) )
		{
			if( !list[ i ]->IsBetterThan( item, rel_abilities ) )
				list->Remove( list[ i-- ] );
		}
		else if( list[ i ]->IsBetterThan( item, rel_abilities ) )
			return;
	}
	list->Add( item );
	inf_decs->Add( item );
}

void AddToList( List_t< Armor^ >^ list, Armor^ armor, List_t< Ability^ >^ rel_abilities, List_t< Armor^ >^ inf_armor )
{
	for( int i = 0; i < list->Count; ++i )
	{
		if( armor->IsBetterThan( list[ i ], rel_abilities ) )
		{
			if( ( !armor->danger || armor->danger == list[ i ]->danger ) && !list[ i ]->IsBetterThan( armor, rel_abilities ) )
			{
				if( list[ i ]->no_skills )
					inf_armor->Remove( list[ i ] );
				list->Remove( list[ i-- ] );
			}
		}
		else if( ( !list[ i ]->danger || list[ i ]->danger == armor->danger ) && list[ i ]->IsBetterThan( armor, rel_abilities ) )
		{
			if( !armor->no_skills )
				inf_armor->Add( armor );
			return;
		}
	}
	list->Add( armor );
	inf_armor->Add( armor );
}

void GetRelevantArmors( Query^ query, List_t< Armor^ >^ rel_armor, List_t< Armor^ >^ list, List_t< Armor^ >^ inf_armor, List_t< Ability^ >^ danger_skills )
{
	unsigned max_slots = 0;
	for each( Armor^ armor in list )
	{
		if( armor->MatchesQuery( query, danger_skills, max_slots ) )
		{
			max_slots = max_slots < armor->num_slots ? armor->num_slots : max_slots;
			AddToList( rel_armor, armor, %query->rel_abilities, inf_armor );
		}
	}
}

void GetRelevantDecorations( Query^ query )
{
	for each( Skill^ skill in query->skills )
	{
		if( !Decoration::static_decoration_ability_map.ContainsKey( skill->ability ) )
			continue;
		for each( Decoration^ decoration in Decoration::static_decoration_ability_map[ skill->ability ] )
		{
			if( decoration->MatchesQuery( query ) )
				AddToList( %query->rel_decorations, decoration, %query->rel_abilities, %query->inf_decorations );
		}
	}
	//for each( Decoration^ decoration in query->rel_decorations )
	//	decoration->dangerous = decoration->abilities.Count == 2 && Utility::Contains( %query->rel_abilities, decoration->abilities[ 1 ]->ability );
}

int NeededPoints( List_t< Skill^ >^ skills, Ability^ ability )
{
	for each( Skill^ skill in skills )
		if( skill->ability == ability )
			return skill->points_required;
	return 0;
}

void LoadedData::GetRelevantData( Query^ query )
{
	for each( Ability^ ab in Ability::static_abilities )
	{
		ab->relevant = false;
	}
	List_t< Ability^ > danger_skills;
	//get relevant abilities
	for each( Skill^ skill in query->skills )
	{
		query->rel_abilities.Add( skill->ability );
		skill->ability->relevant = true;
	}
	query->rel_abilities.TrimExcess();

	//get relevant decorations
	GetRelevantDecorations( query );
	query->rel_decorations.TrimExcess();

	//get danger skills
	/*if( query->danger )
		for each( Decoration^ decoration in query->rel_decorations )
		{
			Ability^ bad_ability = nullptr;
			bool points20 = false;
			for each( AbilityPair^ apair in decoration->abilities )
			{
				if( apair->amount < 0 )
				{
					if( !Utility::Contains( %danger_skills, apair->ability ) )
						bad_ability = apair->ability;
				}
				else if( NeededPoints( %query->skills, apair->ability ) == 20 )
					points20 = true;
			}
			if( points20 && bad_ability )
				danger_skills.Add( bad_ability );
		}*/

	//get relevant armors
	for( int i = 0; i < int( Armor::ArmorType::NumArmorTypes ); ++i )
	{
		GetRelevantArmors( query, query->rel_armor[ i ], Armor::static_armors[ i ], query->inf_armor[ i ], %danger_skills );
		query->rel_armor[ i ]->TrimExcess();
	}
	query->rel_armor.TrimExcess();
}

void SkillTag::Load( System::String^ filename )
{
	tags.Clear();
	tags.Add( gcnew SkillTag( L"All" ) );
	tags.Add( gcnew SkillTag( L"Misc" ) );
	IO::StreamReader fin( filename );
	while( !fin.EndOfStream )
	{
		String^ tag = fin.ReadLine();
		if( tag != L"" )
			tags.Add( gcnew SkillTag( tag ) );
	}
}

void SkillTag::LoadLanguage( String^ filename )
{
	IO::StreamReader fin( filename );
	for( int i = 0; i < tags.Count; )
	{
		String^ line = fin.ReadLine();
		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		tags[ i ]->name = line;
		i++;
	}
}
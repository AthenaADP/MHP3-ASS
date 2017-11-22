#include "stdafx.h"
#include "Decoration.h"
#include "Skill.h"
#include "Solution.h"

using namespace System;

void Decoration::Load( System::String^ filename )
{
	static_decoration_map.Clear();
	static_decoration_ability_map.Clear();
	static_decorations.Clear();
	static_decorations.Capacity = 256;
	IO::StreamReader fin( filename );
	//IO::StreamWriter fout( L"extra.txt" );
	while( !fin.EndOfStream )
	{
		String^ line = fin.ReadLine();
		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		List_t< String^ > split;
		Utility::SplitString( %split, line, L',' );
		Decoration^ decoration = gcnew Decoration;
		decoration->is_event = false;
		//name,jname,rarity,slots,hr,ve,skill1,#,skill2,#,mat1,#,mat2,#,mat3,#,mat4,#
		decoration->name = split[ 0 ];
		//decoration->jname = split[ 1 ];
		decoration->rarity = Convert::ToUInt32( split[ 2 ] );
		decoration->slots_required = Convert::ToUInt32( split[ 3 ] );
		decoration->hr = Convert::ToUInt32( split[ 4 ] );
		decoration->elder_star = Convert::ToUInt32( split[ 5 ] );
		decoration->difficulty = 0;
		for( int i = 0; i < 2; ++i )
		{
			if( split[ 6 + i * 2 ] == L"" ) continue;
			AbilityPair^ apair = gcnew AbilityPair( Ability::FindAbility( split[ 6 + i * 2 ] ), Convert::ToInt32( split[ 7 + i * 2 ] ) );
			Assert( apair->ability, L"No such ability found" );
			decoration->abilities.Add( apair );
		}
		for( unsigned i = 0; i < 4; ++i )
		{
			if( split[ 10 + i * 2 ] != L"" )
			{
				MaterialComponent^ mc = gcnew MaterialComponent;
				mc->amount = Convert::ToUInt32( split[ 11 + i * 2 ] );
				mc->material = Material::FindMaterial( split[ 10 + i * 2 ] );
				Assert( mc->material, L"No such material found: " + split[ 10 + i * 2 ] );
				decoration->components.Add( mc );
				decoration->is_event |= mc->material->event_only;
				decoration->difficulty += mc->material->difficulty;
			}
			if( split[ 18 + i * 2 ] != L"" )
			{
				MaterialComponent^ mc = gcnew MaterialComponent;
				mc->amount = Convert::ToUInt32( split[ 19 + i * 2 ] );
				mc->material = Material::FindMaterial( split[ 18 + i * 2 ] );
				Assert( mc->material, L"No such material found" );
				/*
				if( !mc->material )
				{
					Material^ mat = gcnew Material;
					mat->name = split[ 18 + i * 2 ];
					Material::static_materials.Add( mat );
					Material::static_material_map[ mat->name ] = mat;
					mc->material = mat;
					fout.WriteLine( mat->name );
				}*/
				decoration->components2.Add( mc );
			}
		}
		decoration->index = static_decorations.Count;
		static_decorations.Add( decoration );
		if( !static_decoration_ability_map.ContainsKey( decoration->abilities[ 0 ]->ability ) )
			static_decoration_ability_map.Add( decoration->abilities[ 0 ]->ability, gcnew List_t< Decoration^ > );
		static_decoration_ability_map[ decoration->abilities[ 0 ]->ability ]->Add( decoration );
		static_decoration_map.Add( decoration->name, decoration );
		//static_decoration_map.Add( decoration->jname, decoration );
	}
	static_decorations.TrimExcess();
}

int Decoration::GetSkillAt( Ability^ ability )
{
	for( int i = 0; i < abilities.Count; ++i )
		if( abilities[ i ]->ability == ability )
			return abilities[ i ]->amount;
	return 0;
}

inline bool NotWorse( Decoration^ a, Decoration^ b )
{
	return b->abilities.Count == 2 && ( a->abilities.Count == 1 || a->abilities[ 1 ]->amount * b->slots_required < b->abilities[ 1 ]->amount * a->slots_required );
}

bool Decoration::IsBetterThan( Decoration^ other, List_t< Ability^ >^ rel_abilities )
{
	if( slots_required < other->slots_required || abilities[ 0 ]->ability != other->abilities[ 0 ]->ability )
		return true;
	const int a = abilities[ 0 ]->amount * other->slots_required,
			  b = other->abilities[ 0 ]->amount * slots_required;
	return a != b ? a > b : NotWorse( this, other );
}

Decoration^ Decoration::GetBestDecoration( Ability^ ability, const unsigned max_slots, Query^ query )
{
	Assert( !ability->relevant, L"Ability is relevant here!?" );
	Decoration^ best = nullptr;
	List_t< Ability^ > rel;
	rel.Add( ability );
	for each( Decoration^ dec in static_decoration_ability_map[ ability ] )
	{
		if( dec->hr > query->hr && dec->elder_star > query->elder_star || dec->slots_required > max_slots )
			continue;
		for each( AbilityPair^ ap in dec->abilities )
		{
			if( ap->amount > 0 && ap->ability == ability )
			{
				if( !best || dec->IsBetterThan( best, %rel ) )
					best = dec;
			}
		}
	}
	return best;
}

bool Decoration::MatchesQuery( Query^ query )
{
	//check requirements
	if( hr > query->hr && elder_star > query->elder_star ||
		is_event && !query->allow_event )
		return false;
	//check for relevant skills
	for each( Skill^ skill in query->skills )
		if( skill->ability == abilities[ 0 ]->ability )
			return true;
	return false;
}

Decoration^ Decoration::FindDecoration( System::String^ name )
{
	if( static_decoration_map.ContainsKey( name ) )
		return static_decoration_map[ name ];
	return nullptr;
}

Decoration^ Decoration::GetBestDecoration( Ability^ ability, const unsigned max_slots, List_t< List_t< Decoration^ >^ >% rel_deco_map )
{
	for( int i = max_slots + 1; i --> 0; )
	{
		for each( Decoration^ deco in rel_deco_map[ i ] )
		{
			if( deco->abilities[ 0 ]->ability == ability )
				return deco;
		}
	}
	return nullptr;
}

void Decoration::LoadLanguage( System::String^ filename )
{
	static_decoration_map.Clear();
	IO::StreamReader fin( filename );
	for( int i = 0; i < static_decorations.Count; )
	{
		String^ line = fin.ReadLine();
		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		static_decorations[ i ]->name = line;
		static_decoration_map.Add( line, static_decorations[ i ] );
		i++;
	}
}

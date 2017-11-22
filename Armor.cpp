#include "stdafx.h"
#include "Armor.h"
#include "Skill.h"
#include "Solution.h"
#include "StringManipulation.h"
#include <fstream>

using namespace System;

bool ArmorExists( List_t< Armor^ >^ armors, String^ armor )
{
	for each( Armor^ existing in armors )
		if( existing->name == armor )
			return true;
	return false;
}

void Armor::Load( String^ filename, ArmorType armor_type )
{
	List_t< Armor^ >^ armors = static_armors[ (int)armor_type ];
	armors->Clear();
	armors->Capacity = 512;
	
	IO::StreamReader fin( filename );
	String^ temp;
	while( !fin.EndOfStream && temp != L"" )
	{
		temp = fin.ReadLine();
		if( temp == L"" ) break;
		else if( temp[ 0 ] == L'#' ) continue;
		List_t< String^ > split;
		Utility::SplitString( %split, temp, L',' );
		if( ArmorExists( armors, split[ 0 ] ) )
			continue;
		Armor^ armor = gcnew Armor();
		armor->torso_inc = false;
		armor->is_event = false;
		armor->danger = nullptr;
		//Name,jname,gender012,type012,rarity,slots,hr,ve,def,max def,fire,water,ice,lightning,dragon,skill1,#,skill2,#,skill3,#,skill4,#,skill5,#,mat1,#,mat2,#,mat3,#,mat4,#
		armor->name = split[ 0 ];
		//armor->jname = split[ 1 ];
		const unsigned data_start = 2;
		armor->gender = split[ data_start ] == L"1" ? Gender::MALE : split[ data_start ] == L"2" ? Gender::FEMALE : Gender::BOTH_GENDERS;
		armor->type = split[ data_start + 1 ] == L"1" ? HunterType::BLADEMASTER : split[ data_start + 1 ] == L"2" ? HunterType::GUNNER : HunterType::BOTH_TYPES;
		armor->rarity = Convert::ToInt32( split[ data_start + 2 ] );
		armor->is_piercing = armor->name->Contains( L"Piercing" );
		armor->is_event = false;
		armor->num_slots = Convert::ToUInt32( split[ data_start + 3 ] );
		armor->hr = Convert::ToUInt32( split[ data_start + 4 ] );
		armor->elder_star = Convert::ToUInt32( split[ data_start + 5 ] );
		armor->defence = Convert::ToUInt32( split[ data_start + 6 ] );
		armor->max_defence = Convert::ToUInt32( split[ data_start + 7 ] );
		armor->fire_res = Convert::ToInt32( split[ data_start + 8 ] );
		armor->water_res = Convert::ToInt32( split[ data_start + 9 ] );
		armor->ice_res = Convert::ToInt32( split[ data_start + 10 ] );
		armor->thunder_res = Convert::ToInt32( split[ data_start + 11 ] );
		armor->dragon_res = Convert::ToInt32( split[ data_start + 12 ] );
		armor->difficulty = 0;
		for( unsigned i = 1; i <= 4; ++i )
		{
			if( split[ i * 2 + data_start + 21 ] != L"" )
			{
				MaterialComponent^ comp = gcnew MaterialComponent;
				comp->material = Material::FindMaterial( split[ i * 2 + 23 ] );
				Assert( comp->material, L"No such material found: " + split[ i * 2 + 23 ] );
				/*if( !comp->material )
				{
					Material^ mat = gcnew Material;
					mat->name = split[ data_start + 21 + i * 2 ];
					Material::static_materials.Add( mat );
					Material::static_material_map[ mat->name ] = mat;
					comp->material = mat;
				}*/
				comp->amount = Convert::ToInt32( split[ i * 2 + data_start + 22 ] );
				armor->components.Add( comp );
				armor->is_event |= comp->material->event_only;
				armor->difficulty += comp->material->difficulty * comp->amount;
			}
		}
		
		for( unsigned i = 0; i < 5; ++i )
		{
			const unsigned index = data_start + 13 + i * 2;
			if( split[ index ] != L"" )
			{
				AbilityPair^ p = gcnew AbilityPair( Ability::FindAbility( split[ index ] ), 0 );
				Assert( p->ability, L"No such ability found" );
				if( split[ index + 1 ] != L"" )
					p->amount = Convert::ToInt32( split[ index + 1 ] );
				armor->abilities.Add( p );
				if( p->ability == Ability::torso_inc )
					armor->torso_inc = true;
			}
		}
		armor->index = armors->Count;
		armors->Add( armor );
		Assert( !static_armor_map.ContainsKey( armor->name ), L"Armor already exists!" );
		static_armor_map.Add( armor->name, armor );
	}

	fin.Close();
	armors->TrimExcess();
}

bool Armor::MatchesQuery( Query^ query, List_t< Ability^ >^ danger_skills, const unsigned max_slots )
{
	//check requirements
	if( !query->include_piercings && this->is_piercing ||
		!query->allow_event && this->is_event ||
		!query->allow_low_rank && this->hr < 4 ||
		type != HunterType::BOTH_TYPES && query->hunter_type != type ||
		gender != Gender::BOTH_GENDERS && gender != query->gender ||
		hr > query->hr && elder_star > query->elder_star )
		return false;
	//check for torso inc
	if( torso_inc )
		return true;
	//check for danger skills
	danger = nullptr;
	for each( AbilityPair^ apair in abilities )
	{
		if( apair->amount < 0 && Utility::Contains( danger_skills, apair->ability ) )
		{
			danger = apair->ability;
			break;
		}
	}
	//check for relevant skills
	no_skills = true;
	bool bad_skills = false;
	for( int i = 0; i < abilities.Count; ++i )
	{
		if( Utility::Contains( %query->rel_abilities, abilities[ i ]->ability ) )
		{
			if( abilities[ i ]->amount > 0 )
			{
				no_skills = false;
				return true;
			}
			else bad_skills = false;
		}
	}
	if( num_slots == max_slots ) return !danger && !bad_skills;
	else return num_slots > max_slots && !danger;
}

bool Armor::IsBetterThan( Armor^ other, List_t< Ability^ >^ rel_abilities )
{
	if( num_slots > other->num_slots /*||
		no_skills && !other->no_skills */) return true;
	if( no_skills && other->no_skills )
	{
		return this->defence == other->defence ? this->rarity > other->rarity : this->defence > other->defence;
	}
	if( this->torso_inc && other->torso_inc )
	{
		return this->rarity == other->rarity ? this->defence > other->defence : this->rarity > other->rarity;
	}
	else if( this->torso_inc || other->torso_inc )
		return true;
	for each( Ability^ ability in rel_abilities )
	{
		if( this->GetSkillAt( ability ) > other->GetSkillAt( ability ) )
			return true;
	}
	return false;
}

int Armor::GetSkillAt( Ability^ ability )
{
	for each( AbilityPair^ apair in abilities )
		if( apair->ability == ability )
			return apair->amount;
	return 0;
}

Armor^ Armor::FindArmor( String^ name )
{
	if( static_armor_map.ContainsKey( name ) )
		return static_armor_map[ name ];
	return nullptr;
}

void Armor::LoadLanguage( String^ filename, ArmorType armor_type )
{
	List_t< Armor^ >^ armors = static_armors[ (int)armor_type ];
	IO::StreamReader fin( filename );
	for( int i = 0; i < armors->Count; )
	{
		Armor^ armor = armors[ i ];
		String^ line = fin.ReadLine();
		if( !line )
		{
			array< wchar_t >^ slashes = { L'/', L'\\' };
			const int slash = filename->LastIndexOfAny( slashes );
			Windows::Forms::MessageBox::Show( L"Unexpected end of file: not enough lines of text?", filename->Substring( slash + 1 ) );
			break;
		}

		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		if( static_armor_map.ContainsKey( line ) )
		{
			//const bool has_bracket = line->IndexOf( L"(" ) > 0;
			//if( filename->IndexOf( L"English" ) < 0 )
			//Assert( armor_type != ArmorType::LEGS, L"Duplicate armor name: " + line );
			continue;
		}

		armor->name = line;
		static_armor_map.Add( line, armor );
		i++;
	}
}

String^ Charm::GetName()
{
	System::Text::StringBuilder sb;
	if( abilities.Count > 0 )
	{
		sb.Append( L"+" )->Append( abilities[ 0 ]->amount )->Append( L" " )->Append( abilities[ 0 ]->ability->name );
		if( abilities.Count > 1 )
			sb.Append( abilities[ 1 ]->amount > 0 ? L", +" : L", " )->Append( abilities[ 1 ]->amount )->Append( L" " )->Append( abilities[ 1 ]->ability->name )->Append( L" " );
		else
			sb.Append( L" " );
	}
	sb.Append( Utility::SlotString( num_slots ) );
	return sb.ToString();
}

Charm::Charm( Charm^ other ) : num_slots( other->num_slots ), custom( other->custom ), hacked( false )
{
	for each( AbilityPair^ ap in other->abilities )
	{
		abilities.Add( gcnew AbilityPair( ap->ability, ap->amount ) );
	}
}

Charm::Charm( const unsigned num_slots ) : num_slots( num_slots ), custom( false ), hacked( false ) {}

bool Charm::StrictlyBetterThan( Charm^ other )
{
	if( num_slots < other->num_slots )
		return false;
	bool better = false;
	for each( AbilityPair^ apair in other->abilities )
	{
		if( apair->ability->relevant )
		{
			bool found = false;
			for each( AbilityPair^ ap in abilities )
			{
				if( ap->ability == apair->ability )
				{
					found = true;
					if( ap->amount < apair->amount )
						return false;
					else if( ap->amount > apair->amount )
						better = true;
					break;
				}
			}
			if( !found && apair->amount > 0 )
				return false;
		}
	}
	if( better )
		return true;
	// so far equal
	for each( AbilityPair^ ap in abilities )
	{
		if( ap->ability->relevant )
		{
			bool found = false;
			for each( AbilityPair^ apair in other->abilities )
			{
				if( apair->ability == ap->ability )
				{
					found = true;
					break;
				}
			}
			if( !found )
				return true;
		}	
	}
	return num_slots > other->num_slots;
}

bool EqualAbilities( List_t< AbilityPair^ >% l1, List_t< AbilityPair^ >% l2 )
{
	Assert( l1.Count == l2.Count, L"Ability lists not equal size" );
	for( int i = 0; i < l1.Count; ++i )
		if( l1[ i ]->ability != l2[ i ]->ability ||
			l2[ i ]->amount != l2[ i ]->amount )
			return false;
	return true;
}

bool Charm::operator==( Charm^ other )
{
	return num_slots == other->num_slots &&
		abilities.Count == other->abilities.Count &&
		EqualAbilities( abilities, other->abilities );
}

bool Charm::BasicallyTheSameAs( Charm^ other )
{
	if( num_slots != other->num_slots )
		return false;
	List_t< AbilityPair^ > a1, a2;
	for each( AbilityPair^ ap in abilities )
		if( ap->ability->relevant )
			a1.Add( ap );
	for each( AbilityPair^ ap in other->abilities )
		if( ap->ability->relevant )
			a2.Add( ap );
	return a1.Count == a2.Count && EqualAbilities( a1, a2 );
}

unsigned Charm::GetHash()
{
	if( abilities.Count == 0 )
		return Charm::HashFunction( num_slots, nullptr, 0, nullptr, 0 );
	else if( abilities.Count == 1 )
		return Charm::HashFunction( num_slots, abilities[ 0 ]->ability, abilities[ 0 ]->amount, nullptr, 0 );
	else
		return Charm::HashFunction( num_slots, abilities[ 0 ]->ability, abilities[ 0 ]->amount, abilities[ 1 ]->ability, abilities[ 1 ]->amount );
}

unsigned Charm::HashFunction( const unsigned num_slots, Ability^ a1, const int p1, Ability^ a2, const int p2 )
{
	const unsigned ab1_index = a1 ? a1->static_index : Ability::static_abilities.Count;
	const unsigned ab2_index = a2 ? a2->static_index : Ability::static_abilities.Count;
	//2 bits for slots
	//4 bits for point1
	//5 bits for point2
	//7 bits for ability1
	//7 bits for ability2
	return ( num_slots + ( a1 ? p1 << 2 : 0  ) + ( ( a2 ? p2 + 12 : 12 ) << 6 ) ) + ( ab1_index << 11 ) + ( ab2_index << 18 );
}

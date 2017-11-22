#include "stdafx.h"
#include "Common.h"
#include "Armor.h"
#include "Decoration.h"
#include "CharmDatabase.h"
#include "LoadedData.h"

using namespace System;

namespace Utility
{
	String^ RemoveQuotes( String^ in_out )
	{
		if( in_out == L"" ) return in_out;
		in_out = in_out->Trim();
		if( in_out[ 0 ] == L'\"' ) in_out = in_out->Substring( 1 );
		if( in_out[ in_out->Length - 1 ] == L'\"' ) in_out = in_out->Substring( 0, in_out->Length - 1 );
		return in_out;
	}

	void SplitString( List_t< String^ >^ vec, String^ str, const wchar_t c )
	{
		str = str->Trim();
		int last_non_delim = 0;
		for( int i = 0; i < str->Length; ++i )
		{
			if( str[ i ] == c )
			{
				String^ substr = str->Substring( last_non_delim, i - last_non_delim );
				RemoveQuotes( substr );
				vec->Add( substr );
				last_non_delim = i + 1;
			}
		}
		str = str->Substring( last_non_delim, str->Length - last_non_delim );
		RemoveQuotes( str );
		vec->Add( str );
	}

	bool ContainsString( List_t< String^ >% vec, String^ item )
	{
		for( int i = 0; i < vec.Count; ++i )
			if( vec[ i ] == item ) return true;
		return false;
	}

	void FindLevelReqs( unsigned% available, unsigned% required, String^ input )
	{
		if( input->Length > 0 && input[ 0 ] == L'\"' ) input = Utility::RemoveQuotes( input );
		const int exclamation_point = input->LastIndexOf( L'!' );
		if( exclamation_point == -1 )
		{
			required = 0;
			available = Convert::ToUInt32( input );
			return;
		}
		required = Convert::ToUInt32( input->Substring( 0, exclamation_point ) );
		if( exclamation_point < input->Length - 1 )
			available = Convert::ToUInt32( input->Substring( exclamation_point + 1 ) );
	}

	String^ SlotString( const unsigned slots )
	{
		return slots == 0 ? L"---" :
			slots == 1 ? L"O--" :
			slots == 2 ? L"OO-" : L"OOO";
	}

	unsigned CountChars( String^ str, const wchar_t c )
	{
		unsigned total = 0;
		for( int i = 0; i < str->Length; ++i )
			if( str[ i ] == c ) ++total;
		return total;
	}

	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, List_t< AbilityPair^ >% abilities )
	{
		for each( AbilityPair^ apair in abilities )
		{
			if( apair->amount != 0 && apair->ability != Ability::torso_inc )
				strip->Items->Add( Convert::ToString( apair->amount ) + L" " + apair->ability->name );
			else
				strip->Items->Add( apair->ability->name );
		}
	}

	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, List_t< MaterialComponent^ >^ components )
	{
		strip->Items->Add( L"-" );
		for each( MaterialComponent^ part in components )
			strip->Items->Add( Convert::ToString( part->amount ) + L"x " + part->material->name );
	}

	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, Decoration^ decoration )
	{
		strip->Items->Add( decoration->name );
		strip->Items->Add( L"-" );
		strip->Items->Add( SlotString( decoration->slots_required ) );
		strip->Items->Add( L"-" );
		UpdateContextMenu( strip, decoration->abilities );
		UpdateContextMenu( strip, %decoration->components );
		if( decoration->components2.Count > 0 )
			UpdateContextMenu( strip, %decoration->components2 );
	}

	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, Armor^ armor )
	{
		strip->Items->Add( armor->name );
		strip->Items->Add( L"-" );
		strip->Items->Add( SlotString( armor->num_slots ) );
		strip->Items->Add( L"-" );
		strip->Items->Add( ColonString( Defence ) + Convert::ToString( armor->defence ) + RunString( To ) + Convert::ToString( armor->max_defence ) );
		strip->Items->Add( ColonString( DragonRes ) + Convert::ToString( armor->dragon_res ) );
		strip->Items->Add( ColonString( FireRes ) + Convert::ToString( armor->fire_res ) );
		strip->Items->Add( ColonString( IceRes ) + Convert::ToString( armor->ice_res ) );
		strip->Items->Add( ColonString( ThunderRes ) + Convert::ToString( armor->thunder_res ) );
		strip->Items->Add( ColonString( WaterRes ) + Convert::ToString( armor->water_res ) );
		strip->Items->Add( L"-" );
		UpdateContextMenu( strip, armor->abilities );
		UpdateContextMenu( strip, %armor->components );
	}

	ref class ToolHost : public System::Windows::Forms::ToolStripControlHost
	{
		System::Windows::Forms::RichTextBox rtb;
		String^ rtf;
	public:
		ToolHost( const unsigned table_number, const unsigned mystery, const unsigned shining, const unsigned timeworn )
			: ToolStripControlHost( %rtb )
		{
			using namespace System::Drawing;
			rtb.SelectionColor = Color::Black;
			rtb.AppendText( StartString( Table ) + Convert::ToString( table_number ) + L": " );
			rtb.SelectionColor = Color::Gray;
			rtb.AppendText( Convert::ToString( mystery ) );
			rtb.SelectionColor = rtb.DefaultForeColor;
			rtb.AppendText( L", " );
			rtb.SelectionColor = Color::DarkGoldenrod;
			rtb.AppendText( Convert::ToString( shining ) );
			rtb.SelectionColor = rtb.DefaultForeColor;
			rtb.AppendText( L", " );
			rtb.SelectionColor = Color::Red;
			rtb.AppendText( Convert::ToString( timeworn ) );
			rtb.BorderStyle = System::Windows::Forms::BorderStyle::None;
			rtb.ReadOnly = true;
			rtf = rtb.Rtf;
		}

		virtual void OnPaint( System::Windows::Forms::PaintEventArgs^ args ) override
		{
			rtb.Rtf = rtf;
			ToolStripControlHost::OnPaint( args );
		}
	};

	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, String^ charm )
	{
		CharmLocationData^ results;
		if( CharmDatabase::charm_location_cache.ContainsKey( charm ) )
			results = CharmDatabase::charm_location_cache[ charm ];
		else
		{
			const unsigned num_slots = CountChars( charm->Substring( charm->Length - 3, 3 ), L'O' );
			unsigned points1 = 0, points2 = 0;
			Ability^ ability1 = nullptr, ^ability2 = nullptr;
			if( charm[ 0 ] != L'O' )
			{
				Assert( num_slots >= 0 && num_slots < 4, L"Invalid number of slots" );
				const int comma = charm->IndexOf( L',' );
				if( comma == -1 )
				{
					const int space = charm->IndexOf( L' ' );
					Assert( space > 0, L"Bad charm string" );
					points1 = Convert::ToUInt32( charm->Substring( 1, space - 1 ) );
					ability1 = Ability::FindAbility( charm->Substring( space + 1, charm->Length - space - 5 ) );
				}
				else
				{
					const int space1 = charm->IndexOf( L' ' );
					Assert( space1 > 0, L"Bad charm string" );
					const unsigned points1 = Convert::ToUInt32( charm->Substring( 1, space1 - 1 ) );
					ability1 = Ability::FindAbility( charm->Substring( space1 + 1, comma - space1 - 1 ) );

					const int space2 = charm->IndexOf( L' ', comma + 2 );
					Assert( space2 > 0, L"Bad charm string" );
					points2 = Convert::ToUInt32( charm->Substring( comma + 3, space2 - comma - 3 ) );
					ability2 = Ability::FindAbility( charm->Substring( space2 + 1, charm->Length - space2 - 5 ) );
				}
			}
			results = FindCharm2( ability1, points1, ability2, points2, num_slots );
			CharmDatabase::charm_location_cache.Add( charm, results );
		}
		
/*
		List_t< TableResult^ >^ results = FindCharm( ability1, points1, ability2, points2, num_slots, language );
		Assert( results->Count == 13 || results->Count == 12, L"Bad number of results" );
		if( results->Count == 13 )
			strip->Items->Add( results[ 12 ]->charm );
		else
			strip->Items->Add( charm );
		
		strip->Items->Add( L"-" );
		for( int i = 0; i < 12; ++i )
		{
			if( results[ i ]->mystery > 0 ||
				results[ i ]->timeworn > 0 ||
				results[ i ]->shining > 0 )
			{
				strip->Items->Add( gcnew ToolHost( i + 1, results[ i ]->mystery, results[ i ]->shining, results[ i ]->timeworn ) );
			}
		}*/
		if( results->example )
			strip->Items->Add( results->example->GetName() );
		else
			strip->Items->Add( charm );

		strip->Items->Add( L"-" );
		for( int i = 0; i < results->table->GetLength( 0 ); ++i )
		{
			if( results->table[ i, 0 ] ||
				results->table[ i, 1 ] ||
				results->table[ i, 2 ] )
			{
				strip->Items->Add( gcnew ToolHost( i + 1, results->table[ i, 0 ], results->table[ i, 1 ], results->table[ i, 2 ] ) );
			}
		}
		strip->Invalidate();
	}
}

void Material::LoadMaterials( System::String^ filename )
{
	static_materials.Clear();
	IO::StreamReader fin( filename );
	while( !fin.EndOfStream )
	{
		String^ line = fin.ReadLine();
		if( line == L"" ) continue;
		Material^ mat = gcnew Material;
		List_t< String^ > split;
		Utility::SplitString( %split, line, L',' );
		mat->name = split[ 0 ];
		if( split.Count >= 2 )
		{
			if( split[ 1 ] == L"Event" )
			{
				mat->event_only = true;
				mat->difficulty = 0;
			}
			else if( split[ 1 ] != L"" )
			{
				mat->event_only = false;
				mat->difficulty = Convert::ToUInt32( split[ 1 ] );
			}
		}
		static_materials.Add( mat );
	}
	fin.Close();
	static_material_map.Clear();
	for each( Material^ mat in static_materials )
	{
		static_material_map.Add( mat->name, mat );
		//static_material_map.Add( mat->jname, mat );
	}
}

Material^ Material::FindMaterial( System::String^ name )
{
	if( static_material_map.ContainsKey( name ) )
		return static_material_map[ name ];
	return nullptr;
}

void Material::LoadLanguage( System::String^ filename )
{
	static_material_map.Clear();
	IO::StreamReader fin( filename );
	for( int i = 0; i < static_materials.Count; )
	{
		String^ line = fin.ReadLine();
		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		static_materials[ i ]->name = line;
		static_material_map.Add( line, static_materials[ i ] );
		i++;
	}
}

void StringTable::LoadLanguage( String^ lang )
{
	String^ dir = L"Data/Languages/" + lang + L"/";
	//load strings
	{
		text = gcnew array< String^ >( (int)StringTable::StringIndex::NumStrings );
		IO::StreamReader fin( IO::File::OpenRead( dir + L"strings.txt" ) );
		int i;
		for( i = 0; i < text->Length; )
		{
			String^ line = fin.ReadLine();
			if( !line )
			{
				Windows::Forms::MessageBox::Show( L"Unexpected end of file: not enough lines of text" );
				break;
			}
			if( line == L"" || line[ 0 ] == L'#' )
				continue;
			text[ i++ ] = line;// + L"qq";
		}
		fin.Close();
	}
	//load armor, skills, etc.
	SkillTag::LoadLanguage( dir + L"tags.txt" );
	Material::LoadLanguage( dir + L"components.txt" );

	Armor::static_armor_map.Clear();
	Armor::LoadLanguage( dir + L"head.txt",  Armor::ArmorType::HEAD );
	Armor::LoadLanguage( dir + L"body.txt",  Armor::ArmorType::BODY );
	Armor::LoadLanguage( dir + L"arms.txt",  Armor::ArmorType::ARMS );
	Armor::LoadLanguage( dir + L"waist.txt", Armor::ArmorType::WAIST );
	Armor::LoadLanguage( dir + L"legs.txt",  Armor::ArmorType::LEGS );

	Decoration::LoadLanguage( dir + L"decorations.txt" );

	Skill::LoadLanguage( dir + L"skills.txt" );
}

String^ StripAmpersands( String^ input )
{
	String^ res = L"";
	for each( wchar_t c in input )
	{
		if( c != L'&' )
			res += c;
	}
	return res;
}

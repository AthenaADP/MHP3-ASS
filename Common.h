#pragma once

#ifdef _DEBUG
#define Assert( X, Y ) do { if( !(X) ) System::Windows::Forms::MessageBox::Show( (Y) ); } while( false )
#else
#define Assert( X, Y )
#endif

enum class HunterType { BLADEMASTER = 1, GUNNER, BOTH_TYPES };
enum class Gender { MALE = 4, FEMALE, BOTH_GENDERS };

#define List_t System::Collections::Generic::List
#define Map_t System::Collections::Generic::Dictionary

ref struct Armor;
ref struct Decoration;

namespace Utility
{

	template< class T >
	bool Contains( List_t< T^ >^ cont, const T^ val )
	{
		for( int i = 0; i < cont->Count; ++i )
			if( cont[ i ] == val ) return true;
		return false;
	}

	template< class T >
	int GetIndexOf( List_t< T^ >^ cont, const T^ val )
	{
		for( int i = 0; i < cont->Count; ++i )
			if( cont[ i ] == val ) return i;
		return -1;
	}

	template< class T >
	T^ FindByName( List_t< T^ >^ cont, System::String^ name )
	{
		for each( T^ item in cont )
		{
			if( item->name == name )
				return item;
		}
		return nullptr;
	}

	System::String^ SlotString( const unsigned slots );
	unsigned CountChars( System::String^ str, const wchar_t c );

	bool ContainsString( List_t< System::String^ >% vec, System::String^ item );

	void SplitString( List_t< System::String^ >^ vec, System::String^ str, const wchar_t c );

	void FindLevelReqs( unsigned% available, unsigned% required, System::String^ input );

	System::String^ RemoveQuotes( System::String^ in_out );

	inline unsigned Min( const unsigned a, const unsigned b ) { return a > b ? b : a; }

	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, Decoration^ decoration );
	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, Armor^ armor );
	void UpdateContextMenu( System::Windows::Forms::ContextMenuStrip^ strip, System::String^ charm );
}

inline int Min( const int a, const int b )
{
	return a > b ? b : a;
}

ref struct Material
{
	System::String^ name;
	bool event_only;
	unsigned difficulty;

	static List_t< Material^ > static_materials;
	static Material^ FindMaterial( System::String^ name );
	static void LoadMaterials( System::String^ filename );
	static void LoadLanguage( System::String^ filename );
	static Map_t< System::String^, Material^ > static_material_map;
};

ref struct MaterialComponent
{
	Material^ material;
	unsigned amount;
};

ref struct StringTable
{
	static array< System::String^ >^ text;
	static void LoadLanguage( System::String^ dir );
	enum class StringIndex
	{ 
		File = 0,
		Options,
		Language,
		Help,
		Load,
		Save,
		Exit,
		ClearSettings,
		AllowBadSkills,
		AllowPiercings,
		AllowEventArmor,
		AllowLowRankArmor,
		CheckForUpdates,
		About,
		MyCharms,
		Import,
		QuickSearch,
		AdvancedSearch,
		Cancel,
		Default,
		None,
		Search,
		SelectNone,
		SelectBest,
		AddNew,
		Delete,
		DeleteAll,
		MoveUp,
		MoveDown,
		Close,
		FindNext,
		HR,
		VillageQuests,
		MaxWeaponSlots,
		Male,
		Female,
		FilterResultsByCharm,
		SortResultsBy,
		All,
		DragonRes,
		FireRes,
		IceRes,
		ThunderRes,
		WaterRes,
		BaseDefence,
		MaxDefence,
		Difficulty,
		Rarity,
		SortSlotsSpare,
		Charms,
		UseNoCharms,
		UseOnlyMyCharms,
		UseOnlySlottedCharms,
		UseUpToOneSkillCharms,
		UseOnlyTwoSkillCharms,
		Skills,
		SkillFilters,
		Blademaster,
		Gunner,
		SelectArmor,
		ImportCharmsFromSaveData,
		Characters,
		DeleteExistingCharms,
		NoCharacterDataFound,
		NoCharmsFound,
		Slots,
		NoneBrackets,
		SlotSpare,
		SlotsSpare,
		OrAnythingWithSingular,
		SlotBracket,
		OrAnythingWithPlural,
		SlotsBracket,
		OrAnythingWithTorsoInc,
		SolutionsFound,
		ShowingFirst,
		SolutionsOnly,
		MHP3rdSaveData,
		NotAValidMHP3rdSaveFile,
		Version,
		Find,
		Cheater,
		Error,
		To,
		Defence,
		Table,
		Results,
		NumStrings
	};
};

System::String^ StripAmpersands( System::String^ input );

#define StaticString( X ) StringTable::text[ (int)StringTable::StringIndex::X ]
#define AddString( X ) L" " + StaticString( X )
#define RunString( X ) L" " + StaticString( X ) + L" "
#define StartString( X ) StaticString( X ) + L" "
#define BasicString( X ) StripAmpersands( StaticString( X ) )
#define ColonString( X ) StaticString( X ) + L": "

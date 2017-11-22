#pragma once
#include "Common.h"

ref struct Query;
ref struct Charm;
ref struct Ability;

typedef array< unsigned, 2 > CharmLocationTable;

ref struct CharmLocationData
{
	CharmLocationTable^ table;
	Charm^ example;
};

ref struct CharmDatabase
{
	//[type,slots] Records max values for each single skill for each slot size
	static array< Map_t< Ability^, unsigned >^, 2 >^ table;
	//List of useful charms for each combination of two skills
	static Map_t< Ability^, Map_t< Ability^, List_t< Charm^ >^ >^ > table2;
	//List of user-specified charms
	static List_t< Charm^ > mycharms;
	//List of unique charm hashes
	static List_t< unsigned > charm_hashes;
	//Charm hashes sorted by table then type, slots, AbilityIndex1.
	static array< List_t< unsigned >^, 3 >^ table_hashes;
	//Cache charm table locations
	static Map_t< System::String^, CharmLocationData^ > charm_location_cache;

	static void LoadCustom();
	static void SaveCustom();
	static List_t< Charm^ >^ GetCharms( Query^ query, const bool use_two_skill_charms );

	static void GenerateCharmTable();
};

//List_t< TableResult^ >^ FindCharm( Ability^ ability1, const unsigned points1, Ability^ ability2, const int points2, const unsigned num_slots );

CharmLocationData^ FindCharm2( Ability^ ability1, const unsigned points1, Ability^ ability2, const int points2, const unsigned num_slots );

bool CharmExists( Charm^ charm );
bool CharmIsIllegal( Charm^ charm );
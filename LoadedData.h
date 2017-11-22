#pragma once
#include <vcclr.h>

#include "Armor.h"
#include "Decoration.h"
#include "Solution.h"

ref struct SkillTag
{
	System::String^ name;

	SkillTag( System::String^ s ) : name( s ) {}

	static List_t< SkillTag^ > tags;
	static SkillTag^ FindTag( System::String^ tag );
	static void Load( System::String^ filename );
	static void LoadLanguage( System::String^ filename );
};

ref class LoadedData
{
public:
	Skill^ FindSkill( const unsigned skill );

	void ImportTextFiles();

	void GetRelevantData( Query^ query );
};

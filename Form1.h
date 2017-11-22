#pragma once
#include "LoadedData.h"
#include "frmAbout.h"
#include "frmAdvanced.h"
#include "frmFind.h"
#include "CharmDatabase.h"
#include "Version.h"
#include "ManageCharms.h"
#include "ImportCharms.h"

namespace AthenasASSMHP3rd 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

#pragma region Comparison Functions
	int Compare( const int x, const int y )
	{
		return x < y ? -1 : y < x ? 1 : 0;
	}

	int ReverseCompare( const int x, const int y )
	{
		return x < y ? 1 : y < x ? -1 : 0;
	}

	int CompareSolutionByDefence( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->defence, s2->defence );
	}

	int CompareSolutionByMaxDefence( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->max_defence, s2->max_defence );
	}

	int CompareSolutionByFireRes( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->fire_res, s2->fire_res );
	}

	int CompareSolutionByIceRes( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->ice_res, s2->ice_res );
	}

	int CompareSolutionByWaterRes( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->water_res, s2->water_res );
	}

	int CompareSolutionByThunderRes( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->thunder_res, s2->thunder_res );
	}

	int CompareSolutionByDragonRes( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->dragon_res, s2->dragon_res );
	}

	int CompareSolutionByRarity( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->rarity, s2->rarity );
	}

	int CompareSolutionByDifficulty( Solution^ s1, Solution^ s2 )
	{
		return Compare( s1->difficulty, s2->difficulty );
	}

	int CompareSolutionBySlotsSpare( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->total_slots_spare, s2->total_slots_spare );
	}
#pragma endregion
	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
		typedef System::Windows::Forms::DialogResult DialogResult_t;
		const static DialogResult_t OK = DialogResult_t::OK;
		const static int MAX_LIMIT = 1000;
		const static int NumSkills = 6;
		static Threading::Mutex^ progress_mutex = gcnew Threading::Mutex;
		static Threading::Mutex^ results_mutex = gcnew Threading::Mutex;
		static Threading::Mutex^ charm_map_mutex = gcnew Threading::Mutex;
		String^ CFG_FILE;
		String^ endl;
		String^ last_result;
		bool lock_skills, sort_off, can_save, last_search_gunner, updating_language, construction_complete;
		LoadedData^ data;
		Query^ query;
		frmFind^ find_dialog;
		frmAdvanced::Result_t^ blast_options;
		frmAdvanced::Result_t^ glast_options;
		typedef Generic::Dictionary< unsigned, unsigned > IndexMap;
		typedef Generic::Dictionary< String^, List_t< Solution^ >^ > CharmSolutionMap;
		CharmSolutionMap charm_solution_map;
		Generic::Dictionary< String^, Generic::Dictionary< __int64, bool >^ > existing_armor;

		List_t< Solution^ > final_solutions, no_charm_solutions, all_solutions;

		List_t< ComboBox^ > bSkills, gSkills;
		List_t< ComboBox^ > bSkillFilters, gSkillFilters;
		List_t< IndexMap^ > bIndexMaps, gIndexMaps;
		List_t< String^ > languages;
		List_t< Charm^ > charm_box_charms;
		int language;
#pragma region Members
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuFile;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuExit;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuHelp;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAbout;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuLanguage;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::RadioButton^  rdoFemale;
	private: System::Windows::Forms::RadioButton^  rdoMale;
	private: System::Windows::Forms::Button^  btnAdvancedSearch;
	private: System::Windows::Forms::TabControl^  tabHunterType;
	private: System::Windows::Forms::TabPage^  tabBlademaster;
	private: System::Windows::Forms::TabPage^  tabGunner;
	private: System::Windows::Forms::GroupBox^  grpGSkillFilters;
	private: System::Windows::Forms::GroupBox^  grpGSkills;
	private: System::Windows::Forms::NumericUpDown^  nudHR;
	private: System::Windows::Forms::NumericUpDown^  nudWeaponSlots;
	private: System::Windows::Forms::NumericUpDown^  nudElder;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  lblHR;
	private: System::Windows::Forms::Label^  lblElder;
	private: System::Windows::Forms::Label^  lblSlots;
	private: System::Windows::Forms::Button^  btnCancel;
	private: System::Windows::Forms::GroupBox^  grpBSkills;
	private: System::Windows::Forms::Button^  btnSearch;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::RichTextBox^  txtSolutions;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::GroupBox^  grpResults;
	private: System::Windows::Forms::GroupBox^  grpBSkillFilters;
	private: System::Windows::Forms::GroupBox^  grpSort;
	private: System::Windows::Forms::ComboBox^  cmbSort;
	private: System::Windows::Forms::ContextMenuStrip^  cmsSolutions;
	private: System::Windows::Forms::GroupBox^  grpCharmFilter;
	private: System::Windows::Forms::ComboBox^  cmbCharms;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuOptions;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowBadSkills;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowPiercings;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowEventArmor;
	private: System::Windows::Forms::Button^  btnCharms;
	private: System::Windows::Forms::GroupBox^  grpCharms;
	private: System::Windows::Forms::ComboBox^  cmbCharmSelect;

#pragma endregion

		List_t< BackgroundWorker^ >  workers;
		unsigned finished_workers, total_progress;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuCheckForUpdates;

	private: System::Windows::Forms::ToolStripMenuItem^  mnuClearSettings;

	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
	private: System::Windows::Forms::ContextMenuStrip^  cmsCharms;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuLoad;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuSave;
	private: System::Windows::Forms::Button^  btnImport;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowLowRankArmor;




		void ClearFilters()
		{
			for each( ComboBox^ box in bSkillFilters )
				box->Items->Clear();
			for each( ComboBox^ box in gSkillFilters )
				box->Items->Clear();
		}
	
		void AddFilter( System::String^ str )
		{
			for each( ComboBox^ box in bSkillFilters )
				box->Items->Add( str );
			for each( ComboBox^ box in gSkillFilters )
				box->Items->Add( str );
		}

		void InitFilters()
		{
			ClearFilters();
			for each( SkillTag^ tag in SkillTag::tags )
				AddFilter( tag->name );
			for each( ComboBox^ box in gSkillFilters )
				box->SelectedIndex = 0;
			for each( ComboBox^ box in bSkillFilters )
				box->SelectedIndex = 0;
		}

		void ResetSkill( ComboBox^ box, IndexMap^ map, Skill^ skill )
		{
			if( skill == nullptr ) return;
			IndexMap::Enumerator iter = map->GetEnumerator();
			while( iter.MoveNext() )
			{
				if( Skill::static_skills[ iter.Current.Value ] == skill )
				{
					box->SelectedIndex = iter.Current.Key;
					return;
				}
			}
		}

		void InitSkills( ComboBox^ box, IndexMap^ map, const int filter, List_t< Ability^ >^ disallowed )
		{
			map->Clear();
			box->SelectedIndex = -1;
			box->Items->Clear();
			if( filter == -1 || StringTable::text == nullptr )
				return;
			box->Items->Add( StaticString( NoneBrackets ) );
			for( int i = 0; i < Skill::static_skills.Count; ++i )
			{
				Skill^ skill = Skill::static_skills[ i ];
				if( skill->points_required <= 0 || 
					filter == 1 && !skill->ability->tags.Count == 0 ||
					Utility::Contains( disallowed, skill->ability ) )
					continue;

				if( filter == 0 || filter == 1 && skill->ability->tags.Count == 0 || 
					!!Utility::FindByName( %skill->ability->tags, SkillTag::tags[ filter ]->name ) )
				{
					map[ box->Items->Count ] = i;
					box->Items->Add( skill->name );
				}
			}
		}

		void InitSkills()
		{
			for( unsigned i = 0; i < NumSkills; ++i )
			{
				InitSkills( gSkills[ i ], gIndexMaps[ i ], gSkillFilters[ i ]->SelectedIndex, gcnew List_t< Ability^ > );
				InitSkills( bSkills[ i ], bIndexMaps[ i ], bSkillFilters[ i ]->SelectedIndex, gcnew List_t< Ability^ > );
			}
		}

		ComboBox^ GetNewComboBox( const unsigned width, const unsigned i )
		{
			ComboBox^ box = gcnew ComboBox;
			box->Location = System::Drawing::Point( 6, 19 + i * 27 );
			box->Size = System::Drawing::Size( width, 21 );
			box->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			return box;
		}

		void InitializeComboBoxes()
		{
			for( unsigned i = 0; i < NumSkills; ++i )
			{
				gSkillFilters.Add( GetNewComboBox( 134, i ) );
				bSkillFilters.Add( GetNewComboBox( 134, i ) );
				gSkillFilters[ i ]->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cmbSkillFilter_SelectedIndexChanged);
				bSkillFilters[ i ]->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cmbSkillFilter_SelectedIndexChanged);
				grpGSkillFilters->Controls->Add( gSkillFilters[ i ] );
				grpBSkillFilters->Controls->Add( bSkillFilters[ i ] );

				gSkills.Add( GetNewComboBox( 152, i ) );
				bSkills.Add( GetNewComboBox( 152, i ) );
				gSkills[ i ]->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cmbSkill_SelectedIndexChanged);
				bSkills[ i ]->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cmbSkill_SelectedIndexChanged);
				grpGSkills->Controls->Add( gSkills[ i ] );
				grpBSkills->Controls->Add( bSkills[ i ] );

				gIndexMaps.Add( gcnew IndexMap );
				bIndexMaps.Add( gcnew IndexMap );
			}
			charm_solution_map.Clear();
			cmbSort->SelectedIndex = 0;
			cmbCharms->SelectedIndex = 1;
			cmbCharmSelect->SelectedIndex = 2;
		}

	public:

		Form1(void) : CFG_FILE( L"settings.cfg" ), endl( L"\r\n" ), construction_complete( false )
		{
			//exitToolStripMenuItem_Click( nullptr, nullptr );
			//return;
			DoubleBuffered = true;
			language = -1;
			sort_off = false;
			updating_language = false;
			can_save = true;
			last_search_gunner = false;
			InitializeComponent();
			InitializeComboBoxes();

			data = gcnew LoadedData();
			data->ImportTextFiles();
			CharmDatabase::GenerateCharmTable();
			LoadLanguages();

			InitFilters();
			InitSkills();
		
			lock_skills = false;
			btnCancel->Enabled = false;

			LoadConfig();

			CharmDatabase::LoadCustom();

			Text += " v" +  STRINGIZE( VERSION_NO );

			//Text = StaticString( Charms );

			construction_complete = true;
		}

		void LoadLanguages()
		{
			array< String^ >^ langs = IO::Directory::GetDirectories( L"Data/Languages" );
			for each( String^ lang in langs )
			{
				ToolStripMenuItem^ item = gcnew ToolStripMenuItem( lang->Substring( lang->LastIndexOf( L'\\' ) + 1 ) );
				item->Click += gcnew EventHandler( this, &Form1::LanguageSelect_Click );
				mnuLanguage->DropDownItems->Add( item );
			}
		}

		void AddSolution( String^ line )
		{
			List_t< String^ > split;
			Utility::SplitString( %split, line, L' ' );
			Solution^ solution = gcnew Solution;
			for( unsigned i = 0; i < 5; ++i )
				solution->armors.Add( Armor::static_armors[ i ][ Convert::ToUInt32( split[ i ] ) ] );
			const unsigned num_decorations = Convert::ToUInt32( split[ 9 ] );
			for( unsigned i = 0; i < num_decorations; ++i )
				solution->decorations.Add( Decoration::static_decorations[ Convert::ToUInt32( split[ 10 + i ] ) ] );
			
			const unsigned num_skills = Convert::ToUInt32( split[ 10 + num_decorations ] );
			for( unsigned i = 0; i < num_skills; ++i )
				solution->extra_skills.Add( Skill::static_skills[ Convert::ToUInt32( split[ 11 + num_decorations + i ] ) ] );
			
			const unsigned charm_start = 11 + num_decorations + num_skills;
			if( charm_start < unsigned( split.Count ) )
			{
				solution->charm = gcnew Charm;
				solution->charm->num_slots = Convert::ToUInt32( split[ charm_start ] );
				const unsigned num_abilities = Convert::ToUInt32( split[ charm_start + 1 ] );
				for( unsigned i = 0; i < num_abilities; ++i )
				{
					Ability^ ability = Ability::static_abilities[ Convert::ToUInt32( split[ charm_start + 3 + i * 2 ] ) ];
					const int amount = Convert::ToInt32( split[ charm_start + 2 + i * 2 ] );
					solution->charm->abilities.Add( gcnew AbilityPair( ability, amount ) );
				}
			}

			solution->CalculateData();
			solution->total_slots_spare = Convert::ToUInt32( split[ 5 ] );
			solution->slots_spare = gcnew cli::array< unsigned >( 4 );
			for( unsigned i = 1; i <= 3; ++i )
				solution->slots_spare[ i ] = Convert::ToUInt32( split[ 5 + i ] );
			if( solution->charm )
				AddSolution( solution->charm->GetName(), solution );
			else no_charm_solutions.Add( solution );
		}

		void LoadConfig( String^ file )
		{
			can_save = false;
			if( IO::File::Exists( file ) )
			{
				IO::StreamReader fin( file );
				String^ version = fin.ReadLine();
				if( version != L"1" && version != L"2" )
				{
					fin.Close();
					can_save = true;
					return;
				}
				else
				{
					language = -1;
					LanguageSelect_Click( mnuLanguage->DropDownItems[ Convert::ToUInt32( fin.ReadLine() ) ], nullptr );

					last_search_gunner = Convert::ToBoolean( fin.ReadLine() );
					const int hunter_type = Convert::ToInt32( fin.ReadLine() );
					rdoMale->Checked = Convert::ToBoolean( fin.ReadLine() );
					rdoFemale->Checked = !rdoMale->Checked;
					mnuAllowBadSkills->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuAllowPiercings->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuAllowEventArmor->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuAllowLowRankArmor->Checked = Convert::ToBoolean( fin.ReadLine() );
					cmbSort->SelectedIndex = Convert::ToInt32( fin.ReadLine() );
					cmbCharmSelect->SelectedIndex = Convert::ToInt32( fin.ReadLine() );

					nudHR->Value = Convert::ToInt32( fin.ReadLine() );
					nudElder->Value = Convert::ToInt32( fin.ReadLine() );
					nudWeaponSlots->Value = Convert::ToInt32( fin.ReadLine() );

					tabHunterType->SuspendLayout();
					const unsigned num_skills = version == L"2" ? NumSkills : NumSkills - 1;
					for( unsigned i = 0; i < num_skills; ++i )
					{
						tabHunterType->SelectedIndex = 0;
						bSkillFilters[ i ]->SelectedIndex = Convert::ToInt32( fin.ReadLine() );
						bSkills[ i ]->SelectedIndex = SearchIndexMap( bIndexMaps[ i ], Convert::ToInt32( fin.ReadLine() ) );
						tabHunterType->SelectedIndex = 1;
						gSkillFilters[ i ]->SelectedIndex = Convert::ToInt32( fin.ReadLine() );
						gSkills[ i ]->SelectedIndex = SearchIndexMap( gIndexMaps[ i ], Convert::ToInt32( fin.ReadLine() ) );
					}
					tabHunterType->SelectedIndex = hunter_type;
					tabHunterType->ResumeLayout();

					FormulateQuery( false, last_search_gunner );

					charm_solution_map.Clear();
					all_solutions.Clear();
					no_charm_solutions.Clear();
					while( !fin.EndOfStream )
						AddSolution( fin.ReadLine() );
					last_result = nullptr;
					fin.Close();

					UpdateCharmComboBox( 1 );
				}	
			}
			else if( mnuLanguage->HasDropDownItems )
			{
				language = -1;
				for each( ToolStripItem^ item in mnuLanguage->DropDownItems )
				{
					if( item->ToString()->IndexOf( L"TeamHGG" ) >= 0 )
					{
						LanguageSelect_Click( item, nullptr );
						break;
					}
				}
				if( language == -1 )
					LanguageSelect_Click( mnuLanguage->DropDownItems[ 0 ], nullptr );
			}
			can_save = true;
			SaveConfig();
		}

		void LoadConfig()
		{
			LoadConfig( CFG_FILE );
		}

		void SaveConfig( String^ file )
		{
			if( !can_save ) return;

			IO::StreamWriter fout( file );
			fout.WriteLine( L"2" );
			fout.WriteLine( language );
			fout.WriteLine( last_search_gunner );
			fout.WriteLine( tabHunterType->SelectedIndex );
			fout.WriteLine( rdoMale->Checked );
			fout.WriteLine( mnuAllowBadSkills->Checked );
			fout.WriteLine( mnuAllowPiercings->Checked );
			fout.WriteLine( mnuAllowEventArmor->Checked );
			fout.WriteLine( mnuAllowLowRankArmor->Checked );
			fout.WriteLine( cmbSort->SelectedIndex );
			fout.WriteLine( cmbCharmSelect->SelectedIndex );
			fout.WriteLine( nudHR->Value );
			fout.WriteLine( nudElder->Value );
			fout.WriteLine( nudWeaponSlots->Value );
			for( unsigned i = 0; i < NumSkills; ++i )
			{
				fout.WriteLine( bSkillFilters[ i ]->SelectedIndex );
				if( bSkills[ i ]->SelectedIndex == -1 )
					fout.WriteLine( -1 );
				else fout.WriteLine( bIndexMaps[ i ][ bSkills[ i ]->SelectedIndex ] );
				fout.WriteLine( gSkillFilters[ i ]->SelectedIndex );
				if( gSkills[ i ]->SelectedIndex == -1 )
					fout.WriteLine( -1 );
				else fout.WriteLine( gIndexMaps[ i ][ gSkills[ i ]->SelectedIndex ] );
			}
			
			for each( Solution^ solution in all_solutions )
			{
				for( unsigned i = 0; i < 5; ++i )
				{
					fout.Write( Convert::ToString( Utility::GetIndexOf( Armor::static_armors[ i ], solution->armors[ i ] ) ) );
					fout.Write( L" " );
				}
				fout.Write( Convert::ToString( solution->total_slots_spare ) );
				fout.Write( L" " );
				for( unsigned i = 1; i <= 3; ++i )
				{
					fout.Write( Convert::ToString( solution->slots_spare[ i ] ) );
					fout.Write( L" " );
				}
				fout.Write( Convert::ToString( solution->decorations.Count ) );
				fout.Write( L" " );
				for each( Decoration^ decoration in solution->decorations )
				{
					fout.Write( Convert::ToString( Utility::GetIndexOf( %Decoration::static_decorations, decoration ) ) );
					fout.Write( L" " );
				}
				fout.Write( Convert::ToString( solution->extra_skills.Count ) );
				fout.Write( L" " );
				for each( Skill^ skill in solution->extra_skills )
				{
					fout.Write( Convert::ToString( Utility::GetIndexOf( %Skill::static_skills, skill ) ) );
					fout.Write( L" " );
				}
				if( solution->charm )
				{
					fout.Write( Convert::ToString( solution->charm->num_slots ) );
					fout.Write( L" " );
					fout.Write( Convert::ToString( solution->charm->abilities.Count ) );
					fout.Write( L" " );
					for each( AbilityPair^ ap in solution->charm->abilities )
					{
						fout.Write( Convert::ToString( ap->amount ) );
						fout.Write( L" " );
						fout.Write( Convert::ToString( Utility::GetIndexOf( %Ability::static_abilities, ap->ability ) ) );
						fout.Write( L" " );
					}
				}

				fout.WriteLine();
			}
			fout.Close();
		}

		void SaveConfig()
		{
			SaveConfig( CFG_FILE );
		}

		int SearchIndexMap( IndexMap^ imap, int skill_index )
		{
			for each( Generic::KeyValuePair< unsigned, unsigned > kvp in imap )
			{
				if( kvp.Value == skill_index )
					return kvp.Key;
			}
			return -1;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			SaveConfig();
		}
private: System::ComponentModel::IContainer^  components;
protected: 

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->nudHR = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->nudWeaponSlots = (gcnew System::Windows::Forms::NumericUpDown());
			this->lblElder = (gcnew System::Windows::Forms::Label());
			this->lblSlots = (gcnew System::Windows::Forms::Label());
			this->nudElder = (gcnew System::Windows::Forms::NumericUpDown());
			this->lblHR = (gcnew System::Windows::Forms::Label());
			this->grpBSkills = (gcnew System::Windows::Forms::GroupBox());
			this->btnSearch = (gcnew System::Windows::Forms::Button());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->txtSolutions = (gcnew System::Windows::Forms::RichTextBox());
			this->cmsSolutions = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->btnAdvancedSearch = (gcnew System::Windows::Forms::Button());
			this->btnCancel = (gcnew System::Windows::Forms::Button());
			this->grpResults = (gcnew System::Windows::Forms::GroupBox());
			this->btnCharms = (gcnew System::Windows::Forms::Button());
			this->grpBSkillFilters = (gcnew System::Windows::Forms::GroupBox());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->mnuFile = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuLoad = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuSave = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuExit = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuOptions = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuClearSettings = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->mnuAllowBadSkills = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuAllowPiercings = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuAllowEventArmor = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuAllowLowRankArmor = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuLanguage = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuHelp = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuCheckForUpdates = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mnuAbout = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->rdoFemale = (gcnew System::Windows::Forms::RadioButton());
			this->rdoMale = (gcnew System::Windows::Forms::RadioButton());
			this->tabHunterType = (gcnew System::Windows::Forms::TabControl());
			this->tabBlademaster = (gcnew System::Windows::Forms::TabPage());
			this->tabGunner = (gcnew System::Windows::Forms::TabPage());
			this->grpGSkillFilters = (gcnew System::Windows::Forms::GroupBox());
			this->grpGSkills = (gcnew System::Windows::Forms::GroupBox());
			this->grpSort = (gcnew System::Windows::Forms::GroupBox());
			this->cmbSort = (gcnew System::Windows::Forms::ComboBox());
			this->grpCharmFilter = (gcnew System::Windows::Forms::GroupBox());
			this->cmbCharms = (gcnew System::Windows::Forms::ComboBox());
			this->grpCharms = (gcnew System::Windows::Forms::GroupBox());
			this->btnImport = (gcnew System::Windows::Forms::Button());
			this->cmbCharmSelect = (gcnew System::Windows::Forms::ComboBox());
			this->cmsCharms = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudHR))->BeginInit();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudWeaponSlots))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudElder))->BeginInit();
			this->groupBox4->SuspendLayout();
			this->grpResults->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->tabHunterType->SuspendLayout();
			this->tabBlademaster->SuspendLayout();
			this->tabGunner->SuspendLayout();
			this->grpSort->SuspendLayout();
			this->grpCharmFilter->SuspendLayout();
			this->grpCharms->SuspendLayout();
			this->SuspendLayout();
			// 
			// nudHR
			// 
			this->nudHR->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->nudHR->Location = System::Drawing::Point(109, 20);
			this->nudHR->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {6, 0, 0, 0});
			this->nudHR->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->nudHR->Name = L"nudHR";
			this->nudHR->Size = System::Drawing::Size(35, 20);
			this->nudHR->TabIndex = 2;
			this->nudHR->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {6, 0, 0, 0});
			this->nudHR->ValueChanged += gcnew System::EventHandler(this, &Form1::HRChanged);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->nudWeaponSlots);
			this->groupBox1->Controls->Add(this->nudHR);
			this->groupBox1->Controls->Add(this->lblElder);
			this->groupBox1->Controls->Add(this->lblSlots);
			this->groupBox1->Controls->Add(this->nudElder);
			this->groupBox1->Controls->Add(this->lblHR);
			this->groupBox1->Location = System::Drawing::Point(12, 27);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(154, 115);
			this->groupBox1->TabIndex = 5;
			this->groupBox1->TabStop = false;
			// 
			// nudWeaponSlots
			// 
			this->nudWeaponSlots->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->nudWeaponSlots->Location = System::Drawing::Point(109, 80);
			this->nudWeaponSlots->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
			this->nudWeaponSlots->Name = L"nudWeaponSlots";
			this->nudWeaponSlots->Size = System::Drawing::Size(35, 20);
			this->nudWeaponSlots->TabIndex = 7;
			// 
			// lblElder
			// 
			this->lblElder->AutoSize = true;
			this->lblElder->Location = System::Drawing::Point(11, 52);
			this->lblElder->Name = L"lblElder";
			this->lblElder->Size = System::Drawing::Size(74, 13);
			this->lblElder->TabIndex = 4;
			this->lblElder->Text = L"Village Quests";
			// 
			// lblSlots
			// 
			this->lblSlots->AutoSize = true;
			this->lblSlots->Location = System::Drawing::Point(11, 82);
			this->lblSlots->Name = L"lblSlots";
			this->lblSlots->Size = System::Drawing::Size(97, 13);
			this->lblSlots->TabIndex = 1;
			this->lblSlots->Text = L"Max Weapon Slots";
			// 
			// nudElder
			// 
			this->nudElder->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->nudElder->Location = System::Drawing::Point(109, 50);
			this->nudElder->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {6, 0, 0, 0});
			this->nudElder->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->nudElder->Name = L"nudElder";
			this->nudElder->Size = System::Drawing::Size(35, 20);
			this->nudElder->TabIndex = 5;
			this->nudElder->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {6, 0, 0, 0});
			this->nudElder->ValueChanged += gcnew System::EventHandler(this, &Form1::DeleteOptions);
			// 
			// lblHR
			// 
			this->lblHR->AutoSize = true;
			this->lblHR->Location = System::Drawing::Point(10, 22);
			this->lblHR->Name = L"lblHR";
			this->lblHR->Size = System::Drawing::Size(23, 13);
			this->lblHR->TabIndex = 6;
			this->lblHR->Text = L"HR";
			// 
			// grpBSkills
			// 
			this->grpBSkills->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->grpBSkills->Location = System::Drawing::Point(6, 8);
			this->grpBSkills->Name = L"grpBSkills";
			this->grpBSkills->Size = System::Drawing::Size(166, 183);
			this->grpBSkills->TabIndex = 8;
			this->grpBSkills->TabStop = false;
			this->grpBSkills->Text = L"Skills";
			// 
			// btnSearch
			// 
			this->btnSearch->Location = System::Drawing::Point(6, 12);
			this->btnSearch->Name = L"btnSearch";
			this->btnSearch->Size = System::Drawing::Size(107, 27);
			this->btnSearch->TabIndex = 9;
			this->btnSearch->Text = L"&Quick Search";
			this->btnSearch->UseVisualStyleBackColor = true;
			this->btnSearch->Click += gcnew System::EventHandler(this, &Form1::btnSearch_Click);
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(12, 475);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(338, 10);
			this->progressBar1->Step = 1;
			this->progressBar1->TabIndex = 10;
			// 
			// txtSolutions
			// 
			this->txtSolutions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->txtSolutions->ContextMenuStrip = this->cmsSolutions;
			this->txtSolutions->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtSolutions->Location = System::Drawing::Point(6, 16);
			this->txtSolutions->Name = L"txtSolutions";
			this->txtSolutions->ReadOnly = true;
			this->txtSolutions->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::Vertical;
			this->txtSolutions->Size = System::Drawing::Size(329, 434);
			this->txtSolutions->TabIndex = 11;
			this->txtSolutions->Text = L"";
			this->txtSolutions->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::KeyDown);
			// 
			// cmsSolutions
			// 
			this->cmsSolutions->Name = L"contextMenuStrip1";
			this->cmsSolutions->Size = System::Drawing::Size(61, 4);
			this->cmsSolutions->Opening += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::contextMenuStrip1_Opening);
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->btnAdvancedSearch);
			this->groupBox4->Controls->Add(this->btnCancel);
			this->groupBox4->Controls->Add(this->btnSearch);
			this->groupBox4->Location = System::Drawing::Point(12, 423);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(338, 46);
			this->groupBox4->TabIndex = 14;
			this->groupBox4->TabStop = false;
			// 
			// btnAdvancedSearch
			// 
			this->btnAdvancedSearch->Location = System::Drawing::Point(119, 12);
			this->btnAdvancedSearch->Name = L"btnAdvancedSearch";
			this->btnAdvancedSearch->Size = System::Drawing::Size(122, 27);
			this->btnAdvancedSearch->TabIndex = 11;
			this->btnAdvancedSearch->Text = L"&Advanced Search";
			this->btnAdvancedSearch->UseVisualStyleBackColor = true;
			this->btnAdvancedSearch->Click += gcnew System::EventHandler(this, &Form1::btnAdvancedSearch_Click);
			// 
			// btnCancel
			// 
			this->btnCancel->Location = System::Drawing::Point(247, 12);
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size(85, 27);
			this->btnCancel->TabIndex = 10;
			this->btnCancel->Text = L"&Cancel";
			this->btnCancel->UseVisualStyleBackColor = true;
			this->btnCancel->Click += gcnew System::EventHandler(this, &Form1::btnCancel_Click);
			// 
			// grpResults
			// 
			this->grpResults->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpResults->Controls->Add(this->txtSolutions);
			this->grpResults->Location = System::Drawing::Point(356, 27);
			this->grpResults->Name = L"grpResults";
			this->grpResults->Size = System::Drawing::Size(341, 460);
			this->grpResults->TabIndex = 15;
			this->grpResults->TabStop = false;
			this->grpResults->Text = L"Results";
			// 
			// btnCharms
			// 
			this->btnCharms->Location = System::Drawing::Point(5, 46);
			this->btnCharms->Name = L"btnCharms";
			this->btnCharms->Size = System::Drawing::Size(78, 23);
			this->btnCharms->TabIndex = 25;
			this->btnCharms->Text = L"&My Charms...";
			this->btnCharms->UseVisualStyleBackColor = true;
			this->btnCharms->Click += gcnew System::EventHandler(this, &Form1::btnCharms_Click);
			// 
			// grpBSkillFilters
			// 
			this->grpBSkillFilters->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->grpBSkillFilters->Location = System::Drawing::Point(178, 8);
			this->grpBSkillFilters->Name = L"grpBSkillFilters";
			this->grpBSkillFilters->Size = System::Drawing::Size(146, 183);
			this->grpBSkillFilters->TabIndex = 9;
			this->grpBSkillFilters->TabStop = false;
			this->grpBSkillFilters->Text = L"Skill Filters";
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->mnuFile, this->mnuOptions, 
				this->mnuLanguage, this->mnuHelp});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(709, 24);
			this->menuStrip1->TabIndex = 16;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// mnuFile
			// 
			this->mnuFile->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->mnuLoad, this->mnuSave, 
				this->mnuExit});
			this->mnuFile->Name = L"mnuFile";
			this->mnuFile->Size = System::Drawing::Size(37, 20);
			this->mnuFile->Text = L"&File";
			// 
			// mnuLoad
			// 
			this->mnuLoad->Name = L"mnuLoad";
			this->mnuLoad->Size = System::Drawing::Size(100, 22);
			this->mnuLoad->Text = L"&Load";
			this->mnuLoad->Click += gcnew System::EventHandler(this, &Form1::mnuLoad_Click);
			// 
			// mnuSave
			// 
			this->mnuSave->Name = L"mnuSave";
			this->mnuSave->Size = System::Drawing::Size(100, 22);
			this->mnuSave->Text = L"&Save";
			this->mnuSave->Click += gcnew System::EventHandler(this, &Form1::mnuSave_Click);
			// 
			// mnuExit
			// 
			this->mnuExit->Name = L"mnuExit";
			this->mnuExit->Size = System::Drawing::Size(100, 22);
			this->mnuExit->Text = L"E&xit";
			this->mnuExit->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			// 
			// mnuOptions
			// 
			this->mnuOptions->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->mnuClearSettings, 
				this->toolStripSeparator1, this->mnuAllowBadSkills, this->mnuAllowPiercings, this->mnuAllowEventArmor, this->mnuAllowLowRankArmor});
			this->mnuOptions->Name = L"mnuOptions";
			this->mnuOptions->Size = System::Drawing::Size(61, 20);
			this->mnuOptions->Text = L"&Options";
			// 
			// mnuClearSettings
			// 
			this->mnuClearSettings->Name = L"mnuClearSettings";
			this->mnuClearSettings->Size = System::Drawing::Size(195, 22);
			this->mnuClearSettings->Text = L"&Clear Settings";
			this->mnuClearSettings->Click += gcnew System::EventHandler(this, &Form1::toolStripMenuItem1_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(192, 6);
			// 
			// mnuAllowBadSkills
			// 
			this->mnuAllowBadSkills->CheckOnClick = true;
			this->mnuAllowBadSkills->Name = L"mnuAllowBadSkills";
			this->mnuAllowBadSkills->Size = System::Drawing::Size(195, 22);
			this->mnuAllowBadSkills->Text = L"Allow &Bad Skills";
			this->mnuAllowBadSkills->Click += gcnew System::EventHandler(this, &Form1::OptionsChanged);
			// 
			// mnuAllowPiercings
			// 
			this->mnuAllowPiercings->Checked = true;
			this->mnuAllowPiercings->CheckOnClick = true;
			this->mnuAllowPiercings->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuAllowPiercings->Name = L"mnuAllowPiercings";
			this->mnuAllowPiercings->Size = System::Drawing::Size(195, 22);
			this->mnuAllowPiercings->Text = L"Allow &Piercings";
			this->mnuAllowPiercings->Click += gcnew System::EventHandler(this, &Form1::OptionsChanged);
			// 
			// mnuAllowEventArmor
			// 
			this->mnuAllowEventArmor->Checked = true;
			this->mnuAllowEventArmor->CheckOnClick = true;
			this->mnuAllowEventArmor->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuAllowEventArmor->Name = L"mnuAllowEventArmor";
			this->mnuAllowEventArmor->Size = System::Drawing::Size(195, 22);
			this->mnuAllowEventArmor->Text = L"Allow &Event Armor";
			this->mnuAllowEventArmor->Click += gcnew System::EventHandler(this, &Form1::OptionsChanged);
			// 
			// mnuAllowLowRankArmor
			// 
			this->mnuAllowLowRankArmor->Checked = true;
			this->mnuAllowLowRankArmor->CheckOnClick = true;
			this->mnuAllowLowRankArmor->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuAllowLowRankArmor->Name = L"mnuAllowLowRankArmor";
			this->mnuAllowLowRankArmor->Size = System::Drawing::Size(195, 22);
			this->mnuAllowLowRankArmor->Text = L"Allow &Low Rank Armor";
			this->mnuAllowLowRankArmor->Click += gcnew System::EventHandler(this, &Form1::OptionsChanged);
			// 
			// mnuLanguage
			// 
			this->mnuLanguage->Name = L"mnuLanguage";
			this->mnuLanguage->Size = System::Drawing::Size(71, 20);
			this->mnuLanguage->Text = L"&Language";
			// 
			// mnuHelp
			// 
			this->mnuHelp->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->mnuCheckForUpdates, 
				this->mnuAbout});
			this->mnuHelp->Name = L"mnuHelp";
			this->mnuHelp->Size = System::Drawing::Size(44, 20);
			this->mnuHelp->Text = L"&Help";
			// 
			// mnuCheckForUpdates
			// 
			this->mnuCheckForUpdates->Name = L"mnuCheckForUpdates";
			this->mnuCheckForUpdates->Size = System::Drawing::Size(171, 22);
			this->mnuCheckForUpdates->Text = L"Check for &Updates";
			this->mnuCheckForUpdates->Click += gcnew System::EventHandler(this, &Form1::UpdateMenuItem_Click);
			// 
			// mnuAbout
			// 
			this->mnuAbout->Name = L"mnuAbout";
			this->mnuAbout->Size = System::Drawing::Size(171, 22);
			this->mnuAbout->Text = L"&About";
			this->mnuAbout->Click += gcnew System::EventHandler(this, &Form1::aboutToolStripMenuItem_Click);
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->rdoFemale);
			this->groupBox6->Controls->Add(this->rdoMale);
			this->groupBox6->Location = System::Drawing::Point(12, 148);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(154, 40);
			this->groupBox6->TabIndex = 19;
			this->groupBox6->TabStop = false;
			// 
			// rdoFemale
			// 
			this->rdoFemale->AutoSize = true;
			this->rdoFemale->Location = System::Drawing::Point(78, 16);
			this->rdoFemale->Name = L"rdoFemale";
			this->rdoFemale->Size = System::Drawing::Size(59, 17);
			this->rdoFemale->TabIndex = 18;
			this->rdoFemale->TabStop = true;
			this->rdoFemale->Text = L"Female";
			this->rdoFemale->UseVisualStyleBackColor = true;
			this->rdoFemale->CheckedChanged += gcnew System::EventHandler(this, &Form1::DeleteOptions);
			// 
			// rdoMale
			// 
			this->rdoMale->AutoSize = true;
			this->rdoMale->Checked = true;
			this->rdoMale->Location = System::Drawing::Point(15, 16);
			this->rdoMale->Name = L"rdoMale";
			this->rdoMale->Size = System::Drawing::Size(48, 17);
			this->rdoMale->TabIndex = 0;
			this->rdoMale->TabStop = true;
			this->rdoMale->Text = L"Male";
			this->rdoMale->UseVisualStyleBackColor = true;
			this->rdoMale->CheckedChanged += gcnew System::EventHandler(this, &Form1::DeleteOptions);
			// 
			// tabHunterType
			// 
			this->tabHunterType->Controls->Add(this->tabBlademaster);
			this->tabHunterType->Controls->Add(this->tabGunner);
			this->tabHunterType->Location = System::Drawing::Point(12, 194);
			this->tabHunterType->Name = L"tabHunterType";
			this->tabHunterType->SelectedIndex = 0;
			this->tabHunterType->Size = System::Drawing::Size(338, 223);
			this->tabHunterType->TabIndex = 24;
			// 
			// tabBlademaster
			// 
			this->tabBlademaster->BackColor = System::Drawing::SystemColors::Control;
			this->tabBlademaster->Controls->Add(this->grpBSkills);
			this->tabBlademaster->Controls->Add(this->grpBSkillFilters);
			this->tabBlademaster->Location = System::Drawing::Point(4, 22);
			this->tabBlademaster->Name = L"tabBlademaster";
			this->tabBlademaster->Padding = System::Windows::Forms::Padding(3);
			this->tabBlademaster->Size = System::Drawing::Size(330, 197);
			this->tabBlademaster->TabIndex = 0;
			this->tabBlademaster->Text = L"Blademaster";
			// 
			// tabGunner
			// 
			this->tabGunner->BackColor = System::Drawing::SystemColors::Control;
			this->tabGunner->Controls->Add(this->grpGSkillFilters);
			this->tabGunner->Controls->Add(this->grpGSkills);
			this->tabGunner->Location = System::Drawing::Point(4, 22);
			this->tabGunner->Name = L"tabGunner";
			this->tabGunner->Padding = System::Windows::Forms::Padding(3);
			this->tabGunner->Size = System::Drawing::Size(330, 197);
			this->tabGunner->TabIndex = 1;
			this->tabGunner->Text = L"Gunner";
			// 
			// grpGSkillFilters
			// 
			this->grpGSkillFilters->Location = System::Drawing::Point(178, 8);
			this->grpGSkillFilters->Name = L"grpGSkillFilters";
			this->grpGSkillFilters->Size = System::Drawing::Size(146, 183);
			this->grpGSkillFilters->TabIndex = 10;
			this->grpGSkillFilters->TabStop = false;
			this->grpGSkillFilters->Text = L"Skill Filters";
			// 
			// grpGSkills
			// 
			this->grpGSkills->Location = System::Drawing::Point(6, 8);
			this->grpGSkills->Name = L"grpGSkills";
			this->grpGSkills->Size = System::Drawing::Size(166, 183);
			this->grpGSkills->TabIndex = 9;
			this->grpGSkills->TabStop = false;
			this->grpGSkills->Text = L"Skills";
			// 
			// grpSort
			// 
			this->grpSort->Controls->Add(this->cmbSort);
			this->grpSort->Location = System::Drawing::Point(172, 27);
			this->grpSort->Name = L"grpSort";
			this->grpSort->Size = System::Drawing::Size(178, 45);
			this->grpSort->TabIndex = 0;
			this->grpSort->TabStop = false;
			this->grpSort->Text = L"Sort Results By";
			// 
			// cmbSort
			// 
			this->cmbSort->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->cmbSort->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSort->FormattingEnabled = true;
			this->cmbSort->Items->AddRange(gcnew cli::array< System::Object^  >(11) {L"None", L"Dragon res", L"Fire res", L"Ice res", 
				L"Thunder res", L"Water res", L"Base defence", L"Max defence", L"Difficulty", L"Rarity", L"Slots spare"});
			this->cmbSort->Location = System::Drawing::Point(6, 16);
			this->cmbSort->Name = L"cmbSort";
			this->cmbSort->Size = System::Drawing::Size(166, 21);
			this->cmbSort->TabIndex = 0;
			this->cmbSort->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cmbSort_SelectedIndexChanged);
			// 
			// grpCharmFilter
			// 
			this->grpCharmFilter->Controls->Add(this->cmbCharms);
			this->grpCharmFilter->Location = System::Drawing::Point(172, 78);
			this->grpCharmFilter->Name = L"grpCharmFilter";
			this->grpCharmFilter->Size = System::Drawing::Size(178, 45);
			this->grpCharmFilter->TabIndex = 1;
			this->grpCharmFilter->TabStop = false;
			this->grpCharmFilter->Text = L"Filter Results by Charm";
			// 
			// cmbCharms
			// 
			this->cmbCharms->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->cmbCharms->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbCharms->FormattingEnabled = true;
			this->cmbCharms->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"None", L"All"});
			this->cmbCharms->Location = System::Drawing::Point(6, 16);
			this->cmbCharms->Name = L"cmbCharms";
			this->cmbCharms->Size = System::Drawing::Size(166, 21);
			this->cmbCharms->TabIndex = 0;
			this->cmbCharms->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cmbCharms_SelectedIndexChanged);
			// 
			// grpCharms
			// 
			this->grpCharms->Controls->Add(this->btnImport);
			this->grpCharms->Controls->Add(this->cmbCharmSelect);
			this->grpCharms->Controls->Add(this->btnCharms);
			this->grpCharms->Location = System::Drawing::Point(172, 129);
			this->grpCharms->Name = L"grpCharms";
			this->grpCharms->Size = System::Drawing::Size(176, 77);
			this->grpCharms->TabIndex = 25;
			this->grpCharms->TabStop = false;
			this->grpCharms->Text = L"Charms";
			// 
			// btnImport
			// 
			this->btnImport->Location = System::Drawing::Point(87, 46);
			this->btnImport->Name = L"btnImport";
			this->btnImport->Size = System::Drawing::Size(84, 23);
			this->btnImport->TabIndex = 26;
			this->btnImport->Text = L"&Import";
			this->btnImport->UseVisualStyleBackColor = true;
			this->btnImport->Click += gcnew System::EventHandler(this, &Form1::btnImport_Click);
			// 
			// cmbCharmSelect
			// 
			this->cmbCharmSelect->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->cmbCharmSelect->ContextMenuStrip = this->cmsCharms;
			this->cmbCharmSelect->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbCharmSelect->FormattingEnabled = true;
			this->cmbCharmSelect->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"Use no charms", L"Use only my charms", L"Use only slotted charms", 
				L"Use up to one skill charms", L"Use only two skill charms"});
			this->cmbCharmSelect->Location = System::Drawing::Point(6, 19);
			this->cmbCharmSelect->Name = L"cmbCharmSelect";
			this->cmbCharmSelect->Size = System::Drawing::Size(164, 21);
			this->cmbCharmSelect->TabIndex = 1;
			// 
			// cmsCharms
			// 
			this->cmsCharms->Name = L"cmsCharms";
			this->cmsCharms->Size = System::Drawing::Size(61, 4);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(709, 492);
			this->Controls->Add(this->grpCharmFilter);
			this->Controls->Add(this->grpSort);
			this->Controls->Add(this->groupBox6);
			this->Controls->Add(this->grpResults);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->menuStrip1);
			this->Controls->Add(this->grpCharms);
			this->Controls->Add(this->tabHunterType);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->groupBox4);
			this->Name = L"Form1";
			this->Text = L"Athena\'s A.S.S. for MHP3rd";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudHR))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudWeaponSlots))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudElder))->EndInit();
			this->groupBox4->ResumeLayout(false);
			this->grpResults->ResumeLayout(false);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->groupBox6->ResumeLayout(false);
			this->groupBox6->PerformLayout();
			this->tabHunterType->ResumeLayout(false);
			this->tabBlademaster->ResumeLayout(false);
			this->tabGunner->ResumeLayout(false);
			this->grpSort->ResumeLayout(false);
			this->grpCharmFilter->ResumeLayout(false);
			this->grpCharms->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


private: 
	System::Void FormulateQuery( const bool danger, const bool use_gunner_skills )
	{
		query = gcnew Query;
		for( int i = 0; i < int( Armor::ArmorType::NumArmorTypes ); ++i )
		{
			query->rel_armor.Add( gcnew List_t< Armor^ >() );
			query->inf_armor.Add( gcnew List_t< Armor^ >() );
		}
		query->weapon_slots_allowed = int( nudWeaponSlots->Value );
		query->elder_star = int( nudElder->Value );
		query->hr = int( nudHR->Value );

		query->gender = rdoMale->Checked ? Gender::MALE : Gender::FEMALE;
		query->hunter_type = !use_gunner_skills ? HunterType::BLADEMASTER : HunterType::GUNNER;
		query->include_piercings = mnuAllowPiercings->Checked;
		query->allow_bad = mnuAllowBadSkills->Checked;
		query->allow_event = mnuAllowEventArmor->Checked;
		query->allow_low_rank = mnuAllowLowRankArmor->Checked;
		
		if( !use_gunner_skills )
		{
			for( unsigned i = 0; i < NumSkills; ++i )
				if( bSkills[ i ]->SelectedIndex >= 0 )
					query->skills.Add( data->FindSkill( bIndexMaps[ i ][ bSkills[ i ]->SelectedIndex ] ) );
		}
		else
		{
			for( unsigned i = 0; i < NumSkills; ++i )
				if( gSkills[ i ]->SelectedIndex >= 0 )
					query->skills.Add( data->FindSkill( gIndexMaps[ i ][ gSkills[ i ]->SelectedIndex ] ) );
		}
		data->GetRelevantData( query );
	}

	System::Void OptionsChanged(System::Object^  sender, System::EventArgs^  e)
	{
		SaveConfig();
		if( sender != mnuAllowBadSkills )
			DeleteOptions( sender, e );
	}

	System::Void StartThread( Query^ query, Charm^ ct )
	{
		BackgroundWorker^ new_thread = gcnew BackgroundWorker;
		new_thread->WorkerSupportsCancellation = true;
		new_thread->WorkerReportsProgress = true;
		new_thread->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::backgroundWorker1_DoWork);
		new_thread->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &Form1::backgroundWorker1_RunWorkerCompleted);
		new_thread->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &Form1::backgroundWorker1_ProgressChanged);
		new_thread->RunWorkerAsync( gcnew QueryCharmPair( query, ct ) );
		workers.Add( new_thread );
	}

	System::Void AddCharmToOptimalList( List_t< Charm^ >% lst, Charm^ new_charm )
	{
		for( int i = 0; i < lst.Count; ++i )
		{
			Charm^ curr_charm = lst[ i ];
			if( new_charm->StrictlyBetterThan( curr_charm ) )
				lst.RemoveAt( i-- );
			else if( curr_charm->StrictlyBetterThan( new_charm ) ||
					 curr_charm->BasicallyTheSameAs( new_charm ) )
				return;
		}
		lst.Add( new_charm );
	}

	System::Void StartSearch()
	{
		progressBar1->Value = 0;
		total_progress = 0;
		if( query->skills.Count > 0 )
		{
			existing_armor.Clear();
			btnSearch->Enabled = false;
			btnAdvancedSearch->Enabled = false;
			btnCancel->Enabled = true;
			cmbSort->Enabled = false;
			cmbCharms->Enabled = false;
			charm_solution_map.Clear();
			cmbCharms->SelectedIndex = 1;
			txtSolutions->Text = L"Solutions found: 0";
			final_solutions.Clear();
			all_solutions.Clear();
			no_charm_solutions.Clear();
			last_result = nullptr;
			last_search_gunner = tabHunterType->SelectedIndex == 1;
			finished_workers = 0;

			//Check for Auto-Guard first
			if( cmbCharmSelect->SelectedIndex > 1 ) 
			{
				for each( Ability^ ab in query->rel_abilities )
				{
					if( ab->auto_guard )
					{
						for each( Charm^ ch in CharmDatabase::mycharms )
						{
							if( ch->abilities.Count == 1 && ch->abilities[ 0 ]->ability == ab )
							{
								StartThread( query, ch );
								break;
							}
						}
						return;
					}
				}
			}

			switch( cmbCharmSelect->SelectedIndex )
			{
			case 0: // no charms
				{
					StartThread( query, nullptr );
				}
				break;
			case 1: // my charms only
				{
					List_t< Charm^ > to_search;
					for each( Charm^ ct in CharmDatabase::mycharms )
					{
						AddCharmToOptimalList( to_search, ct );
					}
					StartThread( query, nullptr );
					for each( Charm^ ct in to_search )
					{
						StartThread( query, ct );
					}
				}
				break;
			case 2: // slotted charms only
				{
					/*StartThread( query, nullptr );
					StartThread( query, gcnew Charm( 1 ) );
					if( query->hr >= 4 )
						StartThread( query, gcnew Charm( 2 ) );
					if( query->hr >= 5 )*/
						StartThread( query, gcnew Charm( 3 ) );
				}
				break;
			case 3: // one skill charms
				{
					List_t< Charm^ >^ charms = CharmDatabase::GetCharms( query, false );
					Assert( charms, L"No one-skill charms found" );
					/*
					//plus any relevant two-skill charms
					if( true )
					for each( Charm^ ct in CharmDatabase::mycharms )
					{
						if( ct->abilities.Count == 2 )
						{
							bool ab_rel = true;

							//if both skills relevant, use charm
							for each( AbilityPair^ ap in ct->abilities )
							{
								bool rel = false;
								for each( Ability^ ab in query->rel_abilities )
								{
									rel |= ab == ap->ability;
								}
								if( !rel )
								{
									ab_rel = false;
									break;
								}
							}
							if( ab_rel )
							{
								AddCharmToOptimalList( *charms, ct );
							}
						}
					}*/
					for each( Charm^ ct in charms )
						StartThread( query, ct );
					//StartThread( query, charms[ 4 ] );
					//break;
				}
				break;
			case 4: // two skill charms
				{
					List_t< Charm^ >^ charms = CharmDatabase::GetCharms( query, true );
					Assert( charms, L"No two-skill charms found" );

					for each( Charm^ ct in charms )
						StartThread( query, ct );
					//StartThread( query, charms[ 17 ] );
				}
				break;
			}
		}
	}
/*
	void updatedata( String^ update, String^ jap_file )
	{
		IO::StreamReader eng( update );
		IO::StreamWriter out( update + L".out" );
		while( !eng.EndOfStream )
		{
			String^ eng_line = eng.ReadLine();
			if( eng_line == L"" || eng_line[ 0 ] == L'#' )
				continue;
			int comma = eng_line->IndexOf( L',' );
			String^ eng_name = eng_line->Substring( 0, comma );
			String^ jap_name = eng_line->Substring( comma + 1, eng_line->IndexOf( L',', comma + 1 ) - comma - 1 );

			IO::StreamReader jap( jap_file );
			bool found = false;
			while( !jap.EndOfStream )
			{
				String^ jap_line = jap.ReadLine();
				if( jap_line == L"" || jap_line[ 0 ] == L'#' )
					continue;
				String^ name = jap_line->Substring( 0, jap_line->IndexOf( L',' ) );
				if( name == jap_name )
				{
					out.WriteLine( eng_name + L"," + jap_line );
					found = true;
					break;
				}
			}
			assert( found );
		}
	}

	void updatedata()
	{
		updatedata( L"Data/head.txt", L"C:/Users/Rhys/Desktop/GANSIMU_MHP3_0.9.5/MH3EQUIP_HEAD.txt" );
		updatedata( L"Data/body.txt", L"C:/Users/Rhys/Desktop/GANSIMU_MHP3_0.9.5/MH3EQUIP_BODY.txt" );
		updatedata( L"Data/arms.txt", L"C:/Users/Rhys/Desktop/GANSIMU_MHP3_0.9.5/MH3EQUIP_ARM.txt" );
		updatedata( L"Data/waist.txt", L"C:/Users/Rhys/Desktop/GANSIMU_MHP3_0.9.5/MH3EQUIP_WST.txt" );
		updatedata( L"Data/legs.txt", L"C:/Users/Rhys/Desktop/GANSIMU_MHP3_0.9.5/MH3EQUIP_LEG.txt" );

		updatedata( L"Data/decorations.txt", L"C:/Users/Rhys/Desktop/GANSIMU_MHP3_0.9.5/MH3DECO.txt" );
	}*/

	System::Void btnAdvancedSearch_Click(System::Object^  sender, System::EventArgs^  e)
	{
		//updatedata();
		//return;
		FormulateQuery( true, tabHunterType->SelectedIndex == 1 );
		frmAdvanced advanced_search( query );
		if( tabHunterType->SelectedIndex == 0 )
		{
			advanced_search.CheckResult( blast_options );
			advanced_search.ShowDialog( this );
			blast_options = advanced_search.result;
		}
		else if( tabHunterType->SelectedIndex == 1 )
		{
			advanced_search.CheckResult( glast_options );
			advanced_search.ShowDialog( this );
			glast_options = advanced_search.result;
		}
		else return;

		if( advanced_search.DialogResult != ::DialogResult::OK ) return;

		for( int p = 0; p < int( Armor::ArmorType::NumArmorTypes ); ++p )
		{
			query->rel_armor[ p ]->Clear();
			for( int i = 0; i < query->inf_armor[ p ]->Count; ++i )
			{
				if( advanced_search.boxes[ p ]->Items[ i ]->Checked )
					query->rel_armor[ p ]->Add( query->inf_armor[ p ][ i ] );
			}
		}
		query->rel_decorations.Clear();
		for( int i = 0; i < query->inf_decorations.Count; ++i )
		{
			if( advanced_search.boxes[ int( Armor::ArmorType::NumArmorTypes ) ]->Items[ i ]->Checked )
				query->rel_decorations.Add( query->inf_decorations[ i ] );
		}

		StartSearch();
	}

	System::Void btnSearch_Click(System::Object^  sender, System::EventArgs^  e)
	{		
		FormulateQuery( false, tabHunterType->SelectedIndex == 1 );
		
		StartSearch();
	}

	System::Void btnCancel_Click(System::Object^  sender, System::EventArgs^  e)
	{
		for each( BackgroundWorker^ worker in workers )
			worker->CancelAsync();
		btnCancel->Enabled = false;
		btnSearch->Enabled = true;
		btnAdvancedSearch->Enabled = true;
		progressBar1->Value = 0;
	}

	System::Void cmbSkillFilter_SelectedIndexChanged(System::Object^  sender, List_t< ComboBox^ >% skills, List_t< ComboBox^ >% skill_filters, List_t< IndexMap^ >% index_maps )
	{
		List_t< Ability^ > old_skills;
		int index = -1;
		Skill^ selected_skill = nullptr;
		for( unsigned i = 0; i < NumSkills; ++i )
		{
			if( sender == skill_filters[ i ] )
				index = i;
			if( skills[ i ]->SelectedIndex == -1 ) continue;
			Skill^ skill = Skill::static_skills[ index_maps[ i ][ skills[ i ]->SelectedIndex ] ];
			if( sender == skill_filters[ i ] )
				selected_skill = skill;
			else old_skills.Add( skill->ability );
		}
		if( index == -1 ) return;
		skills[ index ]->BeginUpdate();
		InitSkills( skills[ index ], index_maps[ index ], skill_filters[ index ]->SelectedIndex, %old_skills );
		ResetSkill( skills[ index ], index_maps[ index ], selected_skill );
		skills[ index ]->EndUpdate();
	}

	System::Void cmbSkillFilter_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if( tabHunterType->SelectedIndex == 0 )
		{
			blast_options = nullptr;
			cmbSkillFilter_SelectedIndexChanged( sender, bSkills, bSkillFilters, bIndexMaps );
		}
		else if( tabHunterType->SelectedIndex == 1 )
		{
			glast_options = nullptr;
			cmbSkillFilter_SelectedIndexChanged( sender, gSkills, gSkillFilters, gIndexMaps );
		}
	}

	System::Void cmbSkill_SelectedIndexChanged( System::Object^ sender, List_t< ComboBox^ >% skills, List_t< ComboBox^ >% skill_filters, List_t< IndexMap^ >% index_maps )
	{
		int index = -1;
		for( int i = 0; i < NumSkills; ++i )
			if( sender == skills[ i ] )
				index = i;
		if( index == -1 ) return;
		if( skills[ index ]->SelectedIndex == 0 )
		{
			skills[ index ]->SelectedIndex = -1;
			return;
		}
		lock_skills = true;
		for( int i = 0; i < NumSkills; ++i )
		{
			if( i == index ) continue;
			Skill^ skill = skills[ i ]->SelectedIndex == -1 ? nullptr : Skill::static_skills[ index_maps[ i ][ skills[ i ]->SelectedIndex ] ];
			List_t< Ability^ > old_skills;
			for( int j = 0; j < NumSkills; ++j )
				if( j != i && skills[ j ]->SelectedIndex != -1 )
					old_skills.Add( Skill::static_skills[ index_maps[ j ][ skills[ j ]->SelectedIndex ] ]->ability );
			skills[ i ]->BeginUpdate();
			InitSkills( skills[ i ], index_maps[ i ], skill_filters[ i ]->SelectedIndex, %old_skills );
			ResetSkill( skills[ i ], index_maps[ i ], skill );
			skills[ i ]->EndUpdate();
		}
		lock_skills = false;
	}

	System::Void cmbSkill_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		if( lock_skills ) return;
		if( tabHunterType->SelectedIndex == 0 )
		{
			blast_options = nullptr;
			cmbSkill_SelectedIndexChanged( sender, bSkills, bSkillFilters, bIndexMaps );
		}
		else if( tabHunterType->SelectedIndex == 1 )
		{
			glast_options = nullptr;
			cmbSkill_SelectedIndexChanged( sender, gSkills, gSkillFilters, gIndexMaps );
		}
	}

	System::Void UpdateResultString( List_t< Solution^ >^ solutions )
	{
		//if( !solutions || solutions->Count == 0 ) return;
		if( !StringTable::text )
			return;
		System::Text::StringBuilder sb( solutions->Count * 1024 );
		int limit = solutions->Count;

		if( last_result ) sb.Append( last_result );
		
		System::String^ dash = L"-----------------";
		for each( Solution^ solution in solutions )
		{
			sb.Append( endl );
			for each( Armor^ armor in solution->armors )
			{
				if( armor )
				{
					sb.Append( armor->name );
					if( armor->no_skills )
					{
						if( armor->num_slots == 1 )
							sb.Append( RunString( OrAnythingWithSingular ) )->Append( Convert::ToString( armor->num_slots ) )->Append( AddString( SlotBracket ) );
						else
							sb.Append( RunString( OrAnythingWithPlural ) )->Append( Convert::ToString( armor->num_slots ) )->Append( AddString( SlotsBracket ) );
					}
					else if( armor->torso_inc )
						sb.Append( AddString( OrAnythingWithTorsoInc ) );
					sb.Append( endl );
				}
			}
			if( solution->charm )
			{
				sb.AppendLine( dash );
				sb.AppendLine( solution->charm->GetName() );
			}
			if( solution->decorations.Count > 0 )
			{
				sb.AppendLine( dash );
				Generic::Dictionary< Decoration^, unsigned > deco_dict;
				for each( Decoration^ decoration in solution->decorations )
				{
					if( !deco_dict.ContainsKey( decoration ) )
						deco_dict.Add( decoration, 1 );
					else deco_dict[ decoration ]++;
				}
				Generic::Dictionary< Decoration^, unsigned >::Enumerator iter = deco_dict.GetEnumerator();
				while( iter.MoveNext() )
					sb.Append( Convert::ToString( iter.Current.Value ) )->Append( L"x " )->AppendLine( iter.Current.Key->name );
			}
			if( solution->total_slots_spare > 0 || cmbSort->SelectedIndex == 10 )
			{
				if( solution->total_slots_spare == 1 )
					sb.AppendLine( L"1" + AddString( SlotSpare ) );
				else if( solution->total_slots_spare == 0 )
					sb.AppendLine( L"0" + AddString( SlotsSpare ) );
				else if( !solution->slots_spare || ( solution->total_slots_spare <= 3 && solution->slots_spare[ solution->total_slots_spare ] == 1 ) )
				{
					sb.Append( Convert::ToString( solution->total_slots_spare ) );
					sb.AppendLine( AddString( SlotsSpare ) );
				}
				else
				{
					sb.Append( Convert::ToString( solution->total_slots_spare ) );
					sb.Append( RunString( SlotsSpare ) + L"(" );
					bool first = true;
					for( unsigned i = 1; i <= 3; ++i )
						for( unsigned j = 0; j < solution->slots_spare[ i ]; ++j )
						{
							sb.Append( ( first ? L"" : L"+" ) + Convert::ToString( i ) );
							first = false;
						}
					sb.AppendLine( L")" );
				}
			}
			if( cmbSort->SelectedIndex > 0 && cmbSort->SelectedIndex != 10 )
			{
				if( cmbSort->SelectedIndex == 1 ) sb.Append( solution->dragon_res );
				else if( cmbSort->SelectedIndex == 2 ) sb.Append( solution->fire_res );
				else if( cmbSort->SelectedIndex == 3 ) sb.Append( solution->ice_res );
				else if( cmbSort->SelectedIndex == 4 ) sb.Append( solution->thunder_res );
				else if( cmbSort->SelectedIndex == 5 ) sb.Append( solution->water_res );
				else if( cmbSort->SelectedIndex == 6 ) sb.Append( solution->defence );
				else if( cmbSort->SelectedIndex == 7 ) sb.Append( solution->max_defence );
				else if( cmbSort->SelectedIndex == 8 ) sb.Append( solution->difficulty );
				else if( cmbSort->SelectedIndex == 9 ) sb.Append( solution->rarity );
				sb.Append( L" " )->AppendLine( (String^)cmbSort->SelectedItem );
			}
			if( solution->extra_skills.Count > 0 )
			{
				sb.AppendLine( dash );
				for each( Skill^ skill in solution->extra_skills )
				{
					if( !Utility::Contains( %query->skills, skill ) )
						sb.AppendLine( skill->name );
				}
			}
		}

		if( %final_solutions != solutions )
			final_solutions.AddRange( solutions );

		System::Text::StringBuilder sb2;
		sb2.Append( StartString( SolutionsFound ) )->AppendLine( Convert::ToString( final_solutions.Count ) );
		if( limit > MAX_LIMIT )
		{
			sb2.Append( StartString( ShowingFirst ) )->Append( Convert::ToString( MAX_LIMIT ) )->AppendLine( AddString( SolutionsOnly ) );
			limit = MAX_LIMIT;
		}

		sb2.Append( last_result = sb.ToString() );
		
		results_mutex->WaitOne();
		//if( updating_language || language >= 0 && mnuLanguage->DropDownItems[ language ]->ToString() == L"Japanese" )
		{
			//noobery to ensure entire textbox is correct font
			txtSolutions->SuspendLayout();
			txtSolutions->Enabled = false;
			txtSolutions->Text = sb2.ToString();
			txtSolutions->SelectionStart = 0;
			txtSolutions->SelectionLength = txtSolutions->Text->Length;
			txtSolutions->SelectionFont = gcnew Drawing::Font( L"Microsoft Sans Serif", txtSolutions->Font->Size );
			txtSolutions->SelectionLength = 0;
			txtSolutions->Enabled = true;
			txtSolutions->ResumeLayout();
		}
		//else txtSolutions->Text = sb2.ToString();
		
		results_mutex->ReleaseMutex();
	}

#pragma region Worker Thread Stuff

	__int64 HashArmor( List_t< Armor^ >% armors )
	{
		__int64 res = 0;
		for( int i = 0; i < armors.Count; ++i )
		{
			res += armors[ i ]->index << ( i * 3 );
		}
		return res;
	}

	bool SolutionExists( String^ charm, Solution^ sol )
	{
		if( !existing_armor.ContainsKey( charm ) )
		{
			existing_armor.Add( charm, gcnew Generic::Dictionary< __int64, bool > );
			existing_armor[ charm ]->Add( HashArmor( sol->armors ), true );
			return false;
		}
		else
		{
			Generic::Dictionary< __int64, bool >^ set = existing_armor[ charm ];
			const __int64 armor_hash = HashArmor( sol->armors );
			if( set->ContainsKey( armor_hash ) )
				return true;
			else
			{
				set->Add( armor_hash, true );
				return false;
			}
		}
	}

	System::Void AddSolution( String^ charm, Solution^ sol )
	{
		if( SolutionExists( charm, sol ) ) return;
		if( !charm_solution_map.ContainsKey( charm ) )
			charm_solution_map.Add( charm, gcnew List_t< Solution^ > );
		charm_solution_map[ charm ]->Add( sol );
		all_solutions.Add( sol );
	}

	System::Void AddSolutions( List_t< Solution^ >^ solutions )
	{
		charm_map_mutex->WaitOne();
		for each( Solution^ sol in solutions )
		{
			if( sol->charm )
				AddSolution( sol->charm->GetName(), sol );
			else
			{
				all_solutions.Add( sol );
				no_charm_solutions.Add( sol );
			}
		}
		charm_map_mutex->ReleaseMutex();
	}

	System::Void UpdateCharmComboBox()
	{
		UpdateCharmComboBox( -1 );
	}

	System::Void UpdateCharmComboBox( const int new_index )
	{
		Charm^ selected_charm = cmbCharms->SelectedIndex > 1 ? charm_box_charms[ cmbCharms->SelectedIndex - 2 ] : nullptr;
		const int old_index = cmbCharms->SelectedIndex;
		charm_box_charms.Clear();
		List_t< String^ > to_order;
		Map_t< String^, Charm^ > charm_map;
		for each( Solution^ s in all_solutions )
		{
			if( s->charm )
			{
				String^ name = s->charm->GetName();
				if( !charm_map.ContainsKey( name ) )
				{
					to_order.Add( name );
					charm_map.Add( name, s->charm );
				}
			}
		}
		to_order.Sort();
		cmbCharms->BeginUpdate();
		cmbCharms->Items->Clear();
		cmbCharms->Items->Add( BasicString( None ) );
		cmbCharms->Items->Add( StaticString( All ) );
		for each( String^ str in to_order )
		{
			charm_box_charms.Add( charm_map[ str ] );
			cmbCharms->Items->Add( str );
		}
		if( new_index == -1 )
		{
			if( old_index == -1 )
				cmbCharms->SelectedIndex = 1;
			else if( old_index < 2 )
				cmbCharms->SelectedIndex = old_index;
			else
			{
				for( int i = 2; i < cmbCharms->Items->Count; ++i )
				{
					if( cmbCharms->Items[ i ]->ToString() == selected_charm->GetName() )
					{
						cmbCharms->SelectedIndex = i;
						break;
					}
				}
			}
		}
		else cmbCharms->SelectedIndex = new_index;
		cmbCharms->EndUpdate();
	}

	System::Void backgroundWorker1_RunWorkerCompleted( Object^ /*sender*/, RunWorkerCompletedEventArgs^ e )
	{
		if( e->Error != nullptr )
		{
			MessageBox::Show( e->Error->Message );
			progressBar1->Value = 0;
			return;
		}
		else if( e->Cancelled )
		{
			progressBar1->Value = 0;
			return;
		}
		else if( e->Result )
		{
			AddSolutions( static_cast< List_t< Solution^ >^ >( e->Result ) );
			progress_mutex->WaitOne();
			if( ++finished_workers >= unsigned( workers.Count ) )
			{
				btnSearch->Enabled = true;
				btnAdvancedSearch->Enabled = true;
				btnCancel->Enabled = false;
				cmbSort->Enabled = true;
				cmbCharms->Enabled = true;
				progressBar1->Value = 100;
				workers.Clear();
				SaveConfig();
				UpdateCharmComboBox( 1 );
			}
			else txtSolutions->Text = StartString( SolutionsFound ) + Convert::ToString( all_solutions.Count );
			progress_mutex->ReleaseMutex();
		}
	}		
	

	System::Void backgroundWorker1_ProgressChanged( Object^ /*sender*/, ProgressChangedEventArgs^ e )
	{
		progress_mutex->WaitOne();
		total_progress += e->ProgressPercentage;
		progressBar1->Value = total_progress / workers.Count;
		progress_mutex->ReleaseMutex();
		if( e->UserState )
		{
			AddSolutions( static_cast< List_t< Solution^ >^ >( e->UserState ) );
			txtSolutions->Text = StartString( SolutionsFound ) + Convert::ToString( all_solutions.Count );
		}
	}

	System::Void backgroundWorker1_DoWork( System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e )
	{
		BackgroundWorker^ worker = static_cast< BackgroundWorker^ >( sender );
		QueryCharmPair^ qc = static_cast< QueryCharmPair^ >( e->Argument );
		Query^ query = qc->query;

		List_t< Solution ^ >^ solutions = gcnew List_t< Solution^ >;
		List_t< Armor^ >^ head  = query->rel_armor[ int( Armor::ArmorType::HEAD ) ];
		List_t< Armor^ >^ body  = query->rel_armor[ int( Armor::ArmorType::BODY ) ];
		List_t< Armor^ >^ arms  = query->rel_armor[ int( Armor::ArmorType::ARMS ) ];
		List_t< Armor^ >^ waist = query->rel_armor[ int( Armor::ArmorType::WAIST ) ];
		List_t< Armor^ >^ legs  = query->rel_armor[ int( Armor::ArmorType::LEGS ) ];
 
		if( head->Count  == 0 ) head->Add( nullptr );
		if( body->Count  == 0 ) body->Add( nullptr );
		if( arms->Count  == 0 ) arms->Add( nullptr );
		if( waist->Count == 0 ) waist->Add( nullptr );
		if( legs->Count  == 0 ) legs->Add( nullptr );
		
		//int solutions_found = 0;
		unsigned last_percent = 0;
		bool new_stuff = false;
		for( int i = 0; i < head->Count; ++i )
		{
			for( int j = 0; j < body->Count; ++j )
			{
				const int progress = ( 100 * ( i * body->Count + j ) ) / ( head->Count * body->Count );
			
				if( !new_stuff )
				{
					worker->ReportProgress( progress - last_percent );
					last_percent = progress;
				}
				else
				{
					List_t< Solution ^ >^ temp = solutions; //handle race-condition: shouldn't modify solution list while iterating through it
					solutions = gcnew List_t< Solution^ >;
					worker->ReportProgress( progress - last_percent, temp );
					last_percent = progress;
					new_stuff = false;
				}
				for( int k = 0; k < arms->Count; ++k )
				{
					for( int l = 0; l < waist->Count; ++l )
					{
						for( int m = 0; m < legs->Count; ++m )
						{
							if( worker->CancellationPending )
							{
								e->Result = solutions;
								return;
							}
							Solution^ job = gcnew Solution();
							job->armors.Add( head[ i ] );
							job->armors.Add( body[ j ] );
							job->armors.Add( arms[ k ] );
							job->armors.Add( waist[ l ] );
							job->armors.Add( legs[ m ] );
							job->charm = qc->charm_template ? gcnew Charm( qc->charm_template ) : nullptr;

							if( job->MatchesQuery( query ) )
							{
								solutions->Add( job );
								new_stuff = true;
								/*if( ++solutions_found >= MAX_LIMIT )
								{
									e->Result = solutions;
									worker->ReportProgress( 100 - last_percent );
									return;
								}*/
							}							
						}
					}
				}
			}
		}
		worker->ReportProgress( 100 - last_percent );
		e->Result = solutions;
	}
#pragma endregion

	System::Void FindDialogClosed( System::Object^ sender, System::EventArgs^ e )
	{
		find_dialog = nullptr;
	}

	System::Void FindDialogFoundText( System::Object^ sender, System::EventArgs^ e )
	{
		frmFind^ find = (frmFind^)sender;
		if( find->index == -1 )
		{
			txtSolutions->SelectionStart = txtSolutions->Text->Length;
			txtSolutions->SelectionLength = 0;
		}
		else
		{
			txtSolutions->SelectionStart = find->index;
			txtSolutions->SelectionLength = find->length;
		}
		txtSolutions->ScrollToCaret();
		txtSolutions->Focus();
	}

	System::Void KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		if( sender == txtSolutions && e->Control )
		{
			if( e->KeyValue == L'A' )
			{
				txtSolutions->SelectAll();
				e->Handled = true;
				e->SuppressKeyPress = true;
			}
			else if( e->KeyValue == L'F' && !find_dialog )
			{
				find_dialog = gcnew frmFind( txtSolutions );
				find_dialog->DialogClosing += gcnew EventHandler( this, &Form1::FindDialogClosed );
				find_dialog->TextFound += gcnew EventHandler( this, &Form1::FindDialogFoundText );
				find_dialog->Show( this );
				e->Handled = true;
				e->SuppressKeyPress = true;
			}
			else if( e->KeyValue == L'X' )
			{
				
			}
		}
	}

	System::Void cmbSort_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
	{
		static int last_index = -1;
		if( cmbSort->SelectedIndex == last_index ) return;
		last_index = cmbSort->SelectedIndex;
		if( data )
		{
			SaveConfig();
			if( final_solutions.Count > 0 )
			{
				SortResults();
				last_result = nullptr;
				UpdateResultString( %final_solutions );
			}
		}
	}

	System::Void UpdateComboBoxLanguage( ComboBox^ cb )
	{
		cb->BeginUpdate();
		for( int i = 1; i < cb->Items->Count; ++i )
		{
			String^ name = (String^)cb->Items[ i ];
			Skill^ skill = Skill::FindSkill( name );
			Assert( skill, L"No such skill!" );
			cb->Items[ i ] = skill->name;
		}
		cb->EndUpdate();
	}

	System::Void LanguageSelect_Click(System::Object^  sender, System::EventArgs^  e)
	{
		const int old_language = language;
		for( int index = 0; index < mnuLanguage->DropDownItems->Count; ++index )
		{
			ToolStripMenuItem^ item = static_cast< ToolStripMenuItem^ >( mnuLanguage->DropDownItems[ index ] );
			if( mnuLanguage->DropDownItems[ index ] == sender )
			{
				if( index == language )
					return;
				language = index;
				item->Checked = true;
			}
			else item->Checked = false;
		}
		if( old_language == language )
			return;

		updating_language = true;

		StringTable::LoadLanguage( static_cast< ToolStripMenuItem^ >( sender )->ToString() );

		for each( ComboBox^ box in bSkillFilters )
		{
			box->BeginUpdate();

			for( int i = 0; i < SkillTag::tags.Count; ++i )
				box->Items[ i ] = SkillTag::tags[ i ]->name;
			box->EndUpdate();
		}
		for each( ComboBox^ box in gSkillFilters )
		{
			box->BeginUpdate();
			for( int i = 0; i < SkillTag::tags.Count; ++i )
				box->Items[ i ] = SkillTag::tags[ i ]->name;
			box->EndUpdate();
		}

#define UpdateMenu( X ) mnu##X->Text = StaticString( X )
		UpdateMenu( File );
		UpdateMenu( Options );
		UpdateMenu( Language );
		UpdateMenu( Help );
		UpdateMenu( Load );
		UpdateMenu( Save );
		UpdateMenu( Exit );
		UpdateMenu( ClearSettings );
		UpdateMenu( AllowBadSkills );
		UpdateMenu( AllowEventArmor );
		UpdateMenu( AllowLowRankArmor );
		UpdateMenu( AllowPiercings );
		UpdateMenu( CheckForUpdates );	
		UpdateMenu( About );
#undef UpdateMenu
		lblHR->Text = StaticString( HR );
		lblElder->Text = StaticString( VillageQuests );
		lblSlots->Text = StaticString( MaxWeaponSlots );
		grpSort->Text = StaticString( SortResultsBy );
		grpCharmFilter->Text = StaticString( FilterResultsByCharm );
		grpCharms->Text = StaticString( Charms );
		grpResults->Text = StaticString( Results );
		rdoMale->Text = StaticString( Male );
		rdoFemale->Text = StaticString( Female );
		tabBlademaster->Text = StaticString( Blademaster );
		tabGunner->Text = StaticString( Gunner );
		grpBSkills->Text = StaticString( Skills );
		grpGSkills->Text = StaticString( Skills );
		grpBSkillFilters->Text = StaticString( SkillFilters );
		grpGSkillFilters->Text = StaticString( SkillFilters );
		btnCharms->Text = StaticString( MyCharms );
		btnImport->Text = StaticString( Import );
		btnSearch->Text = StaticString( QuickSearch );
		btnAdvancedSearch->Text = StaticString( AdvancedSearch );
		btnCancel->Text = StaticString( Cancel );
		cmbCharmSelect->Items[ 0 ] = StaticString( UseNoCharms );
		cmbCharmSelect->Items[ 1 ] = StaticString( UseOnlyMyCharms );
		cmbCharmSelect->Items[ 2 ] = StaticString( UseOnlySlottedCharms );
		cmbCharmSelect->Items[ 3 ] = StaticString( UseUpToOneSkillCharms );
		cmbCharmSelect->Items[ 4 ] = StaticString( UseOnlyTwoSkillCharms );
		cmbSort->Items[ 0 ] = BasicString( None );
		cmbSort->Items[ 1 ] = StaticString( DragonRes );
		cmbSort->Items[ 2 ] = StaticString( FireRes );
		cmbSort->Items[ 3 ] = StaticString( IceRes );
		cmbSort->Items[ 4 ] = StaticString( ThunderRes );
		cmbSort->Items[ 5 ] = StaticString( WaterRes );
		cmbSort->Items[ 6 ] = StaticString( BaseDefence );
		cmbSort->Items[ 7 ] = StaticString( MaxDefence );
		cmbSort->Items[ 8 ] = StaticString( Difficulty );
		cmbSort->Items[ 9 ] = StaticString( Rarity );
		cmbSort->Items[ 10 ] = StaticString( SortSlotsSpare );

		charm_solution_map.Clear();
		for each( Solution^ s in all_solutions )
		{
			if( s->charm )
			{
				String^ str = s->charm->GetName();
				if( !charm_solution_map.ContainsKey( str ) )
					charm_solution_map.Add( str, gcnew List_t< Solution^ > );
				charm_solution_map[ str ]->Add( s );
			}
		}
		
		for( int i = 0; i < NumSkills; ++i )
		{
			bSkills[ i ]->BeginUpdate();
			gSkills[ i ]->BeginUpdate();
			cmbSkillFilter_SelectedIndexChanged( bSkillFilters[ i ], bSkills, bSkillFilters, bIndexMaps );
			cmbSkillFilter_SelectedIndexChanged( gSkillFilters[ i ], gSkills, gSkillFilters, gIndexMaps );
			bSkills[ i ]->EndUpdate();
			gSkills[ i ]->EndUpdate();
		}
		
		UpdateCharmComboBox();

		if( construction_complete )
			CharmDatabase::SaveCustom();
		
		updating_language = false;
		cmbCharms_SelectedIndexChanged( nullptr, nullptr );
	}

	System::Void aboutToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		frmAbout about_form;
		about_form.ShowDialog( this );
	}

	System::Void HRChanged(System::Object^  sender, System::EventArgs^  e)
	{
		DeleteOptions( sender, e );
	}

	System::Void DeleteOptions(System::Object^  sender, System::EventArgs^  e)
	{
		blast_options = glast_options = nullptr;
	}

	System::Void SortResults()
	{
		if( cmbSort->SelectedIndex < 1 || sort_off ) return;
		else if( cmbSort->SelectedIndex == 1 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByDragonRes ) );
		else if( cmbSort->SelectedIndex == 2 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByFireRes ) );
		else if( cmbSort->SelectedIndex == 3 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByIceRes ) );
		else if( cmbSort->SelectedIndex == 4 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByThunderRes ) );
		else if( cmbSort->SelectedIndex == 5 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByWaterRes ) );
		else if( cmbSort->SelectedIndex == 6 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByDefence ) );
		else if( cmbSort->SelectedIndex == 7 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByMaxDefence ) );
		else if( cmbSort->SelectedIndex == 8 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByDifficulty ) );
		else if( cmbSort->SelectedIndex == 9 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByRarity ) );
		else if( cmbSort->SelectedIndex == 10 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionBySlotsSpare ) );
	}

	System::Void contextMenuStrip1_Opening(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
	{
		if( txtSolutions->Text == L"" ) return;
		cmsSolutions->Items->Clear();
		e->Cancel = true;

		Point pos = txtSolutions->PointToClient( txtSolutions->MousePosition );
		int chr = txtSolutions->GetCharIndexFromPosition( pos );
		int line = txtSolutions->GetLineFromCharIndex( chr );
		String^ str = txtSolutions->Lines[ line ];
		if( str == L"" ) return;
		
		if( str->Length >= 4 )
		{
			if( str->Substring( 1, 2 ) == L"x " || str->Substring( 2, 2 ) == L"x " )
			{
				Decoration^ decoration = Decoration::FindDecoration( str->Substring( str->IndexOf( L' ' ) + 1 ) );
				if( decoration )
				{
					Utility::UpdateContextMenu( cmsSolutions, decoration );
					e->Cancel = false;
				}
				return;
			}
		}
		Armor^ armor = Armor::FindArmor( str );
		if( armor )
		{
			Utility::UpdateContextMenu( cmsSolutions, armor );
			e->Cancel = false;
		}
		else 
		{
			int bracket = str->LastIndexOf( L'(' );
			if( bracket != -1 )
			{
				str = str->Substring( 0, bracket - 1 );
				armor = Armor::FindArmor( str );
				if( armor )
				{
					Utility::UpdateContextMenu( cmsSolutions, armor );
					e->Cancel = false;
				}
			}
		}
		if( !armor && ( str[ 0 ] == L'+' || str == L"OOO" || str == L"OO-" || str == L"O--" ) )
		{
			Utility::UpdateContextMenu( cmsSolutions, str );
			e->Cancel = false;
		}
	}

	System::Void cmbCharms_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if( updating_language || cmbCharms->SelectedIndex < 0 )
			return;
		last_result = L"";
		final_solutions.Clear();
		String^ charm = (String^)cmbCharms->SelectedItem;
		if( cmbCharms->SelectedIndex == 0 ) // None
		{
			final_solutions.AddRange( %no_charm_solutions );
			SortResults();
			UpdateResultString( %final_solutions );
		}
		else if( cmbCharms->SelectedIndex == 1 ) // All
		{
			final_solutions.AddRange( %all_solutions );
			SortResults();
			UpdateResultString( %final_solutions );
		}
		else if( charm_solution_map.ContainsKey( charm ) )
		{
			final_solutions.AddRange( charm_solution_map[ charm ] );
			SortResults();
			UpdateResultString( %final_solutions );
		}
		else
			UpdateResultString( gcnew List_t< Solution^ > );
	}

	System::Void btnCharms_Click(System::Object^  sender, System::EventArgs^  e)
	{
		ManageCharms mycharms( language, data );
		mycharms.ShowDialog( this );
	}
	
	System::Void UpdateMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		System::Diagnostics::Process::Start( "http://forums.minegarde.com/topic/3458-athenas-armor-set-search-mhp3-edition" );
	}

	System::Void toolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e)
	{
		cmbSort->SelectedIndex = 0;
		for each( ComboBox^ cb in bSkills )
		{
			cb->SelectedIndex = -1;
		}
		for each( ComboBox^ cb in gSkills )
		{
			cb->SelectedIndex = -1;
		}
		for each( ComboBox^ cb in bSkillFilters )
		{
			cb->SelectedIndex = 0;
		}
		for each( ComboBox^ cb in gSkillFilters )
		{
			cb->SelectedIndex = 0;
		}
	}
	
	System::Void mnuLoad_Click(System::Object^  sender, System::EventArgs^  e)
	{
		OpenFileDialog dlg;
		dlg.InitialDirectory = System::Environment::CurrentDirectory;
		dlg.DefaultExt = L".ass";
		dlg.AddExtension = true;
		dlg.Filter = L"ASS settings (*.ass)|*.ass";
		dlg.FilterIndex = 0;
		::DialogResult res = dlg.ShowDialog();
		if( res == ::DialogResult::OK )
			LoadConfig( dlg.FileName );
	}
	
	System::Void mnuSave_Click(System::Object^  sender, System::EventArgs^  e)
	{
		SaveFileDialog dlg;
		dlg.InitialDirectory = System::Environment::CurrentDirectory;
		dlg.DefaultExt = L".ass";
		dlg.AddExtension = true;
		dlg.Filter = L"ASS settings (*.ass)|*.ass";
		dlg.FilterIndex = 0;
		dlg.FileName = L"results.ass";
		::DialogResult res = dlg.ShowDialog();
		if( res == ::DialogResult::OK )
			SaveConfig( dlg.FileName );
	}

	System::Void btnImport_Click(System::Object^  sender, System::EventArgs^  e)
	{
		OpenFileDialog dlg;
		dlg.InitialDirectory = System::Environment::CurrentDirectory;
		dlg.DefaultExt = L".bin";
		dlg.AddExtension = true;
		dlg.Filter = StartString( MHP3rdSaveData ) + L"(*.bin)|*.bin";
		dlg.FilterIndex = 0;
		DialogResult_t res = dlg.ShowDialog();
		//DialogResult_t res = DialogResult_t::OK;
		if( res == DialogResult_t::OK )
		{
			array< Byte >^ data = IO::File::ReadAllBytes( dlg.FileName );
			//array< Byte >^ data = IO::File::ReadAllBytes( L"C:/Users/Rhys/Desktop/patch/ULJM05800/MHP3RD.BIN" );
			if( data->Length != 0x121010 )
			{
				MessageBox::Show( StaticString( NotAValidMHP3rdSaveFile ), StaticString( Error ), MessageBoxButtons::OK, MessageBoxIcon::Error );
				return;
			}
			ImportCharms ic;
			ic.language = language;
			ic.LoadCharms( data );
			res = ic.ShowDialog();
		}
	}

	System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
	{
		//array< Byte >^ data = IO::File::ReadAllBytes( L"C:/Users/Rhys/Desktop/patch/MHP3 Translation/MHP3RD.bin" );
		//data = PSPCryptoEngine().DecryptSaveData( data );
		//DecryptSaveData( data );
		//IO::File::WriteAllBytes( L"C:/Users/Rhys/Desktop/patch/MHP3 Translation/test.out", data );

		//array< Byte >^ data = IO::File::ReadAllBytes( L"C:/Users/Rhys/Desktop/patch/MHP3 Translation/ULJM05800QST.BIN" );
		//EncryptSaveData( data );
		//IO::File::WriteAllBytes( L"C:/Users/Rhys/Desktop/patch/MHP3 Translation/test.out", data );

		//array< Byte >^ data = IO::File::ReadAllBytes( L"C:/Users/Rhys/Desktop/patch/ULJM05800/truebackup/ULJM05800/MHP3RD.BIN" );
		//array< Byte >^ data = IO::File::ReadAllBytes( L"C:/Users/Rhys/Desktop/patch/ULJM05800/MHP3RD.BIN" );
		//PSPCryptoEngine().Test( data );
		//array< Byte >^ sfo_data = PSPCryptoEngine().GetSFOHashes( data );
		//IO::File::WriteAllBytes( L"C:/Users/Rhys/Desktop/patch/ULJM05800/test.out", sfo_data );

		//array< Byte >^ data = IO::File::ReadAllBytes( L"C:/Users/Rhys/Desktop/patch/ULJM05800/MHP3RD3.BIN" );
		//data = PSPCryptoEngine().EncryptSaveData( data );
		//PSPCryptoEngine().Test( data );
		//data = PSPCryptoEngine().DecryptSaveData( data );
		this->Close();
	}
};
}

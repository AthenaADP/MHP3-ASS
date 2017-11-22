#pragma once
#include "CharmDatabase.h"
#include "Armor.h"
#include "PSPCryptoEngine.h"
#include "SaveDataEncryption.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace AthenasASSMHP3rd {

	/// <summary>
	/// Summary for ImportCharms
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ImportCharms : public System::Windows::Forms::Form
	{
		array< List_t< Charm^ >^ >^ charms;
	private: System::Windows::Forms::ColumnHeader^  columnHeader1;
	private: System::Windows::Forms::GroupBox^  grpCharacters;

			 array< RadioButton^ >^ radio_buttons;
	private: System::Windows::Forms::Button^  btnSelectNone;
	private: System::Windows::Forms::Button^  btnSelectBest;


	public:
		unsigned language;

		ImportCharms(void)
		{
			InitializeComponent();

			charms = gcnew array< Generic::List< Charm^ >^ >( 3 );
			radio_buttons = gcnew array< System::Windows::Forms::RadioButton^ >{ rdoChar1, rdoChar2, rdoChar3 };
			
			DialogResult = System::Windows::Forms::DialogResult::Cancel;

			Text = StaticString( ImportCharmsFromSaveData );
			btnOK->Text = StaticString( Import );
			btnSelectBest->Text = StaticString( SelectBest );
			btnSelectNone->Text = StaticString( SelectNone );
			btnCancel->Text = StaticString( Cancel );
			chkDeleteExisting->Text = StaticString( DeleteExistingCharms );
			grpCharms->Text = StaticString( Charms );
			grpCharacters->Text = StaticString( Characters );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ImportCharms()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btnOK;
	protected: 
	private: System::Windows::Forms::Button^  btnCancel;
	private: System::Windows::Forms::ListView^  listView1;
	private: System::Windows::Forms::GroupBox^  grpCharms;

	private: System::Windows::Forms::CheckBox^  chkDeleteExisting;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::RadioButton^  rdoChar3;
	private: System::Windows::Forms::RadioButton^  rdoChar2;
	private: System::Windows::Forms::RadioButton^  rdoChar1;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->btnOK = (gcnew System::Windows::Forms::Button());
			this->btnCancel = (gcnew System::Windows::Forms::Button());
			this->listView1 = (gcnew System::Windows::Forms::ListView());
			this->columnHeader1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->grpCharms = (gcnew System::Windows::Forms::GroupBox());
			this->chkDeleteExisting = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->btnSelectBest = (gcnew System::Windows::Forms::Button());
			this->btnSelectNone = (gcnew System::Windows::Forms::Button());
			this->rdoChar3 = (gcnew System::Windows::Forms::RadioButton());
			this->rdoChar2 = (gcnew System::Windows::Forms::RadioButton());
			this->rdoChar1 = (gcnew System::Windows::Forms::RadioButton());
			this->grpCharacters = (gcnew System::Windows::Forms::GroupBox());
			this->grpCharms->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->grpCharacters->SuspendLayout();
			this->SuspendLayout();
			// 
			// btnOK
			// 
			this->btnOK->Location = System::Drawing::Point(6, 71);
			this->btnOK->Name = L"btnOK";
			this->btnOK->Size = System::Drawing::Size(75, 23);
			this->btnOK->TabIndex = 0;
			this->btnOK->Text = L"&Import";
			this->btnOK->UseVisualStyleBackColor = true;
			this->btnOK->Click += gcnew System::EventHandler(this, &ImportCharms::btnOK_Click);
			// 
			// btnCancel
			// 
			this->btnCancel->Location = System::Drawing::Point(87, 71);
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size(75, 23);
			this->btnCancel->TabIndex = 1;
			this->btnCancel->Text = L"&Cancel";
			this->btnCancel->UseVisualStyleBackColor = true;
			this->btnCancel->Click += gcnew System::EventHandler(this, &ImportCharms::btnCancel_Click);
			// 
			// listView1
			// 
			this->listView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->listView1->CheckBoxes = true;
			this->listView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(1) {this->columnHeader1});
			this->listView1->Location = System::Drawing::Point(6, 19);
			this->listView1->MultiSelect = false;
			this->listView1->Name = L"listView1";
			this->listView1->Size = System::Drawing::Size(281, 293);
			this->listView1->TabIndex = 2;
			this->listView1->UseCompatibleStateImageBehavior = false;
			this->listView1->View = System::Windows::Forms::View::List;
			// 
			// grpCharms
			// 
			this->grpCharms->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpCharms->Controls->Add(this->listView1);
			this->grpCharms->Location = System::Drawing::Point(12, 8);
			this->grpCharms->Name = L"grpCharms";
			this->grpCharms->Size = System::Drawing::Size(293, 318);
			this->grpCharms->TabIndex = 3;
			this->grpCharms->TabStop = false;
			this->grpCharms->Text = L"Charms";
			// 
			// chkDeleteExisting
			// 
			this->chkDeleteExisting->AutoSize = true;
			this->chkDeleteExisting->Checked = true;
			this->chkDeleteExisting->CheckState = System::Windows::Forms::CheckState::Checked;
			this->chkDeleteExisting->Location = System::Drawing::Point(6, 19);
			this->chkDeleteExisting->Name = L"chkDeleteExisting";
			this->chkDeleteExisting->Size = System::Drawing::Size(134, 17);
			this->chkDeleteExisting->TabIndex = 4;
			this->chkDeleteExisting->Text = L"Delete Existing Charms";
			this->chkDeleteExisting->UseVisualStyleBackColor = true;
			// 
			// groupBox2
			// 
			this->groupBox2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->groupBox2->Controls->Add(this->btnSelectBest);
			this->groupBox2->Controls->Add(this->btnSelectNone);
			this->groupBox2->Controls->Add(this->chkDeleteExisting);
			this->groupBox2->Controls->Add(this->btnOK);
			this->groupBox2->Controls->Add(this->btnCancel);
			this->groupBox2->Location = System::Drawing::Point(311, 105);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(186, 101);
			this->groupBox2->TabIndex = 5;
			this->groupBox2->TabStop = false;
			// 
			// btnSelectBest
			// 
			this->btnSelectBest->Location = System::Drawing::Point(87, 42);
			this->btnSelectBest->Name = L"btnSelectBest";
			this->btnSelectBest->Size = System::Drawing::Size(75, 23);
			this->btnSelectBest->TabIndex = 6;
			this->btnSelectBest->Text = L"Select &Best";
			this->btnSelectBest->UseVisualStyleBackColor = true;
			this->btnSelectBest->Click += gcnew System::EventHandler(this, &ImportCharms::btnSelectBest_Click);
			// 
			// btnSelectNone
			// 
			this->btnSelectNone->Location = System::Drawing::Point(6, 42);
			this->btnSelectNone->Name = L"btnSelectNone";
			this->btnSelectNone->Size = System::Drawing::Size(75, 23);
			this->btnSelectNone->TabIndex = 5;
			this->btnSelectNone->Text = L"Select &None";
			this->btnSelectNone->UseVisualStyleBackColor = true;
			this->btnSelectNone->Click += gcnew System::EventHandler(this, &ImportCharms::btnSelectNone_Click);
			// 
			// rdoChar3
			// 
			this->rdoChar3->AutoSize = true;
			this->rdoChar3->Location = System::Drawing::Point(6, 65);
			this->rdoChar3->Name = L"rdoChar3";
			this->rdoChar3->Size = System::Drawing::Size(14, 13);
			this->rdoChar3->TabIndex = 7;
			this->rdoChar3->TabStop = true;
			this->rdoChar3->UseVisualStyleBackColor = true;
			this->rdoChar3->CheckedChanged += gcnew System::EventHandler(this, &ImportCharms::rdoChar3_CheckedChanged);
			// 
			// rdoChar2
			// 
			this->rdoChar2->AutoSize = true;
			this->rdoChar2->Location = System::Drawing::Point(6, 42);
			this->rdoChar2->Name = L"rdoChar2";
			this->rdoChar2->Size = System::Drawing::Size(14, 13);
			this->rdoChar2->TabIndex = 6;
			this->rdoChar2->TabStop = true;
			this->rdoChar2->UseVisualStyleBackColor = true;
			this->rdoChar2->CheckedChanged += gcnew System::EventHandler(this, &ImportCharms::rdoChar2_CheckedChanged);
			// 
			// rdoChar1
			// 
			this->rdoChar1->AutoSize = true;
			this->rdoChar1->Location = System::Drawing::Point(6, 19);
			this->rdoChar1->Name = L"rdoChar1";
			this->rdoChar1->Size = System::Drawing::Size(14, 13);
			this->rdoChar1->TabIndex = 5;
			this->rdoChar1->TabStop = true;
			this->rdoChar1->UseVisualStyleBackColor = true;
			this->rdoChar1->CheckedChanged += gcnew System::EventHandler(this, &ImportCharms::rdoChar1_CheckedChanged);
			// 
			// grpCharacters
			// 
			this->grpCharacters->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->grpCharacters->Controls->Add(this->rdoChar3);
			this->grpCharacters->Controls->Add(this->rdoChar1);
			this->grpCharacters->Controls->Add(this->rdoChar2);
			this->grpCharacters->Location = System::Drawing::Point(311, 8);
			this->grpCharacters->Name = L"grpCharacters";
			this->grpCharacters->Size = System::Drawing::Size(174, 91);
			this->grpCharacters->TabIndex = 6;
			this->grpCharacters->TabStop = false;
			this->grpCharacters->Text = L"Characters";
			// 
			// ImportCharms
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(497, 338);
			this->Controls->Add(this->grpCharacters);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->grpCharms);
			this->Name = L"ImportCharms";
			this->Text = L"Import Charms from Save Data";
			this->grpCharms->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->grpCharacters->ResumeLayout(false);
			this->grpCharacters->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	
		void btnCancel_Click(System::Object^  sender, System::EventArgs^  e)
		{
			Close();
		}

		void AddCharms( const unsigned c )
		{
			List_t< Charm^ >^ char_charms = charms[ c ];
			if( radio_buttons[ c ]->Checked )
			{
				for( int i = 0; i < char_charms->Count; ++i )
				{
					if( listView1->Items[ i ]->Checked && !char_charms[ i ]->hacked )
					{
						char_charms[ i ]->custom = true;
						CharmDatabase::mycharms.Add( char_charms[ i ] );
					}
				}
			}
		}

		void btnOK_Click(System::Object^  sender, System::EventArgs^  e)
		{
			DialogResult = System::Windows::Forms::DialogResult::OK;

			if( chkDeleteExisting )
				CharmDatabase::mycharms.Clear();

			listView1->Enabled = false;

			for( unsigned i = 0; i < 3; ++i )
				AddCharms( i );

			CharmDatabase::SaveCustom();

			Close();
		}

		void RadioButtonChanged( const unsigned c )
		{
			listView1->Clear();
			List_t< Charm^ >^ current_charms = charms[ c ];
			for each( Charm^ charm in current_charms )
			{
				listView1->Items->Add( charm->GetName() );
			}
			SelectDefault( c );			
		}

		void rdoChar1_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		{
			RadioButtonChanged( 0 );
		}

		void rdoChar2_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		{
			RadioButtonChanged( 1 );
		}

		void rdoChar3_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		{
			RadioButtonChanged( 2 );
		}

		void SelectDefault( const unsigned c )
		{
			listView1->BeginUpdate();
			List_t< Charm^ >^ current_charms = charms[ c ];
			for each( Ability^ a in Ability::static_abilities )
			{
				a->relevant = true;
			}
			for( int i = 0; i < current_charms->Count; ++i )
			{
				if( current_charms[ i ]->hacked )
				{
					listView1->Items[ i ]->ForeColor = Color::Red;
					continue;
				}
				else 
					listView1->Items[ i ]->Checked = true;
				for( int j = 0; j < i; j++ )
				{
					if( listView1->Items[ j ]->Checked )
					{
						if( current_charms[ i ]->StrictlyBetterThan( current_charms[ j ] ) )
							listView1->Items[ j ]->Checked = false;
						else if( charms[ c ][ j ]->StrictlyBetterThan( current_charms[ i ] ) ||
							current_charms[ i ] == current_charms[ j ] )
							listView1->Items[ i ]->Checked = false;
					}
				}
			}
			listView1->EndUpdate();
		}

		bool LoadCharms( array< Byte >^ data, const unsigned char_num )
		{
			radio_buttons[ char_num ]->Checked = false;
			const unsigned index = 0x800 + char_num * 0x60000;
			bool cheats = false;
			if( data[ index ] )
			{
				charms[ char_num ] = gcnew List_t< Charm^ >();
				
				unsigned name_end = index;
				while( data[ name_end ] )
					name_end++;

				for( unsigned i = 0; i < 1000; ++i )
				{
					const unsigned equipment_index = index + 0x168 + i * 12;
					array< Byte >^ charm_data = gcnew array< Byte >( 6 );
					array< Byte >::Copy( data, equipment_index, charm_data, 0, charm_data->Length );
					if( charm_data[ 0 ] == 1 && charm_data[ 1 ] == 0x65 )
					{
						//bits:
						//0-3: amount1 - 2
						//4-7: rarity - 1
						//8: 1=positive, 0=negative amount2 so subtract 14
						//10-13: amount
						//17-23: ability1 index
						//24-25: slots
						//26-31 + 16: ability2 index

						const int amount1 = ( ( charm_data[ 2 ] >> 4 ) + 2 ) % 16;
						const int rarity = 1 + ( charm_data[ 2 ] & 0xf );
						const int amount2 = ( ( charm_data[ 3 ] & 0x3c ) >> 2 ) - !( charm_data[ 3 ] & 0x80 ) * 16 + 2;
						const int ability1 = ( charm_data[ 4 ] & 0x7f );
						const int ability2 = !!( charm_data[ 4 ] & 0x80 ) + ( ( charm_data[ 5 ] & 0x3f ) << 1 );
						const int num_slots = charm_data[ 5 ] >> 6;
						Charm^ charm = gcnew Charm( num_slots );
						if( ability1 )
							charm->abilities.Add( gcnew AbilityPair( Ability::ordered_abilities[ ability1 - 1 ], amount1 ) );
						if( ability2 )
							charm->abilities.Add( gcnew AbilityPair( Ability::ordered_abilities[ ability2 - 1 ], amount2 ) );
						if( !CharmExists( charm ) && CharmIsIllegal( charm ) )
						{
							charm->hacked = true;
							cheats = true;
						}
						charms[ char_num ]->Add( charm );
					}
				}
				if( charms[ char_num ]->Count == 0 )
				{
					radio_buttons[ char_num ]->Enabled = false;
					radio_buttons[ char_num ]->Text = StaticString( NoCharmsFound );
				}
				else
				{
					radio_buttons[ char_num ]->Enabled = true;
					System::Text::Encoding^ encoding = System::Text::Encoding::GetEncoding( L"utf-16LE" );
					radio_buttons[ char_num ]->Text = encoding->GetString( data, index, name_end - index );
				}
			}
			else
			{
				charms[ char_num ] = nullptr;
				radio_buttons[ char_num ]->Enabled = false;
				radio_buttons[ char_num ]->Text = StaticString( NoCharacterDataFound );
			}
			return cheats;
		}
public:
		void LoadCharms( array< Byte >^ data )
		{
			data = PSPCryptoEngine().DecryptSaveData( data ); //system encryption
			DecryptSaveData( data ); //game encryption
			bool cheats = false;
			for( unsigned i = 0; i < 3; ++i )
				cheats |= LoadCharms( data, i );
			if( cheats )
				MessageBox::Show( StaticString( Cheater ) );

			for( unsigned i = 0; i < 3; ++i )
			{
				if( radio_buttons[ i ]->Enabled )
				{
					radio_buttons[ i ]->Checked = true;
					break;
				}
			}
		}

		System::Void btnSelectNone_Click(System::Object^  sender, System::EventArgs^  e)
		{
			listView1->BeginUpdate();
			for each( ListViewItem^ item in listView1->Items )
				item->Checked = false;
			listView1->EndUpdate();
		}

		System::Void btnSelectBest_Click(System::Object^  sender, System::EventArgs^  e)
		{
			for( unsigned i = 0; i < 3; ++i )
				if( radio_buttons[ i ]->Checked )
				{
					SelectDefault( i );
					break;
				}
		}

		System::Void listView1_ItemChecked(System::Object^  sender, ItemCheckEventArgs^  e)
		{
			if( charms[ 0 ][ e->Index ]->hacked )
				e->NewValue = e->CurrentValue;	
		}
};
}

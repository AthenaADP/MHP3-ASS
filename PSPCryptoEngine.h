#pragma once

ref class PSPCryptoEngine
{
	bool engine_initialised;
	array< System::Byte >^ mhp3_gamekey;

public:
	PSPCryptoEngine();
	~PSPCryptoEngine() {}

	array< System::Byte >^ DecryptSaveData( array< System::Byte >^ data );
	array< System::Byte >^ EncryptSaveData( array< System::Byte >^ data );
	array< System::Byte >^ GetSFOHashes( array< System::Byte >^ data );

	void Test( array< System::Byte >^ data );
};

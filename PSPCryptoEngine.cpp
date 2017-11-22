#include "stdafx.h"
#include "PSPCryptoEngine.h"

using namespace System;

//const int PSP_KIRK_CMD_DECRYPT_PRIVATE = 0x1;         // Master decryption command, used by firmware modules. Applies CMAC checking.
//const int PSP_KIRK_CMD_ENCRYPT_SIGN = 0x2;            // Used for key type 3 (blacklisting), encrypts and signs data with a ECDSA signature.
//const int PSP_KIRK_CMD_DECRYPT_SIGN = 0x3;            // Used for key type 3 (blacklisting), decrypts and signs data with a ECDSA signature.
const int PSP_KIRK_CMD_ENCRYPT = 0x4;                 // Key table based encryption used for general purposes by several modules.
const int PSP_KIRK_CMD_ENCRYPT_FUSE = 0x5;            // Fuse ID based encryption used for general purposes by several modules.
//const int PSP_KIRK_CMD_ENCRYPT_USER = 0x6;            // User specified ID based encryption used for general purposes by several modules.
const int PSP_KIRK_CMD_DECRYPT = 0x7;                 // Key table based decryption used for general purposes by several modules.
const int PSP_KIRK_CMD_DECRYPT_FUSE = 0x8;            // Fuse ID based decryption used for general purposes by several modules.
//const int PSP_KIRK_CMD_DECRYPT_USER = 0x9;            // User specified ID based decryption used for general purposes by several modules.
//const int PSP_KIRK_CMD_PRIV_SIG_CHECK = 0xA;          // Private signature (SCE) checking command.
//const int PSP_KIRK_CMD_SHA1_HASH = 0xB;               // SHA1 hash generating command.
//const int PSP_KIRK_CMD_ECDSA_GEN_KEYS = 0xC;          // ECDSA key generating mul1 command. 
//const int PSP_KIRK_CMD_ECDSA_MULTIPLY_POINT = 0xD;    // ECDSA key generating mul2 command. 
//const int PSP_KIRK_CMD_PRNG = 0xE;                    // Random number generating command. 
//const int PSP_KIRK_CMD_INIT = 0xF;                    // KIRK initialization command.
//const int PSP_KIRK_CMD_ECDSA_SIGN = 0x10;             // ECDSA signing command.
//const int PSP_KIRK_CMD_ECDSA_VERIFY = 0x11;           // ECDSA checking command.
//const int PSP_KIRK_CMD_CERT_VERIFY = 0x12;            // Certificate checking command.
//
//const int PSP_KIRK_NOT_ENABLED = 0x1;
const int PSP_KIRK_INVALID_MODE = 0x2;
//const int PSP_KIRK_INVALID_HEADER_HASH = 0x3;
//const int PSP_KIRK_INVALID_DATA_HASH = 0x4;
//const int PSP_KIRK_INVALID_SIG_CHECK = 0x5;
//const int PSP_KIRK_UNK1 = 0x6;
//const int PSP_KIRK_UNK2 = 0x7;
//const int PSP_KIRK_UNK3 = 0x8;
//const int PSP_KIRK_UNK4 = 0x9;
//const int PSP_KIRK_UNK5 = 0xA;
//const int PSP_KIRK_UNK6 = 0xB;
//const int PSP_KIRK_NOT_INIT = 0xC;
const int PSP_KIRK_INVALID_OPERATION = 0xD;
//const int PSP_KIRK_INVALID_SEED = 0xE;
const int PSP_KIRK_INVALID_SIZE = 0xF;
const int PSP_KIRK_DATA_SIZE_IS_ZERO = 0x10;  
//const int PSP_SUBCWR_NOT_16_ALGINED = 0x90A;
//const int PSP_SUBCWR_HEADER_HASH_INVALID = 0x920;
//const int PSP_SUBCWR_BUFFER_TOO_SMALL = 0x1000;
//
//const int PSP_KIRK_CMD_MODE_CMD1 = 0x1;
//const int PSP_KIRK_CMD_MODE_CMD2 = 0x2;
//const int PSP_KIRK_CMD_MODE_CMD3 = 0x3;
//const int PSP_KIRK_CMD_MODE_ENCRYPT_CBC = 0x4;
const int PSP_KIRK_CMD_MODE_DECRYPT_CBC = 0x5;

typedef Security::Cryptography::RijndaelManaged AES128;
AES128^ GetAES128()
{
	AES128^ res = gcnew AES128();
	res->BlockSize = 128;
	res->Padding = Security::Cryptography::PaddingMode::None;
	res->Mode = Security::Cryptography::CipherMode::CBC;
	return res;
}

PSPCryptoEngine::PSPCryptoEngine()
{
	engine_initialised = true;
	mhp3_gamekey = gcnew array< Byte >
	{
		0xE3, 0x05, 0xCE, 0xFA,
		0xEB, 0x46, 0xB0, 0x31,
		0x85, 0x9A, 0x27, 0x5B,
		0xDF, 0x32, 0xD8, 0x63
	};
}

array< Byte >^ GetSDKey( const int num )
{
	switch( num )
	{
	case 2: { return gcnew array< Byte >{ 0xFA, 0xAA, 0x50, 0xEC, 0x2F, 0xDE, 0x54, 0x93, 0xAD, 0x14, 0xB2, 0xCE, 0xA5, 0x30, 0x05, 0xDF }; }
	case 3: { return gcnew array< Byte >{ 0x36, 0xA5, 0x3E, 0xAC, 0xC5, 0x26, 0x9E, 0xA3, 0x83, 0xD9, 0xEC, 0x25, 0x6C, 0x48, 0x48, 0x72 }; }
	case 4: { return gcnew array< Byte >{ 0xD8, 0xC0, 0xB0, 0xF3, 0x3E, 0x6B, 0x76, 0x85, 0xFD, 0xFB, 0x4D, 0x7D, 0x45, 0x1E, 0x92, 0x03 }; }
	case 6: { return gcnew array< Byte >{ 0x70, 0x44, 0xA3, 0xAE, 0xEF, 0x5D, 0xA5, 0xF2, 0x85, 0x7F, 0xF2, 0xD6, 0x94, 0xF5, 0x36, 0x3B }; }
	case 7: { return gcnew array< Byte >{ 0xEC, 0x6D, 0x29, 0x59, 0x26, 0x35, 0xA5, 0x7F, 0x97, 0x2A, 0x0D, 0xBC, 0xA3, 0x26, 0x33, 0x00 }; }
	};
	throw gcnew Exception( L"Bad SDKey num" );
	return nullptr;
}

array< Byte >^ getAESKeyFromSeed( const int seed )
{
	switch( seed )
	{
	case 0x03: { return gcnew array< Byte >{ 0x98, 0x02, 0xC4, 0xE6, 0xEC, 0x9E, 0x9E, 0x2F, 0xFC, 0x63, 0x4C, 0xE4, 0x2F, 0xBB, 0x46, 0x68 }; }	
	case 0x04: { return gcnew array< Byte >{ 0x99, 0x24, 0x4C, 0xD2, 0x58, 0xF5, 0x1B, 0xCB, 0xB0, 0x61, 0x9C, 0xA7, 0x38, 0x30, 0x07, 0x5F }; }
	case 0x05: { return gcnew array< Byte >{ 0x02, 0x25, 0xD7, 0xBA, 0x63, 0xEC, 0xB9, 0x4A, 0x9D, 0x23, 0x76, 0x01, 0xB3, 0xF6, 0xAC, 0x17 }; }
	case 0x0C: { return gcnew array< Byte >{ 0x84, 0x85, 0xC8, 0x48, 0x75, 0x08, 0x43, 0xBC, 0x9B, 0x9A, 0xEC, 0xA7, 0x9C, 0x7F, 0x60, 0x18 }; }
	case 0x0D: { return gcnew array< Byte >{ 0xB5, 0xB1, 0x6E, 0xDE, 0x23, 0xA9, 0x7B, 0x0E, 0xA1, 0x7C, 0xDB, 0xA2, 0xDC, 0xDE, 0xC4, 0x6E }; }
	case 0x0E: { return gcnew array< Byte >{ 0xC8, 0x71, 0xFD, 0xB3, 0xBC, 0xC5, 0xD2, 0xF2, 0xE2, 0xD7, 0x72, 0x9D, 0xDF, 0x82, 0x68, 0x82 }; }
	case 0x0F: { return gcnew array< Byte >{ 0x0A, 0xBB, 0x33, 0x6C, 0x96, 0xD4, 0xCD, 0xD8, 0xCB, 0x5F, 0x4B, 0xE0, 0xBA, 0xDB, 0x9E, 0x03 }; }
	case 0x10: { return gcnew array< Byte >{ 0x32, 0x29, 0x5B, 0xD5, 0xEA, 0xF7, 0xA3, 0x42, 0x16, 0xC8, 0x8E, 0x48, 0xFF, 0x50, 0xD3, 0x71 }; }
	case 0x11: { return gcnew array< Byte >{ 0x46, 0xF2, 0x5E, 0x8E, 0x4D, 0x2A, 0xA5, 0x40, 0x73, 0x0B, 0xC4, 0x6E, 0x47, 0xEE, 0x6F, 0x0A }; }
	case 0x12: { return gcnew array< Byte >{ 0x5D, 0xC7, 0x11, 0x39, 0xD0, 0x19, 0x38, 0xBC, 0x02, 0x7F, 0xDD, 0xDC, 0xB0, 0x83, 0x7D, 0x9D }; }
	case 0x38: { return gcnew array< Byte >{ 0x12, 0x46, 0x8D, 0x7E, 0x1C, 0x42, 0x20, 0x9B, 0xBA, 0x54, 0x26, 0x83, 0x5E, 0xB0, 0x33, 0x03 }; }
	case 0x39: { return gcnew array< Byte >{ 0xC4, 0x3B, 0xB6, 0xD6, 0x53, 0xEE, 0x67, 0x49, 0x3E, 0xA9, 0x5F, 0xBC, 0x0C, 0xED, 0x6F, 0x8A }; }
	case 0x3A: { return gcnew array< Byte >{ 0x2C, 0xC3, 0xCF, 0x8C, 0x28, 0x78, 0xA5, 0xA6, 0x63, 0xE2, 0xAF, 0x2D, 0x71, 0x5E, 0x86, 0xBA }; }
	case 0x4B: { return gcnew array< Byte >{ 0x0C, 0xFD, 0x67, 0x9A, 0xF9, 0xB4, 0x72, 0x4F, 0xD7, 0x8D, 0xD6, 0xE9, 0x96, 0x42, 0x28, 0x8B }; }
	case 0x53: { return gcnew array< Byte >{ 0xAF, 0xFE, 0x8E, 0xB1, 0x3D, 0xD1, 0x7E, 0xD8, 0x0A, 0x61, 0x24, 0x1C, 0x95, 0x92, 0x56, 0xB6 }; }
	case 0x57: { return gcnew array< Byte >{ 0x1C, 0x9B, 0xC4, 0x90, 0xE3, 0x06, 0x64, 0x81, 0xFA, 0x59, 0xFD, 0xB6, 0x00, 0xBB, 0x28, 0x70 }; }
	case 0x5D: { return gcnew array< Byte >{ 0x11, 0x5A, 0x5D, 0x20, 0xD5, 0x3A, 0x8D, 0xD3, 0x9C, 0xC5, 0xAF, 0x41, 0x0F, 0x0F, 0x18, 0x6F }; }
	case 0x63: { return gcnew array< Byte >{ 0x9C, 0x9B, 0x13, 0x72, 0xF8, 0xC6, 0x40, 0xCF, 0x1C, 0x62, 0xF5, 0xD5, 0x92, 0xDD, 0xB5, 0x82 }; }
	case 0x64: { return gcnew array< Byte >{ 0x03, 0xB3, 0x02, 0xE8, 0x5F, 0xF3, 0x81, 0xB1, 0x3B, 0x8D, 0xAA, 0x2A, 0x90, 0xFF, 0x5E, 0x61 }; }
   //fuse
	case 0x100:{ return gcnew array< Byte >{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F }; }
	default:
		return nullptr;
	}
}

// CHNNLSV SD context structs.
ref struct SDCtx1
{
	int mode;
	int unk;
	array< System::Byte >^ buf;

	SDCtx1()
	{
		mode = 0;
		unk = 0;
		buf = gcnew array< System::Byte >( 0x10 );
	}
};

ref struct SDCtx2
{
	int mode, padSize;
	array< System::Byte >^ pad;
	array< System::Byte >^ key;

	SDCtx2()
	{
		mode = 0;
		padSize = 0;
		key = gcnew array< Byte >( 0x10 );
		pad = gcnew array< Byte >( 0x10 );
	}
};

ref struct AES128CBCHeader
{
	int mode;
	int unk1;
	int unk2;
	int keySeed;
	int dataSize;

	AES128CBCHeader( int cbc, int seed, int size )
	{
		mode = cbc;
		keySeed = seed;
		dataSize = size;
		unk1 = unk2 = 0;
	}
};

int GetSeed( const int mode )
{
	switch( mode )
	{
	case 0x6: return 0x11;
	case 0x4: return 0xD;
	case 0x2: return 0x5;
	case 0x1: return 0x3;
	case 0x3: return 0xC;
	default:  return 0x10;
	};
}

int Kirk( array< Byte >^ out, AES128CBCHeader^ header, const bool encrypt )
{
	//if( header->mode != PSP_KIRK_CMD_MODE_DECRYPT_CBC )
	//	return PSP_KIRK_INVALID_MODE;  // Only valid for mode DECRYPT_CBC.

	if( header->dataSize == 0 )
		return PSP_KIRK_DATA_SIZE_IS_ZERO;

	array< Byte >^ key = getAESKeyFromSeed( header->keySeed );
	if( key == nullptr )
		return PSP_KIRK_INVALID_SIZE; // Dummy.

	array< Byte >^ iv = gcnew array< Byte >( 16 );
	array< Byte >^ data = gcnew array< Byte >( header->dataSize );
	array< Byte >^ calc = gcnew array< Byte >( header->dataSize );
	array< Byte >::Copy( out, 0, data, 0, header->dataSize );
	Security::Cryptography::ICryptoTransform^ transform = encrypt
		? GetAES128()->CreateEncryptor( key, iv )
		: GetAES128()->CreateDecryptor( key, iv );
	transform->TransformBlock( data, 0, header->dataSize, calc, 0 );
	calc->CopyTo( out, 0 );

	return 0;
}

int hleUtilsBufferCopyWithRange( array< Byte >^ out, int cmd, AES128CBCHeader^ header )
{
	switch( cmd )
	{
		case PSP_KIRK_CMD_ENCRYPT:
			return Kirk( out, header, true );
		case PSP_KIRK_CMD_ENCRYPT_FUSE:
			return Kirk( out, header, true );
		case PSP_KIRK_CMD_DECRYPT:
			return Kirk( out, header, false );
		case PSP_KIRK_CMD_DECRYPT_FUSE:
			return Kirk( out, header, false );
	};
	return PSP_KIRK_INVALID_OPERATION; // Dummy.
}

void ScrambleSD( array< Byte >^ buf, int size, int seed, int cbc, int kirk )
{
	AES128CBCHeader header( cbc, seed, size );
	const int res = hleUtilsBufferCopyWithRange( buf, kirk, %header );
	if( res != 0 )
		throw gcnew Exception( L"KIRK error." );
}
/*
int hleSdRemoveValue( SDCtx2^ ctx, array< Byte >^ data, int length )
{
	if( ctx->padSize > 0x10 || length < 0 )
	{
		// Invalid key or length.
		return -1;
	}
	else if( ctx->padSize + length <= 0x10 )
	{
		// The key hasn't been set yet.
		// Extract the hash from the data and set it as the key.
		array< Byte >::Copy( data, 0, ctx->key, 0, length );
		ctx->padSize = length;
		return 0;
	}
	else
	{
		const int seed = GetSeed( ctx->mode );

		// Set the data's footer as the new key.
		array< Byte >::Copy( data, length - 0x10, ctx->key, 0, 0x10 );

		// Setup a new buffer and re-encrypt the data with KIRK CMD4.
		array< Byte >^ dataBuf = gcnew array< Byte >( length );
		array< Byte >::Copy( data, 0, dataBuf, 0, length );
		ScrambleSD( dataBuf, length, seed, 0x4, PSP_KIRK_CMD_ENCRYPT );

		// Save the data's footer to ctx.pad and write back the data to the original buffer.
		array< Byte >::Copy( dataBuf, length - 0x10, ctx->pad, 0, 0x10 );

		return 0;
	}
}*/

//F21A1FCA
int hleSdRemoveValue( SDCtx2^ ctx, array< Byte >^ data, int length )
{
	if( ctx->padSize > 0x10 || length < 0 )
	{
		// Invalid key or length.
		return -1;
	}
	else if( ctx->padSize + length <= 0x10 )
	{
		// The key hasn't been set yet.
		// Extract the hash from the data and set it as the key.
		array< Byte >::Copy( data, 0, ctx->pad, ctx->padSize, length );
		ctx->padSize = length;
		return 0;
	}
	else
	{
		const int seed = GetSeed( ctx->mode );

		array< Byte >^ scrambleBuf = gcnew array< Byte >( length + ctx->padSize );
		array< Byte >::Copy( ctx->pad, 0, scrambleBuf, 0, ctx->padSize );

		int kLen = ctx->padSize;
		ctx->padSize += length;
		ctx->padSize &= 0xF;
		if( ctx->padSize == 0 )
			ctx->padSize = 0x10;

		length -= ctx->padSize;
		
		// Set the data's footer as the new key.
		array< Byte >::Copy( data, length, ctx->pad, 0, ctx->padSize );

		int blockSize = 0, dataOffset = 0;
		while( length > 0 )
		{
			blockSize = ( length + kLen >= 0x800 ) ? 0x800 : length + kLen;
			array< Byte >::Copy( data, dataOffset, scrambleBuf, kLen, blockSize - kLen );
			for( unsigned i = 0; i < 0x10; ++i )
				scrambleBuf[ i ] ^= ctx->key[ i ];
			ScrambleSD( scrambleBuf, blockSize, seed, 0x4, PSP_KIRK_CMD_ENCRYPT );
			array< Byte >::Copy( scrambleBuf, blockSize + 4, ctx->key, 0, 0x10 );

			length -= blockSize - kLen;
			dataOffset += blockSize - kLen;
		}
		return 0;
	}
}

int hleSdCreateList( SDCtx1^ ctx, int encMode, int genMode, array< Byte >^ data, array< Byte >^ key )
{
	// If the key is not a 16-byte key, return an error.
	if( key->Length < 0x10 )
		return -1;

	// Set the mode and the unknown parameters.
	ctx->mode = encMode;
	ctx->unk = 0x1;

	// Key generator mode 0x1 (encryption): use an encrypted pseudo random number before XORing the data with the given key.
	if( genMode == 0x1 )
	{
		array< Byte >^ header = gcnew array< Byte >( 0x10 );
		/*
		array< Byte >^ random = gcnew array< Byte >( 0x14 );
		array< Byte >^ newKey = gcnew array< Byte >( 0x10 );

		Random().NextBytes( random );

		for( int i = 0xF; i >= 0; i-- )
			newKey[ 0xF - i ] = random[ i ];
		
		array< Byte >::Copy( newKey, 0, header, 0x14, 0x10 );
		for( int i = 0; i < 4; i++ )
			header[ 0x20 + i ] = 0;*/

		array< Byte >^ random = gcnew array< Byte >( 0xC );
		Random().NextBytes( random );
		random->CopyTo( header, 0 );

		if( ctx->mode != 0 )
			throw gcnew Exception( L"Bad CreateList mode" );
		else
		{
			// Encryption mode 0x0: XOR with new SD keys, encrypt with KIRK CMD4 and XOR with the given key.
			array< Byte >^ sdHashKey6 = GetSDKey( 6 );
			for( int i = 0; i < 0x10; i++ )
				header[ i ] ^= sdHashKey6[ i ];
			
			ScrambleSD( header, 0x10, 0x12, 0x4, PSP_KIRK_CMD_ENCRYPT );

			array< Byte >^ sdHashKey7 = GetSDKey( 7 );
			for( int i = 0; i < 0x10; i++ )
				header[ i ] ^= sdHashKey7[ i ];
			
			header->CopyTo( ctx->buf, 0 );
			header->CopyTo( data, 0 );
			// If the key is not null, XOR the hash with it.
			if( key )
				for( int i = 0; i < 0x10; i++ )
					ctx->buf[ i ] ^= key[ i ];
	
			return 0;
		}
	}
	else if( genMode == 0x2 )
	{
		// Key generator mode 0x02 (decryption): directly XOR the data with the given key.
		// Grab the data hash (first 16-bytes).
		array< Byte >::Copy( data, 0, ctx->buf, 0, 0x10 );
		// If the key is not null, XOR the hash with it.
		if( key != nullptr )
		{
			for( int i = 0; i < 0x10; i++ )
				ctx->buf[ i ] ^= key[ i ];
		}
		return 0;
	}
	else return -1; // Invalid mode.
}

int hleSdSetMember( SDCtx1^ ctx, array< Byte >^ data )
{
	if( data->Length <= 0 )
		return -1;

	int finalSeed = 0;
	array< Byte >^ dataBuf = gcnew array< Byte >( data->Length );
	array< Byte >^ keyBuf = gcnew array< Byte >( 0x10 );

	// Copy the hash stored by hleSdCreateList.
	array< Byte >::Copy( ctx->buf, 0, keyBuf, 0, 0x10 );

	if( ctx->mode != 0 )
		throw gcnew Exception( L"Bad SetMember mode" );
	else
	{
		// Decryption master mode: XOR the hash with new SD keys and decrypt with KIRK CMD7.
		array< Byte >^ sdHashKey7 = GetSDKey( 7 );
		for( int i = 0; i < 0x10; i++ )
			keyBuf[ i ] ^= sdHashKey7[ i ];
		
		ScrambleSD( keyBuf, 0x10, 0x12, 5, PSP_KIRK_CMD_DECRYPT );

		array< Byte >^ sdHashKey6 = GetSDKey( 6 );
		for( int i = 0; i < 0x10; i++ )
			keyBuf[ i ] ^= sdHashKey6[ i ];
		
		finalSeed = 0x64;
	}

	if( ctx->unk != 0x1 )
		throw gcnew Exception( L"Context unknown1 is not 1" );

	// Copy the first 0xC bytes of the obtained key and replicate them
	// across a new list buffer. As a terminator, add the ctx.unk parameter's
	// 4 bytes (endian swapped) to achieve a full numbered list.
	for( int i = 0; i < data->Length; i += 0x10 )
	{
		array< Byte >::Copy( keyBuf, 0, dataBuf, i, 0xc );
		dataBuf[ i + 0xC ] = (Byte)(ctx->unk & 0xFF);
		dataBuf[ i + 0xD ] = (Byte)((ctx->unk >> 8) & 0xFF);
		dataBuf[ i + 0xE ] = (Byte)((ctx->unk >> 16) & 0xFF);
		dataBuf[ i + 0xF ] = (Byte)((ctx->unk >> 24) & 0xFF);
		ctx->unk++;
	}

	ScrambleSD( dataBuf, data->Length, finalSeed, 5, PSP_KIRK_CMD_DECRYPT );

	// Finally, XOR the full list with the given data.
	for( int i = 0; i < data->Length; i++ )
		data[ i ] ^= dataBuf[ i ];

	return 0;
}

//sceChnnlsv_C4C494F8
int hleSdGetLastIndex( SDCtx2^ ctx, array< Byte >^ hash, array< Byte >^ key )
{
	if( ctx->padSize > 0x10 )
		// Invalid key length.
		return -1;

	// Setup the buffers.
	array< Byte >^ scrambleEmptyBuf = gcnew array< Byte >( 0x10 );
	array< Byte >^ keyBuf = gcnew array< Byte >( 0x10 );
	array< Byte >^ resultBuf = gcnew array< Byte >( 0x10 );
	array< Byte >^ scrambleKeyBuf = gcnew array< Byte >( 0x10 );
	array< Byte >^ scrambleResultBuf = gcnew array< Byte >( 0x10 );
	array< Byte >^ scrambleResultKeyBuf = gcnew array< Byte >( 0x10 );
	array< Byte >^ scrambled_key = gcnew array< Byte >( 0x10 );

	// Calculate the seed.
	const int seed = GetSeed( ctx->mode );

	// Encrypt the key with KIRK CMD4 and copy it to keyBuf.
	ScrambleSD( scrambleEmptyBuf, 0x10, seed, 0x4, PSP_KIRK_CMD_ENCRYPT );
	scrambleEmptyBuf->CopyTo( keyBuf, 0 );

	// Apply a custom algorithm and copy the contents of keyBuf to ctx->key and scrambleKeyBuf.
	//Byte b = ( keyBuf[ 0 ] & 0x80 ) ? 0x87 : 0;
	//for( int i = 0; i < 0xF; i++ )
	//	keyBuf[ i ] = ( keyBuf[ i ] << 1 ) | ( keyBuf[ i + 1 ] >> 7 );
	//keyBuf[ 0xF ] = ( keyBuf[ 0xF ] << 1 ) ^ b;

	for( int i = 0; i < 0x10; i++ )
		keyBuf[ i ] = ( scrambleEmptyBuf[ ( i + 1 ) % 16 ] >> 7 ) | ( scrambleEmptyBuf[ i ] << 1 );

	if( ctx->padSize < 0x10 )
	{
		Byte bb = ( keyBuf[ 0 ] & 0x80 ) ? 0x87 : 0;
		for( int i = 0; i < 0xF; i++ )
			keyBuf[ i ] = ( keyBuf[ i ] << 1 ) | ( keyBuf[ i + 1 ] >> 7 );
		keyBuf[ 0xF ] = ( keyBuf[ 0xF ] << 1 ) ^ bb;
		
		ctx->pad[ ctx->padSize ] = 0x80;
		if( ctx->padSize + 1 < 0x10 )
			for( int i = 0; i < 0x10 - ctx->padSize - 1; i++ )
				ctx->pad[ ctx->padSize + 1 + i ] = 0;			
	}

	for( int i = 0; i < 0x10; i++ )
		ctx->pad[ i ] ^= keyBuf[ i ];
	
	ctx->pad->CopyTo( scrambleKeyBuf, 0 );
	ctx->key->CopyTo( resultBuf, 0 );
	for( int i = 0; i < 0x10; i++ )
		scrambleKeyBuf[ i ] ^= resultBuf[ i ];

	// Encrypt scrambleKeyBuf with KIRK CMD4 again and copy the pad to ctx->pad.
	ScrambleSD( scrambleKeyBuf, 0x10, seed, 0x4, PSP_KIRK_CMD_ENCRYPT );
	scrambleKeyBuf->CopyTo( resultBuf, 0 );

	// XOR with hash key 2. (or 5 for mode 6)
	{
		array< Byte >^ sdHashKey = GetSDKey( ctx->mode == 6 ? 5 : 2 );
		for( int i = 0; i < 0x10; i++ )
			resultBuf[ i ] ^= sdHashKey[ i ];
	}

	// If ctx.mode is 0x2, 0x4 or 0x6, encrypt with KIRK CMD5, then with KIRK CMD4,
	// XOR with the original key and encrypt again with KIRK CMD4.
	// Modes 0x0, 0x1 and 0x3 don't need the KIRK CMD5 step.
	if( ctx->mode == 0x2 || ctx->mode == 0x4 || ctx->mode == 0x6 )
	{
		resultBuf->CopyTo( scrambleResultBuf, 0 );
		ScrambleSD( scrambleResultBuf, 0x10, 0x100, 0x4, PSP_KIRK_CMD_ENCRYPT_FUSE );
		ScrambleSD( scrambleResultBuf, 0x10, seed, 0x4, PSP_KIRK_CMD_ENCRYPT );
		scrambleResultBuf->CopyTo( resultBuf, 0 );
	}

	if( key )
	{
		for( unsigned i = 0; i < 0x10; ++i )
			resultBuf[ i ] ^= key[ i ];
		resultBuf->CopyTo( scrambleResultKeyBuf, 0 );
		ScrambleSD( scrambleResultKeyBuf, 0x10, seed, 0x4, PSP_KIRK_CMD_ENCRYPT );
		scrambleResultKeyBuf->CopyTo( resultBuf, 0 );
	}

	resultBuf->CopyTo( hash, 0 );
	//FA 7E B8 B2 F3 79 FC AA 80 CF 6E CB 8E 65 1E 4D

	ctx->mode = ctx->padSize = 0;
	for( unsigned i = 0; i < 0x10; ++i )
		ctx->key[ i ] = ctx->pad[ i ] = 0;

	return 0;
}

array< Byte >^ PSPCryptoEngine::DecryptSaveData( array< Byte >^ data )
{
	// Setup the crypto and keygen modes and initialize both context structs.
	int sdEncMode = 0;
	int sdGenMode = 2;
	SDCtx1 ctx1;
	SDCtx2 ctx2;

	// Align the buffers to 16-bytes.
	int alignedSize = ((data->Length + 0xF) >> 4) << 4;
	array< Byte >^ outbuf = gcnew array< Byte >( alignedSize - 0x10 );
	array< Byte >^ dataBuf = gcnew array< Byte >( alignedSize );

	// Fully copy the contents of the encrypted file.
	data->CopyTo( dataBuf, 0 );

	// Call the SD functions.
	ctx2.mode = sdEncMode;
	hleSdCreateList( %ctx1, sdEncMode, sdGenMode, dataBuf, mhp3_gamekey );
	hleSdRemoveValue( %ctx2, dataBuf, 0x10 );
	array< Byte >::Copy( dataBuf, 0x10, outbuf, 0, outbuf->Length );
	hleSdRemoveValue( %ctx2, outbuf, outbuf->Length );
	hleSdSetMember( %ctx1, outbuf );
	
	//
	//array< Byte >^ hash = gcnew array< Byte >( 0x10 );
	//hleSdGetLastIndex( %ctx2, hash, mhp3_gamekey );
	
	return outbuf;
}

array< Byte >^ PSPCryptoEngine::EncryptSaveData( array< Byte >^ data )
{
	// Setup the crypto and keygen modes and initialize both context structs.
	int sdEncMode = 0;
	int sdGenMode = 1;
	SDCtx1 ctx1;
	SDCtx2 ctx2;

	// Align the buffers to 16-bytes.
	int alignedSize = ((data->Length + 0xF) >> 4) << 4;
	array< Byte >^ outbuf = gcnew array< Byte >( alignedSize + 0x10 );
	array< Byte >^ dataBuf = gcnew array< Byte >( alignedSize );

	// Fully copy the contents of the encrypted file.
	data->CopyTo( dataBuf, 0 );

	// Call the SD functions.
	ctx2.mode = sdEncMode;
	hleSdCreateList( %ctx1, sdEncMode, sdGenMode, outbuf, mhp3_gamekey );
	//now, outbuf contains the random hash, ctx1.buf contains hash^gamekey
	hleSdRemoveValue( %ctx2, outbuf, 0x10 );
	hleSdRemoveValue( %ctx2, dataBuf, alignedSize );
	hleSdSetMember( %ctx1, dataBuf );
	dataBuf->CopyTo( outbuf, 0x10 );
	//

	hleSdRemoveValue( %ctx2, dataBuf, alignedSize );
	array< Byte >^ hash = gcnew array< Byte >( 0x10 );
	hleSdGetLastIndex( %ctx2, hash, mhp3_gamekey );

	return outbuf;
}

array< System::Byte >^ PSPCryptoEngine::GetSFOHashes( array< System::Byte >^ data )
{
	SDCtx2 ctx2;
	array< Byte >^ hash = gcnew array< Byte >( 0x10 );
	array< Byte >^ key = gcnew array< Byte >( 0x10 );
	array< Byte >^ savedataParams = gcnew array< Byte >( 0x80 );
	const int alignedSize = ((data->Length + 0xF) >> 4) << 4;

	// Get the first hash from the data file.
	// array< Byte >::Copy( data, 0xD, hash, 0, 0x10 );

	// Generate a new hash using a blank key and encMode.
	ctx2.mode = 4;
	hleSdRemoveValue( %ctx2, data, alignedSize /*- 0x10*/ );
	hleSdGetLastIndex( %ctx2, hash, key );

	// Store this hash at 0x20 in the savedataParams' struct.
	hash->CopyTo( savedataParams, 0x20 );
	savedataParams[ 0 ] |= 0x01;

	// Calculate a new hash with a blank key, but with mode 3.
	savedataParams[ 0 ] |= 0x20;

	ctx2.mode = 3;
	hleSdRemoveValue( %ctx2, data, alignedSize /*- 0x10*/ );
	hleSdGetLastIndex( %ctx2, hash, key );

	// Store this hash at 0x70 in the savedataParams' struct.
	hash->CopyTo( savedataParams, 0x70 );

	// Finally, generate a last hash using a blank key and mode 1.
	ctx2.mode = 1;
	hleSdRemoveValue( %ctx2, data, alignedSize /*- 0x10*/ );
	hleSdGetLastIndex( %ctx2, hash, key );

	// Store this hash at 0x10 in the savedataParams' struct.
	hash->CopyTo( savedataParams, 0x10 );

	return savedataParams;
}

void PSPCryptoEngine::Test( array< System::Byte >^ data )
{
	{
		/*SDCtx2 ctx2;
		ctx2.mode = 4;
		hleSdRemoveValue( %ctx2, data, data->Length );
		array< Byte >^ hash = gcnew array< Byte >( 0x10 );
		hleSdGetLastIndex( %ctx2, hash, mhp3_gamekey );*/
		//hash should be 78 DC 5C 42 68 BC F1 73 3E ED 81 F5 EE A9 83 93
	}
	array< Byte >^ temp = gcnew array< Byte >( data->Length );
	data->CopyTo( temp, 0 );

	data = DecryptSaveData( data );

	// Setup the crypto and keygen modes and initialize both context structs.
	int sdEncMode = 0;
	int sdGenMode = 1;
	SDCtx1 ctx1;
	SDCtx2 ctx2;

	// Align the buffers to 16-bytes.
	int alignedSize = ((data->Length + 0xF) >> 4) << 4;
	array< Byte >^ outbuf = gcnew array< Byte >( alignedSize + 0x10 );
	array< Byte >^ dataBuf = gcnew array< Byte >( alignedSize );

	// Fully copy the contents of the encrypted file.
	data->CopyTo( dataBuf, 0 );

	// Call the SD functions.
	ctx2.mode = sdEncMode;
	ctx1.unk = 1;
	array< Byte >::Copy( temp, 0, outbuf, 0, 0x10 );
	for( unsigned i = 0; i < 0x10; ++i )
		ctx1.buf[ i ] = outbuf[ i ] ^ mhp3_gamekey[ i ];
	//now, outbuf contains the random hash, ctx1.buf contains hash^gamekey
	hleSdRemoveValue( %ctx2, outbuf, 0x10 );
	hleSdRemoveValue( %ctx2, dataBuf, alignedSize );
	hleSdSetMember( %ctx1, dataBuf );
	dataBuf->CopyTo( outbuf, 0x10 );
	//

	hleSdRemoveValue( %ctx2, dataBuf, alignedSize );
	array< Byte >^ hash = gcnew array< Byte >( 0x10 );
	//hleSdGetLastIndex( %ctx2, hash, nullptr );
	hleSdGetLastIndex( %ctx2, hash, mhp3_gamekey );

	int x = 9;
	x++;

	//return outbuf;
}//adsf

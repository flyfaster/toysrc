#ifndef _RIJNDAEL_H_
#define _RIJNDAEL_H_


#define _MAX_KEY_COLUMNS (256/32)
#define _MAX_ROUNDS      14
#define MAX_IV_SIZE      16


typedef unsigned char  UINT8;
typedef unsigned int   UINT32;    // We assume that unsigned int is 32 bits long
typedef unsigned short UINT16;

// Error codes
#define RIJNDAEL_SUCCESS 0
#define RIJNDAEL_UNSUPPORTED_MODE -1
#define RIJNDAEL_UNSUPPORTED_DIRECTION -2
#define RIJNDAEL_UNSUPPORTED_KEY_LENGTH -3
#define RIJNDAEL_BAD_KEY -4
#define RIJNDAEL_NOT_INITIALIZED -5
#define RIJNDAEL_BAD_DIRECTION -6
#define RIJNDAEL_CORRUPTED_DATA -7

class Rijndael
{	
public:
	enum Direction { Encrypt , Decrypt };
	enum Mode { ECB , CBC , CFB1 };
	enum KeyLength { Key16Bytes , Key24Bytes , Key32Bytes };

	Rijndael();
	~Rijndael();
protected:
	// Internal stuff
	enum State { Valid , Invalid };

	State     m_state;
	Mode      m_mode;
	Direction m_direction;
	UINT8     m_initVector[MAX_IV_SIZE];
	UINT32    m_uRounds;
	UINT8     m_expandedKey[_MAX_ROUNDS+1][4][4];
public:
	//////////////////////////////////////////////////////////////////////////////////////////
	// API
	//////////////////////////////////////////////////////////////////////////////////////////



	// mode      :   Rijndael::ECB (we use usually)
        //               Rijndael::CBC
        //            or Rijndael::CFB1
	//
	// dir       : Rijndael::Encrypt or Rijndael::Decrypt
	//             A cipher instance works only in one direction
        //
	// key       : array of unsigned octets , it can be 16 , 24 or 32 bytes long
	//
	// keyLen    : Rijndael::Key16Bytes , Rijndael::Key24Bytes or Rijndael::Key32Bytes
        //
	// initVector: initialization vector, you will usually use 0 here
        // Returns RIJNDAEL_SUCCESS or an error code
	int init(Mode mode,Direction dir,const UINT8 *key,KeyLength keyLen,UINT8 * initVector = 0);

	// Encrypts the input array
	// The input array can be any length , it is automatically padded on a 16 byte boundary.
	// Input len is in BYTES!
	// outBuffer must be at least (inputLen + 16) bytes long
        //                            ~~~~~~~~~~~~~~
	// Returns the encrypted buffer length in BYTES or an error code < 0 in case of error
	int padEncrypt(const UINT8 *input, int inputOctets, UINT8 *outBuffer,int lastbuf);

	// Decrypts the input vector
	// Input len is in BYTES!
	// outBuffer must be at least inputLen bytes long
	// Returns the decrypted buffer length in BYTES and an error code < 0 in case of error
	int padDecrypt(const UINT8 *input, int inputOctets, UINT8 *outBuffer,int lastbuf);
protected:
        /////////////////////////////////////////////////////////////////////////////////
        //  Alogorithm
        /////////////////////////////////////////////////////////////////////////////////


        //get the expanded key from key
	void keySched(UINT8 key[_MAX_KEY_COLUMNS][4]);

        //get the inverse expanded key form expanded key
	void keyEncToDec();

        //encrypt a block(128bits)
	void encrypt(const UINT8 a[16], UINT8 b[16]);

        //decrypt a block(128bits)
	void decrypt(const UINT8 a[16], UINT8 b[16]);
};

#endif // _RIJNDAEL_H_

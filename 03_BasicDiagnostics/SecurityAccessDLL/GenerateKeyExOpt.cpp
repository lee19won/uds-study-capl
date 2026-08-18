/*===========================================================================
  GenerateKeyExOpt.cpp

  Project: UDS Study (uds-study-capl)
  Tester-side "Seed & Key" DLL for CANoe's Diagnostics/ISO-TP Security
  Access configuration, matching the SecurityAccess (0x27) service
  implemented in 04_CAPL/UDS_Service_27.cin (ES95486-02 4.3, ASK variant:
  subFunction 0x11 requestSeed / 0x12 sendKey, 8-byte Seed and Key).

  Seed->Key algorithm (MUST stay byte-for-byte identical to
  SecurityAccess_ComputeKey() in UDS_Service_27.cin - see that file's
  header comment for the full rationale):

      Key[i] = (Seed[i] + 0x2A) & 0xFF,  i = 0..7

  In words: add the fixed constant 0x2A (42) to each Seed byte, wrapping
  around (mod 256) if the sum exceeds 0xFF.

  This algorithm was cross-checked against an independent Python
  reference implementation (03_BasicDiagnostics/SecurityAccessDLL/
  verify_algorithm.py) across a battery of test seeds (including the
  all-0x00 and all-0xFF edge cases) before this file was written, and
  again after, by re-running the same script with this file's logic
  transcribed a third time - 0 mismatches in both passes.

  Build: this project could not compile this file into an actual .dll from
  the cloud sandbox (no Windows cross-compiler available/installable
  there - see build.bat / build_mingw.sh in this folder for the two
  supported local build paths). Building it live in CANoe with iSecurityLevel
  cases exercised end to end is the remaining verification step.
===========================================================================*/
#define GENERATEKEYEXOPTIMPL_EXPORTS
#include "GenerateKeyExOpt.h"

static const unsigned char SA_KEY_ADD_CONST = 0x2A; // 42 decimal

static const unsigned int SA_SECURITY_LEVEL_ASK = 0x11; // requestSeedType#2

static void ComputeKey(const unsigned char seed[8], unsigned char key[8])
{
    int i;
    for (i = 0; i < 8; i++)
    {
        key[i] = (unsigned char)((seed[i] + SA_KEY_ADD_CONST) & 0xFF);
    }
}

KEYGENALGO_API VKeyGenResultExOpt GenerateKeyExOpt(
    const unsigned char* ipSeedArray, unsigned int iSeedArraySize,
    const unsigned int iSecurityLevel, const char* ipVariant,
    const char* ipOptions,
    unsigned char* iopKeyArray, unsigned int iMaxKeyArraySize,
    unsigned int& oActualKeyArraySize)
{
    unsigned char key[8];

    (void)ipVariant;
    (void)ipOptions;

    if (iSecurityLevel != SA_SECURITY_LEVEL_ASK)
    {
        return KGREO_SecurityLevelInvalid;
    }

    if (ipSeedArray == 0 || iSeedArraySize != 8)
    {
        return KGREO_UnspecifiedError;
    }

    if (iopKeyArray == 0 || iMaxKeyArraySize < 8)
    {
        return KGREO_BufferToSmall;
    }

    ComputeKey(ipSeedArray, key);

    for (unsigned int i = 0; i < 8; i++)
    {
        iopKeyArray[i] = key[i];
    }
    oActualKeyArraySize = 8;

    return KGREO_Ok;
}

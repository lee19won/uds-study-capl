/*===========================================================================
  GenerateKeyExOpt.h

  Vector CANoe/CANalyzer "Seed & Key" DLL interface (GenerateKeyExOpt),
  used by the Diagnostics/ISO-TP "Security Access" tester-side key
  computation. Interface verified against:
    - Vector Application Note AN-IDG-1-017 "Security Access" (2020-07-08)
    - a public working reference implementation (Evilpaul/seedkey on GitHub)
  before this project's own algorithm was written, specifically to avoid
  guessing an unverified DLL export signature (see project chat log,
  2026-08-18).
===========================================================================*/
#ifndef SBCM_GENERATEKEYEXOPT_H
#define SBCM_GENERATEKEYEXOPT_H

#ifdef GENERATEKEYEXOPTIMPL_EXPORTS
#define KEYGENALGO_API extern "C" __declspec(dllexport)
#else
#define KEYGENALGO_API extern "C" __declspec(dllimport)
#endif

typedef enum
{
    KGREO_Ok                    = 0,
    KGREO_BufferToSmall          = 1,
    KGREO_SecurityLevelInvalid   = 2,
    KGREO_VariantInvalid         = 3,
    KGREO_UnspecifiedError       = 4
} VKeyGenResultExOpt;

KEYGENALGO_API VKeyGenResultExOpt GenerateKeyExOpt(
    const unsigned char* ipSeedArray, unsigned int iSeedArraySize,
    const unsigned int iSecurityLevel, const char* ipVariant,
    const char* ipOptions,
    unsigned char* iopKeyArray, unsigned int iMaxKeyArraySize,
    unsigned int& oActualKeyArraySize);

#endif /* SBCM_GENERATEKEYEXOPT_H */

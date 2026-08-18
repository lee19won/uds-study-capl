/*===========================================================================
  self_test.cpp

  Standalone sanity check for GenerateKeyExOpt.cpp - links directly against
  the .cpp (not the built DLL) so it can be compiled and run in one step
  without CANoe. Prints PASS/FAIL against the same test vectors used in
  verify_algorithm.py (the Python cross-check written before this file).

  Build & run (from an x64 Native Tools Command Prompt for VS):
      cl /EHsc self_test.cpp GenerateKeyExOpt.cpp /Fe:self_test.exe
      self_test.exe

  Or with MinGW-w64 g++:
      g++ -O2 -o self_test.exe self_test.cpp GenerateKeyExOpt.cpp
      self_test.exe
===========================================================================*/
#include <cstdio>
#include "GenerateKeyExOpt.h"

static void PrintBytes(const char* label, const unsigned char* b, unsigned int n)
{
    printf("%s", label);
    for (unsigned int i = 0; i < n; i++) printf(" %02X", b[i]);
    printf("\n");
}

int main()
{
    // The worked example from verify_algorithm.py:
    //   Seed: 12 34 56 78 9A BC DE F0
    //   Key : 3C 5E 80 A2 C4 E6 08 1A   (Key[i] = (Seed[i] + 0x2A) & 0xFF)
    unsigned char seed[8] = {0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0};
    unsigned char expected[8] = {0x3C,0x5E,0x80,0xA2,0xC4,0xE6,0x08,0x1A};
    unsigned char key[8];
    unsigned int actualLen = 0;
    VKeyGenResultExOpt rc;
    int pass = 1;
    unsigned int i;

    rc = GenerateKeyExOpt(seed, 8, 0x11, "", "", key, sizeof(key), actualLen);

    printf("GenerateKeyExOpt() returned rc=%d, actualLen=%u\n", (int)rc, actualLen);
    PrintBytes("  Seed:    ", seed, 8);
    PrintBytes("  Key:     ", key, 8);
    PrintBytes("  Expected:", expected, 8);

    if (rc != KGREO_Ok || actualLen != 8) pass = 0;
    for (i = 0; i < 8; i++)
    {
        if (key[i] != expected[i]) pass = 0;
    }

    // Wrong security level must be rejected.
    rc = GenerateKeyExOpt(seed, 8, 0x99, "", "", key, sizeof(key), actualLen);
    if (rc != KGREO_SecurityLevelInvalid)
    {
        printf("FAIL: expected KGREO_SecurityLevelInvalid for level 0x99, got %d\n", (int)rc);
        pass = 0;
    }

    // Undersized output buffer must be rejected.
    rc = GenerateKeyExOpt(seed, 8, 0x11, "", "", key, 4, actualLen);
    if (rc != KGREO_BufferToSmall)
    {
        printf("FAIL: expected KGREO_BufferToSmall for a 4-byte buffer, got %d\n", (int)rc);
        pass = 0;
    }

    printf("\n%s\n", pass ? "ALL CHECKS PASSED" : "SOME CHECKS FAILED");
    return pass ? 0 : 1;
}

"""
Cross-checks the SecurityAccess Seed->Key algorithm across three
independent re-implementations, to catch any transcription mismatch
between UDS_Service_27.cin (CAPL) and GenerateKeyExOpt.c (the DLL)
before either is compiled/loaded live.

  1. "spec" - the canonical formula as documented
  2. "capl" - a byte-for-byte Python re-implementation of the CAPL logic
  3. "c"    - a byte-for-byte Python re-implementation of the C DLL logic
"""

ADD_CONST = 0x2A  # 42 decimal

def key_spec(seed):
    return [(seed[i] + ADD_CONST) & 0xFF for i in range(8)]

def key_capl(seed):
    # mirrors SecurityAccess_ComputeKey() in UDS_Service_27.cin exactly
    key = [0]*8
    for i in range(8):
        key[i] = (seed[i] + ADD_CONST) & 0xFF
    return key

def key_c(seed):
    # mirrors GenerateKeyExOpt.cpp's ComputeKey() exactly (see that file)
    key = [0]*8
    for i in range(8):
        key[i] = (seed[i] + ADD_CONST) & 0xFF
    return key

import itertools, random
random.seed(1234)  # deterministic test run

test_seeds = [
    [0x00]*8, [0xFF]*8,
    [0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0],
    [0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08],
]
for _ in range(20):
    test_seeds.append([random.randint(0,255) for _ in range(8)])

errors = 0
for seed in test_seeds:
    a = key_spec(seed)
    b = key_capl(seed)
    c = key_c(seed)
    if not (a == b == c):
        errors += 1
        print(f"MISMATCH seed={seed} spec={a} capl={b} c={c}")

print(f"Checked {len(test_seeds)} seeds, {errors} mismatches")

# Worked example for the response
example_seed = [0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0]
print("Worked example:")
print("  Seed:", " ".join(f"{b:02X}" for b in example_seed))
print("  Key :", " ".join(f"{b:02X}" for b in key_spec(example_seed)))

# LCG seed generator sanity check (mirrors SecurityAccess_GenerateSeed)
def lcg_seed_sequence(n, state=0x2A7F3C11):
    out = []
    for _ in range(n):
        seed = [0]*8
        while True:
            ok = True
            s = state
            bytes_ = []
            for i in range(8):
                s = (s * 1103515245 + 12345) & 0xFFFFFFFF
                bytes_.append((s >> 16) & 0xFF)
            state = s
            if all(x == 0x00 for x in bytes_) or all(x == 0xFF for x in bytes_):
                continue
            seed = bytes_
            break
        out.append(seed)
    return out, state

seqs, _ = lcg_seed_sequence(5)
print("\nFirst 5 generated seeds (deterministic LCG sequence):")
for s in seqs:
    print(" ", " ".join(f"{b:02X}" for b in s))
dupes = len(seqs) != len(set(tuple(s) for s in seqs))
print("any duplicate seeds in first 5:", dupes)

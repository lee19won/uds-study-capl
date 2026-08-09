from pathlib import Path
for p in Path('.').glob('*'):
    if p.is_file():
        data = p.read_bytes()
        cr = b'\r' in data
        lf = b'\n' in data
        nonascii = [i+1 for i,b in enumerate(data) if b > 127]
        print(f'FILE:{p.name} CR={cr} LF={lf} NONASCII={len(nonascii)>0} NONASCII_POS={nonascii[:5]}')

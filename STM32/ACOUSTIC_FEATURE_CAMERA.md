# Acoustic Feature Camera

## Target use cases

Config A
- Musical instrument recognition
- Life log
- Bird chirp recognition

Config B
- Acoustic Scene classification
I am planning to use an ultra-sonic MEMS mic from Knowles.

## DFSDM config (tentativ)

Config A
```
System clock: 80MHz
Clock divider: 64
FOSR(decimation): 64
sinc filter: sinc4
resolution: 2 * 64^4 = 2^25 (25 bits)
right bit shift in DFSDM: 9 (results in 16 bit PCM)
Sampling frequency: 80_000_000/64/64 = 19.5kHz
```

Config B
```
...
```

Co

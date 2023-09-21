# Acoustic Feature Camera

## DFSDM config

```
System clock: 80MHz
Clock divider: 64
FOSR(decimation): 64
sinc filter: sinc4
resolution: 2 * 64^4 = 2^25 (25 bits)
right bit shift in DFSDM: 9 (results in 16 bit PCM)
Sampling frequency: 80_000_000/64/64 = 19.5kHz
```

[日本語](./README.ja.md)

# libadpcm
   Header-only ADPCM helper library and simple recorder/player for X68000 using IOCS calls.
## Download
- [adpcmrec.x](https://raw.githubusercontent.com/renatus-novus-x/libadpcm/main/bin/adpcmrec.x)
- [adpcmplay.x](https://raw.githubusercontent.com/renatus-novus-x/libadpcm/main/bin/adpcmplay.x)

## Usage
```
  adpcmrec.x filename seconds [rate]
    rate: 0=3.9kHz 1=5.2kHz 2=7.8kHz 3=10.4kHz 4=15.6kHz (default 4)
```

```
  adpcmplay.x [rate] [out]  
    rate: 0=3.9kHz 1=5.2kHz 2=7.8kHz 3=10.4kHz 4=15.6kHz (default 4)
    out : 0=off 1=left 2=right 3=stereo (default 3)
```
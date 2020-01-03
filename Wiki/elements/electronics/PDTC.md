# PDTC
### Photon Detector

![Element](https://i.imgur.com/ZeFaZ49.gif)

*PDTC asborbing a photon and applying its ctype to nearby FILT*

**Description:**  Photon Detector, outputs to WIFI channels depending on wavelength, floodfills nearby FILT

Upon detecting a photon in a 2 px radius around the detector, it will (by default delete the photon, see below for settings) and output to any adjacent WIFI and FILT. For FILT, it will copy the photon's ctype and floodfill any FILT nearby with the new ctype. For WIFI, it determines the channel to output to using this formula:
```
channel = (photon ctype) % 101 + 1
```

A trick is to layer a lot of photons with different ctypes and placing a PDTC beside it, it will slowly remove each layer one by one (top down) and output. This can be used for mass photon storage.

## Technical
#### Properties
**life:** Countdown to next measurement

**temp:** Delay before next measurement (in frames in C, < 0 = 0). For example, if the PDTC was 5 C, it would wait 5 C after detecting a PHOT before attempting to detect another.

**tmp:** Restrictions on output:
```
0 = all outputs
1 = only output to FILT
2 = only output to WIFI
```

**tmp2:** What to do with PHOT:
```
0 = delete photon after detection
1 = don't delete photon
```
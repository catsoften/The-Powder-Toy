# AQRG
### Aqua regia

**Description:**  *Aqua regia. Can dissolve certain metals.*

Behaves like ACID, but can dissolve TIN, COPR, BRNZ, BRAS, PTNM and GOLD. When dissolving those metals it will set its ctype 
to that metal. Can also dissolve BRKN with ctypes of those metal, and ROCK (ctype) of those metals.

AQRG will be diluted (life will go down) with WATR, DSTW, BAKS or SOAP. Otherwise, its life will go down naturally. It starts out 
water color, but changes to orange over a few seconds. When it's life reaches 0, it has a chance to decay into WATR and CAUS. If it has a 
ctype, upon changing state (melting, heating, natural decay, etc...) it will release its ctype as a BRKN (ctype).

Freezes at -42 C into ICEI, boils at 108 C into WTRV. When boiled, will release broken forms of the metal dissolved if any.

## Technical
#### Properties
**life:** Used for dissolve timer like acid

**ctype:** Noble metal dissolved

**tmp2:** Used for color timing
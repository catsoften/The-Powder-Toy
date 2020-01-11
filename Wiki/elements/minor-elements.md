# Minor Elements

## GLUE
Sticks powders together into solids (Not a moving solid, just a solid). Hardens on contact or pressure, and melts under higher temperatures (> 70 C). Above 300 C turns to CMRC. Converts GLOW into LCRY. Extremely flammable.

## TPRS (Temperature Preserver)
Use for heaters or coolers, it will stay at whatever temp you set it to (Use HEAT / COOL or console) but still conduct its temperature. For example, if you set it to 0 K and pour LAVA on it, the LAVA will cool down but it will never heat up.

## NLPM
Napalm, sticky long burning liquid. Burns slowly, but can quickly create fireballs when its spread out in the air.

## FREZ
Cold gas that does not lose heat, but will cool down anything it touches as long as its own temperature is above 20 K. Unlike other coolers,
this also puts out fires and PLSM. Ignites C5. Undergoes cold-fusion at low temperatures and high pressures.

## SUGR
Sugar, melts at 186 C, provides food for bacteria. Flammable. Reacts with ACID and CAUS to form CRBN, dissolves in water to form SWTR. YEST 
grows in SUGR. Grows slowly in sugar water.

## SWTR (Sugar water)
Water that has sugar dissolved in it. Grows YEST, basically SLTW but with SUGR instead of SALT. Grows PLNT slowly.

## BGEL (Ballistic Jelly)
Brightens depending on the highest magnitude of the pressure it experienced, useful for bomb testing. Stores the magnitude in pavg[0].

## HONY (Honey)
Sticky liquid. Other life forms (except ANT) will not EAT HONY. HONY slowly kills BCTR. Turns solid near WAX or when cold (< 20 C). Heals STKM 5 HP.

## DFLM (Dark Flame)
Like a combination of CAUS and VIRS. Burns slowly through almost anything. Will decay slowly if it hasn't burnt anything in a long time. 
Turns into FIRE if cooled below 10 K. Undergoes "renewals" where its temperature increases 
400 degrees and life gets renewed.

## SOIL
A nice element to plant SEED in. Freezes solid when < 0 C, above 60 C turns into CLST. Absorbs water to turn into MUD, if it 
absorbs SLTW or SWTR it might create SUGR or SALT. When it's cold it turns blueish. tmp2 is used for ant hills, if tmp2 = 1 or 2
acts like a tunnel (doesn't move, and if tmp2 = 2, slightly darker and allows particles to pass).

## MUD
A liquid that creates slight negative pressure. Turns into BRCK above 500 C, below 0 C or above 100 C it will act solid.

## WEB
Sticky solid, gets displaced like GOO. Will dissolve if there are no surrounding web particles. Draws "connections" like SOAP bubbles. 
If BIRD, ANT or BEE touches WEB it instantly turns into WEB (ctype BEE, BIRD, etc...) and can be eaten by SPDR. Captured prey will be 
freed if their web dissolves. WEB slows down liquids and powders with weight < 50. Above 500 C it instantly ignites.

## SFLD (Superfluid)
Evaporates into NEON at 27.102 K, likes to "flow" along solids. Suffers no loss, air drag or diffusion, and has max thermal conductivity.

## SHPO
Shampoo, evaporates into water vapor at 100 C. Cures VIRS. Will clean any deco color, and reset ALL properties (except temp) back 
to the original defaults. If a particle has a Create function, it will be called too. SHPO will make bubbles and slowly dissolve powders. 
Note: washing HAIR with too much SHPO might dissolve it. 

## HAIR
A powder, will instantly freeze when near the head of STKM or STKM2 (and FIGH, but it won't follow sometimes). It will follow the head 
of STKM and STKM2. You can decorate hair into shapes (ie to make arms or clothes). Hair will also freeze when it has
touched GEL or is below 0 C. When touching powders hair can be "dirtied" and take on the color of the powder as a deco color. If 
the STKM head moves too fast the HAIR will break apart.

## CMNT (Cement)
Slowly hardens into either RCRT, CNCT or STNE depending on how impure it is (determined by tmp2). Adding SLTW or SWTR raises 
impurity by 5 for each particle absorbed, being near CRBN, DUST or SALT randomly adds 2 impurity. Above 50 impurity turns into CNCT 
instead of RCRT, above 120 impurity turns into STNE instead of CNCT.

For every 50 C above 0 C RCRT hardens 1 life faster. Adding water increases life by 200 / particle, life is averaged out in a group of cement. Heats up slightly when cooling, turns darker as it hardens. Maxes out life at 3000, sticks like GEL. Turns into STNE at 983 K.

## CMTP (Cement Powder)
Mix with any type of water to make CMNT. Starts with 500 life. If SLTW or SWTR is used starts at 100 impurity. Melts at 983 K.

## MONY (Money)
Like a magnet, attracts STKM and STKM2. Turns OIL, COAL, BCOL and GOLD into MONY on contact. Glows slightly. Deadly to STKM.

## HOLY (Holy water)
Holy WATR kills:
JCB1, FIGH, STKM, STKM2, MONY, OIL, DEST, BOMB, SING, TANK, ACID, BCTR, VIRS, SPDR, Any fire type, EMP, WARP, GNSH, FFLD, SHLD, LOVE, NPLM

Cools any hot particles down to room temperature. May produce SMKE when cleansing.

## CESM (Cesium)
A supped up rubidium, as suggested [https://powdertoy.co.uk/Discussions/Thread/View.html?Thread=21682](here). Explodes like TNT but slightly 
stronger on contact with any varient of water or water vapor. Conducts electricity. Melts at 28 C in LCSM. Changes PHOT into ELEC. Can 
react with O2 to form cesium oxide and Au to form cesium auride. CsAu conducts as fast as gold.

There are also some reactions with ACID / CAUS to form SALT and HYGN or WATR or GOLD depending on the type of CESM. Cesium oxide 
is darker than regualr CESM and cesium auride is more golden.

## MMSH
Metallic mesh, blocks life (SPDR, BEE, ANT, BIRD, FISH) and allows liquids, powders, gases and all energy particles through. Liquids 
and powders might randomly be stopped for a single frame while passing through. Doesn't get sparked by electron.
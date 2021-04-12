# Adding New Menus
This guide explains how to add new menus to TPT, since the guide in the powder toy wiki is not up to date yet.

### 1.  Get the new font editor
The new font editor can be compiled using `scons --font`. It will generate a file called something like `font` or `font64` in the `/build` folder. As far as I know there isn't a way to do this in VS.

### 2. Run the editor
The editor takes the path to `data/font.cpp` as its single argument, so if you're in the top directory of your mod (with the src folder, data folder, etc...) you can run `build/font64 data/font.cpp`

### 3.  Use the Editor
Go to an unused hex code (As of writing this, it's somewhere around 0x81 or 0x82, just keep going right until it says no char). Click "Add". You can use the `<>` and `><` to change the x width of the editor. Right click and left click to toggle pixel brightness.

You may note that a lot of menu icons are 0xE064 or something in that range. While not displayed below, you can manually type in these values in the editor to get to where the other menu icons are stored.

When you're done making the icon, click Save. The saving should happen instantly, even though the button will look like its still pressed. Remember what hex code you assigned your new icon, and close the editor.

### 4. Fix font.cpp
Open `data/font.cpp` and scroll to where it says

```cpp
extern const short font_ptrs[] = {
```
It should actually be
```cpp
extern const unsigned short font_ptrs[] = {
```
(If it's already unsigned then ignore this step. For some reason sometimes its not unsigned and when compiling with VS it will give some cryptic error message).


### 5. Add your menu to the Simulation Data
Open up `src/SimulationData.h` and near the top it says
```cpp
#define SC_WALL 0
#define SC_ELEC 1
#define SC_POWERED 2
```
Just insert your menu somewhere in. If you wish to change the order of menus (ie, you want to put a menu after SC_POWERED, you have to shift all the menus after). For example, if we wanted to add a new menu SC_ELECTROMAG after SC_POWERED, we can do

**Before:**
```cpp
#define SC_WALL 0
#define SC_ELEC 1
#define SC_POWERED 2
#define SC_SENSOR 3
#define SC_FORCE 4
...
```

**After:**
```cpp
#define SC_WALL 0
#define SC_ELEC 1
#define SC_POWERED 2
#define SC_ELECTROMAG 3
#define SC_SENSOR 4
#define SC_FORCE 5
...
```

Near the bottom of the menu defines you also need to increment `#define SC_TOTAL` by the number of menus you added.

### 7. Add menu to other Simulation Data
Now open `src/SimulationData.cpp` and see where it says
```cpp
return std::vector<menu_section>{
		{0xE041, String("Walls"), 0, 1},
		{0xE042, String("Electronics"), 0, 1},
		{0xE056, String("Powered Materials"), 0, 1},
		...
```
Insert your menu in the vector (Note: it must be in the same order as the defines in `SimulationData.h` or the menus will have wrong labels). The first element is the hex code you memorized earlier, the 2nd is the name of the menu. The third appears to be unused, leave as 0. The fourth is is the menu visible? (1 = yes, 0 =no).

For our example, the menu vector now looks like:
```cpp
		{0xE041, String("Walls"), 0, 1},
		{0xE042, String("Electronics"), 0, 1},
		{0xE056, String("Powered Materials"), 0, 1},
		 // 0x81 was our icon, write down your code!
		{0x81,   String("Electromagnetic"), 0, 1},
		{0xE019, String("Sensors"), 0, 1},
		{0xE062, String("Force"), 0, 1},
		...
```

Now you'll have to recompile everything again and you'll be good to go.




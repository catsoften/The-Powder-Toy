# Embedding Scripts
### 1. Adding the script
In `src/lua/luascripts` add the following files (replace SCRIPT with the name of the script, ie `movingsolids.lua` or whatever)

```
SCRIPT.lua
SCRIPT.lua.cpp
SCRIPT.lua.orig
```

Copy the contents of the script into `SCRIPT.lua`.  Open up `SCRIPT.lua.orig` and copy the following content in, replacing the 2 `@SCRIPT.lua` with the name of the file and `IDI_SCRIPTNAME` with a unique identifier, ie `IDI_MOVINGSOLIDS`. Also optionally replace `script` with the name of your file in the variable names. Remember this identifier for later.

```cpp
#ifdef LUACONSOLE
#ifdef _MSC_VER
#include "Platform.h"
#include "resource.h"
#endif
#include "lua/LuaCompat.h"

void luaopen_script(lua_State *l)
{
#ifndef _MSC_VER
	int script_luac_sz = /*#SIZE*/;
	const char* script_luac = /*#DATA*/;
	luaL_loadbuffer(l, script_luac, script_luac_sz, "@SCRIPT.lua");
	lua_pcall(l, 0, 0, 0);
#else
	unsigned int size = 0;
	const char* data = NULL;
	Platform::LoadFileInResource(IDI_SCRIPTNAME, LUASCRIPT, size, data);
	char *buffer = new char[size+1];
	::memcpy(buffer, data, size);
	buffer[size] = 0;
	luaL_loadbuffer(l, buffer, size, "@SCRIPT.lua");
	lua_pcall(l, 0, 0, 0);
	delete[] buffer;
#endif
}
#endif
```

### 2.  Adding scripts to resources
In `resources/resource.h` add
```cpp
#define IDI_SCRIPTNAME [number]
```
Where `IDI_SCRIPTNAME` is the identifier from earlier and `[number]` is a number. I have no idea how the numbering system works, but it appears to countdown from 122. At the time of writing `[number]` should be `119`, 1 less than `IDI_TPTMP = 120`.

Next open `resources/powder-res.rc` and add the line
```
IDI_SCRIPTNAME LUASCRIPT "../src/lua/luascripts/SCRIPT.lua"
```
Replacing the identifier and file path.

### 2.5. (Modifying script?)
Some scripts assume a normal TPT menu count, however TPT Ultima has a double column layout. Thus, you'll need to make some adjustments to your script to make sure menus align correctly. This should be done before the next steps (You can still do them later, just repeat the later steps if you make an edit)

For example, in the script manager, the following edit is needed:
```lua
elseif tpt.num_menus then
	ypos = 390-16*math.min(tpt.num_menus(), 16)-(not jacobsmod and 16 or 0)+1
end
sidebutton = ui_button.new(gfx.WIDTH-33,ypos,14,14,ui_button.sidepressed,'')
```

A similar edit is needed in TPT Multiplayer
```lua
local function getypos()
	local ypos = 136
	if jacobsmod and tpt.oldmenu and tpt.oldmenu()==1 then
		ypos = 392
	elseif tpt.num_menus then
		ypos = 392-16*math.min(tpt.num_menus(), 14)-(not jacobsmod and 16 or 0)+1
	end
	if using_manager then ypos = ypos - 17 end
	return ypos
end
local jacobsmod_old_menu_check = false
local showbutton = ui_button.new(614,getypos(),14,14,function() if using_manager and not MANAGER.hidden then _print("minimize the manager before opening TPTMP") return end if not hooks_enabled then TPTMP.enableMultiplayer() end L.chatHidden=false TPTMP.chatHidden=false L.flashChat=false end,"<<")
```


### 3. Generating the script
```
cd ./src/lua/luascripts
python file2c.py SCRIPT.lua.cpp SCRIPT.lua SCRIPT.lua.orig
```
Replacing SCRIPT with the name you choose. This will automatically generate the cpp file for the embedded script. Compile and you should be good.

### 4. Make TPT run the script
In `src/lua/LuaScriptInterface.cpp` around line 360 add
```cpp
luaopen_script(l);
```
Replacing script with whatever you named it in the .orig file.



# CNFG

### Config

**Description:** *Config tool. Allows you to configurate certain particles like WIFI or QLOG by clicking on them.*

The config tool allows you to configure certain particles. Here are the particles:

### WIFI / Portals
![Tool](https://i.imgur.com/q8NIRj0.gif)

Clicking on this will allow you to change the channel. The previous channel will be shown, along with an input for the new channel. **Only the wifi particle you clicked on will be configurated**, however portals are floodfilled with the same channel.

### FILT / PHOT
![Tool](https://i.imgur.com/UIbsDn5.gif)

Using this will allow you to modify the wavelength. Wavelengths can be given in hex (Prefix with 0x, ie 0xFAF), or a number. Invalid wavelengths are ignored. If the particle that was clicked was FILT, also gives a "FILT Operation" dropdown option. The property is floodfilled.

### ION
![Tool](https://i.imgur.com/OxjRPTV.gif)

This doesn't allow you to configurate, but rather, view the quantum state of an ION. Shows the number of particles in the state, the state id, the ids of particles in the state, and a list of the represented qbit state, the complex entry in the state vector, and the probability of collapsing to that state when observed.

### FFGN
![Tool](https://i.imgur.com/jplO6Az.gif)

Allows you to change size of the force field generator by clicking and moving your mouse outside the window. You can also drag the force field generator setting window around, change ctype, mode and shape. Only modifies the 1 px of FFGN that was clicked on.

NOTE: The connect to other FFGN mode does not render the connection lines.

### CRAY
![Tool](https://i.imgur.com/zdSfCch.gif)

Lets you change the element, spawn temperature, spawn life, and gap of CRAY. A preview will be shown: red is where the element will be created, dark red is the gap. The distance shown on the line is the gap + length of element line. The "Edit CRAY" Window can be dragged. You can see what the CRAY would fire in different directions by clicking and moving the mouse to change direction and distance. Only affects the 1 px of CRAY that was clicked on.

### CLUD
![Tool](https://i.imgur.com/08yAeKL.gif)

Lets you change the weather type and spawn temperature of the cloud. Floodfills the property onto the CLUD.

### TRBN
![Tool](https://i.imgur.com/IAYPop2.gif)

Allows you to modify the min air and particle velocity required to activate a TRBN. Floodfills the property.

### DTEC / TSNS / PSNS / LSNS
![Tool](https://i.imgur.com/6VrJtpm.gif)

Allows you to configure range of these detectors, along with ctype (DTEC) or temp (others), and any options if they have them.

### HSWC / DLAY / PVOD / PUMP
![Tool https://i.imgur.com/XTYvn9K.gif](https://i.imgur.com/XTYvn9K.gif)

Allows to configure ctypte / temp, along with any additional modes if available. Floodfills the new properties.
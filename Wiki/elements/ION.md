# ION
### Supercooled ion

![Element](https://i.imgur.com/5tvjm4Y.gif)

**Description:**  Supercooled ion, can be used for quantum computing. Decoheres with heat and interactions.

IONs behave like generic energy particles, with high diffusion and a certain "stickness" to other elements. If ION's temperature decreases below 20.0 K, it will not diffuse, decay, or accelerate. (If it goes above 20.0 K, it will spontaneously gain kinetic energy and move). Upon touching another non-ION or QLOG particle, or being heated above 20.0 K, there will be a chance for the ION to undergo decoherence, in which its quantum state becomes unreliable. As such, your quantum computers should be cooled to near absolute zero temperatures.

> (Because ~~the dev was lazy~~ of the no-cloning theorem, quantum states DO NOT presist when the save is reloaded, nor do they save!)

> Quantum states are at most 6 qbits big, any larger states will instantly decohere. This is because the number of computations required grows in powers of two for the number of qbits. There can be at most 1000000000 quantum states present in a sim at one time.

## Technical
#### Properties
**tmp2:** Saves the quantum state ID for the ION. The raw quantum state is inaccessible. Changing this value to an invalid state (0 or non-existent state) will cause any quantum states the ION is in to decohere and a new quantum state to be assigned to the ION.

**flags:** If > 0 then a quantum state was assigned. You should not modify this value manually.

# Known Bugs in Circuits
Here is a list of known bugs in circuits, please do not report these unless you have a good way to fix them:

- **Terminals cannot be shared between junction-nodes and terminal-nodes**
For example, having PSCN as a node and having 3 NSCN come out of it does not make 3 diodes.

- **Non-solid conductors cannot be nodes**
Non-solid conductors (with the exception of solid CRBN) cannot be nodes in a circuit (They can, however, conduct current). This is because non-solids can be highly diffuse, which would create tens, or hundreds of constantly shifting nodes. This tends to murder the framerate, and most people don't care about parallel circuits made entirely of NBLE.

# TRFM

![https://i.imgur.com/ejzqQV0.gif](https://i.imgur.com/ejzqQV0.gif)

**Description:**  *Transform tool. Select a rectangle and resize and rotate selection.*

Select a rectangle, then you can rotate or scale the elements inside. There will be a preview shown: the preview does not fill gaps if "Auto-fill gaps" is enabled. If delete stacked particles is enabled, any particles that become stacked as a result of the rotation are deleted. Scaling using the corner circles preserves aspect ratio, whereas using the other circles only scale X or Y. You can also drag
the center circle to move the selection.

Auto-fill gaps searches for gaps 4 times and repeatedly tries filling them in. A gap is an empty particle surrounded by at least 6 other particles, the element used to fill is selected from a nearby particle (based on particle order, not proportion). Enabling auto-fill gaps 
may result in fuzzy edges.

This tool edits the original selection (does not copy). As a side note, all particles are copied property by property, but ID in
the parts array may change.
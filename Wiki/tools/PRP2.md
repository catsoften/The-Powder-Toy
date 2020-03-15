
# PRP2

![https://i.imgur.com/ce8jYKQ.gif](https://i.imgur.com/ce8jYKQ.gif)

**Description:**  *Multi-Property Drawing Tool. Use to alter the properties of elements in the field.*

Let's you set multiple properties at once, by specifying one per line in the format prop:value, ie

```
life:100000
temp:0
type:DEUT
```

Sets three properties at once. You can also specify relative offsets, ie

```
+tmp:5  // Add 5 to tmp
-tmp:5  // Subtract 5 from tmp
*tmp:5  // Multiply tmp by 5
/tmp:5  // Divide tmp by 5 (Division by 0 does nothing)
|tmp:5  // Bitwise or with 5 and tmp
&tmp:5  // Bitwise AND with 5 and tmp
^tmp:5  // Bitwise XOR with 5 and tmp
```
Note that holding the brush will apply the increment multiple times, so this is recommended for rectangle tool or floodfill. Bitwise 
operators cast the value to unsigned int before applying.

Multiple edits to the same property can be applied in succession in top-down order, ie

```
+temp:100
*temp:2
```

First adds 100 to the temp then multiplies its kelvin value by 2.

Press TAB to cycle between property names in the autocomplete. Ctrl-Enter can also be used instead of clicking the "OK" button.
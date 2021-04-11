#!/usr/bin/env python3
import sys
import re
import os

if sys.version_info.major < 3:
    sys.exit("Run this script with python3, you used {}".format(sys.version))

name = input("Element to modify: ") if len(sys.argv) != 2 else sys.argv[1]
if name == "--help":
    print("""
This script is for modifying elements that already exist, usually right after newelement.py is used
It does some code neatification, and lets you add reactions and copy element properties from similar
elements directly. Note that it may overwrite existing code!

Usage:   python3 modelement.py ELEMENT_NAME
Example: python3 modelement.py GOLD
    """)
    sys.exit(0)

path = "src/simulation/elements/" + name + ".cpp"


def rgb2lab(inputColor):
    num = 0
    RGB = [0, 0, 0]

    for value in inputColor:
        value = float(value) / 255

        if value > 0.04045:
            value = ((value + 0.055) / 1.055) ** 2.4
        else:
            value = value / 12.92

        RGB[num] = value * 100
        num = num + 1

    XYZ = [0, 0, 0, ]

    X = RGB[0] * 0.4124 + RGB[1] * 0.3576 + RGB[2] * 0.1805
    Y = RGB[0] * 0.2126 + RGB[1] * 0.7152 + RGB[2] * 0.0722
    Z = RGB[0] * 0.0193 + RGB[1] * 0.1192 + RGB[2] * 0.9505
    XYZ[0] = round(X, 4)
    XYZ[1] = round(Y, 4)
    XYZ[2] = round(Z, 4)

    # Observer= 2°, Illuminant= D65
    XYZ[0] = float(XYZ[0]) / 95.047         # ref_X =  95.047
    XYZ[1] = float(XYZ[1]) / 100.0          # ref_Y = 100.000
    XYZ[2] = float(XYZ[2]) / 108.883        # ref_Z = 108.883

    num = 0
    for value in XYZ:

        if value > 0.008856:
            value = value ** (0.3333333333333333)
        else:
            value = (7.787 * value) + (16 / 116)

        XYZ[num] = value
        num = num + 1

    Lab = [0, 0, 0]

    L = (116 * XYZ[1]) - 16
    a = 500 * (XYZ[0] - XYZ[1])
    b = 200 * (XYZ[1] - XYZ[2])

    Lab[0] = round(L, 4)
    Lab[1] = round(a, 4)
    Lab[2] = round(b, 4)

    return Lab

def int2lab(n):
    b = n % 256
    g = int(((n - b) / 256) % 256)
    r = int(((n - b) / 256**2) - g / 256)
    return rgb2lab([r, g, b])


def get_elements():
    elements, colors = dict(), dict()
    with open("src/simulation/ElementNumbers.h", "r") as numbers:
        data = numbers.read()
        mod_offset = re.findall("#define MOD_ELEMENT_OFFSET (\d+)", data)[0]

        for nm, pt in re.findall("ELEMENT_DEFINE\\s*\\(\\s*(\\S+)\\s*,\\s*(.+)\\s*\\)", data):
            if nm == "name":
                continue
            elements[nm] = eval(pt.replace("MOD_ELEMENT_OFFSET", mod_offset))

            with open("src/simulation/elements/{}.cpp".format(nm), "r") as element_f:
                code = element_f.read()
                colors[nm] = {
                    "lab" : int2lab(int(re.findall("Colour = PIXPACK\\((.*)\\);", code)[0], 16)),
                    "hex" : re.findall("Colour = PIXPACK\\((.*)\\);", code)[0]
                }
    return (elements, colors)

tmp = get_elements()
elements = tmp[0]
colors = tmp[1]

if not os.path.exists(path):
    print("Element {} does not exist".format(name))
    sys.exit("(Failed to find {})".format(path))

with open(path, "r") as f:
    code = f.read()

    # Extract color and verify color similarity
    chex = re.findall("Colour = PIXPACK\\((.*)\\);", code)[0]
    color = int2lab(int(chex, 16))

    if chex.replace("0x", "").upper() != chex.replace("0x", ""):
        print("Uppercased: " + chex.upper())

    for element in colors:
        other = colors[element]["lab"]
        d = sum([(other[i] - color[i]) ** 2 for i in range(3)]) ** 0.5
        if d < 2.3 and name != element:
            print("Element {} has a very similar color to {} ({} vs {})".format(name, element, chex, colors[element]["hex"]))
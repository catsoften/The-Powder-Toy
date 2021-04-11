import glob
import re

elements_ = glob.glob("./src/simulation/elements/*.cpp")
elements = []

for e in elements_:
    with open(e, "r") as f:
        if "#TPT-Directive" in f.read():
            elements.append(e)

print("\n".join(elements))

for i, e in enumerate(elements):
    with open(e, "r") as f:
        lines = [line for line in f.read().split("\n") if "#TPT-Directive" not in line]
        lines = "\n".join(lines)

        # Update func
        lines = re.sub(r"int Element_[A-Z0-9]+::update\(UPDATE_FUNC_ARGS\)",
            "static int update(UPDATE_FUNC_ARGS)", lines)
        lines = re.sub(r"&Element_[A-Z0-9]+::update", "&update", lines)

        # Graphics func
        lines = re.sub(r"int Element_[A-Z0-9]+::graphics\(GRAPHICS_FUNC_ARGS\)",
            "static int graphics(GRAPHICS_FUNC_ARGS)", lines)
        lines = re.sub(r"&Element_[A-Z0-9]+::graphics", "&graphics", lines)

        # Create func
        lines = re.sub(r"int Element_[A-Z0-9]+::create\(ELEMENT_CREATE_FUNC_ARGS\)",
            "static void create(ELEMENT_CREATE_FUNC_ARGS)", lines)
        lines = re.sub(r"&Element_[A-Z0-9]+::create", "&create", lines)

        # Bottom thing
        lines = re.sub(r"Element_[A-Z0-9]+::~Element_[A-Z0-9]+\(\) {}", "", lines)
        lines = re.sub(r"Element_[A-Z0-9]+::Element_", "void Element::Element_", lines)
        
        # for t in ["void", "int", "short", "char", "double", "float", "bool"]:
        #     lines = re.sub(t + r" Element_([A-Z0-9]+)::", "static " + t + " Element_\\1_", lines)
        lines = re.sub(r"Element_([A-Z0-9]+)::", "Element_\\1_", lines)
        oline = lines
        lines = lines.split("\n")
        final = ""
        added = False

        for line in lines:
            if not added and not line.startswith("#include"):
                if "&update" in oline or "&graphics" in oline or "&create" in oline:
                    final += "\n"
                    if "&update" in oline:
                        final += "static int update(UPDATE_FUNC_ARGS);\n"
                    if "&graphics" in oline:
                        final += "static int graphics(GRAPHICS_FUNC_ARGS);\n"
                    if "&create" in oline:
                        final += "static void create(ELEMENT_CREATE_FUNC_ARGS);\n"
                added = True
            final += line + "\n"

        with open(e, "w") as f2:
            f2.write(final)
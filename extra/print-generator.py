from PIL import Image

im = Image.open("./broadsword-base.png")
w, h = im.size
pix = list(im.getdata())
data = []

# Load image data
for i, px in enumerate(pix):
	if px[-1] != 0: # Non-transparent
		y = int(i / w)
		x = i % w
		data.append([x, y, px])
		
# Process to graphics
out = ""
# draw_str = "ren->addpixel(cpart->x + {x}, cpart->y + {y}, {r}, {g}, {b}, {a});\n"
draw_str = "VehiclePixel({x}, {y}, {r}, {g}, {b}),\n"

for d in data:
	x = d[0]
	y = d[1]
	x -= int(w / 2)
	y -= int(h / 2)
	
	out += draw_str.format(
		x=x, y=y, r=d[2][0], g=d[2][1], b=d[2][2], a=255)
	

with open("image-out.txt", "w") as f:
	f.write(out)
print("Done :)")
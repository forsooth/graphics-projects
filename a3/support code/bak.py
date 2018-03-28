import argparse
import numpy.random as random
from PIL import Image

# Add command line arugment parsing
parser = argparse.ArgumentParser(description='Convert an image into a COMP-175 SceneGraph XML file.', add_help=False)
parser.add_argument('filename', type=str, help='Required argument. The path to the filename to be processed.')
parser.add_argument('-w', '--width', type=int, default=100, help='The width to resize the image to. Default is 100px.')
parser.add_argument('-s', '--shape', type=str, default='cube', help='The shape to use for each pixel; can be any shape expected by the scene parser.')
parser.add_argument('-r', '--no-resize', action='store_true', help='Turn off image resizing and use the image file\'s size.')
parser.add_argument('-n', '--noise', action='store_true', help='Enable noise in the direction each pixel is facing.')
parser.add_argument('-e', '--extrude', action='store_true', help='Turn on extrusion, so pixels are extruded based on brightness values.')
parser.add_argument('-i', '--invert', action='store_true', help='Invert the extrusion, so that light colors are less extruded.')
parser.add_argument('-c', '--extrude-const', type=float, default=1, help='The constant by which to scale the extrusions. Default is 1.')
parser.add_argument('-ax', '--anglex', type=float, default=0, help='The rotation to make about the X axis, in degrees.')
parser.add_argument('-ay', '--angley', type=float, default=0, help='The rotation to make about the Y axis, in degrees.')
parser.add_argument('-h', '--help', action="store_true", help="Shows this help message and exit.")

args = parser.parse_args()

try:
    args = parser.parse_args()
except SystemExit:
    print('Invalid arguments given to program.')
    exit(1)

if args.help:
    parser.print_help()
    exit(0)

filename = args.filename
width = args.width
shape = args.shape
invert = args.invert
extrude = args.extrude
exc = args.extrude_const
anglex = args.anglex
angley = args.angley
noise = args.noise

# Check bounds on some inputs
if exc <= 0:
    print('Extrude constant was <= 0.')
    exit(1)
if shape not in ['cube', 'cylinder', 'cone', 'sphere']:
    print('Warning: shape name not standard.')

im = Image.open(filename).convert('RGB')
aspect = im.height / im.width

# Check that desired resizing is valid
if width <= 1 or aspect * width <= 1:
    print('Desired image width was <= 1 or would result in an image with height <= 1.')
    exit(1)

# Resize the image
if not args.no_resize:
    im.thumbnail((width, int(width * aspect)), Image.ANTIALIAS)
w = im.width
h = im.height
arr = im.getdata()

# Set the image scale to fit within the default window bounds
scalef = 1.0 / max(w, h)

# Print the beginning of the scenegraph; we have three lights, one behind the camera,
# one behind the image, and one at the origin. Our camera is facing the XY plane,
# and is moved back 10 units in Z
print('<scenefile>\n')
print("""    <globaldata>
        <diffusecoeff v="1"/>
        <specularcoeff v="1"/>
        <ambientcoeff v="1"/>
    </globaldata>

    <lightdata>
        <id v="0"/>
        <color r="1" g="1" b="1"/>
        <position x="0" y="0" z="20"/>
    </lightdata>
    <lightdata>
        <id v="1"/>
        <color r="1" g="1" b="1"/>
        <position x="0" y="0" z="0"/>
    </lightdata>
    <lightdata>
        <id v="2"/>
        <color r="1" g="1" b="1"/>
        <position x="0" y="0" z="-20"/>
    </lightdata>

    <cameradata>
        <pos x="0" y="0.1" z="2"/>
        <focus x="0" y="0" z="0"/>
        <up x="0" y="1" z="0"/>
        <heightangle v="45"/>
    </cameradata>""")

# Print the beginning of our object tree
print('    <object type="tree" name="root">')
print('        <transblock>')

# Optionally rotate the image to get a better view of the side
print('            <rotate x="0" y="1" z="0" angle="{0:.5f}" />'.format(angley))
print('            <rotate x="1" y="0" z="0" angle="{0:.5f}" />'.format(anglex))

# Create a sub-tree for the pixel values
print('            <object type="tree">')
# Iterate through pixels in the image, one row at a time in a flattened list
for i, pix in enumerate(arr):
    # Place the shape for this pixel at its coordinates, offset to it's centered
    px = ((i % w) - w // 2) * scalef
    py = (h // 2 - (i // w)) * scalef
    # Scale RGB values to be in [0, 1]
    r = pix[0] / 255
    g = pix[1] / 255
    b = pix[2] / 255
    # We scale in the Z-direction by br, the brightness value of the pixel, if we
    # want to extrude the pixel
    if extrude:
        br = (r + g + b) / 3 * exc * scalef
        if invert:
            # exc * scalef is the max value br can take
            br = exc * scalef - br
    else:
        # If we don't extrude, each pixel becomes scaled equally in all directions
        br = scalef

    # Output the transblock for this pixel
    print('                <transblock>')
    # Scale the X and Y of the pixel to be a reasonable size
    print('                    <translate x="{0:.5f}" y="{1:.5f}" z="0"/>'.format(px, py))
    print('                    <scale x="{0:.5f}" y="{0:.5f}" z="1"/>'.format(scalef))
    # Move the pixel based on px and py, to a place in the world's XY plane
    # Scale in Z by our extrusion
    print('                    <scale x="1" y="1" z="{0:.5f}"/>'.format(br))
    # Turn the solid so its top face us towards us (nice for cylinders, cones, etc.)
    print('                    <rotate x="1" y="1" z="1" angle="90" />')
    if (noise):
        print('                    <rotate x="{}" y="{}" z="0" angle="{}" />'.format(random.normal(1, 0.1), random.normal(1, 0.1), random.normal(0, 15)))
    # Draw the solid with its color
    print('                    <object type="primitive" name="{}">'.format(shape))
    print('                        <diffuse r="{0:.5f}" g="{1:.5f}" b="{2:.5f}"/>'.format(r, g, b))
    print('                    </object>')
    print('                </transblock>')

# End the scene file
print('                </object>')
print('        </transblock>')
print('    </object>')
print('</scenefile>')

import os, sys, argparse
from PIL import Image

parser = argparse.ArgumentParser()
parser.add_argument('--indir',   type=str, required=True, help='Plot directory.')
parser.add_argument('--lookfor', type=str, required=True, help='Look for these common elements.')
parser.add_argument('--out',     type=str, required=True, help='Output file name.')
parser.add_argument('--col',     type=int, required=True, help='How many columns?')
args = parser.parse_args()

images = []
widths = []
heights = []

for fname in sorted(os.listdir(args.indir)):
    if args.lookfor in fname and fname.lower().endswith(('.png', '.jpg', '.jpeg')):
        path = os.path.join(args.indir, fname)
        img = Image.open(path)
        images.append(img)
        widths.append(img.width)
        heights.append(img.height)

if not images:
    print('\033[31mNo matching images found.\033[0m')
    sys.exit(1)

cols = args.col
rows = (len(images) + cols - 1) // cols
grid_width = max(widths) * cols
grid_height = max(heights) * rows
grid_image = Image.new('RGB', (grid_width, grid_height), (255, 255, 255))

for idx, img in enumerate(images):
    x_offset = (idx % cols) * max(widths)
    y_offset = (idx // cols) * max(heights)
    grid_image.paste(img, (x_offset, y_offset))

outfile = os.path.join(args.indir, args.out)
if not outfile.endswith('.png'): outfile += '.png'
grid_image.save(outfile)
print(f'Saved: {outfile}')

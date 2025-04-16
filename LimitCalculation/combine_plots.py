import os, sys
import argparse
from PIL import Image

parser = argparse.ArgumentParser()
parser.add_argument('--indir', type=str, required=True, help='Plot directory.')
parser.add_argument('--out',   type=str, required=True, help='Output image name.')
parser.add_argument('--model', type=str, required=True, help='mu/ele')
parser.add_argument('--test', action='store_true')
parser.add_argument('--dryrun', action='store_true')
args = parser.parse_args()
indir = args.indir
outname = args.out
model = args.model

#----------------------------------------------------------------------
campaigns = ['2018_UL', '2017_UL', '2016postVFP_UL', '2016preVFP_UL']
channels = ['mm', 'me', 'em', 'ee']
#----------------------------------------------------------------------

figures = [f for f in os.listdir(indir) if f.endswith('.png') and model in f]

outdir = os.path.join('plots', 'combined')

modelname = ''
if 'mu'  in model: modelname = 'VLLD_mu'
if 'ele' in model: modelname = 'VLLD_ele'
if not args.dryrun: os.makedirs(outdir, exist_ok=True)
if not outname.endswith('.png'): outname = f'{outname}.png'
if not modelname in outname: outname = f'{modelname}_{outname}'
if not outname.startswith('combined_'): outname = f'combined_{outname}'


outfile = os.path.join(outdir, outname)

images = {}
combined_files = []

for campaign in campaigns:
    for channel in channels:
        for filename in figures:
            if campaign in filename and channel in filename:
                images[(campaign, channel)] = Image.open(os.path.join(indir, filename))
                combined_files.append(filename)
                break

if combined_files:
    print("Combining:")
    for idx, file in enumerate(combined_files, 1):
        print(f"{idx}. {file}")

if not images:
    print("Error: No images found to combine.")
    sys.exit(1)

rows = [c for c in campaigns if any((c, ch) in images for ch in channels)]
cols = [ch for ch in channels if any((c, ch) in images for c in campaigns)]

img_width, img_height = next(iter(images.values())).size
padding = 10
stitched_img = Image.new('RGB', 
                         (img_width * len(cols) + padding * (len(cols) - 1), 
                          img_height * len(rows) + padding * (len(rows) - 1)), 
                         (255, 255, 255))

for i, row in enumerate(rows):
    for j, col in enumerate(cols):
        img = images.get((row, col))
        if img:
            x_offset = j * (img_width + padding)
            y_offset = i * (img_height + padding)
            stitched_img.paste(img, (x_offset, y_offset))

if not args.dryrun: stitched_img.save(outfile)
print(f"Saved: {outfile} ({len(rows)} rows x {len(cols)} cols)")

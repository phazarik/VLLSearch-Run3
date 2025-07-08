import os, sys, argparse
from PIL import Image
from math import ceil, sqrt

parser = argparse.ArgumentParser()
parser.add_argument('--plotname', type=str, required=True, help='Plot file name to search for (e.g., "efficiency.png").')
parser.add_argument('--find',     type=str, required=True, help='Directory to search within.')
parser.add_argument('--name',     type=str, required=False, help='Output name tag without extension.')
parser.add_argument('--relaxgrid', action='store_true', help='Arrange images in a square‑like grid')
parser.add_argument('--test',     action='store_true', help='Run once.')
parser.add_argument('--dryrun',   action='store_true', help='Dryrun.')
args = parser.parse_args()

search_key = args.find
plotname   = args.plotname

#campaigns = ["2018_UL", "2017_UL", "2016postVFP_UL", "2016preVFP_UL"]
#campaigns = ["Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23EE"]
campaigns = ["Run3Summer22EE"]
channels  = ["mm", "me", "em", "ee"]
#channels = ["mm"]

parent_dir = search_key.rstrip('/')
matching_dirs = []
for subdir in os.listdir(parent_dir):
    full_path = os.path.join(parent_dir, subdir)
    if not os.path.isdir(full_path):                 # remove partial_key check
        continue
    matched_campaign = next((c for c in campaigns if c in subdir), None)
    matched_channel  = next((ch for ch in channels if subdir.endswith("_" + ch)), None)
    if matched_campaign and matched_channel:
        matching_dirs.append(((matched_campaign, matched_channel), full_path))

if not matching_dirs:
    print(f"\033[31mNo matching directories found for partial search key: {partial_key}\033[0m")
    sys.exit(1)

ordered_dirs = []
for c in campaigns:
    for ch in channels:
        for (camp, path) in matching_dirs:
            if camp[0] == c and camp[1] == ch:
                ordered_dirs.append(path)

images = []
count = 0
print('\n==> Combining the following images:')
for d in ordered_dirs:
    img_path = os.path.join(d, plotname + '.png')
    if os.path.exists(img_path):
        images.append(Image.open(img_path))
        count +=1
        print(f'{count}. {img_path}')
        if args.test: break

if not args.dryrun and images:
    rows = len(campaigns)
    cols = len(channels)
    if args.relaxgrid:
        cols = ceil(sqrt(len(images)))
        rows = ceil(len(images) / cols)

    w, h = images[0].size

    canvas = Image.new('RGB', (cols * w, rows * h), 'white')

    if args.relaxgrid:
        for idx, img in enumerate(images):
            r, c = divmod(idx, cols)
            canvas.paste(img, (c * w, r * h))
    else:
        for row_idx, camp in enumerate(campaigns):
            for col_idx, ch in enumerate(channels):
                idx = row_idx * cols + col_idx
                if idx >= len(images): continue
                img = images[idx]
                x = col_idx * w
                y = row_idx * h
                canvas.paste(img, (x, y))

    out_dir = os.path.join("plots", "combined")
    os.makedirs(out_dir, exist_ok=True)
    out_name = f"combined_{os.path.basename(search_key.strip('/'))}_{plotname}.png"
    if args.name: out_name = f"combined_{args.name}_{plotname}.png"
    out_path = os.path.join(out_dir, out_name)
    canvas.save(out_path)
    print(f"\nOutput: \033[93m{out_path}\033[0m ({rows} rows x {cols} cols)")

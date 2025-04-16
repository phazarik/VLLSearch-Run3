import os, sys, argparse
from PIL import Image
from math import ceil, sqrt

parser = argparse.ArgumentParser()
parser.add_argument('--plotname', type=str, required=True, help='Plot file name to search for (e.g., "efficiency.png").')
parser.add_argument('--find',     type=str, required=True, help='Directory to search within.')
parser.add_argument('--test',     action='store_true', required=False, help='Run once.')
parser.add_argument('--dryrun',   action='store_true', required=False, help='Dryrun.')
args = parser.parse_args()

search_key = args.find
plotname   = args.plotname

#campaigns = ["2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL"]
campaigns = ["2018_UL", "2017_UL"]
channels  = ["mm", "me", "em", "ee"]

parent_dir = os.path.dirname(search_key.rstrip('/'))
partial_key = os.path.basename(search_key.rstrip('/'))

matching_dirs = []
for subdir in os.listdir(parent_dir):
    full_path = os.path.join(parent_dir, subdir)
    if not os.path.isdir(full_path) or partial_key not in subdir: continue

    matched_campaign = next((c for c in campaigns if c in subdir), None)
    matched_channel  = next((ch for ch in channels if subdir.endswith("_" + ch)), None)
    
    if matched_campaign and matched_channel:
        matching_dirs.append(((matched_campaign, matched_channel), full_path))

if not matching_dirs:
    print(f"No matching directories found for partial search key: {partial_key}")
    sys.exit(1)

ordered_dirs = []
for c in campaigns:
    for ch in channels:
        for (camp, path) in matching_dirs:
            if camp[0] == c and camp[1] == ch:
                ordered_dirs.append(path)

images = []
count = 0
print('\nCombining the following images:')
for d in ordered_dirs:
    img_path = os.path.join(d, plotname + '.png')
    if os.path.exists(img_path):
        images.append(Image.open(img_path))
        count +=1
        print(f'{count}. {img_path}')
        if args.test: break

if not args.dryrun and images:
    n = len(images)
    cols = ceil(sqrt(n))
    rows = ceil(n / cols)
    
    w, h = images[0].size
    canvas = Image.new('RGB', (cols * w, rows * h), 'white')

    for idx, img in enumerate(images):
        x = (idx % cols) * w
        y = (idx // cols) * h
        canvas.paste(img, (x, y))

    out_dir = os.path.join("plots", "combined")
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, f"combined_{os.path.basename(search_key.strip('/'))}_{plotname}.png")
    canvas.save(out_path)
    print(f"\nSaved: {out_path}")

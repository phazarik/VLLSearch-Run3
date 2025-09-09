import os
from PIL import Image

indir = "plots"
outname = "plots/summary.png"

campaigns = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix"
]
channels = ["mm", "me", "em", "ee"]

## Maintaining order
images = []
for camp in campaigns:
    row = []
    for ch in channels:
        fname = f"{camp}_{ch}.png"
        fpath = os.path.join(indir, fname)
        if not os.path.exists(fpath):
            raise FileNotFoundError(f"Missing file: {fpath}")
        row.append(Image.open(fpath))
    images.append(row)

## Assuming all images have the same size
w, h = images[0][0].size
stitched_w = w * len(channels)
stitched_h = h * len(campaigns)
stitched = Image.new("RGB", (stitched_w, stitched_h), (255, 255, 255))
for i, row in enumerate(images):
    for j, img in enumerate(row):
        stitched.paste(img, (j * w, i * h))

stitched.save(outname)
print(f"Created: {outname}")

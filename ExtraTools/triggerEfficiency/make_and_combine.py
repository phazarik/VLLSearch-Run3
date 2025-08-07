import os
from PIL import Image
import subprocess

output_dir = "plots"
os.makedirs(output_dir, exist_ok=True)

campaigns = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix"
]

triggers = ["muon", "electron"]
image_size = (500, 500)  # ROOT default canvas size
created_images = []

# 'Creating plots first.
for campaign in campaigns:
    #cmd = f"root -q -b -l 'fit.C(\"{campaign}\")'"
    cmd = f"root -q -b -l 'fit.C(\"{campaign}\", false, false)'"
    print(f"Running: {cmd}")
    subprocess.run(cmd, shell=True)

print("\nThe following images were created:")
for img in created_images: print(f" - {img}")

#Stiching images:
for trigger in triggers:
    rows = []
    for i in range(0, 8, 4):
        row_imgs = []
        for campaign in campaigns[i:i+4]:
            path = f"{output_dir}/trigeff_{campaign}_{trigger}.png"
            if os.path.exists(path):
                img = Image.open(path)
            else:
                img = Image.new("RGB", image_size, color="white")
            row_imgs.append(img)

        row_width = sum(img.width for img in row_imgs)
        row_height = max(img.height for img in row_imgs)
        row_combined = Image.new("RGB", (row_width, row_height))
        x_offset = 0
        for img in row_imgs:
            row_combined.paste(img, (x_offset, 0))
            x_offset += img.width
        rows.append(row_combined)

    final_width = max(row.width for row in rows)
    final_height = sum(row.height for row in rows)
    final_image = Image.new("RGB", (final_width, final_height))

    y_offset = 0
    for row in rows:
        final_image.paste(row, (0, y_offset))
        y_offset += row.height

    output_path = f"{output_dir}/combined_{trigger}_triggers.png"
    final_image.save(output_path)
    print(f"\nStitched: {output_path}")

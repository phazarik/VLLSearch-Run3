import os, sys
import argparse
from PIL import Image, ImageDraw, ImageFont
import svgutils.compose as sc
from svgutils.transform import fromfile
from svgutils.transform import fromfile, SVGFigure, GroupElement, TextElement
from cairosvg import svg2png

parser = argparse.ArgumentParser()
parser.add_argument('--indir', type=str, required=True, help='Plot directory.')
parser.add_argument('--out',   type=str, required=True, help='Output image name.')
parser.add_argument('--model', type=str, required=True, help='mu/ele')
parser.add_argument('--svg', action='store_true', help='Enable SVG mode.')
parser.add_argument('--test', action='store_true')
parser.add_argument('--dryrun', action='store_true')
args    = parser.parse_args()
indir   = args.indir
outname = args.out
model   = args.model
svg     = args.svg


#----------------------------------------------------------------------
#campaigns = ['2018_UL', '2017_UL', '2016postVFP_UL', '2016preVFP_UL']
campaigns = ['Run3Summer22', 'Run3Summer22EE']
channels = ['mm', 'me', 'em', 'ee']
#----------------------------------------------------------------------

ext = '.svg' if svg else '.png'
figures = [f for f in os.listdir(indir) if f.endswith(ext) and model in f]

outdir = os.path.join('plots', 'combined')

modelname = ''
if 'mu'  in model: modelname = 'VLLD_mu'
if 'ele' in model: modelname = 'VLLD_ele'
if not args.dryrun: os.makedirs(outdir, exist_ok=True)
if not outname.endswith(ext): outname = f'{outname}{ext}'
if not modelname in outname: outname = f'{modelname}_{outname}'
if not outname.startswith('combined_'): outname = f'combined_{outname}'

outfile = os.path.join(outdir, outname)

images = {}
combined_files = []

'''
for campaign in campaigns:
    for channel in channels:
        for filename in figures:
            if 'Run3' in campaign:
                name_parts = filename.replace(ext, '').split('_')
                if len(name_parts) >= 5 and name_parts[3] == campaign and name_parts[4] == channel:
                    if svg: images[(campaign, channel)] = fromfile(os.path.join(indir, filename)).getroot()
                    else: images[(campaign, channel)] = Image.open(os.path.join(indir, filename))
                    combined_files.append(filename)
                    break
            else:
                if campaign in filename and channel in filename:
                    if svg: images[(campaign, channel)] = fromfile(os.path.join(indir, filename)).getroot()
                    else: images[(campaign, channel)] = Image.open(os.path.join(indir, filename))
                    combined_files.append(filename)
                    break
'''

for campaign in campaigns:
    for channel in channels:
        match_found = False
        for filename in figures:
            name_parts = filename.replace(ext, '').split('_')
            if 'VLLD' in name_parts and modelname.split('_')[1] in name_parts and name_parts[-1] == channel:
                if 'Run3' in campaign:
                    if name_parts[-2] == campaign:
                        match_found = True
                        break
                else:
                    if len(name_parts) >= 6 and name_parts[3] + '_' + name_parts[4] == campaign:
                        match_found = True
                        break
        if match_found:
            filepath = os.path.join(indir, filename)
            if svg: images[(campaign, channel)] = fromfile(filepath).getroot()
            else:  images[(campaign, channel)] = Image.open(filepath)
            combined_files.append(filename)
        else:
            print(f"Missing: {modelname}_{campaign}_{channel}{ext}")
                    
if combined_files:
    print("Combining:")
    for idx, file in enumerate(combined_files, 1):
        print(f"{idx}. {file}")

'''
if not images:
    print("Error: No images found to combine.")
    sys.exit(1)
'''
if not combined_files:
    print("Error: No matching files found in directory.")
    sys.exit(1)
    
rows = campaigns
cols = channels
if svg: img_width, img_height = 565, 540
else:   img_width, img_height = next(iter(images.values())).size
padding = 10
header_height = 50
sidebar_width = 100

#Save svg image:
if svg:
    layout = []
    for i, row in enumerate(rows):
        for j, col in enumerate(cols):
            x = j * img_width
            y = i * img_height
            img = images.get((row, col))
            if img:
                img.moveto(j * img_width, i * img_height)
                layout.append(img)
            else:
                group = GroupElement([])
                group.moveto(x, y)
                layout.append(group)

    figure_width = img_width * len(cols)
    figure_height = img_height * len(rows)
    if not args.dryrun:
        sc.Figure(f"{figure_width}px", f"{figure_height}px", *layout).save(outfile)
        png_outfile = outfile+'.png'
        svg2png(url=outfile, write_to=png_outfile)
        os.remove(outfile)

#Save png image:
else:
    stitched_img = Image.new('RGB', 
                             (img_width * len(cols) + padding * (len(cols) - 1), 
                              img_height * len(rows) + padding * (len(rows) - 1)), 
                             (255, 255, 255))
    draw = ImageDraw.Draw(stitched_img)
    try:    font = ImageFont.truetype("/mnt/c/Windows/Fonts/Calibri.ttf", 60)
    except: font = ImageFont.load_default()
    for i, row in enumerate(rows):
        for j, col in enumerate(cols):
            img = images.get((row, col))
            x_offset = j * (img_width + padding)
            y_offset = i * (img_height + padding)
            if img:
                stitched_img.paste(img, (x_offset, y_offset))
            else:
                draw.text((x_offset + img_width // 2, y_offset + img_height // 2), 
                          "No yield", 
                          fill=(255, 0, 0), 
                          font=font, 
                          anchor="mm")
    if not args.dryrun: stitched_img.save(outfile)

print(f"Saved: {outfile} ({len(rows)} rows x {len(cols)} cols)")

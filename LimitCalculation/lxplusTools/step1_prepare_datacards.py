import os
shape_zip = "shapes.tar.gz"
os.makedirs("shapes", exist_ok=True) ## Make sure the shapes directory exists
os.system("rm -rf shapes/*")         ## remove existing shapes
os.system(f"tar -xzvf {shape_zip}")

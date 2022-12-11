from PIL import Image, ImageDraw, ImageFont, ImageFilter, ImageOps, ImageChops
import shutil
import os
import random
import hashlib
import base64
import sys


def enhance(R):
    files = os.listdir("./output/")
    l = (R+1) * len(files)
    i = 1
    for f in files:
        print(i,"/",l)
        og = Image.open("./output/" + f)
        mod = og.copy()
        filename = hashlib.sha256(str(random.getrandbits(256)).encode('utf-8')).hexdigest()[:8]
        fname = f[0]+"__"+filename+".bmp"
        mod.save("./noise/" + fname, format="BMP")
        i += 1
    for r in range(R):
        for f in files:
            print(i,"/",l)
            randrot = random.randrange(-10, 10)
            randoff = random.randrange(-2, 2)
            og = Image.open("./output/" + f)
            mod = og.copy()
            mod = mod.rotate(randrot, fillcolor=0)
            mod = ImageChops.offset(mod, randoff)
            filename = hashlib.sha256(str(random.getrandbits(256)).encode('utf-8')).hexdigest()[:8]
            fname = f[0]+"__"+filename+".bmp"
            mod.save("./noise/" + fname, format="BMP")
            i += 1

def to_dir(path):
    files = os.listdir(path)
    labels = open(path+"labels.nrl")
    mapper = []
    for l in labels:
        mapper.append(l.strip())
    print(mapper)
    for f in files:
        if f != "labels.nrl":
            y = int(f[0]) - 1
            x = int(f[2]) - 1 
            filename = hashlib.sha256(str(random.getrandbits(256)).encode('utf-8')).hexdigest()[:8]
            final = "./output/" + mapper[y * 9 + x] + "__" + filename + ".png"
            shutil.copyfile(path+f, final)

def equalize():
    files = os.listdir("./output")
    histo = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    for f in files:
        idx = int(f[0])
        histo[idx] += 1
    print(histo)
   # lower = min(histo)
   # to_delete = [ x - lower for x in histo ]
   # print(to_delete)
   # for f in files:
   #     idx = int(f[0])
   #     if to_delete[idx] != 0:
   #         os.remove("./output/"+f)
   #         to_delete[idx] -= 1

if __name__ == "__main__":
    if sys.argv[1] == 'eq':
        equalize()
    elif sys.argv[1] == 't':
        to_dir(sys.argv[2])
    elif sys.argv[1] == 'e':
        # shutil.rmtree("./noise")
        # os.mkdir("./noise")
        enhance(int(sys.argv[2]))

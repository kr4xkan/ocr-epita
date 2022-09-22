from PIL import Image, ImageDraw, ImageFont, ImageFilter
import shutil
import os
import random
import hashlib
import base64

WHITE = (255,255,255,255)
DISPLACEMENT = 2
RANDOM_NOISE = 30

width = 28
height = 28

fonts = ["Gidole-Regular.ttf", "Lato-Regular.ttf", "Roboto-Regular.ttf", "AbhayaLibre-Regular.ttf", "PTSerif-Regular.ttf"]
lenfont = len(fonts)

def create_image(num, i):
    img = Image.new(mode = "RGB", size= (width, height), color=WHITE)

    rotate = random.randrange(-20, 20);
    img = img.rotate(rotate, fillcolor=WHITE)

    draw = ImageDraw.Draw(img)

    d_x = random.randrange(-DISPLACEMENT, DISPLACEMENT)
    d_y = random.randrange(-DISPLACEMENT, DISPLACEMENT)

    fontname = fonts[random.randrange(0, lenfont)]
    font = ImageFont.truetype(fontname, 24)
    draw.text((width/2+d_x,height/2+d_y), str(num), font=font, anchor="mm", fill=(0,0,0,0))

    img = img.rotate(-rotate, fillcolor=WHITE)
    
    for k in range(RANDOM_NOISE):
        x = random.randrange(0, width)
        y = random.randrange(0, height)
        grey = random.randrange(100, 255)
        img.putpixel((x,y), (grey, grey, grey, 255))

    blur = img.filter(ImageFilter.GaussianBlur(0.4))
    filename = hashlib.sha256(str(random.getrandbits(256)).encode('utf-8')).hexdigest()[:8]
    blur.save("./output/"+str(num)+"__"+filename+".bmp", format="BMP")

shutil.rmtree("./output")
os.mkdir("./output")

for i in range(1000):
    create_image(random.randrange(0, 10), i)

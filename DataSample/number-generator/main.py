from PIL import Image, ImageDraw, ImageFont, ImageFilter
import shutil
import os
import random
import hashlib
import base64
import sys

WHITE = (255,255,255,255)
DISPLACEMENT = 4
RANDOM_NOISE = 30

width = 28
height = 28

fonts = []
dir_list = os.listdir('./fonts')
for f in dir_list:
    fonts.append('./fonts/'+f)

lenfont = len(fonts)

def create_image(num, i):
    rotate = random.randrange(-20, 20)
    col_rand = random.randrange(200, 255)
    coltext_rand = random.randrange(0, 90)
    greycol = (col_rand, col_rand, col_rand, 255)
    greycoltext = (coltext_rand, coltext_rand, coltext_rand, 255)

    img = Image.new(mode = "RGB", size= (width, height), color=greycol)
    img = img.rotate(rotate, fillcolor=greycol)

    draw = ImageDraw.Draw(img)

    for k in range(RANDOM_NOISE):
        x = random.randrange(0, width)
        y = random.randrange(0, height)
        grey = random.randrange(100, 255)
        x_size = random.randrange(1, 3)
        y_size = random.randrange(1, 3)
        draw.rectangle((x,y,x+x_size,y+y_size), fill=(grey, grey, grey, 255))
    
    imgx = img.filter(ImageFilter.GaussianBlur(0.8))
    drawx = ImageDraw.Draw(imgx)

    d_x = random.randrange(-DISPLACEMENT, DISPLACEMENT)
    d_y = random.randrange(-DISPLACEMENT, DISPLACEMENT)

    fontname = fonts[random.randrange(0, lenfont)]
    font = ImageFont.truetype(fontname, 24)
    drawx.text((width/2+d_x,height/2+d_y), str(num), font=font, anchor="mm", fill=greycoltext)

    for k in range(RANDOM_NOISE):
        x = random.randrange(0, width)
        y = random.randrange(0, height)
        grey = random.randrange(100, 255)
        imgx.putpixel((x,y), (grey, grey, grey, 255))
    
    imgx = imgx.rotate(-rotate, fillcolor=greycol)

    blur = imgx.filter(ImageFilter.GaussianBlur(0.4))
    filename = hashlib.sha256(str(random.getrandbits(256)).encode('utf-8')).hexdigest()[:8]
    blur.save("./output/"+str(num)+"__"+filename+".bmp", format="BMP")

if __name__ == "__main__":
    shutil.rmtree("./output")
    os.mkdir("./output")

    for i in range(int(sys.argv[1])):
        create_image(random.randrange(0, 10), i)

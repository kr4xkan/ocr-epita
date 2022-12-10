from PIL import Image, ImageDraw, ImageFont, ImageFilter
import shutil
import os
import random
import hashlib
import base64
import sys

BLACK = (255,255,255,255)
WHITE = (0,0,0,255)
DISPLACEMENT = 2
RANDOM_NOISE = 5

width = 28
height = 28

fonts = []
dir_list = os.listdir('./fonts')
for f in dir_list:
    fonts.append('./fonts/'+f)

lenfont = len(fonts)
def binarize(img):

    thresh=200

    img=img.convert('L') 

    width,height=img.size
    for x in range(width):
        for y in range(height):
            if img.getpixel((x,y)) < thresh:
                img.putpixel((x,y),0)
            else:
                img.putpixel((x,y),255)
    return img

def create_image(num, i):
    rotate = random.randrange(-10, 10)
    col_rand = random.randrange(200, 255)
    coltext_rand = random.randrange(0, 90)
    greycol = (255, 255, 255, 255)
    greycoltext = (0, 0, 0, 255)

    img = Image.new(mode = "RGB", size= (width, height), color=WHITE)
    img = img.rotate(rotate, fillcolor=WHITE)

    draw = ImageDraw.Draw(img)

    for k in range(random.randrange(0, 2)):
        x = random.randrange(0, width)
        y = random.randrange(0, height)
        x_size = random.randrange(1, 2)
        y_size = random.randrange(1, 2)
        draw.rectangle((x,y,x+x_size,y+y_size), fill=BLACK)
    
    imgx = img.filter(ImageFilter.GaussianBlur(0.1))
    drawx = ImageDraw.Draw(imgx)

    d_x = random.randrange(-DISPLACEMENT, DISPLACEMENT)
    d_y = random.randrange(-DISPLACEMENT, DISPLACEMENT)

    if num != 0:
        fontname = fonts[random.randrange(0, lenfont)]
        font = ImageFont.truetype(fontname, random.randrange(10, 24))
        drawx.text((width/2+d_x,height/2+d_y), str(num), font=font, anchor="mm", fill=BLACK)

    imgx = imgx.rotate(-rotate, fillcolor=WHITE)

    blur = imgx.filter(ImageFilter.GaussianBlur(0.1))
    
    bin = binarize(blur)
    filename = hashlib.sha256(str(random.getrandbits(256)).encode('utf-8')).hexdigest()[:8]
    bin.save("./output/"+str(num)+"__"+filename+".bmp", format="BMP")

if __name__ == "__main__":
    shutil.rmtree("./output")
    os.mkdir("./output")

    for i in range(int(sys.argv[1])):
        print(i, "/", sys.argv[1])
        create_image(random.randrange(0, 10), i)

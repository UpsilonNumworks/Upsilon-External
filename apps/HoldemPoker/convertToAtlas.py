from PIL import Image
from imageToObm import * 


HEIGHT = 75
obm = True

def scaleImage(img, height):
    method = Image.LANCZOS
    scale = height / img.height
    new_size = (int(img.width * scale), height)
    return img.resize(new_size, method)


def sumHeights(imgs):
    s = 0
    for el in imgs:
        s += el.height
    return s

def createAtlas(imgs, color):
    savePathAtlas = "./imageSource/atlas/"
    headerPath = "./inc/textures/"
    height =  sumHeights(imgs)
    width = imgs[0].width
    imgHeight = imgs[0].height
    atlas = Image.new('RGBA', (width, height))
    for i in range(0, len(imgs)):
        atlas.paste(imgs[i], (0, i * imgHeight))
    atlas.save(savePathAtlas + color + "-atlas.png")
    
    if obm: png_to_obm_header(savePathAtlas + color + "-atlas.png", headerPath + color + "-atlas.h", color + "_atlas")


def createTextureAtlas(color):
    imgs = [] 
    ext = ".png"
    path = "./imageSource/cards/"
    separator = "-"
    cards = ["2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"]
    for el in cards:
        
        img = Image.open(path + el + separator + color + ext)
        img = scaleImage(img, HEIGHT)
        imgs.append(img)
    createAtlas(imgs, color)


createTextureAtlas("H")
createTextureAtlas("C")
createTextureAtlas("D")
createTextureAtlas("S")
png_to_obm_header("./imageSource/cards/card-deck.png", "inc/textures/card-deck.h", "card_deck")
png_to_obm_header("./imageSource/cards/card-back.png", "inc/textures/card-back.h", "card_back")
png_to_obm_header("./imageSource/cards/card-back-medium.png", "inc/textures/card-back-med.h", "card_back_med")
png_to_obm_header("./imageSource/cards/card-flip.png", "inc/textures/card-flip.h", "card_flip")

png_to_obm_header("./imageSource/tokens/blue-tokens.png", "inc/textures/blue-tokens.h", "blue_tokens")
png_to_obm_header("./imageSource/tokens/red-tokens.png", "inc/textures/red-tokens.h", "red_tokens")
png_to_obm_header("./imageSource/tokens/green-tokens.png", "inc/textures/green-tokens.h", "green_tokens")
png_to_obm_header("./imageSource/tokens/black-tokens.png", "inc/textures/black-tokens.h", "black_tokens")

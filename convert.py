#!/usr/bin/env python3
from PIL import Image
import sys

def main ():
    Image.MAX_IMAGE_PIXELS = 10000000000
    print ("Opening image")
    im = Image.open ("mandel.bmp")
    print ("Saving image")
    im.save ("./images/" + sys.argv [1])
    print ("Finished.")

if __name__ == "__main__":
    main ()

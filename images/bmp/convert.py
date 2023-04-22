#!/usr/bin/env python3
from PIL import Image
import sys
import os

def main ():
    Image.MAX_IMAGE_PIXELS = 10000000000
    for name in os.listdir ():
        if name.endswith (".bmp"):
            #Open image
            print ("Opening image", name, "...")
            im = Image.open (name)
            #Save image
            savename = name [:len (name) - 4] + ".png"
            print ("Saving image", savename, "...")
            im.save ("../" + savename)
            #Delete original
            print ("Deleting image", name, "...")
            os.remove (name)
            print ()
    print ("Finished.")

if __name__ == "__main__":
    main ()

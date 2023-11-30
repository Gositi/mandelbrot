#!/usr/bin/env python3

import os

def main ():
    saveConfig (getConfig ())

def getConfig ():
    names = ["aspect ratio", "image width", "max iterations", "zoom", "focusx", "focusy", "scalematch", "threadcount", "smooth colors", "gray colors", "dark colors"]
    if not os.path.exists("config"):
        open("config", 'w').close()
    with open ("config", "r") as file:
        config = file.readlines ()
    while len (config) < len (names):
        config.append ("blank")
    for i in range (len (names)):
        value = config [i].rstrip ()
        inp = input ("New value for " + names [i] + " (" + value + ")? Leave blank to keep current value. ")
        #TODO validate inputs
        if len (inp) != 0:
            value = str (eval (inp))
        config [i] = value
        print (config [i])
    return config

def saveConfig (config):
    with open ("config", "w") as file:
        for value in config:
            file.write (value + "\n")

if __name__ == "__main__":
    main ()

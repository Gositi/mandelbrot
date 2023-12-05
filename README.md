# mandelbrot
A pretty fast mandelbrot renderer written in C.
Now threaded.

Credits for the libbmp library goes to <https://github.com/marc-q/libbmp>, with small changes to resolve compiler warnings.

## Features and limitations
Features:
- Threaded, can run on all CPU cores at once or just a single core, you decide
- Image in memory shared between threads, making it use less memory
- Mathematical optimization, it should detect cycles and stop iterating, providing a HUGE performance boost
- Config file/script makes running it with new configs easy
- Batteries included - converter to other image formats and list of nice parameters included

Limitations:
- CPU only, GPU would give it much greater speed at the expence of float precision
- Bad at detecting errors, for example at thread creation
- The implementation of threads is pretty damn ugly, using structs to hold structs
- The slowest thread decides the final runspeed, if a thread "gets stuck" you will have to wait for it

## Usage
For the Windows cross-compiled version, refer to the `windows` branch of the repo.

### Initial setup
The compiled code is for 64-bit Linux, it should work right out of the box on most systems and is tested on later versions of Ubuntu.
I'd recommend you compile it anyways, it's only a few hundred lines of code so it's really quick and easy.
You compile with `make mandelbrot`.
Running the program is then done by running the produced executable, `mandelbrot`.

### Config
Out of the box a simple 2k (1920x1080) config file is ready.
It can preferrably be used to test the renderer.
A file with other parameters that will yield nice-looking results is also included, `coords.txt`.
This file is somewhat outdated and contains references to "HP guys", the reason for which is that the ancestor of this program lives on my graphing calculator, made by HP.
The file itself is dated to the Python ancestor of this program, which was later ported to C for performance.

The program reads the config parameters from the `config` file, naturally.
Although you _could_ edit it manually, I'd reccommend you instead use the `config.py` Python script.
It accepts mathematical expressions as well as pure numbers.
You should keep the types (int, float, etc.) the same as the current values (but I'm not sure it's needed).
Booleans must be either a `1` for True, or `0` for False.

The `config.py` script is a heavy work-in-progress.
Thus it doesn't tell you much about the input it expects and it doesn't evaluate it either.
Some day I will get around to making it better, likely moving it to it's own project (as it already is in use in another program).

### Running the program
Running the program is, as mentioned above, not harder than typing `./mandelbrot` in your terminal of choice.
Each thread will print its progress every 100 columns, which should give you an idea of how long time is left.

The program will produce a file called `mandel.bmp`, because that was easiest.
A Python script for converting the image to a more sensible (preferrably `.png`) format is included.
It depends on Python Imaging Library/PIL/pillow, so you'll have to install that too.
The usage of the script is easy, you just provide the preferred new filename (with format extension) as a command-line-argument and the converted file will magically appear in `images` after a short while.
The converter script uses about twice as much memory as the size of the raw `mandel.bmp` though, which (speaking from experience, using 64GB of swap) can be a problem for very hi-res images.

## TODO
- Improve on the `config.py` script
- Add checking of return codes for errors
- Improve the `coords.txt` file
- Add some way of quickly loading preset configs (files in a directory?)

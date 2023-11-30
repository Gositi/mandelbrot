# mandelbrot
A mandelbrot renderer written in C.

Credits for the libbmp library goes to <https://github.com/marc-q/libbmp>, with small changes to resolve compiler warnings.

I have not had more time to fill this README in, but you can change the neccessary settings using the `config.py` script.
Note that expressions are supported, but booleans must be entered as `1` for `True` and `0` for `False`.
To convert the outputted `mandel.bmp` image, run `convert.py` with the new filename as argument.
The compiled code is for 64-bit linux (successfully ran on later versions of Ubuntu).

## Todo
* Fully test the config script
* Add some way of quickly loading preset configs (files in a directory?)
* Fill in this README (obviously)

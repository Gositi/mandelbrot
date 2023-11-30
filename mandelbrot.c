#include "libbmp.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

//Struct for rendering config data
typedef struct _cfg {
	double aspectRatio;
	int width;
	int maxiter;
	double zoom;
	double focusx;
	double focusy;
	int scalematch;
	bool smooth;
	bool gray;
	bool dark;
} cfg;

//Load config from file
cfg loadConfig () {
	cfg conf;
	int i;
	char d;
	FILE *fp = fopen ("config", "r");

	//aspectRatio
	i = fscanf (fp, "%lf\n", &conf.aspectRatio);
	if (i != 1) {fclose (fp); exit (1);}
	//width
	i = fscanf (fp, "%d\n", &conf.width);
	if (i != 1) {fclose (fp); exit (1);}
	//maxiter
	i = fscanf (fp, "%d\n", &conf.maxiter);
	if (i != 1) {fclose (fp); exit (1);}
	//zoom
	i = fscanf (fp, "%lf\n", &conf.zoom);
	if (i != 1) {fclose (fp); exit (1);}
	//focusx
	i = fscanf (fp, "%lf\n", &conf.focusx);
	if (i != 1) {fclose (fp); exit (1);}
	//focusy
	i = fscanf (fp, "%lf\n", &conf.focusy);
	if (i != 1) {fclose (fp); exit (1);}
	//scalematch
	i = fscanf (fp, "%d\n", &conf.scalematch);
	if (i != 1) {fclose (fp); exit (1);}
	//smooth
	i = fscanf (fp, "%c\n", &d);
	if (i != 1) {fclose (fp); exit (1);}
	if (d == '1') {conf.smooth = true;} else {conf.smooth = false;}
	//gray
	i = fscanf (fp, "%c\n", &d);
	if (i != 1) {fclose (fp); exit (1);}
	if (d == '1') {conf.gray = true;} else {conf.gray = false;}
	//dark
	i = fscanf (fp, "%c\n", &d);
	if (i != 1) {fclose (fp); exit (1);}
	if (d == '1') {conf.dark = true;} else {conf.dark = false;}

	fclose (fp);
	return conf;
}

void color (double i, int maxi, double scale, int rgb [], bool gray, bool dark) {
	double c = asin (1) * 4;
	double v = i * c / (maxi * scale);
	int r, g, b;

	//Grayscale coloring
	if (gray) {
		r = (-cos (v) + 1) * 255 / 2;
		g = r;
		b = r;
	} else {
		//Dark coloring scheme
		if (dark) {
			r = (-cos (v) * fabs (cos (v)) + 1) * 255 / 4;
			g = 0;
			b = (cos (v) * fabs (cos (v)) + 1) * 255 / 6;
		} else {
			//Normal coloring
			r = (sin (v) + 1) * 255 / 2;
			g = (sin (v - (c / 3)) + 1) * 255 / 2;
			b = (sin (v - (2 * c / 3)) + 1) * 255 / 2;
		}
	}

	rgb [0] = r;
	rgb [1] = g;
	rgb [2] = b;
}

double iterate (double xc, double yc, int maxiter, bool smooth) {
	//Declare variables	
	double xz, yz, oldxz, oldyz, tempxz, tempyz, out;
	int i, period;

	//Initialize variables
	xz = xc;
	yz = yc;
	oldxz = xz;
	oldyz = yz;

	i = 0;
	period = 4;

	//Iterate the point
	while (i <= maxiter && fabs ((xz * xz) + (yz * yz)) <= 4 + 124 * smooth) {
		tempxz = (xz * xz) - (yz * yz) + xc;
		tempyz = (2 * xz * yz) + yc;

		xz = tempxz;
		yz = tempyz;

		i++;

		if (xz == oldxz && yz == oldyz) {
			i = maxiter + 1;
		}

		if (i == period) {
			period <<= 1;
			oldxz = xz;
			oldyz = yz;
		}
	}

	//Correctly set smoothness
	if (i <= maxiter && smooth) {
		out = ((double) i) - ((log (log (sqrt ((xz * xz) + (yz * yz))))) / log (2.0));
	} else {
		out = (double) i;
	}

	return out;
}

void mandelbrot (cfg conf) {
	//Declare variables
	bmp_img img;
	int height, width;
	double yrange, xrange, minx, maxx, miny, maxy, colorscale, x, y, i;
	int rgb [] = {0, 0, 0};

	//Set control variables
	height = (int) (((double) conf.width) / conf.aspectRatio);
	width = conf.width;	//Used so much it's worth it

	yrange = 1 / conf.zoom;
	xrange = yrange * conf.aspectRatio;

	minx = conf.focusx - xrange / 2;
	maxx = conf.focusx + xrange / 2;
	miny = conf.focusy + yrange / 2;
	maxy = conf.focusy - yrange / 2;

	colorscale = ((double) conf.scalematch) / ((double) conf.maxiter);

	//Create image
	bmp_img_init_df (&img, width, height);

	//Start rendering
	for (int xs = 0; xs < width; xs++) {
		x = (xs * (maxx - minx) / width) + minx; 
		for (int ys = 0; ys < height; ys++) {
			y = (ys * (maxy - miny) / height) + miny;

			i = iterate (x, y, conf.maxiter, conf.smooth);

			if (i < conf.maxiter + 1) {
				color (i, conf.maxiter, colorscale, rgb, conf.gray, conf.dark);
			} else {
				rgb [0] = 0;
				rgb [1] = 0;
				rgb [2] = 0;
			}

			bmp_pixel_init (&img.img_pixels[ys][xs], rgb [0], rgb [1], rgb [2]);
		}
		if (xs % 100 == 0) printf ("%d\n", xs);
	}

	//Save image
	bmp_img_write (&img, "mandel.bmp");
	bmp_img_free (&img);
}

int main () {
	//Load config from file
	cfg conf = loadConfig ();

	//Render the fractal
	mandelbrot (conf);

	//Exit with code 0
	return 0;
}

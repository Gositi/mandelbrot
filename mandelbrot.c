#include "libbmp.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define NUM_THREADS 3

//Struct for rendering config data
typedef struct _config {
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

//Struct for actual setup data, for passing to threads
typedef struct _setup {
	int width;
	int height;
	double minx;
	double maxx;
	double miny;
	double maxy;
	int maxiter;
	double colorscale;
	bool smooth;
	bool gray;
	bool dark;
	int module; //Number of threads
	bmp_img* img;
} stp;

//Struct for carrying data over to thread
//This is not a nice solution, but it at least works
typedef struct _data {
	stp* setup;
	int offset; //Offset to ensure threads aren't interfering
} data;
	
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

void *mandelWorker (void* args) {
	//Load setup
	data* carrier = args;
	stp* setup = carrier->setup;
	int offset = carrier->offset;
	
	//Unloading data from setup
	int width = setup->width;
	int height = setup->height;
	double minx = setup->minx;
	double maxx = setup->maxx;
	double miny = setup->miny;
	double maxy = setup->maxy;
	int maxiter = setup->maxiter;
	double colorscale = setup->colorscale;
	bool smooth = setup->smooth;
	bool gray = setup->gray;
	bool dark = setup->dark;
	int module = setup->module;
	bmp_img* img = setup->img;
	
	//Start rendering
	int rgb [] = {0, 0, 0};
	double x, y;
	double i;
	for (int xs = offset; xs < width; xs+=module) {
		x = (xs * (maxx - minx) / width) + minx; 
		for (int ys = 0; ys < height; ys++) {
			y = (ys * (maxy - miny) / height) + miny;

			i = iterate (x, y, maxiter, smooth);

			if (i < maxiter + 1) {
				color (i, maxiter, colorscale, rgb, gray, dark);
			} else {
				rgb [0] = 0;
				rgb [1] = 0;
				rgb [2] = 0;
			}

			bmp_pixel_init (&(img->img_pixels[ys][xs]), rgb [0], rgb [1], rgb [2]);
		}
		if ((xs / module) % 100 == 0) printf ("%d\t%d\n", offset, xs);
	}

	return NULL;
}

void mandelbrot (cfg conf) {
	//Declare variables
	bmp_img img;
	int height;
	double yrange, xrange, minx, maxx, miny, maxy, colorscale;

	//Set control variables
	height = (int) (((double) conf.width) / conf.aspectRatio);

	yrange = 1 / conf.zoom;
	xrange = yrange * conf.aspectRatio;

	minx = conf.focusx - xrange / 2;
	maxx = conf.focusx + xrange / 2;
	miny = conf.focusy + yrange / 2;
	maxy = conf.focusy - yrange / 2;

	colorscale = ((double) conf.scalematch) / ((double) conf.maxiter);

	//Create image
	bmp_img_init_df (&img, conf.width, height);

	//Create setup struct for data transfer
	stp setup;
	setup.width = conf.width;
	setup.height = height;
	setup.minx = minx;
	setup.maxx = maxx;
	setup.miny = miny;
	setup.maxy = maxy;
	setup.maxiter = conf.maxiter;
	setup.colorscale = colorscale;
	setup.smooth = conf.smooth;
	setup.gray = conf.gray;
	setup.dark = conf.dark;
	setup.module = NUM_THREADS;
	setup.img = &img;

	//Create threads
	pthread_t threads [NUM_THREADS];
	data* carriers [NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; i++) {
		carriers [i] = malloc (sizeof (data));
		(carriers [i])->setup = &setup;
		(carriers [i])->offset = i;
		pthread_create (&(threads [i]), NULL, mandelWorker, carriers [i]);
	}

	//Join threads
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join (threads [i], NULL);
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

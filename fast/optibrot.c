#include "libbmp.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

//Get the correct color for a pixel
void color (double i, int maxi, double scale, int rgb [], bool gray) {
	double c = asin (1) * 4;
	double v = i * c / (maxi * scale);
	int r, g, b;

	if (gray) {
		r = (-cos (v) + 1) * 255 / 2;
		g = r;
		b = r;
	} else {
		r = (sin (v) + 1) * 255 / 2;
		g = (sin (v - (c / 3)) + 1) * 255 / 2;
		b = (sin (v - (2 * c / 3)) + 1) * 255 / 2;
	}

	rgb [0] = r;
	rgb [1] = g;
	rgb [2] = b;
}

//Iterate and set color for a pixel
double drawPixel (int xs, int ys, double xc, double yc, int maxiter, double colorscale, bool smooth, bool gray, bmp_img img) {
	//Declare variables	
	double xz, yz, tempxz, tempyz;
	double i = 0.0;
	int rgb [3];

	//Initialize variables
	xz = xc;
	yz = yc;

	//Iterate the point
	while (i <= maxiter && fabs ((xz * xz) + (yz * yz)) <= 4 + 124 * smooth) {
		tempxz = (xz * xz) - (yz * yz) + xc;
		tempyz = (2 * xz * yz) + yc;

		xz = tempxz;
		yz = tempyz;

		i = i + 1.0;
	}

	//Correctly set smoothness
	if (i <= maxiter && smooth) {
		i = i - ((log (log (sqrt ((xz * xz) + (yz * yz))))) / log (2.0));
	}

	//Get correct color
	if (i < maxiter + 1) {
		//Pixel not in set, get color as hue
		color (i, maxiter, colorscale, rgb, gray);
	} else {
		//Pixel in set, set color to black
		rgb [0] = 0;
		rgb [1] = 0;
		rgb [2] = 0;
	}

	//Place pixel
	bmp_pixel_init (&img.img_pixels[ys][xs], rgb [0], rgb [1], rgb [2]);

	//Return i
	return i;
}

//Draw a rectangle inside the image
void drawRect (int width, int height, int lox, int hix, int loy, int hiy, double minx, double maxx, double miny, double maxy, int maxiter, double colorscale, bool smooth, bool gray, int limit, bmp_img img) {
	int a, b;
	double x, y, i;
	bool same = true;

	//Get reference pixel
	a = lox;
	b = loy;
	x = (a * (maxx - minx) / width) + minx; 
	y = (b * (maxy - miny) / height) + miny;
	double ref = drawPixel (a, b, x, y, maxiter, colorscale, smooth, gray, img);

	//Check the loy border
	for (int a = lox + 1; a < hix; a++) {
		x = (a * (maxx - minx) / width) + minx;
		i = drawPixel (a, b, x, y, maxiter, colorscale, smooth, gray, img);
		if (i != ref) {
			same = false;
		}
	}

	//Check the hiy border
	b = hiy;
	y = (b * (maxy - miny) / height) + miny;
	for (int a = lox + 1; a < hix; a++) {
		x = (a * (maxx - minx) / width) + minx;
		i = drawPixel (a, b, x, y, maxiter, colorscale, smooth, gray, img);
		if (i != ref) {
			same = false;
		}
	}

	//Check the lox border
	a = lox;
	x = (a * (maxx - minx) / width) + minx;
	for (int b = loy + 1; b <= hiy; b++) {
		y = (b * (maxy - miny) / height) + miny;
		i = drawPixel (a, b, x, y, maxiter, colorscale, smooth, gray, img);
		if (i != ref) {
			same = false;
		}
	}

	//Check the hix border
	a = hix;
	x = (a * (maxx - minx) / width) + minx;
	for (int b = loy; b <= hiy; b++) {
		y = (b * (maxy - miny) / height) + miny;
		i = drawPixel (a, b, x, y, maxiter, colorscale, smooth, gray, img);
		if (i != ref) {
			same = false;
		}
	}

	//Check if same
	if (same) {
		//Fill rectangle with correct color
		//Get correct color
		int rgb [3];
		if (ref < maxiter + 1) {
			//Pixel not in set, get color as hue
			color (ref, maxiter, colorscale, rgb, gray);
		} else {
			//Pixel in set, set color to black
			rgb [0] = 0;
			rgb [1] = 0;
			rgb [2] = 0;
		}
		//Fill area with color
		for (int xs = lox + 1; xs < hix; xs++) {
			for (int ys = loy + 1; ys < hiy; ys++) {
				bmp_pixel_init (&img.img_pixels[ys][xs], rgb [0], rgb [1], rgb [2]);
				//bmp_pixel_init (&img.img_pixels[ys][xs], 0, 0, 0);
			}
		}

	} else if (hix-lox>limit || hiy-loy>limit) {
		//Divide rectangle into four new rectangles to render
		int midx = lox + ((hix - lox) / 2);
		int midy = loy + ((hiy - loy) / 2);

		drawRect (width, height, lox + 1, midx, loy + 1, midy, minx, maxx, miny, maxy, maxiter, colorscale, smooth, gray, limit, img);
		drawRect (width, height, lox + 1, midx, midy, hiy - 1, minx, maxx, miny, maxy, maxiter, colorscale, smooth, gray, limit, img);
		drawRect (width, height, midx, hix - 1, loy + 1, midy, minx, maxx, miny, maxy, maxiter, colorscale, smooth, gray, limit, img);
		drawRect (width, height, midx, hix - 1, midy, hiy - 1, minx, maxx, miny, maxy, maxiter, colorscale, smooth, gray, limit, img);

	} else {
		//Calculate each pixel in region
		for (int a = lox + 1; a < hix; a++) {
			x = (a * (maxx - minx) / width) + minx;
			for (int b = loy + 1; b < hiy; b++) {
				y = (b * (maxy - miny) / height) + miny;
				drawPixel (a, b, x, y, maxiter, colorscale, smooth, gray, img);
			}
		}
	}
}

//Control the drawing of the mandelbrot fractal
void mandelbrot (double aspectRatio, int width, int maxiter, double zoom, double focusx, double focusy, int scalematch, char savename [], bool smooth, bool gray) {
	//Declare variables
	bmp_img img;
	int height;
	double yrange, xrange, minx, maxx, miny, maxy;

	//Set control variables
	height = (int) (((double) width) / aspectRatio);

	yrange = 1 / zoom;
	xrange = yrange * aspectRatio;

	minx = focusx - xrange / 2;
	maxx = focusx + xrange / 2;
	miny = focusy + yrange / 2;
	maxy = focusy - yrange / 2;

	double colorscale = ((double) scalematch) / ((double) maxiter);

	//Create image
	bmp_img_init_df (&img, width, height);

	//Render image	
	drawRect (width, height, 0, width-1, 0, height-1, minx, maxx, miny, maxy, maxiter, colorscale, smooth, gray, 3, img);

	//Save image
	bmp_img_write (&img, savename);
	bmp_img_free (&img);
}

int main () {
	//Settings
	double aspectRatio = 16.0 / 9.0;
	int width = 1920;
	int maxiter = 2000;
	double zoom = 0.4;
	double focusx = -0.5;
	double focusy = 0.0;
	int scalematch = 50;
	char savename[] = "../images/bmp/test.bmp";
	bool smooth = true;
	bool gray = false;

	//Render the fractal
	mandelbrot (aspectRatio, width, maxiter, zoom, focusx, focusy, scalematch, savename, smooth, gray);

	//Exit with code 0
	return 0;
}

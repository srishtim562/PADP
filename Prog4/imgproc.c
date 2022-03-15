#include <stdio.h>
#include <error.h>
#include <gd.h>
#include <omp.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *fp, *fp1 = {0};
	gdImagePtr img;
	char *iname = NULL, *oname = NULL;
	int color, x, y, w, h;
	int red, green, blue, tmp;	
	int nt = 4;
	color = x = y = w = h = 0;
	red = green = blue = 0;
	if (argc != 3)
	{
		error(1, 0, "Usage: gdnegat input.png output.png");
		exit(0);
	}
	
	iname = argv[1];
	oname = argv[2];
	if ((fp = fopen(iname, "r")) == NULL)
	{
		error(1, 0, "Error - fopen(): %s", iname);
		exit(0);
	}
	
	img = gdImageCreateFromPng(fp);
	w = gdImageSX(img);
	h = gdImageSY(img);
	double t = omp_get_wtime();
	printf("Width = %d, Height = %d\n", w, h);
	
	omp_set_num_threads(nt);
	#pragma omp parallel for private(y, color, red, green, blue, tmp) schedule(static, 20)	/* schedule(dynamic, 20), schedule(guided, 20) */
	for (x = 0; x < w; x++)
	{
		for (y = 0; y < h; y++)
		{
			color = gdImageGetPixel(img, x, y);
			red = gdImageRed(img, color);
			green = gdImageGreen(img, color);
			blue = gdImageBlue(img, color);
			tmp = (red + green + blue) / 3;
			red = green = blue = tmp;
			color = gdImageColorAllocate(img, red, green, blue);
			gdImageSetPixel(img, x, y, color);
		}
	}
	
	fclose(fp);
	fp1 = fopen(oname, "w");
	if (fp1 == NULL)
	{
		error(1, 0, "Error - fopen(): %s", oname);
		exit(0);
	}
	gdImagePng(img, fp1);
	fclose(fp1);	
	t = omp_get_wtime() - t;
	gdImageDestroy(img);
	printf("Time taken = %g\n", t);
	return 0;
}

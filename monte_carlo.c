#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void main()
{
	int t, n;
	for (t = 1; t <= 16; t *= 2)
	{
		for (n = 500; n <= 2000; n += 500)
		{
			int i, count = 0;
			double z_sqr, pi, x, y;

			srand(time(0));

			double t1 = omp_get_wtime();
			omp_set_num_threads(t);
			#pragma omp parallel for 
			for (int i = 0; i < n; i++)
			{
				x = (double)rand() / RAND_MAX;
				y = (double)rand() / RAND_MAX;
				z_sqr = x * x + y * y;
				if (sqrt(z_sqr) <= 1)		//If the point lies inside the circle
					count++;
			}		
			pi = ((double)count / n) * 4;

			double t2 = omp_get_wtime();
			printf("%d Trials (Input Size), %d Threads, Pi = %g in time = %lf\n", n, t, pi, t2 - t1);
		}
		printf("\n");
	}
}

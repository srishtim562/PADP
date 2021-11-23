#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
 
int main()
{ 
    int rows, cols, i, j, dim, count = 0, k, n;

        
        printf("Enter the dimensions of the matrix: ");
		scanf("%d", &dim);
		rows = dim;
		cols = dim;	

        int **arr1 = (int **)malloc(rows * sizeof(int *));
        for (i = 0; i < rows; i++)
            arr1[i] = (int *)malloc(cols * sizeof(int));
        int **arr2 = (int **)malloc(rows * sizeof(int *));
        for (i = 0; i < rows; i++)
            arr2[i] = (int *)malloc(cols * sizeof(int));
        int **arr3 = (int **)malloc(rows * sizeof(int *));
        for (i = 0; i < rows; i++)
            arr3[i] = (int *)malloc(cols * sizeof(int));
        

        //Initialization of matrices
        for (i = 0; i < rows; i++)
        {
            for (j = 0; j < cols; j++)
            {
                arr1[i][j] = count++;
                arr2[i][j] = count++;
            }
        }          
        
		/*
		printf("\nEnter the elements of the 1st matrix:\n");
		for (i = 0; i < rows; i++)
		{
			for (j = 0; j < cols; j++)
			{
				scanf("%d", &arr1[i][j]);
			}
		}
		
		printf("\nEnter the elements of the 2nd matrix:\n");
		for (i = 0; i < rows; i++)
		{
			for (j = 0; j < cols; j++)
			{
				scanf("%d", &arr2[i][j]);
			}
		}
		*/
        
        //Extra code for parallelization
        for(n = 1; n <= 16; n *= 2)
        {
            double x = omp_get_wtime();
            omp_set_num_threads(n);
            #pragma omp parallel for private(j, k)
            for (i = 0; i < rows; i++)
            {
                for (j = 0; j < cols; j++)
                {
                	arr3[i][j] = 0;
                    for (k = 0; k < rows; k++)
                    {
                    	arr3[i][j] += arr1[i][k] * arr2[k][j];		//Main multiplication operation
                    }  
                }
            }
            
            double y = omp_get_wtime();
            printf("For %d threads, time = %lf\n", n, y - x);
        }
        /*
         	printf("Result:\n");
            for (i = 0; i < rows; i++)
            {
            	for (j = 0; j < cols; j++)
            	{
            		printf("%d  ", arr3[i][j]);
            	}
            	
            	printf("\n");
            }
        */
        
   return 0;
}

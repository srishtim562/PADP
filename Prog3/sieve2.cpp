#include <math.h>
#include <omp.h>
#include <iostream>
#include <string.h>

using namespace std;

double t = 0.0;

inline long Strike(bool composite[], long i, long stride, long limit)
{
    for (; i <= limit; i += stride)
        composite[i] = true;

    return i;
}

long CacheUnfriendlySieve(long n)
{
    long count = 0;
    long m = (long)sqrt((double)n);
    bool *composite = new bool[n + 1];
    memset(composite, 0, n);
    t = omp_get_wtime();

    for (long i = 2; i <= m; ++i)
    {
        if (!composite[i])
        {
            count++;
            Strike(composite, 2 * i, i, n);
        }
    }

    for (long i = m + 1; i <= n; ++i)
    {
        if (!composite[i])
            count++;
    }

    t = omp_get_wtime() - t;
    delete[] composite;
    return count;
}

long CacheFriendlySieve(long n)
{
    long count = 0;
    long m = (long)sqrt((double)n);
    long *factor = new long[m];
    long n_factor = 0;

    t = omp_get_wtime();

    bool *composite = new bool[n + 1];
    long *striker = new long[m];

    memset(composite, 0, n);
    for (long i = 2; i <= m; ++i)
    {
        if (!composite[i])
        {
            count++;
            striker[n_factor] = Strike(composite, 2 * i, i, m);
            factor[n_factor++] = i;
        }
    }

    long k, i, window, limit;
    for (window = m + 1; window <= n; window += m)
    {
        limit = min(window + m - 1, n);
        for (k = 0; k < n_factor; ++k)
            striker[k] = Strike(composite, striker[k], factor[k], limit);

        for (i = window; i <= limit; i++)
        {
            if (!composite[i])
                count++;
        }
    }

    delete[] striker;
    delete[] composite;

    t = omp_get_wtime() - t;
    delete[] factor;
    return count;
}

long ParallelSieve(long n, int numThreads)
{
    long count = 0;
    long m = (long)sqrt((double)n);
    long *factor = new long[m];
    long n_factor = 0;

    t = omp_get_wtime();
    omp_set_num_threads(numThreads);

    #pragma omp parallel
    {
        bool *composite = new bool[n + 1];
        long *striker = new long[m];

        #pragma omp single
        {
            memset(composite, 0, n);
            for (long i = 2; i <= m; ++i)
            {
                if (!composite[i])
                {
                    count++;
                    striker[n_factor] = Strike(composite, 2 * i, i, m);
                    factor[n_factor++] = i;
                }
            }
        }

        long k, i, window, limit;
        #pragma omp parallel for reduction(+ : count) private(k, i, window, limit)
        for (window = m + 1; window <= n; window += m)
        {
            limit = min(window + m - 1, n);
            for (k = 0; k < n_factor; ++k)
                striker[k] = Strike(composite, striker[k], factor[k], limit);

            for (i = window; i <= limit; i++)
            {
                if (!composite[i])
                    count++;
            }
        }

        delete[] striker;
        delete[] composite;
    }

    t = omp_get_wtime() - t;
    delete[] factor;
    return count;
}

int main()
{
    long n;
    cout<<"Enter the upper limit for searching no. of primes: ";
    cin>>n;

    long count = CacheUnfriendlySieve(n);
    cout<<"Cache Unfriendly Sieve\n";
    cout<<"Count: "<<count<<endl;
    cout<<"Time: "<<t<<endl;
    cout<<"********************************\n";

    count = CacheFriendlySieve(n);
    cout<<"Cache Friendly Sieve\n";
    cout<<"Count: "<<count<<endl;
    cout<<"Time: "<<t<<endl;
    cout<<"********************************\n";

    cout<<"Parallel Sieve\n";

    for (int numThreads = 1; numThreads <= 8; numThreads *= 2)
    {
        count = ParallelSieve(n, numThreads);
        cout<<numThreads<<" threads\n"<<"Count: "<<count<<endl<<"Time: "<<t<<endl; 
    }
    cout<<"*********************************\n";
}

#include <math.h>
#include <string.h>
#include <omp.h>
#include <iostream>

using namespace std;

double t = 0.0;

inline long Strike(bool composite[], long i, long stride, long limit)
{
    // To strike out multiples of a chosen prime number (to strike out the composite numbers)
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
            ++count; // count of prime numbers (upto m)
            // Strike walks array of size n here.
            Strike(composite, 2 * i, i, n); // 2 * i is the smallest multiple of i
        }
    }

    for (long i = m + 1; i <= n; ++i)
    {
        if (!composite[i])
            ++count; // count of prime numbers (after m)
    }

    t = omp_get_wtime() - t;
    delete[] composite;
    return count;
}

long CacheFriendlySieve(long n)
{
    long count = 0;
    long m = (long)sqrt((double)n);
    long *factor = new long[m];     //1
    long n_factor = 0;      //2
    t = omp_get_wtime();

    bool *composite = new bool[n + 1];
    long *striker = new long[m];    //3

    memset(composite, 0, n);

    for (long i = 2; i <= m; ++i)
    {
        if (!composite[i])
        {
            ++count;
            striker[n_factor] = Strike(composite, 2 * i, i, m);
            factor[n_factor++] = i;
        }
    }

    // Chops sieve into windows of size sqrt(n)
    for (long window = m + 1; window <= n; window += m)
    {
        long limit = min(window + m - 1, n);
        for (long k = 0; k < n_factor; ++k)
        {
            // Strike walks window of size sqrt(n) here.
            striker[k] = Strike(composite, striker[k], factor[k], limit);
        }

        for (long i = window; i <= limit; ++i)
        {
            if (!composite[i])
                ++count;
        }
    }

    t = omp_get_wtime() - t;
    delete[] striker;
    delete[] composite;
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
                    ++count;
                    striker[n_factor] = Strike(composite, 2 * i, i, m);
                    factor[n_factor++] = i;
                }
            }
        }

        long k, i, window, limit;
        // Chops sieve into windows of size sqrt(n)
        #pragma omp parallel for reduction(+ : count) private(k, i, window, limit)
        for (window = m + 1; window <= n; window += m)
        {
            limit = min(window + m - 1, n);
            for (k = 0; k < n_factor; ++k)
            {
                // Strike walks window of size sqrt(n) here.
                striker[k] = Strike(composite, striker[k], factor[k], limit);
            }

            for (i = window; i <= limit; ++i)
            {
                if (!composite[i])
                    ++count;
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
    int numThreads;
    cout << "Enter the upperlimit for searching no. of primes: " << endl;
    cin >> n;
    long count = CacheUnfriendlySieve(n);
    cout << "**************************" << endl;
    cout << "Cache Unfriendly\n";
    cout << "count: " << count << endl;
    cout << "Time : " << t << endl;

    count = CacheFriendlySieve(n);
    cout << "**************************" << endl;
    cout << "Cache Friendly\n";
    cout << "count: " << count << endl;
    cout << "time : " << t << endl;
    cout << "**************************" << endl;

    cout << "Parallel Sieve\n";

    //Extra code for parallelization
    for (int numThreads = 1; numThreads <= 8; numThreads *= 2)
    {
        cout << numThreads << " threads" << endl;
        count = ParallelSieve(n, numThreads);
        cout << "count: " << count << endl;
        cout << "time : " << t << endl;
    }
    cout << "**************************" << endl;
}

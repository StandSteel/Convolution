//
// Created by shahnoor on 2/1/2018.
//

#include <cmath>
#include <iostream>
#include <thread>
#include <mutex>
#include "../include/convolution.h"
#include "../include/binomial.h"
#include <omp.h>

using namespace std;


/**
 * Perform convolution of a given data list (single column) using the following formula
 *    X_p = B(N, n, p) * X_n
 *    where, B(N, n, p) = N^C_n * p^n * (1-p)^(N-n)
 * Since it is costly to use this formula directly, this function uses a simplified one
 *    B(N,i,p) = 1  # is set arbitrarily
 *    B(N,n,p) = B(N, n-1, p) * ((N-n+1)/n) * (p/(1-p))    # for i > n
 *    B(N,n,p) = B(N, n+1, p) * (n+1)/(N-n)) * ((1-p)/p)   # for i < n
 *
 * Complexity, O(2.426)
 *
 * @param data : array of double valued data
 * @return     : array of double valued convolved data
 */
vector<double> convolve_v1(const vector<double>& data) {

    size_t size = data.size();
    vector<double> binomial;
    vector<double> out_data(size);
    double p;
//    size_t len = size / 100;
    for(size_t i{}; i != size; ++i) {
//        if(i % len == 0){
//            cout << "Running for row " << i << endl;
//        }
        p = (i + 1) / double(size);
        binomial = binomial_distribution_v2(size, i, p);
        for(size_t j{}; j != size; ++j){
            out_data[i] += binomial[j]*data[j];
        }
    }
    return out_data;
}



/**
 * Perform convolution of a given data list (single column) using the following formula
 *    X_p = B(N, n, p) * X_n
 *    where, B(N, n, p) = N^C_n * p^n * (1-p)^(N-n)
 * Since it is costly to use this formula directly, this function uses a simplified one
 *    B(N,i,p) = 1  # is set arbitrarily
 *    B(N,n,p) = B(N, n-1, p) * ((N-n+1)/n) * (p/(1-p))    # for i > n
 *    B(N,n,p) = B(N, n+1, p) * (n+1)/(N-n)) * ((1-p)/p)   # for i < n
 *
 * Complexity, O(2.769)
 *
 * @param data : array of double valued data
 * @return     : array of double valued convolved data
 */
vector<double> convolve_v2(const vector<double>& data) {

    size_t size = data.size();
    vector<double> out_data(size);
    double probability, a, b, sm, normalization_const, factor;
//    size_t len = size / 100;
    for(size_t i{}; i != size; ++i) {
//        if(i % len == 0){
//            cout << "Running for row " << i << endl;
//        }

        probability = (i + 1) / double(size);
        a = b = 1;
        sm = data[i];   //# sum
        normalization_const = 1;
        for(size_t j{i+1}; j != size; ++j) {  // for j > i
            factor = (size - j + 1) * probability / (j * (1 - probability));
            b = a * factor;
            normalization_const += b;
            sm += b * data[j];
            a = b;  //# swapping
        }
        a = b = 1; //# reassign to unity
        for(size_t j{i-1}; j != -1; --j) {  // for j < i
            factor = (j + 1) * (1 - probability) / ((size - j) * probability);
            a = b * factor;
            sm += a * data[j];
            normalization_const += a;
            b = a;  //# swapping
        }
        sm /= normalization_const;  //# normalize    the result
        out_data[i] = sm;
    }
    return out_data;
}

/**
 * Perform convolution of a given data list (single column) using the following formula
 *    X_p = B(N, n, p) * X_n
 *    where, B(N, n, p) = N^C_n * p^n * (1-p)^(N-n)
 * Since it is costly to use this formula directly, this function uses a simplified one
 *    B(N,i,p) = 1  # is set arbitrarily
 *    B(N,n,p) = B(N, n-1, p) * ((N-n+1)/n) * (p/(1-p))    # for i > n
 *    B(N,n,p) = B(N, n+1, p) * (n+1)/(N-n)) * ((1-p)/p)   # for i < n
 *
 * Complexity, O(2.628)
 * todo Uses symmetry of the binomial ??
 * @param data : array of double valued data
 * @return     : array of double valued convolved data
 */
vector<double> convolve_v3(const vector<double>& data) {

    size_t size = data.size();
    vector<double> binomial(size);
    vector<double> out_data(size);
    double p;
//    size_t len = size / 100;
    for(size_t i{}; i != size; ++i) {
//        if(i % len == 0){
//            cout << "Running for row " << i << endl;
//        }
        p = (i + 1) / double(size);
        binomial[i] = 1;
        double factor;
        double normalization_const={1};
        for (size_t j{i + 1}; j != size; ++j) {  // for j > center
            factor = (size - j + 1) * p / (j * (1 - p));
            binomial[j] = binomial[j - 1] * factor;
            normalization_const += binomial[j];
        }

        for (size_t j{i - 1}; j != -1; --j) {  // for j < center
            factor = (j + 1) * (1 - p) / ((size - j) * p);
            binomial[j] = binomial[j + 1] * factor;
            normalization_const += binomial[j];
        }

        for(size_t j{}; j != size; ++j){
            out_data[i] += binomial[j]*data[j]/normalization_const;
        }
    }
    return out_data;
}

/**
 * Perform convolution of a given data list (single column) using the following formula
 *    X_p = B(N, n, p) * X_n
 *    where, B(N, n, p) = N^C_n * p^n * (1-p)^(N-n)
 * Since it is costly to use this formula directly, this function uses a simplified one
 *    B(N,i,p) = 1  # is set arbitrarily
 *    B(N,n,p) = B(N, n-1, p) * ((N-n+1)/n) * (p/(1-p))    # for i > n
 *    B(N,n,p) = B(N, n+1, p) * (n+1)/(N-n)) * ((1-p)/p)   # for i < n
 * Optimized by Digonto vai
 * @param data_in : array of double valued data
 * @return     : array of double valued convolved data
 */
vector<double> convolve_v4(const vector<double>& data_in) {
    size_t N = data_in.size();

    vector<double> _forward_factor(N);
    vector<double> _backward_factor(N);

    for (size_t i=0; i < N; ++i)
    {
        _forward_factor[i]  = (double) (N - i + 1) / i;
        _backward_factor[i] = (double) (i + 1) / (N - i);
    }

    vector<double> data_out(N);

    long step = N / 1000;
    for (long j=0; j <N; ++j) // start from j=1
    {
        double prob     = (double) j / N;
        double factor   = 0;
        double binom    = 0;
        double prev     = 0;
        double bn_tot   = 1; // normalization factor
        double sum      = data_in[j];


        // forward iteraion part
        factor = prob / (1-prob);
        prev   = 1;

        for (long i=j+1; i<N; ++i)
        {
            binom     = prev * _forward_factor[i] * factor;
            bn_tot += binom;
            sum      += data_in[i] * binom;
            prev      = binom;
        }

        // backward iteration part
        factor = (1-prob)/prob;
        prev   = 1;

        for (long i=j-1; i>=0; --i)
        {
            binom     = prev * _backward_factor[i] * factor;
            bn_tot += binom;
            sum      += data_in[i] * binom;
            prev      = binom;
        }

        // normalizing data_in
        data_out[j] = sum / bn_tot;
        cout << bn_tot << endl;
//        if(j % step == 0) {
//            cout << "\33[2K"; // erase the current line
//            cout << '\r'; // return the cursor to the start of the line
////            cout << "row " << j << " ";
//            cout << "progress " << j * 100 / double(N) << " %";
//            std::fflush(stdout);
//        }

    }


    return data_out;
}

/**
 * Progress information in the console.
 * Perform convolution of a given data list (multiple column) using the following formula
 *    X_p = B(N, n, p) * X_n
 *    where, B(N, n, p) = N^C_n * p^n * (1-p)^(N-n)
 * Since it is costly to use this formula directly, this function uses a simplified one
 *    B(N,i,p) = 1  # is set arbitrarily
 *    B(N,n,p) = B(N, n-1, p) * ((N-n+1)/n) * (p/(1-p))    # for i > n
 *    B(N,n,p) = B(N, n+1, p) * (n+1)/(N-n)) * ((1-p)/p)   # for i < n
 *
 * Complexity, O(2.482) with 3 columns
 *
 * @param data : n-dimensional array of double valued data
 *      for example: following data has 3 columns and N rows
 *              0.25    0.454   0.548
 *              0.457   0.187   0.154
 *              0.578   0.951   0.487
 *              .       .       .
 *              .       .       .
 *              .       .       .
 * @return     : n-dimensional array of double valued convolved data
 */
vector<vector<double>> convolve_multi_v1(const vector<vector<double>>& data) {
    size_t n_columns = data.size();
    size_t n_rows = data[0].size();
    vector<vector<double>> out_data(data.size());
    vector<double> binomial;
    double probability;
    size_t len = n_columns / 1000;

    for(size_t i{}; i != n_columns; ++i) {
        if(i % len == 0){
//          cout << "Running for row " << i << endl;
            cout << "\33[2K"; // erase the current line
            cout << '\r'; // return the cursor to the start of the line
            cout << "at " << 100 * i / double(n_columns) << "%";
            std::fflush(stdout);
//            cout << endl;
        }

        probability = (i + 1) / double(n_columns);
        binomial = binomial_distribution_v2(n_columns, i, probability);

        out_data[i] = vector<double>(n_rows);
        for (size_t k{}; k != n_rows; ++k) {
            for (size_t j{}; j != n_columns; ++j) {
                out_data[i][k] += data[j][k] * binomial[j];
            }
        }
    }
    return out_data;
}



/**
 * Perform convolution of a given data list (multiple column) using the following formula
 *    X_p = B(N, n, p) * X_n
 *    where, B(N, n, p) = N^C_n * p^n * (1-p)^(N-n)
 * Since it is costly to use this formula directly, this function uses a simplified one
 *    B(N,i,p) = 1  # is set arbitrarily
 *    B(N,n,p) = B(N, n-1, p) * ((N-n+1)/n) * (p/(1-p))    # for i > n
 *    B(N,n,p) = B(N, n+1, p) * (n+1)/(N-n)) * ((1-p)/p)   # for i < n
 *
 * Complexity, O(3.184) with 3 columns
 *
 * @param data : n-dimensional array of double valued data
 *      for example: following data has 3 columns and N rows
 *              0.25    0.454   0.548
 *              0.457   0.187   0.154
 *              0.578   0.951   0.487
 *              .       .       .
 *              .       .       .
 *              .       .       .
 * @return     : n-dimensional array of double valued convolved data
 */
vector<vector<double>> convolve_multi_v2(const vector<vector<double>>& data) {
    size_t size = data.size();
    vector<vector<double>> out_data(data.size());
    vector<double> sum(data[0].size());
    vector<double> binomial;
    double probability, a, b, normalization_const, factor;
//    size_t len = size / 100;
    for(size_t i{}; i != size; ++i) {
//        if(i % len == 0){
//            cout << "Running for row " << i << endl;
//        }

        probability = (i + 1) / double(size);
        a = b = 1;
        for(size_t k{}; k != sum.size(); ++k) {
            sum[k] = data[i][k];   //# sum
        }
        normalization_const = 1;
        for(size_t j{i+1}; j != size; ++j) {  // for j > i
            factor = (size - j + 1) * probability / (j * (1 - probability));
            b = a * factor;
            normalization_const += b;
            for(size_t k{}; k != sum.size(); ++k) {
                sum[k] += b*data[j][k];   //# sum
            }
            a = b;  //# swapping
        }
        a = b = 1; //# reassign to unity
        for(size_t j{i-1}; j != -1; --j) {  // for j < i
            factor = (j + 1) * (1 - probability) / ((size - j) * probability);
            a = b * factor;
            for(size_t k{}; k != sum.size(); ++k) {
                sum[k] += a*data[j][k];   //# sum
            }
            normalization_const += a;
            b = a;  //# swapping
        }
        for(size_t k{}; k != sum.size(); ++k) {
            sum[k] /= normalization_const;   //normalize the result
        }

        out_data[i] = sum;
    }
    return out_data;
}

/**
 * Perform convolution of a given data list (multiple column) using the following formula
 *    X_p = B(N, n, p) * X_n
 *    where, B(N, n, p) = N^C_n * p^n * (1-p)^(N-n)
 * Since it is costly to use this formula directly, this function uses a simplified one
 *    B(N,i,p) = 1  # is set arbitrarily
 *    B(N,n,p) = B(N, n-1, p) * ((N-n+1)/n) * (p/(1-p))    # for i > n
 *    B(N,n,p) = B(N, n+1, p) * (n+1)/(N-n)) * ((1-p)/p)   # for i < n
 *
 * Optimized by Digonto vai
 *
 * @param data_in : n-dimensional array of double valued data
 *      for example: following data has 3 columns and N rows
 *              0.25    0.454   0.548
 *              0.457   0.187   0.154
 *              0.578   0.951   0.487
 *              .       .       .
 *              .       .       .
 *              .       .       .
 * @return     : n-dimensional array of double valued convolved data
 */
vector<vector<double>> convolve_multi_v3(const vector<vector<double>>& data_in) {
    size_t n_columns = data_in[0].size(); // number of columns
    size_t n_rows = data_in.size(); // number of rows

//    cout << "rows " << n_rows << endl;
//    cout << "cols " << n_columns << endl;

    vector<double> _forward_factor(n_rows);
    vector<double> _backward_factor(n_rows);

    for (size_t i=0; i < n_rows; ++i)
    {
        _forward_factor[i]  = (double) (n_rows - i + 1) / i;
        _backward_factor[i] = (double) (i + 1) / (n_rows - i);
    }


    vector<vector<double>> data_out(n_rows);

    size_t step = n_rows / 1000;

    for (long row=0; row < n_rows; ++row){
        data_out[row].resize(n_columns); // space for columns
        double prob     = (double) row / n_rows;
        double factor   = 0;
        double binom    = 0;
        double prev     = 0;
        double binomNormalization_const = 1;


        vector<double> sum(n_columns);
        for(size_t k{}; k < n_columns; ++k){
            sum[k] = data_in[row][k];
        }

//        cout << "line : " << __LINE__ << endl;

        // forward iteration part
        factor = prob / (1-prob);
        prev   = 1;

        for (long i=row+1; i < n_rows; ++i)
        {
            binom     = prev * _forward_factor[i] * factor;
            binomNormalization_const += binom;
            for(size_t j{}; j < n_columns; ++j){
                sum[j] += data_in[i][j] * binom;
            }
            prev      = binom;
        }
//        cout << "line : " << __LINE__ << endl;
        // backward iteration part
        factor = (1-prob)/prob;
        prev   = 1;

        for (long i=row-1; i>=0; --i)
        {
            binom     = prev * _backward_factor[i] * factor;
            binomNormalization_const += binom;
            for(size_t j{}; j < n_columns; ++j){
                sum[j] += data_in[i][j] * binom;
            }
            prev      = binom;
        }

        // normalizing data_in
        for(size_t j{}; j < n_columns; ++j){
            data_out[row][j] = sum[j] / binomNormalization_const;

        }
        if(row % step == 0) {
            cout << "\33[2K"; // erase the current line
            cout << '\r'; // return the cursor to the start of the line
            cout << "progress " << row * 100 / double(n_rows) << " %";
            std::fflush(stdout);
        }
    }

    return data_out;
}

/**
 *
 * @param data
 * @param out_data
 * @param i
 */
void calculate_convolution_for_rows(
        const size_t start,
        const size_t end,
        const vector<vector<double>> &data,
        vector<vector<double>> &out_data
) {
    size_t size = data.size();
    double probability{};
    vector<double> binomial;
    double sum{};
    for(size_t i{start}; i != end; ++i) {
        probability = (i + 1) / double(size);
        binomial = binomial_distribution_v2(size, i, probability);
        for (size_t k{}; k != data[0].size(); ++k) {
            sum = 0;
            for (size_t j{0}; j != size; ++j) {
                 sum += data[j][k] * binomial[j];
            }
            out_data[i][k] = sum;
        }
    }
}



/**
 *
 * @param data
 * @param out_data
 * @param i
 */
void calculate_convolution_for_rows_v2(
        const size_t start,
        const size_t end,
        const vector<vector<double>> &data,
        vector<vector<double>> &out_data,
        const size_t id,
        vector<size_t> &iterations
) {
    size_t size = data.size();
    double probability{};
    vector<double> binomial;
    double sum{};
    for(size_t i{start}; i != end; ++i) {
        // keeps track of the number of iteration  performed
        // so that it can be accessed from another thread
        ++iterations[id];

        probability = (i + 1) / double(size);
        binomial = binomial_distribution_v2(size, i, probability);
        for (size_t k{}; k != data[0].size(); ++k) {
            sum = 0;
            for (size_t j{0}; j != size; ++j) {
                sum += data[j][k] * binomial[j];
            }
            out_data[i][k] = sum;
        }
    }
}

/**
 * Multithreading is used. // todo
 * Perform convolution of a given data list (multiple column) using the following formula
 *    X_p = B(N, n, p) * X_n
 *    where, B(N, n, p) = N^C_n * p^n * (1-p)^(N-n)
 * Since it is costly to use this formula directly, this function uses a simplified one
 *    B(N,i,p) = 1  # is set arbitrarily
 *    B(N,n,p) = B(N, n-1, p) * ((N-n+1)/n) * (p/(1-p))    # for i > n
 *    B(N,n,p) = B(N, n+1, p) * (n+1)/(N-n)) * ((1-p)/p)   # for i < n
 *
 * Complexity, O(2.482) with 3 columns
 *
 * @param data_in : n-dimensional array of double valued data
 *      for example: following data has 3 columns and N rows
 *              0.25    0.454   0.548
 *              0.457   0.187   0.154
 *              0.578   0.951   0.487
 *              .       .       .
 *              .       .       .
 *              .       .       .
 * @return     : n-dimensional array of double valued convolved data
 */
vector<vector<double>> convolve_multi_threaded_v1(const vector<vector<double>> &data_in) {
    size_t size = data_in.size();
    vector<vector<double>> data_out(data_in.size());

    for(size_t i{}; i != size; ++i) { // initialize sizes
        data_out[i] = vector<double>(data_in[0].size());
    }

    size_t len = size / 1000;

    size_t number_of_threads = std::thread::hardware_concurrency(); // number of threads

    size_t division = size / number_of_threads;

    vector<thread> threads(number_of_threads);

    size_t start, end;
    for(size_t i{}; i != number_of_threads; ++i){
        start = i*division;
        end = (i+1) * division;
        // each thread finishes independently
        threads[i] = std::thread(calculate_convolution_for_rows, start, end, std::ref(data_in), std::ref(data_out));

    }

    // join the threads
    for(size_t j{}; j != number_of_threads; ++j){
        if(threads[j].joinable())  threads[j].join();
    }

    return data_out;
}


/**
 * Multithreading is used with progress information in the console.
 * Perform convolution of a given data list (multiple column) using the following formula
 *    X_p = B(N, n, p) * X_n
 *    where, B(N, n, p) = N^C_n * p^n * (1-p)^(N-n)
 * Since it is costly to use this formula directly, this function uses a simplified one
 *    B(N,i,p) = 1  # is set arbitrarily
 *    B(N,n,p) = B(N, n-1, p) * ((N-n+1)/n) * (p/(1-p))    # for i > n
 *    B(N,n,p) = B(N, n+1, p) * (n+1)/(N-n)) * ((1-p)/p)   # for i < n
 *
 * Complexity, O(2.482) with 3 columns
 *
 * @param data_in : n-dimensional array of double valued data
 *      for example: following data has 3 columns and N rows
 *              0.25    0.454   0.548
 *              0.457   0.187   0.154
 *              0.578   0.951   0.487
 *              .       .       .
 *              .       .       .
 *              .       .       .
 * @return     : n-dimensional array of double valued convolved data
 */
vector<vector<double>> convolve_multi_threaded_v2(const vector<vector<double>> &data_in) {
    size_t size = data_in.size();
    vector<vector<double>> data_out(data_in.size());

    for(size_t i{}; i != size; ++i) { // initialize sizes
        data_out[i] = vector<double>(data_in[0].size());
    }

    size_t number_of_threads = std::thread::hardware_concurrency(); // number of threads

    size_t division = size / number_of_threads;

    vector<thread> threads(number_of_threads); // holds the threads
    vector<size_t> iterations(number_of_threads); // progress report

    size_t start, end;

    std::mutex mutex_main_thread;
    for(size_t i{}; i != number_of_threads; ++i){
        start = i*division;
        end = (i+1) * division;
        // each thread finishes independently
        threads[i] = std::thread(
                calculate_convolution_for_rows_v2,
                start,
                end,
                std::ref(data_in),
                std::ref(data_out),
                i,
                std::ref(iterations)
        );

    }


    // summary in the main thread
    if(size > 60000) {
        size_t sum{};
        while (sum < size) {
            sum = 0;
            {
                std::lock_guard<mutex> _locker(mutex_main_thread);
                for (size_t i{}; i != iterations.size(); ++i) {
                    sum += iterations[i];
                }
            }
            cout << "\33[2K"; // erase the current line
            cout << '\r'; // return the cursor to the start of the line
            cout << "progress : " << 100 * sum / double(size) << "%";
            std::fflush(stdout);
            std::this_thread::sleep_for(std::chrono::duration<double>(5));
        }
    }

    cout  << endl;
    // join the threads
    for(size_t j{}; j != number_of_threads; ++j){
        if(threads[j].joinable())  threads[j].join();
    }

    return data_out;
}


/*****************************************************
 * Methods of the Convolution class
 */

Convolution::Convolution(int threads) {
    _number_of_threads = threads;
    if(_number_of_threads <= 0 || _number_of_threads > omp_get_max_threads()){
        _number_of_threads = omp_get_max_threads();
    }
}

/**
 * Initializes the binomial expansion
 */
void Convolution::initialize(size_t n)  {
    auto t0 = chrono::system_clock::now();
    N = n;
    _forward_factor.resize(N);
    _backward_factor.resize(N);

    for (size_t i=0; i < N; ++i)
    {
        _forward_factor[i]  = (double) (N - i + 1) / i;
        _backward_factor[i] = (double) (i + 1) / (N - i);
    }

    auto t1 = chrono::system_clock::now();
    _time_elapsed_initialization = chrono::duration<double>(t1 - t0).count();
}

/**
 * Basic structure of this function was originally design by Digonto Islam.
 * Run convolution on a array of data
 * @param data_in : input array
 * @return : convolved version of data_in
 */
vector<double> Convolution::run(vector<double>& data_in) {
    size_t N = data_in.size();
    initialize(N);

    vector<double> data_out(N);
    auto t0 = chrono::system_clock::now();
    long step = N / 1000;
    for (long j=0; j <N; ++j) // start from j=1
    {
        double prob     = (double) j / N;
        double factor   = 0;
        double binom    = 0;
        double prev     = 0;
        double bn_tot   = 1; // normalization factor
        double sum      = data_in[j];


        // forward iteraion part
        factor = prob / (1-prob);
        prev   = 1;

        for (long i=j+1; i<N; ++i)
        {
            binom     = prev * _forward_factor[i] * factor;
            bn_tot += binom;
            sum      += data_in[i] * binom;
            prev      = binom;
        }

        // backward iteration part
        factor = (1-prob)/prob;
        prev   = 1;

        for (long i=j-1; i>=0; --i)
        {
            binom     = prev * _backward_factor[i] * factor;
            bn_tot += binom;
            sum      += data_in[i] * binom;
            prev      = binom;
        }

        // normalizing data
        data_out[j] = sum / bn_tot;
        cout << bn_tot << endl;
//        if(j % step == 0) {
//            cout << "\33[2K"; // erase the current line
//            cout << '\r'; // return the cursor to the start of the line
////            cout << "row " << j << " ";
//            cout << "progress " << j * 100 / double(N) << " %";
//            std::fflush(stdout);
//        }

    }
    auto t1 = chrono::system_clock::now();
    _time_elapsed_convolution = chrono::duration<double>(t1 - t0).count();
    return data_out;
}


/**
 * OpenMP version
 * Run convolution on a array of data
 * @param data_in : input array
 * @return : convolved version of data_in
 */
vector<double> Convolution::run_omp(vector<double>& data_in) {
    size_t N = data_in.size();
    initialize(N);

    vector<double> data_out(N);
    auto t0 = chrono::system_clock::now();
    long step = N / 1000;
    // entering parallel region
#pragma omp parallel for schedule(dynamic) num_threads(_number_of_threads)
    for (long j=0; j <N; ++j)
    {
        double prob     = (double) j / N;
        double factor   = 0;
        double binom    = 0;
        double prev     = 0;
        double binomNormalization_const = 1;
        double sum      = data_in[j];
        ++_count;

        // forward iteraion part
        factor = prob / (1-prob);
        prev   = 1;

        for (long i=j+1; i<N; ++i)
        {
            binom     = prev * _forward_factor[i] * factor;
            binomNormalization_const += binom;
            sum      += data_in[i] * binom;
            prev      = binom;
        }

        // backward iteration part
        factor = (1-prob)/prob;
        prev   = 1;

        for (long i=j-1; i>=0; --i)
        {
            binom     = prev * _backward_factor[i] * factor;
            binomNormalization_const += binom;
            sum      += data_in[i] * binom;
            prev      = binom;
        }

        // normalizing data
        data_out[j] = sum / binomNormalization_const;
        if(j % step == 0) {
            cout << "\33[2K"; // erase the current line
            cout << '\r'; // return the cursor to the start of the line
//            cout << "row " << j << " ";
            cout << "progress " << j * 100 / double(N) << " %";
            std::fflush(stdout);
        }

    }
    auto t1 = chrono::system_clock::now();
    _time_elapsed_convolution = chrono::duration<double>(t1 - t0).count();
    return data_out;
}

/**
 * Basic structure of this function was originally design by Digonto Islam.
 * OpenACC version
 * Run convolution on a array of data
 * @param data_in : input array
 * @return : convolved version of data_in
 */
vector<double> Convolution::run_acc(vector<double>& data_in) {
    size_t N = data_in.size();
    initialize(N);

    vector<double> data_out(N);
    auto t0 = chrono::system_clock::now();
    long step = N / 1000;
    // entering parallel region
    // take copy of arrays to each loop
#pragma acc data copy(data_out[0:_number_of_data]) copyin(_forward_factor[0:_number_of_data],_backward_factor[0:_number_of_data],d[0:_number_of_data])
#pragma acc parallel loop independent
    for (long j=0; j <N; ++j)
    {
        double prob     = (double) j / N;
        double factor   = 0;
        double binom    = 0;
        double prev     = 0;
        double binomNormalization_const = 1;
        double sum      = data_in[j];
        ++_count;

        // forward iteraion part
        factor = prob / (1-prob);
        prev   = 1;

        for (long i=j+1; i<N; ++i)
        {
            binom     = prev * _forward_factor[i] * factor;
            binomNormalization_const += binom;
            sum      += data_in[i] * binom;
            prev      = binom;
        }

        // backward iteration part
        factor = (1-prob)/prob;
        prev   = 1;

        for (long i=j-1; i>=0; --i)
        {
            binom     = prev * _backward_factor[i] * factor;
            binomNormalization_const += binom;
            sum      += data_in[i] * binom;
            prev      = binom;
        }

        // normalizing data
        data_out[j] = sum / binomNormalization_const;
        if(j % step == 0) {
            cout << "\33[2K"; // erase the current line
            cout << '\r'; // return the cursor to the start of the line
//            cout << "row " << j << " ";
            cout << "progress " << j * 100 / double(N) << " %";
            std::fflush(stdout);
        }

    }
    auto t1 = chrono::system_clock::now();
    _time_elapsed_convolution = chrono::duration<double>(t1 - t0).count();
    return data_out;
}

/**
 * Multithreading is used.
 * pthread version.
 * Run convolution on a array of data
 * @param data_in : input array
 * @return : convolved version of data_in
 */
vector<double> Convolution::run_pthread(vector<double> &data_in) {
    size_t N = data_in.size();
    initialize(N);
    vector<double> data_out(N);

    size_t number_of_threads = std::thread::hardware_concurrency(); // number of threads
    size_t loop_per_thread = N / number_of_threads;
    vector<thread> threads(number_of_threads); // holds the threads
    size_t start, stop;

    auto t0 = chrono::system_clock::now();
    for(size_t i{}; i != number_of_threads; ++i){
        start = i*loop_per_thread;
        stop = (i+1) * loop_per_thread;
        ////each thread finishes independently
        threads[i] = std::thread(
                &Convolution::convolution_single_range,
                this,
                start,
                stop,
                std::ref(data_in),
                std::ref(data_out)
        );
        cout << "thread " << i << " : id " << threads[i].get_id() << " range " << start << " to " << stop << endl;

    }
    // join the threads
//    double prgrss{};
//    while (true){
//
//        prgrss = progress();
//        cout << "progress " << prgrss << endl;
//        if (prgrss > 0.99){
//            break;
//        }
//        std::this_thread::sleep_for(std::chrono::duration<double>(5));
//    }

    for(size_t i{}; i != number_of_threads; ++i){
        if(threads[i].joinable())
        {
            cout << "joining thread " << i << " : id " << threads[i].get_id() << endl;
            threads[i].join();
        }
    }

    auto t1 = chrono::system_clock::now();
    _time_elapsed_convolution = chrono::duration<double>(t1 - t0).count();
    return data_out;
}

/**
 *
 * @param row_start
 * @param row_stop
 * @param data_in
 * @param data_out
 */
void Convolution::convolution_single_range(
        long row_start,
        long row_stop,
        const vector<double> &data_in,
        vector<double> &data_out
) {
    for (long j=row_start; j < row_stop; ++j)
    {
        double prob     = (double) j / N;
        double factor   = 0;
        double binom    = 0;
        double prev     = 0;
        double binomNormalization_const = 1;
        double sum      = data_in[j];
        ++_count; // only to keep track of % progress

        // forward iteraion part
        factor = prob / (1-prob);
        prev   = 1;

        for (long i=j+1; i < N; ++i)
        {
            binom     = prev * _forward_factor[i] * factor;
            binomNormalization_const += binom;
            sum      += data_in[i] * binom;
            prev      = binom;
        }

        // backward iteration part
        factor = (1-prob)/prob;
        prev   = 1;

        for (long i=j-1; i>=0; --i)
        {
            binom     = prev * _backward_factor[i] * factor;
            binomNormalization_const += binom;
            sum      += data_in[i] * binom;
            prev      = binom;
        }

        // normalizing data
        data_out[j] = sum / binomNormalization_const;

    }

}

/**
 * Run convolution on array of multiple columns of data
 * @param data_in : n-dimensional array of double valued data
 *      for example: following data has 3 columns and N rows
 *              0.25    0.454   0.548
 *              0.457   0.187   0.154
 *              0.578   0.951   0.487
 *              .       .       .
 *              .       .       .
 *              .       .       .
 *
 *        therefore shape will be
 *        data_in.size() == N
 *        data_in[0].size() == 3
 * @return     : n-dimensional array of double valued convolved data
 */
std::vector<std::vector<double>> Convolution::run_multi(vector<vector<double>> &data_in) {
    size_t n_columns = data_in[0].size(); // number of columns
    size_t n_rows = data_in.size(); // number of rows

//    cout << "rows " << n_rows << endl;
//    cout << "cols " << n_columns << endl;

    initialize(n_rows);
    vector<vector<double>> data_out(n_rows);

    size_t step = n_rows / 1000;
    auto t0 = chrono::system_clock::now();
    for (long row=0; row < n_rows; ++row){
        data_out[row].resize(n_columns); // space for columns
        double prob     = (double) row / n_rows;
        double factor   = 0;
        double binom    = 0;
        double prev     = 0;
        double binomNormalization_const = 1;
        ++_count;

        vector<double> sum(n_columns);
        for(size_t k{}; k < n_columns; ++k){
            sum[k] = data_in[row][k];
        }

//        cout << "line : " << __LINE__ << endl;

        // forward iteration part
        factor = prob / (1-prob);
        prev   = 1;

        for (long i=row+1; i < n_rows; ++i)
        {
            binom     = prev * _forward_factor[i] * factor;
            binomNormalization_const += binom;
            for(size_t j{}; j < n_columns; ++j){
                sum[j] += data_in[i][j] * binom;
            }
            prev      = binom;
        }
//        cout << "line : " << __LINE__ << endl;
        // backward iteration part
        factor = (1-prob)/prob;
        prev   = 1;

        for (long i=row-1; i>=0; --i)
        {
            binom     = prev * _backward_factor[i] * factor;
            binomNormalization_const += binom;
            for(size_t j{}; j < n_columns; ++j){
                sum[j] += data_in[i][j] * binom;
            }
            prev      = binom;
        }
//        cout << "line : " << __LINE__ << endl;
        // normalizing data
        for(size_t j{}; j < n_columns; ++j){
            data_out[row][j] = sum[j] / binomNormalization_const;
//            cout << "j " << j << endl;
        }
        if(row % step == 0) {
            cout << "\33[2K"; // erase the current line
            cout << '\r'; // return the cursor to the start of the line
//            cout << "row " << row << " ";
            cout << "progress " << row * 100 / double(n_rows) << " %";
            std::fflush(stdout);
        }
    }
    auto t1 = chrono::system_clock::now();
    _time_elapsed_convolution = chrono::duration<double>(t1 - t0).count();
    return data_out;
}


/**
 * Run convolution on array of multiple columns of data
 * OpenMP version
 * @param data_in : n-dimensional array of double valued data
 *      for example: following data has 3 columns and N rows
 *              0.25    0.454   0.548
 *              0.457   0.187   0.154
 *              0.578   0.951   0.487
 *              .       .       .
 *              .       .       .
 *              .       .       .
 *
 *        therefore shape will be
 *        data_in.size() == N
 *        data_in[0].size() == 3
 * @return     : n-dimensional array of double valued convolved data
 */
std::vector<std::vector<double>> Convolution::run_multi_omp(vector<vector<double>> &data_in) {
    size_t n_columns = data_in[0].size(); // number of columns
    size_t n_rows = data_in.size(); // number of rows

//    cout << "rows " << n_rows << endl;
//    cout << "cols " << n_columns << endl;

    initialize(n_rows);
    vector<vector<double>> data_out(n_rows);


    auto t0 = chrono::system_clock::now();

    // entering parallel region
    cout << endl;
    long step = n_rows / 1000 + 1;
#pragma omp parallel for schedule(dynamic) num_threads(_number_of_threads)
    for (long row=0; row < n_rows; ++row){
//        cout << "Threads " << omp_get_num_threads() << endl;
        data_out[row].resize(n_columns); // space for columns
        double prob     = (double) row / n_rows;
        double factor   = 0;
        double binom    = 0;
        double prev     = 0;
        double binomNormalization_const = 1;

        vector<double> sum(n_columns);
        for(size_t k{}; k < n_columns; ++k){
            sum[k] = data_in[row][k];
        }


        // forward iteration part
        factor = prob / (1-prob);
        prev   = 1;

        for (long i=row+1; i < n_rows; ++i)
        {
            binom     = prev * _forward_factor[i] * factor;
            binomNormalization_const += binom;
            for(size_t j{}; j < n_columns; ++j){
                sum[j] += data_in[i][j] * binom;
            }
            prev      = binom;
        }
        // backward iteration part
        factor = (1-prob)/prob;
        prev   = 1;

        for (long i=row-1; i>=0; --i)
        {
            binom     = prev * _backward_factor[i] * factor;
            binomNormalization_const += binom;
            for(size_t j{}; j < n_columns; ++j){
                sum[j] += data_in[i][j] * binom;
            }
            prev      = binom;
        }
        // normalizing data
        for(size_t j{}; j < n_columns; ++j){
            data_out[row][j] = sum[j] / binomNormalization_const;
        }
        if(row % step == 0) {
            cout << "\33[2K"; // erase the current line
            cout << '\r'; // return the cursor to the start of the line
            cout << "progress " << row * 100 / double(n_rows) << " %";
            std::fflush(stdout);
        }

    }
    auto t1 = chrono::system_clock::now();
    _time_elapsed_convolution = chrono::duration<double>(t1 - t0).count();
    cout << endl;
    return data_out;
}

std::vector<std::vector<double>> Convolution::run_multi_omp_v2(vector<vector<double>> &data_in) {
    size_t n_columns = data_in[0].size(); // number of columns
    size_t n_rows = data_in.size(); // number of rows

//    cout << "rows " << n_rows << endl;
//    cout << "cols " << n_columns << endl;

    initialize(n_rows);
    vector<vector<double>> data_out(n_rows);


    auto t0 = chrono::system_clock::now();

    // entering parallel region
    cout << endl;
    long step = n_rows / 1000 + 1;
#pragma omp parallel for schedule(dynamic) num_threads(_number_of_threads)
    for (long row=0; row < n_rows; ++row){
//        cout << "Threads " << omp_get_num_threads() << endl;
        data_out[row].resize(n_columns); // space for columns

        vector<double> sum;
        double binomNormalization_const = compute_for_row(data_in, n_columns, n_rows, row, sum);

        // normalizing data
        for(size_t j{}; j < n_columns; ++j){
            data_out[row][j] = sum[j] / binomNormalization_const;
        }
        if(row % step == 0) {
            cout << "\33[2K"; // erase the current line
            cout << '\r'; // return the cursor to the start of the line
            cout << "progress " << row * 100 / double(n_rows) << " %";
            std::fflush(stdout);
        }

    }
    auto t1 = chrono::system_clock::now();
    _time_elapsed_convolution = chrono::duration<double>(t1 - t0).count();
    cout << endl;
    return data_out;
}

double Convolution::compute_for_row(
        const vector<vector<double>> &data_in,
        size_t n_columns, size_t n_rows,
        long row, vector<double> &sum) const {
    double binomNormalization_const = 1;
    double prob     = (double) row / n_rows;
    double factor   = 0;
    double binom    = 0;
    double prev     = 0;
    for(size_t k{}; k < n_columns; ++k){
        sum[k] = data_in[row][k];
    }

//        cout << "line : " << __LINE__ << endl;

    // forward iteration part
    factor = prob / (1-prob);
    prev   = 1;

    for (long i=row+1; i < n_rows; ++i)
    {
        binom     = prev * _forward_factor[i] * factor;
        binomNormalization_const += binom;
        for(size_t j{}; j < n_columns; ++j){
            sum[j] += data_in[i][j] * binom;
        }
        prev      = binom;
    }
//        cout << "line : " << __LINE__ << endl;
    // backward iteration part
    factor = (1-prob)/prob;
    prev   = 1;

    for (long i=row-1; i>=0; --i) {
        binom     = prev * _backward_factor[i] * factor;
        binomNormalization_const += binom;
        for(size_t j{}; j < n_columns; ++j){
            sum[j] += data_in[i][j] * binom;
        }
        prev      = binom;
    }
    return binomNormalization_const;
}

/**
 * Run convolution on array of multiple columns of data.
 * pthread version.
 * @param data_in : n-dimensional array of double valued data
 *      for example: following data has 3 columns and N rows
 *              0.25    0.454   0.548
 *              0.457   0.187   0.154
 *              0.578   0.951   0.487
 *              .       .       .
 *              .       .       .
 *              .       .       .
 *
 *        therefore shape will be
 *        data_in.size() == N
 *        data_in[0].size() == 3
 * @return     : n-dimensional array of double valued convolved data
 */
std::vector<std::vector<double>> Convolution::run_multi_pthread(vector<vector<double>> &data_in) {
    size_t n_columns = data_in[0].size(); // number of columns
    size_t n_rows = data_in.size(); // number of rows

//    cout << "rows " << n_rows << endl;
//    cout << "cols " << n_columns << endl;
    initialize(n_rows);

    vector<vector<double>> data_out(n_rows);

    size_t step = n_rows / 1000;

    size_t number_of_threads = std::thread::hardware_concurrency(); // number of threads
    size_t loop_per_thread = N / number_of_threads;
    vector<thread> threads(number_of_threads); // holds the threads
    size_t start, stop;

    auto t0 = chrono::system_clock::now();
    for(size_t i{}; i != number_of_threads; ++i){
        start = loop_per_thread * i;
        stop  = loop_per_thread * (i+1);
        ////each thread finishes independently
        threads[i] = std::thread(
                &Convolution::convolution_multi_range,
                this,
                start,
                stop,
                std::ref(data_in),
                std::ref(data_out)
        );
        cout << "thread " << i << " : id " << threads[i].get_id() << " range " << start << " to " << stop << endl;

    }

    // join the threads
//    double prgrss{};
//    while (true){
//
//        prgrss = progress();
//        cout << "progress " << prgrss << endl;
//        if (prgrss > 0.99){
//            break;
//        }
//        std::this_thread::sleep_for(std::chrono::duration<double>(5));
//    }

    for(size_t i{}; i != number_of_threads; ++i){
        if(threads[i].joinable())
        {
            cout << "joining thread " << i << " : id " << threads[i].get_id() << endl;
            threads[i].join();
        }
    }

    auto t1 = chrono::system_clock::now();
    _time_elapsed_convolution = chrono::duration<double>(t1 - t0).count();
    return data_out;
}

void Convolution::convolution_multi_range(
        long row_start,
        long row_stop,
        const std::vector<std::vector<double>>  &data_in,
        std::vector<std::vector<double>> &data_out
) {
    size_t n_columns = data_in[0].size(); // number of columns
    size_t n_rows = data_in.size(); // number of rows
    for (long row=row_start; row < row_stop; ++row){
        data_out[row].resize(n_columns); // space for columns
        double prob     = (double) row / n_rows;
        double factor   = 0;
        double binom    = 0;
        double prev     = 0;
        double binomNormalization_const = 1;
        ++_count;

        vector<double> sum(n_columns);

        for(size_t k{}; k < n_columns; ++k){
            sum[k] = data_in[row][k];
        }

//        cout << "line : " << __LINE__ << endl;

        // forward iteration part
        factor = prob / (1-prob);
        prev   = 1;

        for (long i=row+1; i < n_rows; ++i)
        {
            binom     = prev * _forward_factor[i] * factor;
            binomNormalization_const += binom;
            for(size_t j{}; j < n_columns; ++j){
                sum[j] += data_in[i][j] * binom;
            }
            prev      = binom;
        }
//        cout << "line : " << __LINE__ << endl;
        // backward iteration part
        factor = (1-prob)/prob;
        prev   = 1;

        for (long i=row-1; i>=0; --i)
        {
            binom     = prev * _backward_factor[i] * factor;
            binomNormalization_const += binom;
            for(size_t j{}; j < n_columns; ++j){
                sum[j] += data_in[i][j] * binom;
            }
            prev      = binom;
        }
//        cout << "line : " << __LINE__ << endl;
        // normalizing data
        for(size_t j{}; j < n_columns; ++j){
            data_out[row][j] = sum[j] / binomNormalization_const;
//            cout << "j " << j << endl;
        }

    }

}

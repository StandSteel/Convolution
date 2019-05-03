//
// Created by shahnoor on 2/1/2018.
//

#include <cmath>
#include <iostream>
#include <thread>
#include <mutex>
#include <omp.h>
#include "convolution.h"
#include "binomial.h"

using namespace std;


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
//        cout << bn_tot << endl;
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
#pragma omp parallel for schedule(dynamic)
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

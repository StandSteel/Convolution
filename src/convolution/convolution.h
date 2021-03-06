//
// Created by shahnoor on 2/1/2018.
//

#ifndef CONVOLUTION_CONVOLUTION_H
#define CONVOLUTION_CONVOLUTION_H

#include <vector>
#include <cstddef>
#include <iostream>


/**
 * Combitable with OpenMP and OpenACC. Flags must be provided during compiletime
 */
std::vector<double> convolve_1d(std::vector<double>& data_in, int thread_count=1);
std::vector<std::vector<double>> convolve_2d(std::vector<std::vector<double>> &data_in, int thread_count=1);

std::vector<double> convolve_1d_fast(
        std::vector<double>& data_in,
        int thread_count=1,
        double threshold=1e-15);

std::vector<std::vector<double>> convolve_2d_fast(
        std::vector<std::vector<double>> &data_in,
        int thread_count=1,
        double threshold=1e-15);

/***
 * Perform derivative along with convolution
 * **/
double D_1i(long i, size_t N, double p); // first derivative coefficient
double D_2i(long i, size_t N, double p); // second derivative coefficient

std::vector<double> convolve_1d_fast_diff(
        std::vector<double> &data_in,
        int thread_count = 1, int diff = 1,
        double threshold = 1e-15);

std::vector<std::vector<double>> convolve_2d_fast_diff(
        std::vector<std::vector<double>> &data_in,
        int thread_count=1, int diff = 1,
        double threshold=1e-15);
/**
 * A Class to make using convolution user friendly
 */
class Convolution{
    std::vector<double> _forward_factor;
    std::vector<double> _backward_factor;
    size_t N{};

    double _count{}; // keep record of progress percentage. very useful for multithreaded environment ???
    double _time_elapsed_initialization{};
    double _time_elapsed_convolution{};
    int _number_of_threads{1};
public:
    ~Convolution() = default;

    explicit Convolution(int threads=-1);

    std::vector<double> factor_forward() const { return _forward_factor;}
    std::vector<double> factor_backward() const { return _backward_factor;}

    // array of values
    // single column version
    std::vector<double> run(std::vector<double>& data_in); // single column and single threaded version
    std::vector<double> run_omp(std::vector<double>& data_in);
    std::vector<double> run_acc(std::vector<double>& data_in);
    std::vector<double> run_pthread(std::vector<double> &data_in);

    // array of columns of values
    // multiple column version
    std::vector<std::vector<double>> run_multi(std::vector<std::vector<double>>& data_in);
    std::vector<std::vector<double>> run_multi_omp(std::vector<std::vector<double>>& data_in);
    std::vector<std::vector<double>> run_multi_omp_v2(std::vector<std::vector<double>>& data_in);
    std::vector<std::vector<double>> run_multi_pthread(std::vector<std::vector<double>>& data_in);

    void timeElapsed() const {
        std::cout << "Initialization time " << _time_elapsed_initialization << " sec" << std::endl;
        std::cout << "Convolution time " << _time_elapsed_convolution << " sec" << std::endl;
    }

    double progress() {return _count/N;}
private:
    void initialize(size_t n) ;

    void convolution_single_range(long row_start, long row_stop, const std::vector<double> &data_in,
                                  std::vector<double> &data_out);

    void convolution_multi_range(long row_start, long row_stop, const std::vector<std::vector<double>>  &data_in,
                                 std::vector<std::vector<double>> &data_out);

    double compute_for_row(const std::vector<std::vector<double>> &data_in, size_t n_columns, size_t n_rows, long row,
                           std::vector<double> &sum) const;
};


#endif //CONVOLUTION_CONVOLUTION_H

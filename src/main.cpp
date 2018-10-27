#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>
#include <cstring>

#include "convolution/binomial.h"
#include "convolution/convolution.h"
#include "data_reader.h"
#include "printer.h"
#include "string_methods.h"


using namespace std;


/**
 * string to int conversion. using recursion.
 * @param str
 * @param h
 * @return
 */
constexpr unsigned int str2int(const char* str, int h=0)
{
    return !str[h] ? 1 : (str2int(str, h+1) * 2) ^ str[h];
}

/**
 * string to int conversion. using loop.
 * @param str
 * @return
 */
unsigned int str_to_int(const char* str)
{
    unsigned x = 1;
    int len = strlen(str);
    // cout << "len, str " << len << ", " << str << endl;
    for(int i=len-1; i>=0 ; --i){
        x *= 2;
        x = x ^ str[i];
    }
    // cout << x << endl;
    return x;
}

void version(){
    cout << "version 1.0" << endl;
}

void help(){
    string hlp = R"***(Usage:
convolution [-f <STRING>] [-a <INT>,<INT>,...[:[<STRING>,<STRING>,...]]] [-b <INT>,<INT>,...[:[<STRING>,<STRING>,...]]] [-h] [-t <INT>] [-i <STRING>]

perform convolution based on provided options.

Options                      Description
  -a,                        columns that we want in the output file without performing convolution.
                             No default value.
  -b,                        columns that we want in the output file with performing convolution.
                             No default value.
  -c,                        If provided the header and comment from the input file will be written
                             without modification to the output file. Header is the first line of the
                             input file.
  -d                         Delimeter to use. Default value is ' '.
  -f                         name of the input file that we want to convolute. No default value.
  -i                         Info to write as comment in the output file
  -o                         name of the output file. If not provided the string '_convoluted.txt' will be
                             appended to the input file.
  -p, --precision            Floating point precision when writing in the data file. Default value is 10
  -s                         Number of rows to skip from the input file. Default value is 0.
  -t                         to test the performance of the convolution program. No default value.
      --threads              Explicitly specify number of thread to use. Default is the max number of thread
                             allowed by the system.
  -h, --help                 display this help and exit
  -v, --version              output version information and exit
  -w                         If provided input b data will be written to the output file.


The INT argument is an integer.
The STRING argument is a string of characters.

A line that begins with '#' is considered a commented line.

Exit status:
 0  if OK,
 1  if minor problems (e.g., cannot access subdirectory),
 2  if serious trouble (e.g., cannot access command-line argument).
)***";
    cout << hlp << endl;
}


//map<string,string>
//parse_argument(int argc, char* argv[]){
//    map<string,string> arguemnts;
////    arguemnts["in_filename"] = "";
////    arguemnts["out_filename"] = "";
////    arguemnts["in_filename"] = "";
////    arguemnts["in_filename"] = "";
////    arguemnts["in_filename"] = "";
////    arguemnts["in_filename"] = "";
//
//    cout << "Arguments" << endl;
//
//    size_t test_size{0};
//    vector<int> a_usecols, b_usecols; // b_usecols will be convolved and a_usecols will remain unchanged
//    vector<string> a_names, b_names;
//    string info;
//    bool write_header_and_comment{false};
//    int skiprows{0};
//    char delimeter{' '};
//    bool write_input_data {false};
//    int flg;
//    if(argc == 1){
//        help();
//        exit(0);
//    }
//    for(int i{1}; i < argc;){
//        cout << argv[i] << endl;
//
//        flg = str2int(argv[i]);
//        switch (flg){
//
//            case str2int("-a"):
//                cout << "parse : line " << __LINE__ << endl;
//                ++i;
//                if(i < argc) {
//                    string tmp = argv[i];
//                    cout << tmp << endl;
//                    int sep = tmp.find(':');
//                    if(sep < 0){
//                        // no name is provided
//                        a_usecols = explode_to_int(tmp, ',');
//                        for(size_t k{}; k < a_usecols.size(); ++k){
//                            a_names.push_back("<>");
//                        }
//                    }else {
//                        string first = tmp.substr(0, sep);
//
//                        string second = tmp.substr(sep + 1);
////                        cout << "first " << first << endl;
////                        cout << "second " << second << endl;
//
//                        a_usecols = explode_to_int(first, ',');
//                        a_names = explode_to_string(second, ',');
//                        if(a_names.size() != a_usecols.size()){
//                            cout << "not enough column index or name" ;
//                            cout << ": line " << __LINE__ ; // comment this on deployment
//                            cout << endl;
//                        }
//                    }
//                }
//                ++i;
//                break;
//            case str2int("-b"):
//                cout << "parse : line " << __LINE__ << endl;
//                ++i;
//                if(i < argc) {
//                    string tmp = argv[i];
//                    cout << tmp << endl;
//                    int sep = tmp.find(':');
//                    if(sep < 0){
//                        // no name is provided
//                        b_usecols = explode_to_int(tmp, ',');
//                        for(size_t k{}; k < b_usecols.size(); ++k){
//                            b_names.push_back("<>");
//                        }
//                    }else {
//                        string first = tmp.substr(0, sep);
//
//                        string second = tmp.substr(sep + 1);
////                        cout << "first " << first << endl;
////                        cout << "second " << second << endl;
//
//                        b_usecols = explode_to_int(first, ',');
//                        b_names = explode_to_string(second, ',');
//                        if(b_names.size() != b_usecols.size()){
//                            cout << "not enough column index or name" ;
//                            cout << ": line " << __LINE__ ; // comment this on deployment
//                            cout << endl;
//                        }
//                    }
//                }
//                ++i;
//                break;
//            case str2int("-c"):
//                write_header_and_comment = true;
//                ++i;
//                break;
//            case str2int("-d"):
//                ++i;
//                if(i < argc) {
//                    delimeter = argv[i][0];
//                }
//                ++i;
//                break;
//            case str2int("-f"):
//                cout << "parse : line " << __LINE__ << endl;
//                ++i;
//                if(i < argc) {
//                    arguemnts["in_filename"] = argv[i];
//                }
//                ++i;
//                break;
//            case str2int("-i"):
//                ++i;
//                if(i < argc) {
//                    info = argv[i];
//                }
//                ++i;
//                break;
//
//            case str2int("-o"):
//                ++i;
//                if(i < argc) {
//                    arguemnts["out_filename"] = argv[i];
//                }
//                ++i;
//                break;
//            case str2int("-s"):
//                ++i;
//                if(i < argc) {
//                    skiprows = stoi(argv[i]);
//                }
//                ++i;
//                break;
//            case str2int("-t"):
//                ++i;
//                if(i < argc) {
//                    arguemnts["test_size"] = (size_t)stoi(argv[i]);
//                }
//                ++i;
//                break;
//            case str2int("-h"):
//            case str2int("--help"):
//                help();
//                exit(0);
//                break;
//            case str2int("-v"):
//            case str2int("--version"):
//                version();
//                exit(0);
//                break;
//            case str2int("-w"):
//                write_input_data = true;
//                break;
//            default:
//                help();
//                exit(0);
//        }
//
//    }
//
////    print_vector(a_usecols);
////    print_vector(a_names);
////    print_vector(b_usecols);
////    print_vector(b_names);
//
//    if(out_filename.empty()){
//        out_filename = in_filename + out_file_flag;
//    }
//}

void cmd_args(int argc, char* argv[]){
    cout << "Arguments" << endl;
    string in_filename, out_filename;
    string out_file_flag = "_convoluted.txt";
    int header_line{0};
    size_t test_size{0};
    vector<int> a_usecols, b_usecols; // b_usecols will be convolved and a_usecols will remain unchanged
    vector<string> a_names, b_names;
    string info;
    bool write_header_and_comment{false};
    int skiprows{0};
    char delimeter{' '};
    bool write_input_data {false};
    int flg;
    int f_precision{10};
    int n_threads{-1};
    if(argc == 1){
        help();
        exit(0);
    }
    for(int i{1}; i < argc;){
        cout << argv[i] << endl;

        flg = str2int(argv[i]);
        switch (flg){

            case str2int("-a"):
                cout << "parse : line " << __LINE__ << endl;
                ++i;
                if(i < argc) {
                    string tmp = argv[i];
                    cout << tmp << endl;
                    int sep = tmp.find(':');
                    if(sep < 0){
                        // no name is provided
                        a_usecols = explode_to_int(tmp, ',');
                        for(size_t k{}; k < a_usecols.size(); ++k){
                            a_names.push_back("<>");
                        }
                    }else {
                        string first = tmp.substr(0, sep);

                        string second = tmp.substr(sep + 1);
//                        cout << "first " << first << endl;
//                        cout << "second " << second << endl;

                        a_usecols = explode_to_int(first, ',');
                        a_names = explode_to_string(second, ',');
                        if(a_names.size() != a_usecols.size()){
                            cout << "not enough column index or name" ;
                            cout << ": line " << __LINE__ ; // comment this on deployment
                            cout << endl;
                        }
                    }
                }
                ++i;
                break;
            case str2int("-b"):
                cout << "parse : line " << __LINE__ << endl;
                ++i;
                if(i < argc) {
                    string tmp = argv[i];
                    cout << tmp << endl;
                    int sep = tmp.find(':');
                    if(sep < 0){
                        // no name is provided
                        b_usecols = explode_to_int(tmp, ',');
                        for(size_t k{}; k < b_usecols.size(); ++k){
                            b_names.push_back("<>");
                        }
                    }else {
                        string first = tmp.substr(0, sep);

                        string second = tmp.substr(sep + 1);
//                        cout << "first " << first << endl;
//                        cout << "second " << second << endl;

                        b_usecols = explode_to_int(first, ',');
                        b_names = explode_to_string(second, ',');
                        if(b_names.size() != b_usecols.size()){
                            cout << "not enough column index or name" ;
                            cout << ": line " << __LINE__ ; // comment this on deployment
                            cout << endl;
                        }
                    }
                }
                ++i;
                break;
            case str2int("-c"):
                write_header_and_comment = true;
                ++i;
                break;
            case str2int("-d"):
                ++i;
                if(i < argc) {
                    delimeter = argv[i][0];
                }
                ++i;
                break;
            case str2int("-f"):
                cout << "parse : line " << __LINE__ << endl;
                ++i;
                if(i < argc) {
                    in_filename = argv[i];
                    cout << in_filename << endl;
                }
                ++i;
                break;
            case str2int("-i"):
                ++i;
                if(i < argc) {
                    info = argv[i];
                }
                ++i;
                break;

            case str2int("-o"):
                ++i;
                if(i < argc) {
                    out_filename = argv[i];
                }
                ++i;
                break;
            case str2int("-p"):
            case str2int("--precision"):
                ++i;
                if(i < argc) {
                    f_precision = stoi(argv[i]);
                }
                ++i;
                break;
            case str2int("-s"):
                ++i;
                if(i < argc) {
                    skiprows = stoi(argv[i]);
                }
                ++i;
                break;
            case str2int("-t"):
                ++i;
                if(i < argc) {
                    test_size = (size_t)stoi(argv[i]);
                }
                ++i;
                break;
            case str2int("--threads"):
                ++i;
                if(i < argc) {
                    n_threads = stoi(argv[i]);
                }
                ++i;
                break;
            case str2int("-h"):
            case str2int("--help"):
                help();
                exit(0);
                break;
            case str2int("-v"):
            case str2int("--version"):
                version();
                exit(0);
                break;
            case str2int("-w"):
                write_input_data = true;
                break;
            default:
                help();
                exit(0);
        }

    }

//    print_vector(a_usecols);
//    print_vector(a_names);
//    print_vector(b_usecols);
//    print_vector(b_names);

    if(out_filename.empty()){
        out_filename = in_filename + out_file_flag;
    }

    // reading input file
    vector<vector<double>> a_data = loadtxt(in_filename, a_usecols, skiprows, delimeter);
    vector<vector<double>> b_data_in = loadtxt(in_filename, b_usecols, skiprows, delimeter);

    // performing convolution
    Convolution conv(n_threads);
    vector<vector<double>> b_data_out = conv.run_multi_omp(b_data_in);
//    vector<vector<double>> b_data_out = conv.run_multi_omp_v2(b_data_in);
    conv.timeElapsed();

    // writing output to file
    savetxt_multi(in_filename,
                  out_filename,
                  info,
                  write_header_and_comment,
                  delimeter,
                  write_input_data,
                  a_data,
                  b_data_in,
                  b_data_out,
                  f_precision);

}



/***
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]) {
    cout << "Convolution of big data" << endl;

    auto t0 = std::chrono::system_clock::now();

    cmd_args(argc, argv);

    auto t1 = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = t1-t0;
    std::time_t end_time = std::chrono::system_clock::to_time_t(t1);


    cout << "Program finished at " << std::ctime(&end_time) << endl;
    cout << "Total Time elapsed " << elapsed_seconds.count()/60 << " minutes" << endl;
    return 0;
}

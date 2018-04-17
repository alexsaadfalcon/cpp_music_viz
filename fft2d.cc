// Distributed two-dimensional Discrete FFT transform
// YOUR NAME HERE

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <thread>
#include <cmath>
#include <mutex>

#include "Complex.h"
#include "InputImage.h"

constexpr unsigned int NUMTHREADS = 100;

using namespace std;
double pi = M_PI;
std::mutex mut;

//undergrad students can assume NUMTHREADS will evenly divide the number of rows in tested images
//graduate students should assume NUMTHREADS will not always evenly divide the number of rows in tested images.
// I will test with a different image than the one given
void Transform1D(Complex* img, int rows, int cols, Complex* H, int thread_num, int num_threads, bool row_trans);
void inv1D(Complex* img, int rows, int cols, Complex* H, int thread_num, int num_threads, bool row_transform);


void Transform2D(const char* inputFN) {
    // Do the 2D transform here.
    // 1) Use the InputImage object to read in the Tower.txt file and
    //    find the width/height of the input image.
  // 2) Create a vector of complex objects of size width * height to hold
  //    values calculated
  // 3) Do the individual 1D transforms on the rows assigned to each thread
  // 4) Force each thread to wait until all threads have completed their row calculations
  //    prior to starting column calculations
  // 5) Perform column calculations
  // 6) Wait for all column calculations to complete
  // 7) Use SaveImageData() to output the final results
    InputImage image(inputFN);  // Create the helper object for reading the image


    int rows = image.GetHeight();
    int cols = image.GetWidth();
    int num_threads = NUMTHREADS;
    std::thread threads[num_threads];

    Complex* img = image.GetImageData();
    Complex* row_transform = new Complex[rows * cols];
    Complex* col_transform = new Complex[rows * cols];
    Complex* col_transform2 = new Complex[rows * cols];
    Complex* original = new Complex[rows * cols];


    for (int i = 0; i < num_threads; ++i) {
      threads[i] = std::thread(Transform1D, img, rows, cols, row_transform, i, num_threads, true);
    }
    for (int i = 0; i < num_threads; ++i){
        threads[i].join();
        //h[i].get();
    }
    image.SaveImageData("asdf.txt", row_transform, cols, rows);

    for (int i = 0; i < num_threads; ++i) {
        threads[i] = std::thread(Transform1D, row_transform, rows, cols, col_transform, i, num_threads, false);
    }
    for (int i = 0; i < num_threads; ++i){
        threads[i].join();
        //h[i].get();
    }
    /*
    Complex* tot_transform = new Complex[rows * cols];
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            tot_transform[i*cols + j] = row_transform[i*cols + j] * col_transform[i*cols + j];
        }
    }*/
    image.SaveImageData("MyAfter2D.txt", col_transform, cols, rows);


    //                  INVERSE

    for (int i = 0; i < num_threads; ++i) {
        threads[i] = std::thread(inv1D, col_transform, rows, cols, col_transform2, i, num_threads, true);
    }
    for (int i = 0; i < num_threads; ++i){
        threads[i].join();
        //h[i].get();
    }

    for (int i = 0; i < num_threads; ++i) {
        threads[i] = std::thread(inv1D, col_transform2, rows, cols, original, i, num_threads, false);
    }
    for (int i = 0; i < num_threads; ++i){
        threads[i].join();
        //h[i].get();
    }
    image.SaveImageDataReal("MyAfterInverse.txt", original, cols, rows);
}

void Transform1D(Complex* img, int rows, int cols, Complex* H, int thread_num, int num_threads, bool row_transform)
{
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
    if (row_transform) {
        for (int row = rows * thread_num / num_threads; row < rows * (thread_num+1) / num_threads; ++row) {
            //std::cout << "ROW" << row << std::endl;
            double theta;
            double real;
            double imag;
            Complex sum(0, 0);
            for (int n = 0; n < cols; ++n) {
                for (int k = 0; k < cols; ++k) {
                    theta = (-2 * pi * n * k) / cols;
                    real = std::cos(theta);
                    imag = std::sin(theta);
                    sum = sum + (Complex(real, imag) * img[row * cols + k]);
                }
                mut.lock();
                //std::cout << row*cols + n << std::endl;
                H[row * cols + n] = sum;
                sum = Complex(0, 0);
                mut.unlock();
            }
        }
    } else {
        for (int col = cols * thread_num / num_threads; col < cols * (thread_num+1) / num_threads; ++col) {
            //std::cout << "ROW" << row << std::endl;
            double theta;
            double real;
            double imag;
            Complex sum(0, 0);
            for (int n = 0; n < rows; ++n) {
                for (int k = 0; k < rows; ++k) {
                    theta = (-2 * pi * n * k) / rows;
                    real = std::cos(theta);
                    imag = std::sin(theta);
                    sum = sum + (Complex(real, imag) * img[k * cols + col]);
                }
                mut.lock();
                //std::cout << row*cols + n << std::endl;
                H[n * cols + col] = sum;
                sum = Complex(0, 0);
                mut.unlock();
            }
        }
    }
}

void inv1D(Complex* img, int rows, int cols, Complex* H, int thread_num, int num_threads, bool row_transform)
{
    // Implement a simple 1-d DFT using the double summation equation
    // given in the assignment handout.  h is the time-domain input
    // data, w is the width (N), and H is the output array.
    if (row_transform) {
        for (int row = rows * thread_num / num_threads; row < rows * (thread_num+1) / num_threads; ++row) {
            //std::cout << "ROW" << row << std::endl;
            double theta;
            double real;
            double imag;
            Complex sum(0, 0);
            for (int n = 0; n < cols; ++n) {
                for (int k = 0; k < cols; ++k) {
                    theta = (2 * pi * n * k) / cols;
                    real = std::cos(theta);
                    imag = std::sin(theta);
                    sum = sum + (Complex(real, imag) * img[row * cols + k]);
                }
                sum = sum * Complex(1.0/rows, 0);
                mut.lock();
                //std::cout << row*cols + n << std::endl;
                H[row * cols + n] = sum;
                sum = Complex(0, 0);
                mut.unlock();
            }
        }
    } else {
        for (int col = cols * thread_num / num_threads; col < cols * (thread_num+1) / num_threads; ++col) {
            //std::cout << "ROW" << row << std::endl;
            double theta;
            double real;
            double imag;
            Complex sum(0, 0);
            for (int n = 0; n < rows; ++n) {
                for (int k = 0; k < rows; ++k) {
                    theta = (2 * pi * n * k) / rows;
                    real = std::cos(theta);
                    imag = std::sin(theta);
                    sum = sum + (Complex(real, imag) * img[k * cols + col]);
                }
                sum = sum * Complex(1.0/cols, 0);
                mut.lock();
                //std::cout << row*cols + n << std::endl;
                H[n * cols + col] = sum;
                sum = Complex(0, 0);
                mut.unlock();
            }
        }
    }
}

int main(int argc, char** argv)
{
  string fn("../Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  Transform2D(fn.c_str()); // Perform the transform.
}  
  

  

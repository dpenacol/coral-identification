/********************************************
 * FILE NAME: filters.h                     *
 * DESCRIPTION: Contains functions that are *
 *              useful for defining and     *
 *              using filters.              *
 * VERSION: 2.0 (11-NOV-2017)               *
 * AUTHORS: Diego Peña, Victor García,      *
 *          Fabio Morales, Andreina Duarte  *
 ********************************************/

#ifndef FILTERS_H
#define FILTERS_H
// We need to define this in order to use M_PI
#define _USE_MATH_DEFINES

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>
//#include <iostream>
#include <string>

void getNormalizedGaussianKernel(cv::Mat kernel, float sigma);
// Computes the kernel of a gaussian filter, with standard
// deviation sigma, normalized so that its mean is 0 and
// its L1 norm is 1.
// The traditional gaussian kernel is:
//      k(x,y) = [ 1 / (2*pi*sigma^2) ] * e^{-(x^2 + y^2)/(2*sigma^2)}
// CONSTRAINTS:
//      * kernel's depth must be CV_32FC1 (float).
//      * kernel's height and width must be odd.

void getNormalizedLoGKernel(cv::Mat kernel, float sigma);
// Computes the kernel of a LoG (Laplacian of Gaussian) filter, with
// standard deviation sigma, normalized so that its mean is 0 and
// its L1 norm is 1.
// The traditional LoG kernel is:
//      k(x,y) = [ (x^2 + y^2 - 2*sigma^2) / (2*pi*sigma^4) ] * e^{-(x^2 + y^2)/(2*sigma^2)}
// CONSTRAINTS:
//      * kernel's depth must be CV_32FC1 (float).
//      * kernel's height and width must be odd.

void getNormalizedDoGKernel(cv::Mat kernel, float sigma, float angle);
// Computes the kernel of a Derivative of Gaussian filter, oriented
// in a direction that makes an angle "angle" with the horizontal,
// normalized so that its mean is 0 and its L1 norm is 1. If angle=0,
// the filter is oriented in the horizontal direction and its kernel is,
// before normalizing:
//      k(x,y) = sigma * { \partial g(x,3*sigma)*g(y,sigma) } / (\partial y)
// where g(z,t) is a 1D gaussian distribution with mean=0 and variance=t^2.
// Therefore, its equation, before normalizing, is:
//      k(x,y) = [ -y / (6*pi*sigma^3) ] * e^{-(x^2 + 9*y^2)/(18*sigma^2)}
// The general kernel (i.e., when angle!=0), before normaliing, can be
// obtained with the following substitutions:
//      x --> x*cos(angle) - y*sin*angle)
//      y --> x*sin(angle) + y*cos(angle)
// CONSTRAINTS:
//      * kernel's depth must be CV_32FC1 (float).
//      * kernel's height and width must be odd.

void getNormalizedSDoGKernel(cv::Mat kernel, float sigma, float angle);
// Computes the kernel of a Second Derivative of Gaussian filter, oriented
// in a direction that makes an angle "angle" with the horizontal,
// normalized so that its mean is 0 and its L1 norm is 1. If angle=0,
// the filter is oriented in the horizontal direction and its kernel is,
// before normalizing:
//      k(x,y) = sigma^2 * { \partial^2 g(x,3*sigma)*g(y,sigma) } / (\partial y^2)
// where g(z,t) is a 1D gaussian distribution with mean=0 and variance=t^2.
// Therefore, its equation, before normalizing, is:
//      k(x,y) = [ y^2 - sigma^2 / (6*pi*sigma^4) ] * e^{-(x^2 + 9*y^2)/(18*sigma^2)}
// The general kernel (i.e., when angle!=0), before normaliing, can be
// obtained with the following substitutions:
//      x --> x*cos(angle) - y*sin*angle)
//      y --> x*sin(angle) + y*cos(angle)
// CONSTRAINTS:
//      * kernel's depth must be CV_32FC1 (float).
//      * kernel's height and width must be odd.

void printKernel(cv::Mat kernel, std::string filename);
// Creates an image that represents the kernel, and saves it
// in a file named filename. The values in kernel's pixels are
// linearly scaled in a way that transforms its maximum to 255
// and its minimum t0 0.
// CONSTRAINTS:
//      * kernel's depth must be CV_32FC1 (float).

void getMaximumResponseFilterKernels(cv::Mat kernel[38], float sigmaSmall, float sigmaMedium, float sigmaLarge, float sigmaSymmetrical);
// Computes the kernels needed for the Maximum Response Filter.
// The computed kernels are:
//      -  0- 5: DoG, small scale (sigma = sigmaSmall)
//      -  6-11: DoG, medium scale (sigma = sigmaMedium)
//      - 12-17: DoG, large scale (sigma = sigmaLarge)
//      - 18-23: SDoG, small scale (sigma = sigmaSmall)
//      - 24-29: SDoG, medium scale (sigma = sigmaMedium)
//      - 30-35: SDoG, large scale (sigma = sigmaLarge)
//      -    36: Gaussian  (sigma = sigmaSymmetrical)
//      -    37: LoG (sigma = sigmaSymmetrical)
// CONSTRAINTS:
//      * kernel[i]'s depth must be CV_32FC1 (float).
//      * kernel[i]'s height and width must be odd.

void maximumResponseFilter(cv::Mat img_Lab, cv::Mat img_MR, cv::Mat kernel[38]);
// Applies the Maximum Response filter to img_Lab, and saves its
// response in img_MR. This function needs to receive the kernels
// of the 38 filters as an input.
// CONSTRAINTS:
//      * img_Lab's depth must be CV_8UC3.
//      * img_MR's depth must be CV_32FC(24).
//      * img_Lab and img_MR must have the same dimensions
//      * kernel[i]'s depth must be CV_32FC1 (float).
//      * The 38 kernels should be ordered as follows
//          -  0- 5: DoG, small scale
//          -  6-11: DoG, medium scale
//          - 12-17: DoG, large scale
//          - 18-23: SDoG, small scale
//          - 24-29: SDoG, medium scale
//          - 30-35: SDoG, large scale
//          -    36: Gaussian
//          -    37: LoG

cv::Mat getMaximumResponseFilter(std::string filename);
#endif

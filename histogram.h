/********************************************
 * FILE NAME: histogram.h                   *
 * DESCRIPTION: Contains functions that are *
 *              useful for working with     *
 *              histograms                  *
 * VERSION: 1.0 (02-NOV-2017)               *
 * AUTHORS: Diego Peña, Victor García,      *
 *          Fabio Morales, Andreina Duarte  *
 ********************************************/

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
//#include <iostream>

void getHistogram(cv::Mat img, int histogram[3][256]);
// Computes the intensity distribution histograms for the
// three channels of the image img, and saves them in
// Histogram[3][256].
//      Histogram[0] corresponds to the Blue channel
//      Histogram[1] corresponds to the Green channel
//      Histogram[2] corresponds to the Red channel

void printHistogram(int histogram[256], std::string filename, cv::Scalar color);
// Creates an image that represents the Histogram, and saves it
// in a file named filename. The bins are colored with the specified
// color. The background is white.

void colorChannelStretch(cv::Mat imgOriginal, cv::Mat imgStretched, int lowerPercentile, int higherPercentile);
// Transform imgOriginal so that, for each channel histogram, its
// lowerPercentile and higherPercentile values are moved to 0 and 255,
// respectively. Values in between are linearly scaled. Values smaller
// than lowerPercentile are set to 0, and values greater than
// higherPercentle are set to 1. The resulting image is saved in
// imgStretched.
// CONSTRAINTS:
//      * imgOriginal and imgStretched must have the same dimensions.
//      * lowerPercentile and higherPercentle must be integers between
//        0 and 100, and lowerPercentile must be smaller than
//        higherPercentile

#endif
/********************************************
 * FILE NAME: image.h                       *
 * DESCRIPTION: Contains functions that are *
 *              useful to read data from    *
 *              images                      *
 * AUTHORS: Diego Peña, Victor García,      *
 *          Fabio Morales, Andreina Duarte  *
 ********************************************/

#ifndef IMAGE_H
#define IMAGE_H

#define maxKeypoints 200
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <string>

struct keyPoint{
    cv::Point pt;
    int type;
    float r24[24];
};

struct img_data{
    int index;
    int year;
    int n_labels;
    struct keyPoint key_Point[maxKeypoints];
};

void testTxt(void);

struct img_data getDescriptor(std::string fileName,cv::Mat img_MR, int year, int index);
// Load the information of all the clasification point from a text file, and associate 
// an r24 vector from the Maximun Response filter.

void saveDescriptor(struct img_data data);
// Save the entire information of an image, include the coordinate, it's label
// and the associated r24 vector from the Maximum Response filter in a binary file.

struct img_data loadDescriptor();
// Load the entire information of an image, include the coordinate, it's label
// and the associated r24 vector from a binary file.

void getDictionaryTextons(cv::Mat dictionaryTextons, struct img_data data[200], int start_index, int finish_index);

#endif
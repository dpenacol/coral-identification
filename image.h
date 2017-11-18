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
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <string>

struct keyPoint{
    cv::Point pt;
    int8_t type;
    float r24[24];
};

struct img_data{
    uint16_t index;
    uint16_t year;
    uint16_t n_labels;
    struct keyPoint key_Point[maxKeypoints];
};

struct keyPointHistogram{
    cv::Point pt;
    int8_t type;
    float histogram[540];
};

struct img_dataHistogram{
    uint16_t index;
    uint16_t year;
    uint16_t n_labels;
    struct keyPointHistogram key_Point[maxKeypoints];
};

void testTxt(void);

uint8_t str2label(std::string str);

struct img_data getDescriptor(std::string fileName,cv::Mat img_MR, uint16_t year, uint16_t index);
// Load the information of all the clasification point from a text file, and associate 
// an r24 vector from the Maximun Response filter.

void saveDescriptor(struct img_data* data, uint16_t n_images);
// Save the entire information of an image, include the coordinate, it's label
// and the associated r24 vector from the Maximum Response filter in a binary file.

bool saveDictionaryTextons(cv::Mat dictionary, std::string path);

struct img_data* loadDescriptor(uint16_t n_images);
// Load the entire information of an image, include the coordinate, it's label
// and the associated r24 vector from a binary file.

bool loadDictionaryTextons(cv::Mat dictionary, std::string path);

void getDictionaryTextons(cv::Mat dictionaryTextons, struct img_data data[2055], uint16_t start_index, uint16_t finish_index);
// Computes the K-means algorithm to each class creating a 
// matrix with 135 textons each one with 24 values.

uint8_t porcentage(uint16_t index_data, uint16_t n_images);
// Shows porcentage of completition of the getDataSet function

bool getDataSet(struct img_data* data, uint16_t n_images);
// Load the Data of the images and the txt of the three 2008, 2009 and 2010 sets of images.
// The sets needs to be saved on the project folder by this structure:
// ~/ProjectRoot
//        /Vision_MCR
//             /2008
//             /2009
//             /2010

uint8_t getNearestTexton(cv::Mat dictionaryTextons, float r24[24]);

bool getDataHistogram(struct img_dataHistogram* dataH, cv::Mat dictionary, uint16_t n_images);

struct img_dataHistogram getHistogramDescriptor(std::string fileName, cv::Mat img_MR, cv::Mat dictionary, uint16_t year, uint16_t index);

void getPatchs(cv::Mat img_MR, cv::Mat dictionary, struct keyPointHistogram* key_Point);

void getHistogramTextons(cv::Mat img, uint16_t histogram[][135], uint8_t hSize[4]);

void normalizeHistogramsTextons(uint16_t histograms[][135] , float histogram[540]);

int checkPatchCompatibility(struct img_data* data, int n_images, int max_patch);
#endif
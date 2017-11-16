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

int str2label(std::string str);

struct img_data getDescriptor(std::string fileName,cv::Mat img_MR, int year, int index);
// Load the information of all the clasification point from a text file, and associate 
// an r24 vector from the Maximun Response filter.

void saveDescriptor(struct img_data* data, int n_images);
// Save the entire information of an image, include the coordinate, it's label
// and the associated r24 vector from the Maximum Response filter in a binary file.

bool saveDictionaryTextons(cv::Mat dictionary, std::string path);

struct img_data* loadDescriptor(int n_images);
// Load the entire information of an image, include the coordinate, it's label
// and the associated r24 vector from a binary file.

bool loadDictionaryTextons(cv::Mat dictionary, std::string path);

void getDictionaryTextons(cv::Mat dictionaryTextons, struct img_data data[200], int start_index, int finish_index);
// Computes the K-means algorithm to each class creating a 
// matrix with 135 textons each one with 24 values.

int porcentage(int index_data, int n_images);
// Shows porcentage of completition of the getDataSet function

bool getDataSet(struct img_data* data, int n_images);
// Load the Data of the images and the txt of the three 2008, 2009 and 2010 sets of images.
// The sets needs to be saved on the project folder by this structure:
// ~/ProjectRoot
//        /Vision_MCR
//             /2008
//             /2009
//             /2010

#endif
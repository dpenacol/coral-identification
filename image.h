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
#define _nImg_2008 671
#define _nImg_2009 695
#define _nImg_2010 689
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h> 
#include <iomanip>
#include <vector>
// Color for terminal output
const std::string green("\033[1;32m");
const std::string reset("\033[0m");

// Global parameters of Kmeans Clustering
struct kmeans_param{
    int attempts;
    int iterations;
    int epsilon;
};

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

struct keyPointHistogram{
    cv::Point pt;
    int type;
    float histogram[540];
};

struct img_dataHistogram{
    int index;
    int year;
    int n_labels;
    struct keyPointHistogram key_Point[maxKeypoints];
};

void testTxt(void);

int str2label(std::string str);

struct img_data getDescriptor(std::string fileName,cv::Mat img_MR, int year, int index);
// Load the information of all the clasification point from a text file, and associate 
// an r24 vector from the Maximun Response filter.

void saveDescriptor(struct img_data* data, int n_images, std::string filename);
// Save the entire information of an image, include the coordinate, it's label
// and the associated r24 vector from the Maximum Response filter in a binary file.

void saveDescriptorH(struct img_dataHistogram* data, int n_images, std::string filename);
// Save the entire information of the images from the dataH set, include the coordinate, it's label
// and the associated textons histogram in a binary file.

bool saveDictionaryTextons(cv::Mat dictionary, std::string path);

struct img_data* loadDescriptor(int n_images, std::string filename);
// Load the entire information of an image, include the coordinate, it's label
// and the associated r24 vector from a binary file.

struct img_dataHistogram* loadDescriptorH(int n_images, std::string filename);

bool loadDictionaryTextons(cv::Mat dictionary, std::string path);

void getDictionaryTextons(cv::Mat dictionaryTextons, struct img_data* data, int start_index, int finish_index, struct kmeans_param km);
// Computes the K-means algorithm to each class creating a 
// matrix with 135 textons each one with 24 values.

int porcentage(int index_data, int n_images);
// Shows porcentage of completition of the getDataSet function

std::vector<std::string> getFileNames( bool* valid_sets, int* n_imgs);
// Load the filenames of the three 2008, 2009 and 2010 sets of images to study.
// The sets needs to be saved on the project folder by this structure:
// ~/ProjectRoot
//        /Vision_MCR
//             /2008
//             /2009
//             /2010

int getNearestTexton(cv::Mat dictionaryTextons, float r24[24]);

bool getDataHistogram(struct img_dataHistogram* dataH, cv::Mat dictionary, int n_images);

struct img_dataHistogram getHistogramDescriptor(std::string fileName, cv::Mat img_MR, cv::Mat dictionary, int year, int index);

void getPatchs(cv::Mat img_MR, cv::Mat dictionary, struct keyPointHistogram* key_Point);

void getHistogramTextons(cv::Mat img, int histogram[][135], int hSize[4]);

void normalizeHistogramsTextons(int histograms[][135] , float histogram[540]);

int checkPatchCompatibility(struct img_data* data, int n_images, int max_patch);

void readTextonsMatlab(cv::Mat dictionary, std::string filename);

void printMAXHistogramTextons(struct img_dataHistogram* dataH, int n_keypoints);

void saveSVMtxt(struct img_dataHistogram* dataH);
void saveSVMtxt2(struct img_dataHistogram* dataH_2008, struct img_dataHistogram* dataH_2009, int n_images);

void matrizC (int* predict, int* real, int total_k, float mat_conf[9][9], float mat_CvsNC[2][2], int totalRC[9]);

int* fileToArray(std::string fileName, int* total_k);

void getConfusionMatrix(int test_n);

#endif 
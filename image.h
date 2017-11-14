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
    struct keyPoint* key_Point;
};

void testTxt(void);

struct img_data getDescriptor(std::string fileName,cv::Mat img_MR, int year, int index);

void saveDescriptor(struct img_data data);

void getDictionaryTextons(cv::Mat dictionaryTextons, struct img_data data[200], int start_index, int finish_index);

bool getDataSet(struct img_data* data_2008, struct img_data* data_2009,struct img_data* data_2010);

bool getDataSet_2008(struct img_data* data_2008);

bool getDataSet_2009(struct img_data* data_2008);

bool getDataSet_2010(struct img_data* data_2008);

#endif
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

<<<<<<< HEAD
void testTxt(void );

struct img_data readtxt(std::string fileName, int year, int index);
=======
>>>>>>> 71381626a06d415e2400e21dcb6578f00e443547
void getDiccionaryTextons(cv::Mat diccionaryTextons, struct img_data data[200], int start_index, int finish_index);

#endif
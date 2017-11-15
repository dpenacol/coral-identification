/********************************************
 * FILE NAME: main.cpp                      *
 * DESCRIPTION: main function for our coral *
 *              identification algorithm    *
 * AUTHORS: Diego Peña, Victor García,      *
 *          Fabio Morales, Andreina Duarte  *
 ********************************************/

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include "histogram.h"
#include "filters.h"
#include "svm.h"
#include "image.h"

int main() {
    // Creating the data struct where the information is going to be saved
    int n_images = 1338;
    img_data* data = new img_data[n_images];

    // Applying the Maximum Response Filter to each image of the 2008, 2009 and
    // 2010 set. Then reads each image info .txt to save the results on data structure
    getDataSet(data, n_images);

    // save the image data in a binary file
    //saveDescriptor(data);

    // load the image data from a binary file
    // data = loadDescriptor();

    // print test, first the coordinates and labels, next an r24 vector information
    int image = 500;
    for(int i = 0; i<data[image].n_labels ;i++){
        std::cout << data[image].key_Point[i].pt.x*2 << " " << data[image].key_Point[i].pt.y*2 << " " << data[image].key_Point[i].type << std::endl;
    }
/*
    // Freeing space of the data struct
    for(int i=0; i<n_images; i++){
        delete [] data[i].key_Point;      
    }
    free(data);
*/
    return 0;
}

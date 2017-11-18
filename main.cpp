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
    int n_images = 2055;
    struct img_data* data = new struct img_data[n_images];

    // Applying the Maximum Response Filter to each image of the 2008, 2009 and
    // 2010 set. Then reads each image info .txt to save the results on data structure
    // getDataSet(data, n_images);

    // Save the image data in a binary file
    // saveDescriptor(data, n_images);

    // Load the image data from a binary file
    data = loadDescriptor(n_images);

    // print test, first the coordinates and labels, next an r24 vector information
    /*int image = 0;
    for(int i = 0; i<data[image].n_labels ;i++){
        std::cout << data[image].key_Point[i].pt.x*2 << " " << data[image].key_Point[i].pt.y*2 << " " << data[image].key_Point[i].type << std::endl;
    }
     std::cout << "\n------------------------------------------------\n";
    image =2055;
    for(int i = 0; i<data[image].n_labels ;i++){
        std::cout << data[image].key_Point[i].pt.x*2 << " " << data[image].key_Point[i].pt.y*2 << " " << data[image].key_Point[i].type << std::endl;
    }*/
    
    // Obtaining the textons from a group of images of the data
    cv::Mat dictionaryTextons;
    dictionaryTextons = cv::Mat(135, 24, CV_32FC1);
    int start_index = 0, finish_index = 2055;

    //getDictionaryTextons(dictionaryTextons, data, start_index, finish_index);

    //saveDictionaryTextons(dictionaryTextons, "dictionary.bin");

    //std::cout << "\nPrimer texton: " << dictionaryTextons.at<float>(0,0);
    loadDictionaryTextons(dictionaryTextons, "dictionary.bin");
    
    // Obtaining the Textons Histograms from the data_set
    struct img_dataHistogram* dataH = new struct img_dataHistogram[n_images];

    getDataHistogram(dataH, dictionaryTextons, n_images);

    for(int i=0; i<540; i++){
        std::cout << dataH->key_Point[0].histogram[i] << "\n";
    }

    // Freeing space of the data struct
    delete [] data;

    return 0;
}

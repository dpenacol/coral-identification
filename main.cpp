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
    // Reading the image.
    cv::Mat img = cv::imread("./Vision_MCR/2008/mcr_lter1_fringingreef_pole1-2_qu1_20080415.jpg",CV_LOAD_IMAGE_COLOR);

    // Resizing the image.
    cv::Mat img_resized(img.size().height/2,img.size().width/2, CV_8UC3, cv::Scalar(255,255,255));
    resize(img, img_resized, img_resized.size(), 0, 0, cv::INTER_CUBIC);

    // Creating the preprocessed image.
    cv::Mat img_preprocessed(img_resized.size().height,img_resized.size().width, CV_8UC3, cv::Scalar(255,255,255));
    colorChannelStretch(img_resized, img_preprocessed, 1, 99);

    // Transforming the image to the L*a*b* color space.
    cv::Mat img_Lab(img_preprocessed.size().height,img_preprocessed.size().width, CV_8UC3, cv::Scalar(255,255,255));
    cv::cvtColor(img_preprocessed, img_Lab, cv::COLOR_BGR2Lab);

    // Creating the kernels for the Maximum Response filter.
    int i;
    cv::Mat kernel[38];
    for(i=0; i<38; i++){
        kernel[i] = cv::Mat(73,73, CV_32FC1);
    }
    getMaximumResponseFilterKernels(kernel, 1, 3, 8, 3);

    // Applying the Maximum Response Filter.
    cv::Mat img_MR(img_Lab.size().height,img_Lab.size().width, CV_32FC(24));
    maximumResponseFilter(img_Lab, img_MR, kernel);

    std::string fileName = "./Vision_MCR/2008/mcr_lter1_fringingreef_pole1-2_qu1_20080415.jpg.txt";
    struct img_data data = getDescriptor(fileName, img_MR, 2008, 1);
    //readtxt(fileName, data, 2008, 1);

    // Impresion de prueba
    for(int i = 0; i<data.n_labels ;i++){
        std::cout << data.key_Point[i].pt.x << " " << data.key_Point[i].pt.y << " " << data.key_Point[i].type << std::endl;
    }
    for(int i = 0; i<24 ;i++){
        std::cout << data.key_Point[i].r24[i] << std::endl;
    }
    // Creating diccionary of textons
    cv::Mat dictionaryTextons(135, 24, CV_32FC1);
    delete [] data.key_Point;
    return 0;
}

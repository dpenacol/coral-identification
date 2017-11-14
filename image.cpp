/********************************************
 * FILE NAME: image.cpp                     *
 * DESCRIPTION: Contains functions that are *
 *              useful to read data from    *
 *              images                      *
 * AUTHORS: Diego Peña, Victor García,      *
 *          Fabio Morales, Andreina Duarte  *
 ********************************************/

#include "image.h"

int str2label(std::string str);

void testTxt(void){
    std::string fileName = "./Vision_MCR/2008/mcr_lter1_fringingreef_pole1-2_qu1_20080415.jpg.txt";
    struct img_data data = readtxt(fileName, 2008, 1);

    for(int i = 0; i<200 ;i++){
        std::cout << data.key_Point[i].pt.x << " " << data.key_Point[i].pt.y << " " << data.key_Point[i].type << std::endl;
    }
    delete [] data.key_Point;
}

struct img_data readtxt(std::string fileName, int year, int index){
    struct img_data data;
    struct keyPoint labels[200];
    data.index = 0;
    data.year = 0;
    data.n_labels = 200;

    std::ifstream file(fileName);
    if (file.is_open()){
        std::string str;
        
        std::getline(file,str);
        data.key_Point = new struct keyPoint[200];

        for(int i=0; i<200; i++){        
            file >> str;
            data.key_Point[i].pt.x = atoi(str.c_str());
            file >> str; 
            data.key_Point[i].pt.y = atoi(str.c_str());
            file >> str;
            data.key_Point[i].type = str2label(str);
        }

        data.index = index;
        data.year = year;
        file.close();
    } else{
        std::cout << "No se pudo abrir el archivo!" << std::endl;
    }
    return data;
}

int str2label(std::string str){
    int label = 0;
    /*
    1 = Sand
    2 = Macro
    3 = CCA
    4 = off
    5 = Porit
    6 = Turf
    7 = Pocill
    8 = Monta
    9 = Pavon
    */
    if(str == "Sand")
        label = 1;
    else if(str == "Macro")
        label = 2;
    else if(str == "CCA")
        label = 3;
    else if(str == "Off")
        label = 4;
    else if(str == "Porit")
        label = 5;
    else if(str == "Turf")
        label = 6;
    else if(str == "Pocill")
        label = 7;
    else if(str == "Monta")
        label = 8;
    else if(str == "Pavon")
        label = 9;
    return label;
}

void getDictionaryTextons(cv::Mat dictionaryTextons, struct img_data data[200], int start_index, int finish_index){
    // Parameters of K-means algorithm
    int clusters = 15;
    int attempts = 10;
    int initial_centers = cv::KMEANS_PP_CENTERS;

    // Creating the structures for each class
    cv::Mat class_data[9];
    cv::Mat labels[9];
    cv::Mat centers[9];
    int row[9];

    // Checking for each class label and saves the R^24 values on each class_data 
    for(int j=start_index; j<finish_index; j++){
        for(int k=0; k<data[j].n_labels; k++){
            for(int m=0; m<9; m++){
                if(data[j].key_Point[k].type == m + 1){
                    for(int n=0; n<24; n++){
                        class_data[m].at<float>(row[m],n) = data[j].key_Point[k].r24[n];
                    }
                    row[m]++;
                }
            }
        }
    }

    // Applying the K-means algorithm for each class_data
    for(int m=0; m<9; m++){
        kmeans(class_data[m], clusters, labels[m], cv::TermCriteria(CV_TERMCRIT_ITER,100,0.001), attempts, initial_centers, centers[m]);
    }

    // Saving the 135 textons on the dictionary
    int n = 0;
    for(int j=0; j<9; j++){
        for(int k=0; k<15; k++){
            for(int m=0; m<24; m++){
                dictionaryTextons.at<float>(n,m) = class_data[j].at<float>(k,m);
            }
            n++;
        }
    }
}
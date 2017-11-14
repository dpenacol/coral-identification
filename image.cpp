/********************************************
 * FILE NAME: image.cpp                     *
 * DESCRIPTION: Contains functions that are *
 *              useful to read data from    *
 *              images                      *
 * AUTHORS: Diego Peña, Victor García,      *
 *          Fabio Morales, Andreina Duarte  *
 ********************************************/

#include "image.h"
#include "filters.h"
#include "histogram.h"

int str2label(std::string str);

void saveDescriptor(struct img_data data){

}

struct img_data getDescriptor(std::string fileName,cv::Mat img_MR, int year, int index){
    struct img_data data;
    data.index = 0;
    data.year = 0;
    data.n_labels = 200;

    std::ifstream file(fileName);
    if (file.is_open()){
        int i;
        std::string str;
        std::getline(file,str);
        data.index = index;
        data.year = year;
        data.key_Point = new struct keyPoint[200];
        for(i=0; i<200; i++){        
            file >> str;
            data.key_Point[i].pt.x = atoi(str.c_str())/2;
            file >> str; 
            data.key_Point[i].pt.y = atoi(str.c_str())/2;
            file >> str;
            data.key_Point[i].type = str2label(str);
        }
        for(i=0; i< data.n_labels ; i++){
            for(int j=0; j<24; j++){
                data.key_Point[i].r24[j] = img_MR.at<cv::Vec<float, 24>>(data.key_Point[i].pt.x, data.key_Point[i].pt.y)[j];
            }
        }
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

bool getDataSet(struct img_data* data_2008, struct img_data* data_2009,struct img_data* data_2010){
    getDataSet_2008(data_2008);

    return true;
}

bool getDataSet_2008(struct img_data* data_2008){
    std::string directory = "./Vision_MCR/2008/mcr_lter";

    std::string img_directory;
    std::string n_lter = "1";

    std::string type = "_fringingreef";
    std::string pole = "_pole";
    std::string qu = "_qu";
    std::string id = "_20080415.jpg";
    int index_data = 0;
    int k = 1;

    img_directory = directory + n_lter + type + pole + std::to_string(k) + "-" + std::to_string(k+1) + qu + std::to_string(1) + id;
    std::string txt = img_directory + ".txt";
    std::cout << img_directory + "\n";
    cv::Mat img_MR;
    img_MR = getMaximumResponseFilter(img_directory);
    data_2008[index_data] = getDescriptor(txt, img_MR, 2008, index_data);

    /*for(int i = 1; i<6; i++){
        for(int j=1; j<9; j++){
            img_directory = directory + n_lter + type + pole + std::to_string(k) + "-" + std::to_string(k+1) + qu + std::to_string(j) + id;
            std::cout << img_directory + "\n";
            data_2008[index_data] = getDescriptor(img_directory + ".txt", getMaximumResponseFilter(img_directory), 2008, index_data);
            index_data++;
        }
        k++;
    }*/

    return true;
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


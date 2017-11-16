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
        std::cout << "Error loading "  + fileName << std::endl;
    }
    return data;
}

void saveDescriptor(struct img_data* data, int n_images){
    std::ofstream fout;
    fout.open("data_set.bin",std::ios::out| std::ios::binary);

    for(int i = 0; i < n_images; i++){
        fout.write((char *)&data[i], sizeof(struct img_data));
        for(int j=0; j<data[i].n_labels; j++){
            fout.write((char *)&data[i].key_Point[j], sizeof(struct keyPoint));
            for(int k=0; k<24; k++){
                fout.write((char *)&data[i].key_Point[j].r24[k], sizeof(data[i].key_Point[j].r24[k]));
            }
        }
    }
    fout.close();
}

struct img_data* loadDescriptor(int n_images){
    struct img_data* data = new struct img_data[2055];
    std::ifstream fin;
    fin.open("data_set.bin", std::ios::in| std::ios::binary);
    if(fin.is_open()){
        std::cout << "Success loading of data_set.bin" << std::endl;

        for(int i = 0; i < n_images; i++){
            fin.read((char *)&data[i], sizeof(struct img_data));
            for(int j=0; j<data[i].n_labels; j++){
                fin.read((char *)&data[i].key_Point[j], sizeof(struct keyPoint));
                for(int k=0; k<24; k++){
                    fin.read((char *)&data[i].key_Point[j].r24[k], sizeof(data[i].key_Point[j].r24[k]));
                }
            }
        }
        fin.close();
    }else{
        std::cout << "Error loading data_set.bin" << std::endl;
    }
    return data;
}

int str2label(std::string str){
    int label = 0;
    /*
    1 = CCA
    2 = Turf
    3 = Macro
    4 = Sand
    5 = Acrop
    6 = Pavon
    7 = Monti
    8 = Pocill
    9 = Porit
    */
    if(str == "CCA")
        label = 1;
    else if(str == "Turf" || str == "TURF")
        label = 2;
    else if(str == "Macro" || str == "MACRO")
        label = 3;
    else if(str == "Sand" || str == "SAND")
        label = 4;
    else if(str == "Acrop" || str == "ACROP")
        label = 5;
    else if(str == "Pavon" || str == "PAVON")
        label = 6;
    else if(str == "Monti" || str == "MONTI")
        label = 7;
    else if(str == "Pocill" || str == "POCILL")
        label = 8;
    else if(str == "Porit" || str == "P. Irr" || str == "P. Rus" || str == "P mass" || str == "PORIT"|| str == "P. IRR" || str == "P. RUS" || str == "P MASS")
        label = 9;
    else
        label = 0;
    return label;
}


int porcentage(int index_data, int n_images){
    return index_data*100/n_images;
}

bool getDataSet(struct img_data* data, int n_images){
// Reading 2008 set
    // Directory of the 2008 set
    std::string directory = "./Vision_MCR/2008/";

    // Creating a vector of strings to save the names of the images and txt
    std::vector<std::string> file_names;

    // Reading the folder for each element and save their names in file_names
    DIR *dir;
    struct dirent *ent;
    int i=0;
    int index_data = 0;

    if ((dir = opendir ("./Vision_MCR/2008/")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            file_names.push_back(std::string(ent->d_name));
        }
        closedir (dir);
    } else {
    // If the directory could not be opened
        perror ("");
        return false;
    }
    // Sorting the vector of strings so it is alphabetically ordered
    std::sort(file_names.begin(), file_names.end());

    // Obtaining the 2008 data set    
    for(int i = 2; i<1344; i=i+2){
        std::cout << "[" + std::to_string(porcentage(index_data, n_images)) + '%' + "] ./Vision_MCR/2008/" + file_names.at(i) + "\n";
        data[index_data] = getDescriptor("./Vision_MCR/2008/" + file_names.at(i+1), getMaximumResponseFilter("./Vision_MCR/2008/" + file_names.at(i)), 2008, index_data);
        index_data++;
    }

// Reading 2009 set
    file_names.erase(file_names.begin(), file_names.end());

    if ((dir = opendir ("./Vision_MCR/2009/")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            file_names.push_back(std::string(ent->d_name));
        }
        closedir (dir);
    } else {
    // If the directory could not be opened
        perror ("");
        return false;
    }
    // Sorting the vector of strings so it is alphabetically ordered
    std::sort(file_names.begin(), file_names.end());

    // Obtaining the 2009 data set    
    for(int i = 2; i<1392; i=i+2){
        std::cout << "[" + std::to_string(porcentage(index_data, n_images)) + '%' + "] ./Vision_MCR/2009/" + file_names.at(i) + "\n";
        data[index_data] = getDescriptor("./Vision_MCR/2009/" + file_names.at(i+1), getMaximumResponseFilter("./Vision_MCR/2009/" + file_names.at(i)), 2009, index_data);
        index_data++;
    }

// Reading 2010 set
    file_names.erase(file_names.begin(), file_names.end());

    if ((dir = opendir ("./Vision_MCR/2010/")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            file_names.push_back(std::string(ent->d_name));
        }
        closedir (dir);
    } else {
    // If the directory could not be opened
        perror ("");
        return false;
    }
    // Sorting the vector of strings so it is alphabetically ordered
    std::sort(file_names.begin(), file_names.end());

    // Obtaining the 2010 data set    
    for(int i = 2; i<1380; i=i+2){
        std::cout << "[" + std::to_string(porcentage(index_data, n_images)) + '%' + "] ./Vision_MCR/2010/" + file_names.at(i) + "\n";
        data[index_data] = getDescriptor("./Vision_MCR/2010/" + file_names.at(i+1), getMaximumResponseFilter("./Vision_MCR/2010/" + file_names.at(i)), 2010, index_data);
        index_data++;
    }
    return true;
}

void getDictionaryTextons(cv::Mat dictionaryTextons, struct img_data data[200], int start_index, int finish_index){
    // Parameters of K-means algorithm
    int clusters = 15;
    int attempts = 500;
    int initial_centers = cv::KMEANS_PP_CENTERS;

    // Creating the structures for each class
    cv::Mat class_data[9];
    cv::Mat labels[9];
    cv::Mat centers[9];
    int row[9];

    // Initializing each class textons matrix
    for(int i=0; i<9; i++){
        centers[i] = cv::Mat(15,24,CV_32FC1);        
        row[i] = 0;
    }

    // Checking the number of labels for each class
    for(int j=start_index; j<finish_index; j++){
        for(int k=0; k<data[j].n_labels; k++){
            for(int m=0; m<9; m++){
                if(data[j].key_Point[k].type == m + 1){
                    row[m]++;
                }
            }
        }
    }

    // Initializing each class data matrix
    for(int i=0; i<9; i++){
        class_data[i] = cv::Mat(row[i],24,CV_32FC1);        
        row[i] = 0;
    }

    // Saving each R^24 values on each class_data matrix
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
        kmeans(class_data[m], clusters, labels[m], cv::TermCriteria(CV_TERMCRIT_ITER,1000,0.001), attempts, initial_centers, centers[m]);
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

void saveDictionaryTextons(cv::Mat Dictionary){
    int n_textons = 135;
    std::ofstream fout;
}
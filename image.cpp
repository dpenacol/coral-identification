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

struct img_data getDescriptor(std::string fileName, cv::Mat img_MR, uint16_t year, uint16_t index){
    struct img_data data;
    uint16_t n_lables;
    data.index = 0;
    data.year = 0;
    if(year == 2008 || year == 2009){
        n_lables = 200;
    }
    if(year == 2010){
        n_lables = 199;
    }
    data.n_labels = n_lables;
    std::ifstream file(fileName);
    if (file.is_open()){
        uint16_t i, j;
        std::string str;
        std::getline(file,str);
        data.index = index;
        data.year = year;
        for(i=0; i<n_lables; i++){        
            file >> str;
            data.key_Point[i].pt.x = atoi(str.c_str())/2;
            file >> str; 
            data.key_Point[i].pt.y = atoi(str.c_str())/2;
            file >> str;
            data.key_Point[i].type = str2label(str);
        }
        for(i=0; i< data.n_labels ; i++){
            for(j=0; j<24; j++){
                data.key_Point[i].r24[j] = img_MR.at<cv::Vec<float, 24>>(data.key_Point[i].pt.x, data.key_Point[i].pt.y)[j];
            }
        }
        file.close();
    } else{
        std::cout << "Error loading "  + fileName << std::endl;
    }
    return data;
}

void saveDescriptor(struct img_data* data, uint16_t n_images){
    std::ofstream fout;
    fout.open("data_set.bin",std::ios::out| std::ios::binary);
    uint16_t i, j;
    uint8_t k;
    for(i = 0; i < n_images; i++){
        fout.write((char *)&data[i], sizeof(struct img_data));
        for(j=0; j<data[i].n_labels; j++){
            fout.write((char *)&data[i].key_Point[j], sizeof(struct keyPoint));
            for(k=0; k<24; k++){
                fout.write((char *)&data[i].key_Point[j].r24[k], sizeof(data[i].key_Point[j].r24[k]));
            }
        }
    }
    fout.close();
}

struct img_data* loadDescriptor(uint16_t n_images){
    struct img_data* data = new struct img_data[2055];
    std::ifstream fin;
    fin.open("data_set.bin", std::ios::in| std::ios::binary);
    uint16_t i, j;
    uint8_t k;
    if(fin.is_open()){
        std::cout << "Loading data_set.bin..." << std::endl;

        for(i = 0; i < n_images; i++){
            fin.read((char *)&data[i], sizeof(struct img_data));
            for(j=0; j<data[i].n_labels; j++){
                fin.read((char *)&data[i].key_Point[j], sizeof(struct keyPoint));
                for(k=0; k<24; k++){
                    fin.read((char *)&data[i].key_Point[j].r24[k], sizeof(data[i].key_Point[j].r24[k]));
                }
            }
        }
        std::cout << "Success loading of data_set.bin" << std::endl;
        fin.close();
    }else{
        std::cout << "Error loading data_set.bin" << std::endl;
    }
    return data;
}

uint8_t str2label(std::string str){
    uint8_t label = 0;
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


uint8_t porcentage(uint16_t index_data, uint16_t n_images){
    return index_data*100/n_images;
}

bool getDataSet(struct img_data* data, uint16_t n_images){
// Reading 2008 set
    // Directory of the 2008 set
    std::string directory = "./Vision_MCR/2008/";

    // Creating a vector of strings to save the names of the images and txt
    std::vector<std::string> file_names;

    // Reading the folder for each element and save their names in file_names
    DIR *dir;
    struct dirent *ent;
    uint16_t i;
    uint16_t index_data = 0;

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
    for(i = 2; i<1344; i=i+2){
        std::cout << "[" + std::to_string(porcentage(index_data, n_images)) + '%' + "] ./Vision_MCR/2008/" + file_names.at(i) + "\n";
        data[index_data] = getDescriptor("./Vision_MCR/2008/" + file_names.at(i+1), getMaximumResponseFilter("./Vision_MCR/2008/" + file_names.at(i)), 2008, index_data);
        index_data++;
    }

    // Deleting the names of the set
    file_names.erase(file_names.begin(), file_names.end());

// Reading 2009 set
    // Directory of the 2009 set
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
    for(i = 2; i<1392; i=i+2){
        std::cout << "[" + std::to_string(porcentage(index_data, n_images)) + '%' + "] ./Vision_MCR/2009/" + file_names.at(i) + "\n";
        data[index_data] = getDescriptor("./Vision_MCR/2009/" + file_names.at(i+1), getMaximumResponseFilter("./Vision_MCR/2009/" + file_names.at(i)), 2009, index_data);
        index_data++;
    }

    // Deleting the names of the set
    file_names.erase(file_names.begin(), file_names.end());

// Reading 2010 set
    // Directory of the 2010 set
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
    for(i = 2; i<1380; i=i+2){
        std::cout << "[" + std::to_string(porcentage(index_data, n_images)) + '%' + "] ./Vision_MCR/2010/" + file_names.at(i) + "\n";
        data[index_data] = getDescriptor("./Vision_MCR/2010/" + file_names.at(i+1), getMaximumResponseFilter("./Vision_MCR/2010/" + file_names.at(i)), 2010, index_data);
        index_data++;
    }

    // Deleting the names of the set
    file_names.erase(file_names.begin(), file_names.end());

    return true;
}

void getDictionaryTextons(cv::Mat dictionaryTextons, struct img_data data[2055], uint16_t start_index, uint16_t finish_index){
    // Parameters of K-means algorithm
    uint8_t clusters = 15;
    uint16_t attempts = 200;
    uint32_t initial_centers = cv::KMEANS_RANDOM_CENTERS;

    // Creating the structures for each class
    cv::Mat class_data[9];
    cv::Mat labels[9];
    cv::Mat centers[9];
    uint32_t row[9];

    // Initializing each class textons matrix
    uint16_t i, j, k;
    uint8_t m, n;
    for(i=0; i<9; i++){
        centers[i] = cv::Mat(15,24,CV_32FC1);        
        row[i] = 0;
    }

    // Checking the number of labels for each class
    for(j=start_index; j<finish_index; j++){
        for(k=0; k<data[j].n_labels; k++){
            for(m=0; m<9; m++){
                if(data[j].key_Point[k].type == m + 1){
                    row[m]++;
                }
            }
        }
    }

    // Initializing each class data matrix
    for(i=0; i<9; i++){
        class_data[i] = cv::Mat(row[i],24,CV_32FC1);        
        row[i] = 0;
    }

    // Saving each R^24 values on each class_data matrix
    for(j=start_index; j<finish_index; j++){
        for(k=0; k<data[j].n_labels; k++){
            for(m=0; m<9; m++){
                if(data[j].key_Point[k].type == m + 1){
                    for(n=0; n<24; n++){
                        class_data[m].at<float>(row[m],n) = data[j].key_Point[k].r24[n];
                    }
                    row[m]++;
                }
            }
        }
    }

    // Applying the K-means algorithm for each class_data
    for(int m=0; m<9; m++){
        std::cout << "[" + std::to_string(porcentage(m, 8)) + '%' + "] Obtaining textons of the " + std::to_string(m + 1) + " class.\n";
        kmeans(class_data[m], clusters, labels[m], cv::TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 1000, 0.001), attempts, initial_centers, centers[m]);
    }

    // Saving the 135 textons on the dictionary
    n = 0;
    for(j=0; j<9; j++){
        for(k=0; k<15; k++){
            for(m=0; m<24; m++){
                dictionaryTextons.at<float>(n,m) = class_data[j].at<float>(k,m);
            }
            n++;
        }
    }
}

uint8_t getNearestTexton(cv::Mat dictionaryTextons, float r24[24]){
    float nearest_distance = 0;
    uint8_t nearest_texton = 0;
    float euclidean = 0;
    uint8_t i, j;

    for(i=0; i<135; i++){
        for(j=0; j<24; j++){
            euclidean = euclidean + pow(r24[j] - dictionaryTextons.at<float>(i,j), 2);
        }
        if(i==0){
            nearest_distance = euclidean;
        }
        else if(euclidean < nearest_distance){
            nearest_distance = euclidean;
            nearest_texton = i;
        }
        euclidean = 0;
    }

    return nearest_texton;
}

bool saveDictionaryTextons(cv::Mat dictionary, std::string path){

    std::ofstream file(path.c_str(), std::ios::out | std::ios::binary );
    if (!file)
        return false;
 
    //file.write((const char*) &matWidth, sizeof(matWidth));
    //file.write((const char*) &matHeight, sizeof(matHeight));

    uint8_t j, k, m, n = 0;
    for(j=0; j<9; j++){
        for(k=0; k<15; k++){
            for(m=0; m<24; m++){
                file.write((const char*) &dictionary.at<float>(n,m), sizeof(dictionary.at<float>(n,m)));
                //dictionaryTextons.at<float>(n,m) = class_data[j].at<float>(k,m);
            }
            n++;
        }
    }
    std::cout << "Writing CV_32F image" << std::endl;
    file.close();

    return true;
}

bool loadDictionaryTextons(cv::Mat dictionary, std::string path){

    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary );
    if (!file){
        std::cout << "Error loading dictionary.bin..." << std::endl;
        return false;
    }
    std::cout << "Loading dictionary.bin..." << std::endl;
    uint8_t j, k, m, n = 0;
    for(j=0; j<9; j++){
        for(k=0; k<15; k++){
            for(m=0; m<24; m++){
                file.read((char*) &dictionary.at<float>(n,m), sizeof(dictionary.at<float>(n,m)));
                //dictionaryTextons.at<float>(n,m) = class_data[j].at<float>(k,m);
            }
            n++;
        }
    }
    std::cout << "Success loading of dictionary.bin" << std::endl;
    return true;
}

bool getDataHistogram(struct img_dataHistogram* dataH, cv::Mat dictionary, uint16_t n_images){
// Reading 2008 set
    // Directory of the 2008 set
    std::string directory = "./Vision_MCR/2008/";

    // Creating a vector of strings to save the names of the images and txt
    std::vector<std::string> file_names;

    // Reading the folder for each element and save their names in file_names
    DIR *dir;
    struct dirent *ent;
    uint16_t i=0;
    uint16_t index_data = 0;

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
    for(i = 2; i<4; i=i+2){
        std::cout << "[" + std::to_string(porcentage(index_data, n_images)) + '%' + "] Image: ./Vision_MCR/2008/" + file_names.at(i) + "\n";
        dataH[index_data] = getHistogramDescriptor("./Vision_MCR/2008/" + file_names.at(i+1), getMaximumResponseFilter("./Vision_MCR/2008/" + file_names.at(i)), dictionary, 2008, index_data);
        index_data++;
    }

    // Deleting the names of the set
    file_names.erase(file_names.begin(), file_names.end());

    return true;
}

struct img_dataHistogram getHistogramDescriptor(std::string fileName, cv::Mat img_MR, cv::Mat dictionary, uint16_t year, uint16_t index){
    struct img_dataHistogram data;
    cv::Mat img_Texton(img_MR.size().width, img_MR.size().height, CV_8SC1);
    uint16_t n_labels;
    uint16_t n_overflow =0;
    uint16_t i, j;
    uint8_t k;
    float r24[24];
    data.index = 0;
    data.year = 0;
    
    if(year == 2008 || year == 2009){
        n_labels = 200;
    }
    if(year == 2010){
        n_labels = 199;
    }
    data.n_labels = n_labels;
    std::ifstream file(fileName);

    // Get the nearest texton for all the vectors in img_MR
    // And saving the texton number in img_Texton
    for(i=0; i<img_MR.size().width; i++){
        for(j=0; j<img_MR.size().height; j++){
            for(k=0; k<24; k++){
                r24[k] = img_MR.at<cv::Vec<float, 24>>(i, j)[k];
            }
            img_Texton.at<int8_t>(i, j) = getNearestTexton(dictionary, r24);
        }
    }

    if (file.is_open()){
        
        std::string str;
        std::getline(file,str);
        data.index = index;
        data.year = year;
        for(i=0; i<n_labels; i++){        
            file >> str;
            data.key_Point[i].pt.x = atoi(str.c_str())/2;
            file >> str; 
            data.key_Point[i].pt.y = atoi(str.c_str())/2;
            file >> str;
            data.key_Point[i].type = str2label(str);
        }
        for(i=0; i< data.n_labels ; i++){
            if(data.key_Point[i].type != 0){
                getPatchs(img_Texton, dictionary, &data.key_Point[i]);
                if(data.key_Point[i].type==0)
                    n_overflow++;
            }
        }
        file.close();
    } else{
        std::cout << "Error loading "  + fileName << std::endl;
    }
    return data;
}

int checkPatchCompatibility(struct img_data* data, int n_images, int max_patch){
    int n_overflows = 0;
    for(int i=0; i<n_images; i++){
        for(int j=0; j<data[i].n_labels; j++){
            if(data[i].key_Point[j].pt.x - ((max_patch-1)/2) < 0 || data[i].key_Point[j].pt.y - ((max_patch-1)/2) < 0 )
                n_overflows++;
        }
    }
    return n_overflows;
}

void getPatchs(cv::Mat img_Texton, cv::Mat dictionary, struct keyPointHistogram* key_Point){

    cv::Mat p221(221, 221, CV_8SC1, cv::Scalar(0));

    uint16_t histograms[4][135];
    uint16_t x = 0, y = 0;
    uint16_t i, j;
    // Half Size (height or width of rectangle) of study areas
    uint8_t hSize[4] = {10, 30, 60, 110};

    //if(key_Point->pt.x - hSize[3]>0 && key_Point->pt.x + hSize[3]+1<img_Texton.size().width){
        //if(key_Point->pt.y - hSize[3]>0 && key_Point->pt.y + hSize[3]+1<img_Texton.size().height){
            // Obtaining the R^24 vectors with patchs 21, 61, 121 and 221
            for(int j=key_Point->pt.y - hSize[3]; j<key_Point->pt.y + hSize[3]+1; j++){
                for(int k=key_Point->pt.x - hSize[3]; k<key_Point->pt.x + hSize[3]+1; k++){
                    // if the study area is fully inside the image
                    if((j > 0) && (j < img_Texton.size().height) && (k > 0) && (k < img_Texton.size().width)){
                        p221.at<int8_t>(x,y) = img_Texton.at<int8_t>(j, k);
                    }else{
                        p221.at<int8_t>(x,y) = -1;
                    }
                    y++;
                }
                y = 0;
                x++;
            }
        //}else {
        //    key_Point->type = 0;
        //    return; 
        //}
    //}else {
    //    key_Point->type = 0;
    //    return;
    //}
    // Initializing the histograms
    for(j=0; j<4; j++){
        for(i=0; i<135; i++){
            histograms[j][i] = 0;
        }
    }

    // Obtaining the histograms of textons for each patch applied
    getHistogramTextons(p221, histograms, hSize);
    // Normalizing the histograms and saving them in the structure
    normalizeHistogramsTextons(histograms, key_Point->histogram);
}

void getHistogramTextons(cv::Mat img, uint16_t histogram[][135], uint8_t hSize[4]){
    uint16_t max = 0;
    uint8_t center = hSize[3];
    uint16_t j, k;
    uint8_t n;
    
    // Computing the histogram
    for(j= center - hSize[3]; j< center + hSize[3] + 1; j++){
        for(k=center - hSize[3]; k<center + hSize[3] + 1; k++){
            if(img.at<int8_t>(j,k)!=-1){
                // For patch 21, 61 and 121
                for(n=0; n<3; n++){
                    if(j < center + hSize[n] + 1 && j > center - hSize[n] && k < center + hSize[n] + 1 && k > center - hSize[n])
                        histogram[n][img.at<int8_t>(j,k)] += 1;
                }
                // For patch 221
                histogram[3][img.at<int8_t>(j,k)] += 1;
            }
        }
    }

}

void normalizeHistogramsTextons(uint16_t histograms[][135] , float histogram[540]){
    uint16_t max = 0;
    uint16_t aux = 0;
    uint8_t i, j;
    // Normalizing each histogram and then saving them to the histogram array
    for(i=0; i<4; i++){
        for(j=0; j<135; j++){
            if(histograms[i][j] > max)
                max = histograms[i][j];
        }
        for(j=0; j<135; j++){
            histogram[aux] = (float)histograms[i][j]/(float)max;
            aux++;
        }
        max = 0;
    }
}
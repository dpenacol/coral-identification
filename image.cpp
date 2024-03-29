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
// Global parameters of Kmeans Clustering

struct img_data getDescriptor(std::string fileName, cv::Mat img_MR, int year, int index){
    struct img_data data;
    data.index = 0;
    data.year = 0;
    if(year == 2008 || year == 2009){
        data.n_labels = 200;
    }
    if(year == 2010){
        data.n_labels = 199;
    }
    std::ifstream file(fileName);
    if (file.is_open()){
        int i, j;
        std::string str;
        std::getline(file,str);
        data.index = index;
        data.year = year;
        for(i=0; i<data.n_labels; i++){//while(!file.eof()){        
            file >> str;
            data.key_Point[i].pt.y = atoi(str.c_str())/2;
            file >> str; 
            data.key_Point[i].pt.x = atoi(str.c_str())/2;
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

void readTextonsMatlab(cv::Mat dictionary, std::string filename){
    std::ifstream file(filename);
    if (file.is_open()){
        int i, j;
        std::string str;

        for(i=0; i<135; i++){
            for(j=0; j<24; j++){    
                file >> str;
                dictionary.at<float>(i,j)= atof(str.c_str());
            }
        }
        file.close();
    } else{
        std::cout << "Error loading "  + filename << std::endl;
    }
}

void saveDescriptor(struct img_data* data, int n_images, std::string filename){
    std::ofstream fout;
    fout.open(filename.c_str(),std::ios::out| std::ios::binary);
    int i, j, k;

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

void saveDescriptorH(struct img_dataHistogram* dataH, int n_images, std::string filename){
    std::ofstream fout;
    fout.open(filename.c_str(),std::ios::out| std::ios::binary);
    int i, j, k;

    for(i = 0; i < n_images; i++){
        fout.write((char *)&dataH[i], sizeof(struct img_dataHistogram));
        for(j=0; j<dataH[i].n_labels; j++){
            
            fout.write((char *)&dataH[i].key_Point[j], sizeof(struct keyPointHistogram));
            for(k=0; k<540; k++){
                fout.write((char *)&dataH[i].key_Point[j].histogram[k], sizeof(dataH[i].key_Point[j].histogram[k]));
            }
        }
    }
    fout.close();
}

struct img_data* loadDescriptor(int n_images, std::string filename){
    struct img_data* data = new struct img_data[n_images];
    std::ifstream fin;
    fin.open(filename.c_str(), std::ios::in| std::ios::binary);
    int i, j;
    int k;
    if(fin.is_open()){
        for(i = 0; i < n_images; i++){
            fin.read((char *)&data[i], sizeof(struct img_data));
            for(j=0; j<data[i].n_labels; j++){
                fin.read((char *)&data[i].key_Point[j], sizeof(struct keyPoint));
                for(k=0; k<24; k++){
                    fin.read((char *)&data[i].key_Point[j].r24[k], sizeof(data[i].key_Point[j].r24[k]));
                }
            }
        }
        std::cout << "Success loading of "+filename << std::endl;
        fin.close();
    }else{
        std::cout << "Error loading "+filename << std::endl;
    }
    return data;
}

struct img_dataHistogram* loadDescriptorH(int n_images, std::string filename){
    if(n_images == 0){
        n_images = 2055;
    }
    std::cout <<n_images << std::endl;
    struct img_dataHistogram* dataH = new struct img_dataHistogram[n_images];
    std::ifstream fin;
    fin.open(filename.c_str(), std::ios::in| std::ios::binary);
    int i=0, j, k;

    if(fin.is_open()){
        for(i = 0; i < n_images; i++){
            if(!fin.read((char *)&dataH[i], sizeof(struct img_dataHistogram)))
                break;
            for(j=0; j<dataH[i].n_labels; j++){
                fin.read((char *)&dataH[i].key_Point[j], sizeof(struct keyPointHistogram));
                for(k=0; k<540; k++){
                    fin.read((char *)&dataH[i].key_Point[j].histogram[k], sizeof(dataH[i].key_Point[j].histogram[k]));
                }
            }
        }
        fin.close();
        struct img_dataHistogram* auxH = new struct img_dataHistogram[i];
        for(k=0;k<i; k++){
            auxH[k] = dataH[k];
        }
        delete [] dataH;
        return auxH;
    }else{
        std::cout << "Error loading dataH.bin" << std::endl;
    }
    return dataH;
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

std::vector<std::string> getFileNames( bool* valid_sets, int* n_imgs){
    // Directories of the 2008, 2009 and 2010 sets
    std::string directories[3] = {"./Vision_MCR/2008/", "./Vision_MCR/2009/", "./Vision_MCR/2010/"};
    
    // Creating a vector of strings to save the names of the images and txt
    std::vector<std::string> file_names_aux;
    std::vector<std::string> file_names;

    // Reading the folder for each element and save their names in file_names
    DIR *dir;
    struct dirent *ent;
    int i, j;
    int index_data = 0;

    for(i=0; i< 3 ; i++){
        if(valid_sets[i]){
            if ((dir = opendir(directories[i].c_str())) != NULL) {
                while ((ent = readdir (dir)) != NULL) {
                    file_names_aux.push_back(std::string(ent->d_name));
                }
                closedir (dir);
            } else {
            // If the directory could not be opened
                perror ("Directory could not be opened");
                return file_names;
            }
            // Sorting the vector of strings so it is alphabetically ordered
            std::sort(file_names_aux.begin(), file_names_aux.end());

            // Obtaining the data set   
            for(j = 2; j<n_imgs[i]*2+2; j+=2){
                file_names.push_back(directories[i] + file_names_aux.at(j));
            }
            // Deleting the names of the set
            file_names_aux.erase(file_names_aux.begin(), file_names_aux.end());
        }
    }
    return file_names;
}

void getDictionaryTextons(cv::Mat dictionaryTextons, struct img_data* data, int start_index, int finish_index, struct kmeans_param km){
    // Parameters of K-means algorithm
    int clusters = 15;
    int initial_centers = cv::KMEANS_PP_CENTERS;
    int iterations = km.iterations;
    int attempts = km.attempts;
    double epsilon = km.epsilon;

    // Creating the structures for each class
    cv::Mat class_data[9];
    cv::Mat labels[9];
    cv::Mat centers[9];
    int row[9];

    // Initializing each class textons matrix
    int i=0, j=0, k=0;
    int m=0, n=0;
    for(i=0; i<9; i++){
        centers[i] = cv::Mat(15,24,CV_32FC1);        
        row[i] = 0;
    }
    std::cout << "Calculating the dictionary of texture elements "<<  std::endl;
    std::cout << "From image # "<<start_index <<"\n"<<"To image   # "<< finish_index<<std::endl;

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
        class_data[i] = cv::Mat(row[i],24,CV_32FC1, cv::Scalar(0));        
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
        std::cout << "\r" <<"[ " <<green<<porcentage(m, 8)<<reset<<" %] Obtaining textons of the " << m + 1 << " class."<<std::flush<<"\r";
        kmeans(class_data[m], clusters, labels[m], cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, iterations, epsilon), attempts, initial_centers, centers[m]);
    }
    std::cout << std::endl;

    // Saving the 135 textons on the dictionary
    n = 0;
    for(j=0; j<9; j++){
        for(k=0; k<15; k++){
            for(m=0; m<24; m++){
                dictionaryTextons.at<float>(n,m) = centers[j].at<float>(k,m);
            }
            n++;
        }
    }
}

int getNearestTexton(cv::Mat dictionaryTextons, float r24[24]){
    float nearest_distance = 0;
    int nearest_texton = 0;
    float euclidean = 0;
    int i, j;

    // Calculate the minimun euclidean distance for all the textons in the dicionary
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
    // Write the dictionary in the binary file
    int j, k, m, n = 0;
    for(j=0; j<9; j++){
        for(k=0; k<15; k++){
            for(m=0; m<24; m++){
                file.write((const char*) &dictionary.at<float>(n,m), sizeof(dictionary.at<float>(n,m)));
            }
            n++;
        }
    }
    file.close();

    return true;
}

bool loadDictionaryTextons(cv::Mat dictionary, std::string path){

    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary );
    if (!file){
        std::cout << "Error loading dictionary.bin..." << std::endl;
        return false;
    }
    std::cout << "Loading dictionary from binary file... " << std::endl;
    int j, k, m, n = 0;
    // Reading the dictionary of textons from the binary file
    for(j=0; j<9; j++){
        for(k=0; k<15; k++){
            for(m=0; m<24; m++){
                file.read((char*) &dictionary.at<float>(n,m), sizeof(dictionary.at<float>(n,m)));
                //dictionaryTextons.at<float>(n,m) = class_data[j].at<float>(k,m);
            }
            n++;
        }
    }
    return true;
}

struct img_dataHistogram getHistogramDescriptor(std::string fileName, cv::Mat img_MR, cv::Mat dictionary, int year, int index){
    struct img_dataHistogram dataH;
    cv::Mat img_Texton(img_MR.size().height, img_MR.size().width, CV_32FC1);
    int n_overflow =0;
    int i, j;
    int k;
    float r24[24];
    dataH.index = 0;
    dataH.year = 0;

    // Modify the number of labeled points for each set
    if(year == 2008 || year == 2009){
        dataH.n_labels = 200;
    }
    if(year == 2010){
        dataH.n_labels = 199;
    }
    std::ifstream file(fileName);

    // Get the nearest texton for all the vectors in img_MR
    // And saving the texton number in img_Texton
    for(i=0; i<img_MR.size().height; i++){
        for(j=0; j<img_MR.size().width; j++){
            for(k=0; k<24; k++){
                r24[k] = img_MR.at<cv::Vec<float, 24>>(i, j)[k];
            }
            img_Texton.at<int>(i, j) = getNearestTexton(dictionary, r24);
        }
    }
    // Read the coordinates and label data from each txt image data
    if (file.is_open()){
        std::string str;
        std::getline(file,str);
        dataH.index = index;
        dataH.year = year;
        for(i=0; i<dataH.n_labels; i++){ 
            file >> str;       
            dataH.key_Point[i].pt.y = atoi(str.c_str())/2;
            file >> str;
            dataH.key_Point[i].pt.x = atoi(str.c_str())/2;
            file >> str;
            dataH.key_Point[i].type = str2label(str);
        }
        // Save the histogram descriptor in img_DataHistogram structure
        for(i=0; i< dataH.n_labels ; i++){
            if(dataH.key_Point[i].type != 0){
                getPatchs(img_Texton, dictionary, &dataH.key_Point[i]);
                if(dataH.key_Point[i].type==0)
                    n_overflow++;
            }
        }
        file.close();
    } else{
        std::cout << "Error loading "  + fileName << std::endl;
    }
    return dataH;
}

void getPatchs(cv::Mat img_Texton, cv::Mat dictionary, struct keyPointHistogram* key_Point){

    cv::Mat p221(221, 221, CV_32FC1, cv::Scalar(0));
    int histograms[4][135];
    int x = 0, y = 0;
    int i, j;
    // Half Size (height or width of rectangle) of study areas
    int hSize[4] = {10, 30, 60, 110};
    y = key_Point->pt.y;
    x = key_Point->pt.x;
    x=0;
    y=0;
    // Obtaining the R^24 vectors with patchs 21, 61, 121 and 221
    for(int j=key_Point->pt.y - hSize[3]; j<key_Point->pt.y + hSize[3]+1; j++){
        for(int k=key_Point->pt.x - hSize[3]; k<key_Point->pt.x + hSize[3]+1; k++){
            // if the study area is fully inside the image
            if((j > 0) && (j < img_Texton.size().height) && (k > 0) && (k < img_Texton.size().width)){
                p221.at<int>(x,y) = img_Texton.at<int>(j, k);
            }else{
                p221.at<int>(x,y) = -1;
            }
            y++;
        }
        y = 0;
        x++;
    }

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

void getHistogramTextons(cv::Mat img, int histogram[4][135], int hSize[4]){
    int max = 0;
    int center = hSize[3];
    int j, k;
    int n; 
    int aux =0;
    
    // Computing the histogram
    for(j= center - hSize[3]; j< center + hSize[3] + 1; j++){
        for(k=center - hSize[3]; k<center + hSize[3] + 1; k++){
            if(img.at<int>(j,k)!=-1){
                // For patch 21, 61 and 121
                for(n=0; n<3; n++){
                    if(j < center + hSize[n] + 1 && j > center - hSize[n] && k < center + hSize[n] + 1 && k > center - hSize[n])
                        histogram[n][img.at<int>(j,k)] += 1;
                }
                // For patch 221
                aux = img.at<int>(j,k);
                histogram[3][img.at<int>(j,k)] += 1;
            }
        }
    }
}

void normalizeHistogramsTextons(int histograms[][135] , float histogram[540]){
    int max = 0;
    int aux = 0;
    int i, j;
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

void saveSVMtxt(struct img_dataHistogram* dataH, int n_images){
    std::string filename = "data-for-libsvm";
    int i, j, k;
    std::ofstream file(filename);
    if(n_images == 0)
        n_images = sizeof(dataH)/sizeof(struct img_dataHistogram);

    // write the histogram descriptor in a file to use with libsvm library
    if (file.is_open()){
        for(k=0; k< n_images; k++){
            for(i=0; i<dataH[k].n_labels; i++){
                if(dataH[k].key_Point[i].type != 0){
                    file << dataH[k].key_Point[i].type;
                    file << " ";
                    for(j=0; j<540; j++){
                        if(dataH[k].key_Point[i].histogram[j] != 0.0){
                            file << j << ":";
                            file << dataH[k].key_Point[i].histogram[j];
                            file << " ";
                        }
                    }
                    file << std::endl;
                }
            }
            std::cout << "[ " <<green<<porcentage(k,n_images)<<reset<< " %]" << std::flush << "\r";
        }
        std::cout << "["+green+"100"+reset+" %] "+green+"OK"+reset << std::flush << std::endl;
        file.close();
    } else{
        std::cout << "Error openning "  + filename << std::endl;
    } 
}


// FUNCION PARA IMPRIMIR EN ARCHIVO EL SVM_PROBLEM  DESDE DOS SETS DISTINTOS
void saveSVMtxt2(struct img_dataHistogram* dataH_2008, struct img_dataHistogram* dataH_2009, int n_images){
   std::string filename = "coral_svm_20082009";
   int array[10];
   std::ofstream file(filename);
    if (file.is_open()){
        int i, j, k;

        for(i=0;i<10;i++)
            array[i] =0;

        for(k=0; k< n_images; k++){
            for(i=0; i<dataH_2008[k].n_labels; i++){
                if(dataH_2008[k].key_Point[i].type != 0){
                    file << dataH_2008[k].key_Point[i].type;
                    file << " ";
                    array[dataH_2008[k].key_Point[i].type]++;// Para mostrar cuantos hay de cada clase (se puede borrar)
                    for(j=0; j<540; j++){
                        if(dataH_2008[k].key_Point[i].histogram[j] != 0.0){
                            file << j << ":";
                            file << dataH_2008[k].key_Point[i].histogram[j];
                            file << " ";
                        }
                    }
                    file << std::endl;
                }
            }
        }

        for(k=0; k< n_images; k++){
            for(i=0; i<dataH_2009[k].n_labels; i++){
                if(dataH_2009[k].key_Point[i].type != 0){
                    file << dataH_2009[k].key_Point[i].type;
                    file << " ";
                    array[dataH_2009[k].key_Point[i].type]++;// Para mostrar cuantos hay de cada clase (se puede borrar)
                    for(j=0; j<540; j++){
                        if(dataH_2009[k].key_Point[i].histogram[j] != 0.0){
                            file << j << ":";
                            file << dataH_2009[k].key_Point[i].histogram[j];
                            file << " ";
                        }
                    }
                    file << std::endl;
                }
            }
        }

        file.close();
    } else{
        std::cout << "Error openning "  + filename << std::endl;
    } 
}

void matrizC (int* predict, int* real, int total_k, float mat_conf[9][9], float mat_CvsNC[2][2], int totalRC[9]){
		
	for(int i=0;i<9; i++){ //inicializar arreglos y matriz
		for(int j=0;j<9; j++){
		mat_conf[i][j]=0;	
		}
		totalRC[i]=0;	
	}
		
	//
	//   1  | 2  | 3   4   5   6   7   8   9   <-- valores reales
	//1| 00 | 01 
	//2| 10 | 11
	//3| 20 | 21
	//4| 30 | 31
	//5| 40 | .
	//6| 50 | .
	//7| 60 | .
	//8| 70 |
	//9| 80 |
	//
    float full = 0;
	for(int i=0;i<total_k; i++){
	   totalRC[real[i]-1]++;  //cuenta la cantidad de etiquetas reales de cada clase
       mat_conf[real[i]-1][predict[i]-1]++;
       if(real[i]==predict[i]) full++;
	}
	full/=total_k;
	//Coral: Acrop Pavon Monti Pocill Porit
	//No coral: CCA Turf Macro  Sand
    float CNC = 0;
	int CvsNC[2]={0,0};
	CvsNC[0]= totalRC[0]+totalRC[1]+totalRC[2]+totalRC[3];
	CvsNC[1]= totalRC[4]+totalRC[5]+totalRC[6]+totalRC[7]+totalRC[8];
	for(int i=0;i<4; i++){//No corales clasificados como no corales  :)
		for(int j=0;j<4; j++){
			mat_CvsNC[0][0]+=mat_conf[i][j];
		}	
	}
    CNC = mat_CvsNC[0][0];
	mat_CvsNC[0][0]/=CvsNC[0];
	for(int i=4;i<9; i++){//No corales clasificados como corales   :(
		for(int j=0;j<4; j++){
			mat_CvsNC[1][0]+=mat_conf[i][j];
		}	
	}
	mat_CvsNC[1][0]/=CvsNC[1];
	for(int i=0;i<4; i++){//Corales clasificados como no corales   :(
		for(int j=4;j<9; j++){
			mat_CvsNC[0][1]+=mat_conf[i][j];
		}	
	}
	mat_CvsNC[0][1]/=CvsNC[0];
	for(int i=4;i<9; i++){//Corales clasificados como corales      :)
		for(int j=4;j<9; j++){
			mat_CvsNC[1][1]+=mat_conf[i][j];
		}	
	}
    CNC+=mat_CvsNC[1][1];
	mat_CvsNC[1][1]/=CvsNC[1];
	
    CNC/=(CvsNC[0]+CvsNC[1]);
    std::cout << "Full: " << full*100 << '%' << std::endl;
    std::cout << "Coral, non-coral: " << CNC*100 << '%' << std::endl;
	for(int i=0;i<9; i++){
		for(int j=0;j<9; j++){
			mat_conf[j][i]/=totalRC[j];		
		}	
	}
			
}

int* fileToArray(std::string fileName, int* total_k){
		*total_k=0;
		int* array;
		std::vector<int> aux_array;
		int i=0;
		std::ifstream file(fileName);
    	if (file.is_open()){
        	std::string str;
        	
        	while(!file.eof()){        
            	file >> str;
				aux_array.push_back(atoi(str.c_str()));
				std::getline(file,str);
				(*total_k)++;
				//std::cout<< *total_k << std::endl;
        	}
        	file.close();
		} else{
			std::cout << "Error loading "  + fileName << std::endl;
		}
		array = new int[aux_array.size()];
		for(int i=0; i<aux_array.size();i++){
			array[i] = aux_array.at(i);
		}
	return array;
}
	
	
void getConfusionMatrix(int test_n){
	int* predict;
	int* real;
	float mat_CvsNC[2][2]={{0,0},{0,0}};
	float mat_conf[9][9];
	int total_k=0;
    int totalRC[9];
	int* ptr = &total_k;
    std::string fileName;

    fileName = "./Results/Test" + std::to_string(test_n) + "/";

	predict = fileToArray(fileName + "predict", ptr);
	real = fileToArray(fileName + "real",&total_k);
	//int total_k = (sizeof real / sizeof real[0]);
    std::cout << std::endl << "Results of Test " << test_n << ":" << std::endl;
	matrizC(predict,real, total_k,mat_conf,mat_CvsNC, totalRC);
	std::string str[] = {"CCA   ", "Turf  ", "Macro ","Sand  ", "Acrop ","Pavon ","Monti ","Pocill","Porit "};
	std::cout<<std::endl<<"CONFUSION MATRIX"<<std::endl<<"         CCA    Turf   Macro  Sand   Acrop  Pavon  Monti  Pocill Porit"<<std::endl;
	for(int i=0;i<9; i++){
		std::cout<<str[i]<<"  |";
		for(int j=0;j<9; j++){
				std::cout<<std::fixed<<std::setprecision(2) <<mat_conf[i][j]<<"   ";
		}
        std::cout<<std::fixed<<std::setprecision(2) <<totalRC[i]<<"   ";
		std::cout<<std::endl;
	}
    return;	
}
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
#include <tuple>
#include "./args/args.hxx"

std::istream& operator>>(std::istream& is, std::tuple<int, int, int>& ints){
    is >> std::get<0>(ints);
    is.get();
    is >> std::get<1>(ints);
    is.get();
    is >> std::get<2>(ints);
    return is;
}

int main(int argc, char **argv){
    int n_images;
    std::string outfile, infile;

    // Argument Parser flags
    args::ArgumentParser parser("[EC5803] Coral Identification", "Authors: Diego Peña, Victor Garcia, Fabio Morales, Andreina Duarte");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<std::string> load(parser, "File.bin", "name of binary file to load (with .bin)", {'l', "load"});
    args::ValueFlag<std::string> output(parser, "Filename", "name of binary file to save data (with .bin), no file will be saved by default", {'o', "output"});
    args::ValueFlag<int> n_img(parser, "Integer", "Number of images to test (Temporal option)", {'n',"number"});
    args::ValueFlag<std::string> dic(parser, "Filename", "Create a Dictionary of Textons and save in Filename.bin (with .bin)", {'d'});
    args::Flag train(parser, "t", "Train the SVM (must specify a years of dataset)", {'t', "train"});
    args::Group aux(parser, "  (Optional)", args::Group::Validators::All);
    args::Group nxor(aux, "Modify the parameters of K-means algorithm:", args::Group::Validators::AllOrNone);
    args::ValueFlag<int> kmeans(nxor, "Criteria", "Stop criteria for K-means clustering (1 for iteration, 2 for epsilon, 3 both)[If set mus specify all the K-means criteria parameters]", {'k'});
    args::ValueFlag<int> i(nxor, "Integer", "Iteration numbers for K-means clustering (0 if not used)", {'i'});
    args::ValueFlag<int> a(nxor, "Integer", "Attempt number for K-means clustering (0 if not used)", {'a'});
    args::ValueFlag<float> e(nxor, "Float", "epsilon: distance theshold (0 if not used)", {'e'});
    args::Positional<std::tuple<int, int, int> > year(parser, "YEARS", "years of set to work with (separate by commas)");
    
    try{
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help){
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e){
        /*
        INSERTE CODIGO PARA ERRORES DE VALIDACION DE PARAMETROS
        */
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError e){
        /*
        INSERTE CODIGO PARA ERRORES DE VALIDACION DE GRUPO
        */
        std::cerr << "If k flag is set, must specify all the K-means criteria parameters" << std::endl;
        std::cerr << parser;
        return 1;
    }
    if(load){
        /*
        INSERTE CODIGO PARA LA BANDERA L
        */
        infile = args::get(load);
        std::cout << "File to load: " << "\"" <<infile <<"\""<<std::endl;
    }
    if (output){
        /*
        INSERTE CODIGO PARA LA BANDERA O
        */
        outfile = args::get(output);
        std::cout << std::endl << "Output file: " << "\"" << outfile << "\""<< std::endl; 
    }
    if (year){
        /*
        INSERTE CODIGO PARA LA BANDERA YEAR
        */
        int valid_year[3] = {2008, 2009, 2010};
        int get_years[3] = {0,0,0};
        bool yr_check=false;
        get_years[0]=std::get<0>(args::get(year));
        get_years[1]=std::get<1>(args::get(year));
        get_years[2]=std::get<2>(args::get(year));
        for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
                if( get_years[i] == valid_year[j] ){
                    yr_check=true;
                }
            }
            if(yr_check){
                std::cout << "\nSet to load: ";
                std::cout << " " << get_years[i];
                yr_check=false;
            }
        }
        std::cout << std::endl;
    } 
    if(train){
        /*
        INSERTE CODIGO PARA LA BANDERA K
        */
        std::cout << "SVM train: " << std::endl;
    }
    if(n_img){
        /*
        INSERTE CODIGO PARA LA BANDERA N
        */
        n_images = args::get(n_img);
        std::cout << "Test with: " <<  n_images << std::endl;
    }
    /*
    CODIGO EN FUNCION DE LAS BANDERAS ACTIVAS
    */

    // Creating the data struct where the information is going to be saved
    n_images = 2055;
    struct img_data* data = new struct img_data[n_images];

    // Applying the Maximum Response Filter to each image of the 2008, 2009 and
    // 2010 set. Then reads each image info .txt to save the results on data structure
    // getDataSet(data, n_images);

    // Save the image data in a binary file
    // saveDescriptor(data, n_images);

    // Load the image data from a binary file
    data = loadDescriptor(n_images);
    // print test, first the coordinates and labels, next an r24 vector information
    int image = 0;
    /*for(int i = 0; i<data[image].n_labels ;i++){
        std::cout << data[image].key_Point[i].pt.y*2 << " " << data[image].key_Point[i].pt.x*2 << " " << data[image].key_Point[i].type << std::endl;
    }
     std::cout << "\n------------------------------------------------\n";*/
    
    // Obtaining the textons from a group of images of the data
    cv::Mat dictionaryTextons;
    dictionaryTextons = cv::Mat(135, 24, CV_32FC1);
    int start_index = 0, finish_index = 2055;

    //getDictionaryTextons(dictionaryTextons, data, start_index, finish_index);

    //saveDictionaryTextons(dictionaryTextons, "dictionary.bin");

    //loadDictionaryTextons(dictionaryTextons, "dictionary.bin");
    readTextonsMatlab(dictionaryTextons, "textonMapMATLAB.txt");

    // Obtaining the Textons Histograms from the data_set
    struct img_dataHistogram* dataH = new struct img_dataHistogram[n_images];

    getDataHistogram(dataH, dictionaryTextons, n_images);

    for(int k=0; k<20;k++){
        std::cout << "Key point: " << k << std::endl;
        for(int j=0; j<4;j++){
            for(int i=135*j; i<135*(j+1); i++){
                if(dataH->key_Point[k].histogram[i]==1){
                    std::cout << "Patch"<< j <<": " <<(i-135*j)/15+1 << std::endl;
                }
            }
        }
        std::cout << "Correct class: " << dataH->key_Point[k].type << std::endl;
        std::cout << "----------------------" << std::endl;
    }

    // Freeing space of the data struct
    delete [] data;

    return 0;
}

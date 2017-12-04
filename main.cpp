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
#include "options.h"


int main(int argc, char **argv){
    // Descriptor structure declaration
    struct img_data* descriptors;
    struct img_dataHistogram* descriptorsH;
    struct kmeans_param k_m;
    // SVM structure declaration
    struct svm_problem prob;
    struct svm_parameter param;
    struct svm_model *model;
    struct svm_grid grid_params;
    // Local variables declaration
    const int years[3] = {2008, 2009, 2010};
    int n_imgs[] = {_nImg_2008,  _nImg_2009,  _nImg_2010};
    bool valid_sets[] = {false,false, false};
    bool arg_svm = false;
    int n_images, i, j, k, n;
    int start_index, finish_index;
    cv::Mat* imgs_MR;
    std::string loadfile, savefile;
    std::vector<std::string> file_names;
    const char *error_msg;

    // Initialize Argument Parser library
    try{
        parser.ParseCLI(argc, argv);
        std::cout << std::endl;
    }
    // Show help menu
    catch (args::Help){
        std::cout << parser;
        return 0;
    }
    // Args group validation
    catch (args::ValidationError e){
        /*if(!descriptor || !dictionary || !histogram)
            std::cerr << "Must select one command" << std::endl;
        if(kmeans)
            std::cerr << "If Kmeans flag is set, must specify all the Kmeans criteria parameters" << std::endl;
        std::cout << "Use -h, --help command to see usage" << std::endl;
        return 0;*/
    }
    // Args imput data validation
    catch (args::ParseError e){
        std::cerr << e.what() << std::endl << std::endl;
        std::cerr << "Use -h, --help command to see usage" << std::endl;
        //std::cerr << parser;
        return 1;
    }
    // If YEARS values is recived. years of image sets
    if (year){
        std::vector<int> get_years;
        bool yr_check=false;
        // Read the imput years
        get_years.push_back(std::get<0>(args::get(year)));
        get_years.push_back(std::get<1>(args::get(year)));
        get_years.push_back(std::get<2>(args::get(year)));
        // Check for valid years set
        for(i=0; i<3; i++){
            for(j=0; j<3; j++){
                if( get_years.at(j) == years[i] ){
                    valid_sets[i]=true;
                    break;
                }
            }
        }
        for(i=0; i<3; i++)
            if(valid_sets[i])
                std::cout << "Valid Set: " << years[i] << std::endl;
    }else if(!dictionary){
        std::cerr << "Must select one command" << std::endl;
        std::cerr << parser;
        return 1;
    }
    // If the -n flag is set. Number of images to test
    if(n_img){
        std::vector<int> get_nimages;
        // Get the recived parameters for -n flag
        get_nimages.push_back(std::get<0>(args::get(n_img)));
        get_nimages.push_back(std::get<1>(args::get(n_img)));
        get_nimages.push_back(std::get<2>(args::get(n_img)));
        n=0;
        // For --dictionary command must have min,max format
        if(dictionary){
            start_index = get_nimages.at(0);
            finish_index = get_nimages.at(1);
            n_images = finish_index - start_index;
            if(n_images<0){
                std::cout << "For --dictionary command must specify in start,finish format" << std::endl<< std::endl;
                std::cerr << "Use -h, --help command to see usage" << std::endl;
                return 1;
            }
        }
        // For rest of commands must have #img2008,#img2009,#img2010 format, one for each year
        else{
            for(i=0;i<3;i++){
                if(valid_sets[i]){
                    n_imgs[i]=get_nimages.at(n++);
                    n_images+=n_imgs[i];
                    std::cout << "Test set "<< years[i] <<" with " << n_imgs[i]<< " images" << std::endl;
                }
                else{
                    n_imgs[i]=0;
                }
            }

        }
        if((n_images < 200) && ((histogram && !load_d) || dictionary)){
            std::cout << "Not enought images to calculate a dictionary of textons " << std::endl;
            std::cout << "Try with > 200 images" << std::endl<< std::endl;
            std::cerr << "Use -h, --help command to see usage" << std::endl;
            //std::cerr << parser;
            return 1;
        }
        std::cout << "total images to use: "<< n_images <<std::endl;
    }
    // If -n flag is not set, is assigned the maximum number of images for each set
    // Set 2008: 671
    // Set 2009: 695
    // Set 2010: 689
    else if(years){//(!tolibsvm || !grid || !train || !predict){
        for(i=0; i<3; i++){
            if(valid_sets[i]){
                n_images+=n_imgs[i];
                std::cout << "Test set "<< years[i] <<" with " << n_imgs[i]<< " images" << std::endl;
            }
        }
        if(n_images == 0)
            std::cout << "no set selected " <<std::endl;
        else
            std::cout << "total images to use: "<< n_images <<std::endl;
    }

    // If --kmeans flag is set, modify the kmeans criteria parameters
    if(kmeans){
        k_m.attempts = args::get(k_a);
        k_m.iterations = args::get(k_i);
        k_m.epsilon = args::get(k_e);
        std::cout << "Kmeans criteria modified: " <<std::endl;
        std::cout << "attempts:   "<< k_m.attempts<<std::endl;
        std::cout << "iterations: "<< k_m.iterations<<std::endl;
        std::cout << "epsilon :   "<< k_m.epsilon<<std::endl;
    }
    // Else set the default values
    else{
        k_m.attempts = 100;
        k_m.iterations = 800;
        k_m.epsilon = 0.0005;
    }
    if((descriptor || histogram) && year){
        // Read the filenames from default directory
        std::cout << "Reading filenames from  Vision_MCR directory" << std::endl;
        file_names = getFileNames(valid_sets, n_imgs);
    }
    // If is necesarry to calculate the Maximum Response Filter
    if(load_d && (dictionary || histogram)){
        // Load the image data from a binary file
        loadfile = args::get(load_d);
        std::cout << "Loading descriptor: "<< "\""<< loadfile << "\"" << std::endl;
        descriptors = loadDescriptor(n_images, loadfile);
    }else if (year && (descriptor || dictionary || histogram)){
        // Get the maximun Response Filter
        std::cout << "calculating  the Maximun Response Filter for: "<<n_images <<" images" << std::endl;
        imgs_MR = new cv::Mat[n_images];
        for(j=0; j<n_images ; j++){
            std::cout <<"[ " << green<<porcentage(j, n_images)<<reset<<" %]"<<std::flush<<"\r";
            imgs_MR[j] = getMaximumResponseFilter(file_names.at(j));
        }
        std::cout << "[" << green+"100"+reset<< " %]"<<green<<" OK"<<reset<< std::endl;
            
        if(!histogram){
            // Initialize the descriptor structure array
            descriptors = new struct img_data[n_images];
            std::cout << "Calculating texture descriptor for: "<< n_images << " images" << std::endl;
            // Get the texture descriptor for selected images
            n=0;
            for(k=0; k<3; k++){
                if(valid_sets[k]){
                    std::cout <<"Set: " << years[k]<< std::endl;
                    for(j=0; j<n_imgs[k] ; j++){
                        std::cout <<"[ " << green<<porcentage(j, n_imgs[k])<<reset << " %] "<<std::flush<<"\r";
                        descriptors[n] = getDescriptor(file_names.at(n)+".txt", imgs_MR[n], years[k],n);
                        n++;
                    }
                    std::cout << "["+green+"100"+reset+" %] "+green+"OK"+reset << std::flush << std::endl;
                }
            }
            std::cout <<std::endl;
            // Save the Maximum Response Filter information in a binary file
            if(save && descriptor){
                savefile = args::get(save);
                saveDescriptor(descriptors, n_images,  savefile );
                std::cout << "saved Descriptor in  "<< "\"" <<  savefile << "\"" << std::endl;
            }
            else if(!save && descriptor){
                saveDescriptor(descriptors, n_images, "descriptor.bin");
                std::cout << "saved Descriptor in  "<< "descriptor.bin" << std::endl;
            }
        }
    }
    cv::Mat dictionaryTextons(135, 24, CV_32FC1);
    if(load_tex && histogram){
        
        // Load the image data from a binary file
        loadDictionaryTextons(dictionaryTextons, args::get(load_tex));
        std::cout << "Dictionary of texture elemets Loaded: "<< "\"" <<args::get(load_tex) << "\""<<std::endl;
    }else if((year || load_d) && (dictionary || histogram)){
        if(!n_img){
            std::cout << "Must specify start and finish index to calculate the dictionary "<<  args::get(save) << std::endl;
            return 1;
        }
        // Obtaining the dictionary of textons from a group of images of the data
        getDictionaryTextons(dictionaryTextons, descriptors, start_index, finish_index, k_m);

        // Save the Maximum Response Filter information in a binary file
        if(save && dictionary){
            saveDictionaryTextons(dictionaryTextons, args::get(save));  
            std::cout << "saved Dictionary in  "<<  args::get(save) << std::endl;
        }
        else if(!save && dictionary){
            saveDictionaryTextons(dictionaryTextons, "dictionary.bin");  
            std::cout << "saved Dictionary in  "<<  "dictionary.bin" << std::endl;
        }
    }
    // If --histogram command is set. calculate the multipath histogram for the labeled points
    if(year && histogram){
        descriptorsH = new struct img_dataHistogram[n_images];
        n=0;
        // Calculate the histogram of textons for all the labeled points (multipath histogram)
        for(k=0; k<3; k++){
            if(valid_sets[k]){
                std::cout <<"Set: "<<years[k]<<std::endl; 
                for(j = 0; j<n_imgs[k]; j++){
                    std::cout << "[ " <<green<<porcentage(j,n_imgs[k])<<reset<< " %]" << std::flush << "\r";
                    descriptorsH[n] = getHistogramDescriptor(file_names.at(n)+".txt", imgs_MR[n], dictionaryTextons, years[k], n);
                    n++;
                }
                std::cout << "["+green+"100"+reset+" %] "+green+"OK"+reset << std::flush << std::endl;
            }
        }
        // Deleting the names of the set
        file_names.erase(file_names.begin(), file_names.end());

        savefile = "histogram.bin";
        // Save the Maximum Response Filter information in a binary file
        if(save){
            savefile = args::get(save);   
        }
        saveDescriptorH(descriptorsH, n_images, savefile);
        std::cout << "Saved histogram descriptor in  "<<  savefile << std::endl;  
    }
    // If --problem command is set. Create a txt file with the structure format to work with libsvm
    if(tolibsvm){
        std::cout << "Loaded "+args::get(tolibsvm)<< std::endl;  
        
        loadfile = args::get(tolibsvm); ;
        // Get the data from a binary file. 
        std::cout << "Obtaining the structure problem file from: "<<  loadfile << std::endl;
        descriptorsH = loadDescriptorH(n_images, loadfile);
        
        std::cout << "Converting into libSVM file format "<< std::endl;
        // Saving in libSVM structure
        saveSVMtxt(descriptorsH, n_images);
        std::cout << "Output filename: data-for-libsvm "<< std::endl;
    }
    if(grid){

        // Get the recived parameters for -n flag
        grid_params.min  = std::get<0>(args::get(grid));
        grid_params.max  = std::get<1>(args::get(grid));
        grid_params.step = std::get<2>(args::get(grid));
        grid_params.best_c =0;
        grid_params.best_g =0;
        std::cout << "Grid Search Grid(C,G) = { "<< grid_params.min<<", "<<grid_params.max <<", "<<grid_params.step <<" }"<< std::endl;
        /*
        bestParametersSVM(prob, param, grid_params);
        */
    }
    if(train){
        std::cout << "Trainin "+args::get(train)<< std::endl;
        std::cout << "Created SVM.model file"<< std::endl;
        /*
        getParamSVM(&param, exp2(grid_params.best_c), exp2(grid_params.best_g));
        getProblemSVM(&prob, descriptorsH, n_imgs);
        model = svm_train(&prob, &param); 
        svm_save_model(args::get(train), model); 
        */
    }
    if(predict){
        std::cout << "PREDICT COMMAND :"+args::get(predict)<< std::endl;
        /*
        model = svm_load_model(args::get(predict));
        */
        // FALTA hACER EL PREDICT
        // svm_predict(model, );
    }

    // Freeing space for used arrays
    delete [] descriptors;
    delete [] descriptorsH;
    delete [] imgs_MR;
    return 0;
}

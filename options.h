/********************************************
 * FILE NAME: options.h                       *
 * DESCRIPTION: Contains functions that are *
 *              useful to read data from    *
 *              images                      *
 * AUTHORS: Diego Peña, Victor García,      *
 *          Fabio Morales, Andreina Duarte  *
 ********************************************/

#ifndef OPTIONS
#define OPTIONS
#include <tuple>
#include "./args/args.hxx"
// Operator for reading the tuple of integers parameter separated by commas
std::istream& operator>>(std::istream& is, std::tuple<int, int, int>& ints){
    is >> std::get<0>(ints);
    is.get();
    is >> std::get<1>(ints);
    is.get();
    is >> std::get<2>(ints);
    return is;
}
// Argument Parser flags
args::ArgumentParser parser("[EC5803] Universidad Simon Bolivar \nAutomated Annotation of Coral Reef Survey Images", "Authors: Diego Peña, Victor Garcia, Fabio Morales, Andreina Duarte");

args::Group p(parser, "", args::Group::Validators::DontCare);
args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
args::Group imgproc(p, "IMAGE PROCESSING COMMANDS");
args::Flag descriptor(imgproc, "descriptor", "Get the texture descriptor for the labeled images. Use the -s command to change the default output binary filename = \"descriptor.bin\"", {"descriptor"});
args::Flag dictionary(imgproc, "dictionary", "Get the dictionary of textons. By default the texture descriptor will be calculated for this step. to use an existing \
                                    descriptor file use the -d argument. Use the -s argument to change the default binary output filename = \"dictionary.bin\"", {"dictionary"});
args::Flag histogram(imgproc, "histogram", "Get the histogram descriptor used for clasification. By default the dictionary of textons will be calculate for this step. to use an existing \
                                    texton dictionary file use the -t argument. Use the -s argument to change the default output binary filename = \"histogram.bin\"",  {"histogram"});
args::Positional<std::tuple<int, int, int> > year(imgproc, "YEARS", "years of set to work with (separate by commas)");

args::Group arguments(imgproc, "ARGUMENTS");
args::ValueFlag<std::string> load_d(arguments, "file.bin", "Load a previous descriptor file data (binary). Specifyname name with \".bin\"", {'d'});
args::ValueFlag<std::string> load_tex(arguments, "file.bin", "Load a previous texton dictionary file data (binary). Specifyname name with \".bin\"", {'t'});
args::ValueFlag<std::string> save(arguments, "file", "Use to modify the default file name for commands outputs. Speficy name with \".bin\"", {'s'});
args::ValueFlag<std::tuple<int, int, int>> n_img(arguments, "Integer", "Number of images to use. For multiple year sets specify the numbers separated by \
                                                             commas. For --dictionary command must specify in (start_index,final_index) format", {'n'});

args::Group km(arguments, "(Optional) Modify the parameters of K-means algorithm to calculate the dictionary of textons. this options is only for dictionary step.", args::Group::Validators::AllOrNone);
args::ValueFlag<int> kmeans(km, "Criteria", "Stop criteria for K-means clustering (1 for iteration, 2 for epsilon, 3 both)[If set mus specify all the K-means criteria parameters].", {'k'});
args::ValueFlag<int> k_i(km, "Integer", "Iteration numbers for K-means clustering (0 if not used).", {'i'});
args::ValueFlag<int> k_a(km, "Integer", "Attempt number for K-means clustering (0 if not used).", {'a'});
args::ValueFlag<float> k_e(km, "Float", "epsilon: distance theshold (0 if not used).", {'e'});

args::Group svm(p, "SVM COMMANDS");
args::ValueFlag<std::string> tolibsvm(svm, "file", "Generate a txt problem file to use with external libsvm. specify the input histogram filename.bin,\
                                                a svm-problem file will be generated.", {"problem"});
args::ValueFlag<std::tuple<int, int, int>>  grid(svm, "svm-grid", "optimize the C ang gamma parameters using an logarithmic\
                                                      grid search with 4-fold cross validation in the {-5,5} interval. imput format = min,max,step",{"grid"});
args::ValueFlag<std::string> train(svm, "svm-train", "train the SVM. By default Radial Basis Functionkernel will be used. Specify the input\
                             training file to generate an output model file",{"train"});
args::ValueFlag<std::string> predict(svm, "svm-predict", "Specify the imput test file to predict. An output file will be generated with the results",{"predict"});


#endif
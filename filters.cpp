/********************************************
 * FILE NAME: filters.h                     *
 * DESCRIPTION: Contains functions that are *
 *              useful for defining and     *
 *              using filters.              *
 * VERSION: 2.0 (11-NOV-2017)               *
 * AUTHORS: Diego Peña, Victor García,      *
 *          Fabio Morales, Andreina Duarte  *
 ********************************************/

#include "filters.h"
#include "histogram.h"

void getNormalizedGaussianKernel(cv::Mat kernel, float sigma){
    float center_x = ( kernel.size().width - 1 ) / 2;
    float center_y = ( kernel.size().height - 1 ) / 2;
    // Computing the exponential terms. All the coefficient should 
    // be divided by
    //      (2 * M_PI * sigma * sigma)
    // We do not care, however, because we are going to normalize
    // the kernel with respect to the L1 norm.
    float sum = 0, sum_abs=0;
    int i,j;
    for(i=0; i<kernel.size().width; i++){
        for(j=0; j<kernel.size().height; j++){
            kernel.at<float>(j,i) = exp( -( (i-center_x)*(i-center_x) + (center_y-j)*(center_y-j) ) / ( 2 * sigma * sigma ) );
            sum += kernel.at<float>(j,i);
        }
    }
    // Subtracting the mean value from every pixel, so the new mean is 0.
    kernel = kernel - sum/( kernel.size().width * kernel.size().height );
    // Normalizing with respect to the L1 norm.
    for(i=0; i<kernel.size().width; i++){
        for(j=0; j<kernel.size().height; j++){
            sum_abs += fabs(kernel.at<float>(j,i));
        }
    }
    kernel = kernel * (1 / sum_abs);
    /*
        // Testing the values:
        std::cout << "Oriented SDoG:\n";
        i = center_x;
        j = 0;
        std::cout << "\t(" << i << "," << j << ") = " << kernel.at<float>(j,i) << "\n";
        i = center_x;
        j = center_y - 4;
        std::cout << "\t(" << i << "," << j << ") = " << kernel.at<float>(j,i) << "\n";
        i = center_x;
        j = center_y - 3;
        std::cout << "\t(" << i << "," << j << ") = " << kernel.at<float>(j,i) << "\n";
        i = center_x;
        j = center_y - 2;
        std::cout << "\t(" << i << "," << j << ") = " << kernel.at<float>(j,i) << "\n";
        i = center_x;
        j = center_y - 1;
        std::cout << "\t(" << i << "," << j << ") = " << kernel.at<float>(j,i) << "\n";
        i = center_x;
        j = center_y;
        std::cout << "\t(" << i << "," << j << ") = " << kernel.at<float>(j,i) << "\n";
        i = center_x;
        j = center_y + 1;
        std::cout << "\t(" << i << "," << j << ") = " << kernel.at<float>(j,i) << "\n";
        i = center_x;
        j = center_y + 2;
        std::cout << "\t(" << i << "," << j << ") = " << kernel.at<float>(j,i) << "\n";
        std::cout << "\tsum = " << sum << "\n";
        std::cout << "\tsum_abs = " << sum_abs << "\n";
    */
}

void getNormalizedLoGKernel(cv::Mat kernel, float sigma){
    float center_x = ( kernel.size().width - 1 ) / 2;
    float center_y = ( kernel.size().height - 1 ) / 2;
    // Computing the exponential terms (we do not care about the
    // coefficients.)
    float sum = 0, sum_abs = 0, sum_temp = 0;
    int i,j;
    for(i=0; i<kernel.size().width; i++){
        for(j=0; j<kernel.size().height; j++){
            kernel.at<float>(j,i) = exp( -( (i-center_x)*(i-center_x) + (center_y-j)*(center_y-j) ) / ( 2 * sigma * sigma ) );
            sum_temp += kernel.at<float>(j,i);
        }
    }
    // Computing the coefficients.
    for(i=0; i<kernel.size().width; i++){
        for(j=0; j<kernel.size().height; j++){
            kernel.at<float>(j,i) = ( (i-center_x)*(i-center_x) + (center_y-j)*(center_y-j) - 2 * sigma * sigma ) / ( 2 * M_PI * sigma * sigma * sigma * sigma * sigma * sigma * sum_temp) * kernel.at<float>(j,i);
            sum += kernel.at<float>(j,i);
        }
    }
    // Subtracting the mean value from every pixel, so the new mean is 0.
    kernel = kernel - sum/( kernel.size().width * kernel.size().height );
    // Normalizing with respect to the L1 norm.
    for(i=0; i<kernel.size().width; i++){
        for(j=0; j<kernel.size().height; j++){
            sum_abs += fabs(kernel.at<float>(j,i));
        }
    }
    kernel = kernel * (1 / sum_abs);
}

void getNormalizedDoGKernel(cv::Mat kernel, float sigma, float angle){
    float center_x = ( kernel.size().width - 1 ) / 2;
    float center_y = ( kernel.size().height - 1 ) / 2;
    float sinA = sin(angle);
    float cosA = cos(angle);
    // Computing the traditional DoGX kernel.
    // All the coefficients should be divided by
    //      (6 * M_PI * sigma * sigma * sigma)
    // We do not care, however, because we are going to normalize
    // the kernel with respect to the L1 norm.
    float sum = 0, sum_abs = 0;
    int i,j;
    for(i=0; i<kernel.size().width; i++){
        for(j=0; j<kernel.size().height; j++){
            kernel.at<float>(j,i) = -( (i-center_x)*sinA + (center_y-j)*cosA ) * exp( -( (i-center_x)*(i-center_x)*(1 + 8*sinA*sinA) + (center_y-j)*(center_y-j)*(1 + 8*cosA*cosA) + 16*(i-center_x)*(center_y-j)*sinA*cosA ) / ( 18 * sigma * sigma ) );
            sum += kernel.at<float>(j,i);
        }
    }
    // Subtracting the mean value from every pixel, so the new mean is 0.
    kernel = kernel - sum / (kernel.size().width * kernel.size().height);
    // Normalizing with respect to the L1 norm.
    for(i=0; i<kernel.size().width; i++){
        for(j=0; j<kernel.size().height; j++){
            sum_abs += fabs(kernel.at<float>(j,i));
        }
    }
    kernel = kernel * (1 / sum_abs);
}

void getNormalizedSDoGKernel(cv::Mat kernel, float sigma, float angle){
    float center_x = ( kernel.size().width - 1 ) / 2;
    float center_y = ( kernel.size().height - 1 ) / 2;
    float sinA = sin(angle);
    float cosA = cos(angle);
    // Computing the traditional DoGX kernel.
    // All the coefficients should be divided by
    //      (6 * M_PI * sigma * sigma * sigma * sigma)
    // We do not care, however, because we are going to normalize
    // the kernel with respect to the L1 norm.
    float sum = 0, sum_abs = 0;
    int i,j;
    for(i=0; i<kernel.size().width; i++){
        for(j=0; j<kernel.size().height; j++){
            kernel.at<float>(j,i) = ( (i-center_x)*(i-center_x)*sinA*sinA + (center_y-j)*(center_y-j)*cosA*cosA - sigma*sigma + 2*(i-center_x)*(center_y-j)*sinA*cosA ) * exp( -( (i-center_x)*(i-center_x)*(1 + 8*sinA*sinA) + (center_y-j)*(center_y-j)*(1 + 8*cosA*cosA) + 16*(i-center_x)*(center_y-j)*sinA*cosA ) / ( 18 * sigma * sigma ) );
            sum += kernel.at<float>(j,i);
        }
    }
    // Subtracting the mean value from every pixel, so the new mean is 0.
    kernel = kernel - sum / (kernel.size().width * kernel.size().height);
    // Normalizing with respect to the L1 norm.
    for(i=0; i<kernel.size().width; i++){
        for(j=0; j<kernel.size().height; j++){
            sum_abs += fabs(kernel.at<float>(j,i));
        }
    }
    kernel = kernel * (1 / sum_abs);
}

void printKernel(cv::Mat kernel, std::string filename){
    // Creating the image that will be saved.
    cv::Mat img(kernel.size().height,kernel.size().width, CV_32FC1);
    // Finding kernel's minimum and maximum values.
    float min, max;
    min = *std::min_element(kernel.begin<float>(), kernel.end<float>());
    max = *std::max_element(kernel.begin<float>(), kernel.end<float>());
    // Scaling and saving the image.
    img = (kernel - min) * (255/(max-min));
    img.convertTo(img,CV_8U);
    cv::imwrite(filename, img);
}

void getMaximumResponseFilterKernels(cv::Mat kernel[38], float sigmaSmall, float sigmaMedium, float sigmaLarge, float sigmaSymmetrical){
    int i;
    // Defining the Derivative of Gaussian filters.
    for(i=0; i<6; i++){
        // Small scale.
        getNormalizedDoGKernel(kernel[i], sigmaSmall, M_PI*i/6);
        // Medium scale.
        getNormalizedDoGKernel(kernel[6+i], sigmaMedium, M_PI*i/6);
        // Large scale.
        getNormalizedDoGKernel(kernel[12+i], sigmaLarge, M_PI*i/6);
    }

    // Defining the Second Derivative of Gaussian filters.
    for(i=0; i<6; i++){
        // With sigma=1
        getNormalizedSDoGKernel(kernel[18+i], sigmaSmall, M_PI*i/6);
        // With sigma=3
        getNormalizedSDoGKernel(kernel[24+i], sigmaMedium, M_PI*i/6);
        // With sigma=8
        getNormalizedSDoGKernel(kernel[30+i], sigmaLarge, M_PI*i/6);
    }

    // Defining the Gaussian filter.
    getNormalizedGaussianKernel(kernel[36], sigmaSymmetrical);
    
    // Defining the Laplacian of Gaussian filter.
    getNormalizedLoGKernel(kernel[37], sigmaSymmetrical);
}

void maximumResponseFilter(cv::Mat img_Lab, cv::Mat img_MR, cv::Mat kernel[38]){
    // To store the filtered images:
    cv::Mat img_filtered[38];
    int i;
    for(i=0; i<38; i++){
        img_filtered[i] = cv::Mat(img_Lab.size().height,img_Lab.size().width, CV_32FC3);
    }

    // Applying the Derivative of Gaussian filters
    for(i=0; i<6; i++){
        // Small scale
        cv::filter2D(img_Lab, img_filtered[i], CV_32FC3, kernel[i], cv::Point(-1,-1), 0, cv::BORDER_ISOLATED);
        // Medium scale
        cv::filter2D(img_Lab, img_filtered[6+i], CV_32FC3, kernel[6+i], cv::Point(-1,-1), 0, cv::BORDER_ISOLATED);
        // Large scale
        cv::filter2D(img_Lab, img_filtered[12+i], CV_32FC3, kernel[12+i], cv::Point(-1,-1), 0, cv::BORDER_ISOLATED);
    }

    // Applying the Second Derivative of Gaussian filters
    for(i=0; i<6; i++){
        // Small scale
        cv::filter2D(img_Lab, img_filtered[18+i], CV_32FC3, kernel[18+i], cv::Point(-1,-1), 0, cv::BORDER_ISOLATED);
        // Medium scale
        cv::filter2D(img_Lab, img_filtered[24+i], CV_32FC3, kernel[24+i], cv::Point(-1,-1), 0, cv::BORDER_ISOLATED);
        // Large scale
        cv::filter2D(img_Lab, img_filtered[30+i], CV_32FC3, kernel[30+i], cv::Point(-1,-1), 0, cv::BORDER_ISOLATED);
    }

    // Applying the Gaussian filter
    cv::filter2D(img_Lab, img_filtered[36], CV_32FC3, kernel[36], cv::Point(-1,-1), 0, cv::BORDER_ISOLATED);

    // Applying the Laplacian of Gaussian filter
    cv::filter2D(img_Lab, img_filtered[37], CV_32FC3, kernel[37], cv::Point(-1,-1), 0, cv::BORDER_ISOLATED);

    // Defining the image where the MR filter response will be saved,
    // and applying the MR filter.
    // cv::Mat img_MR[8];
    // For the first six entries, we find the maximum response.
    int j, k, l;
    float max[3];
    for(i=0; i<6; i++){
        //img_MR[i] = cv::Mat(img_Lab.size().height,img_Lab.size().width, CV_32FC3);
        for(j=0; j<img_Lab.size().height; j++){
            for(k=0; k<img_Lab.size().width; k++){
                // L channel
                max[0] = 0;
                for(l=0; l<6; l++){
                    if ( fabs(img_filtered[6*i+l].at<cv::Vec3f>(j,k)[0]) > max[0] ) max[0] = fabs(img_filtered[6*i+l].at<cv::Vec3f>(j,k)[0]);
                }
                img_MR.at<cv::Vec<float, 24>>(j,k)[i] = max[0];
                // a channel
                max[1] = 0;
                for(l=0; l<6; l++){
                    if ( fabs(img_filtered[6*i+l].at<cv::Vec3f>(j,k)[1]) > max[1] ) max[1] = fabs(img_filtered[6*i+l].at<cv::Vec3f>(j,k)[1]);
                }
                img_MR.at<cv::Vec<float, 24>>(j,k)[i+8] = max[1];
                // b channel
                max[2] = 0;
                for(l=0; l<6; l++){
                    if ( fabs(img_filtered[6*i+l].at<cv::Vec3f>(j,k)[2]) > max[2] ) max[2] = fabs(img_filtered[6*i+l].at<cv::Vec3f>(j,k)[2]);
                }
                img_MR.at<cv::Vec<float, 24>>(j,k)[i+16] = max[2];
            }
        }
    }
    // For the last two, we use the Gaussian and LoG filters.
    for(i=0; i<6; i++){
        for(j=0; j<img_Lab.size().height; j++){
            for(k=0; k<img_Lab.size().width; k++){
                for(l=0; l<3; l++){
                    img_MR.at<cv::Vec<float, 24>>(j,k)[6 + 8*l] = img_filtered[36]. at<cv::Vec3f>(j,k)[l];
                    img_MR.at<cv::Vec<float, 24>>(j,k)[7 + 8*l] = img_filtered[37].at<cv::Vec3f>(j,k)[l];
                }
            }
        }
    }

    // Normalizing according to Fowles:
    float norm[3];
    for(j=0; j<img_Lab.size().height; j++){
        for(k=0; k<img_Lab.size().width; k++){
            // Computing the norm in the L channel
            norm[0] = 0;
            for(i=0; i<8; i++){
                norm[0] += img_MR.at<cv::Vec<float, 24>>(j,k)[i]*img_MR.at<cv::Vec<float, 24>>(j,k)[i];
            }
            norm[0] = sqrt(norm[0]);
            // Computing the norm in the a channel
            norm[1] = 0;
            for(i=0; i<8; i++){
                norm[1] += img_MR.at<cv::Vec<float, 24>>(j,k)[i+8]*img_MR.at<cv::Vec<float, 24>>(j,k)[i+8];
            }
            norm[1] = sqrt(norm[1]);
            // Computing the norm in the b channel
            norm[2] = 0;
            for(i=0; i<8; i++){
                norm[2] += img_MR.at<cv::Vec<float, 24>>(j,k)[i+16]*img_MR.at<cv::Vec<float, 24>>(j,k)[i+16];
            }
            norm[2] = sqrt(norm[2]);
            // Normalizing
            for(i=0; i<8; i++){
                // L channel
                img_MR.at<cv::Vec<float, 24>>(j,k)[i] = img_MR.at<cv::Vec<float, 24>>(j,k)[i] * log(1 + norm[0] / 0.03) / norm[0];
                // a channel
                img_MR.at<cv::Vec<float, 24>>(j,k)[i+8] = img_MR.at<cv::Vec<float, 24>>(j,k)[i+8] * log(1 + norm[1] / 0.03) / norm[1];
                // b channel
                img_MR.at<cv::Vec<float, 24>>(j,k)[i+16] = img_MR.at<cv::Vec<float, 24>>(j,k)[i+16] * log(1 + norm[2] / 0.03) / norm[2];
            }
        }
    }
}

cv::Mat getMaximumResponseFilter(std::string filename){
    // Reading the image.
    cv::Mat img = cv::imread(filename,CV_LOAD_IMAGE_COLOR);

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

    return img_MR;
}

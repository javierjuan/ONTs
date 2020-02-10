/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2020 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Adaptive histogram Equalization                                          *
***************************************************************************/

#include <ITKUtils.hpp>
#include <itkImage.h>
#include <itkAdaptiveHistogramEqualizationImageFilter.h>


template<typename ImageType>
void AdaptiveHistogramEqualization(int argc, char *argv[])
{
    using AdaptiveHistogramEqualizationImageFilterType = itk::AdaptiveHistogramEqualizationImageFilter<ImageType>;

    // Read image
    typename ImageType::Pointer image = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[1]));
    // Default radius, alpha and beta parameters
    unsigned int radius = 3;
    double alpha = 0.8;
    double beta = 1;
    // Get user parameters
    if (argc >= 5)
        radius = (unsigned int) std::atoi(argv[3]);
    if (argc >= 6)
        alpha = std::strtod(argv[4], NULL);
    if (argc >= 7)
        beta = std::strtod(argv[5], NULL);
    // Adaptive Histogram equalization
    typename AdaptiveHistogramEqualizationImageFilterType::Pointer filter = AdaptiveHistogramEqualizationImageFilterType::New();
    filter->SetInput(image);
    filter->SetRadius(radius);
    filter->SetAlpha(alpha);
    filter->SetBeta(beta);
    // Save image
    ITKUtils::WriteNIfTIImage<ImageType>(filter->GetOutput(), std::string(argv[2]));
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: AdaptiveHistogramEqualization inputImage outputImage [radius=3] [alpha=0.8] [beta=1]" << std::endl;
        return EXIT_FAILURE;
    }

    typename itk::ImageIOBase::Pointer imageIO = ITKUtils::ReadImageInformation(std::string(argv[1]));
    const unsigned int ImageDimension = imageIO->GetNumberOfDimensions();

    if (ImageDimension < 2 || ImageDimension > 4)
    {
        std::cerr << "Unsupported image dimensions" << std::endl;
        return EXIT_FAILURE;   
    }

    try
    {
        if (ImageDimension == 2)
            AdaptiveHistogramEqualization<itk::Image<float, 2>>(argc, argv);
        else if (ImageDimension == 3)
            AdaptiveHistogramEqualization<itk::Image<float, 3>>(argc, argv);
        else
            AdaptiveHistogramEqualization<itk::Image<float, 4>>(argc, argv);
    }
    catch (itk::ExceptionObject & err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
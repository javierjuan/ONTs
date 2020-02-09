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


template<class ImageType>
void AdaptiveHistogramEqualization(int argc, char *argv[])
{
    using AdaptiveHistogramEqualizationImageFilterType = itk::AdaptiveHistogramEqualizationImageFilter<ImageType>;

    // Read image
    typename ImageType::Pointer image = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[2]));
    // Default radius, alpha and beta parameters
    unsigned int radius = 3;
    double alpha = 0.8;
    double beta = 1;
    // Get user parameters
    if (argc >= 5)
        radius = (unsigned int) std::atoi(argv[4]);
    if (argc >= 6)
        alpha = std::strtod(argv[5], NULL);
    if (argc >= 7)
        beta = std::strtod(argv[6], NULL);
    // Adaptive Histogram equalization
    typename AdaptiveHistogramEqualizationImageFilterType::Pointer filter = AdaptiveHistogramEqualizationImageFilterType::New();
    filter->SetInput(image);
    filter->SetRadius(radius);
    filter->SetAlpha(alpha);
    filter->SetBeta(beta);
    // Save image
    ITKUtils::WriteNIfTIImage<ImageType>(filter->GetOutput(), std::string(argv[3]));
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: AdaptiveHistogramEqualization dimensions inputImage outputImage [radius=3] [alpha=0.8] [beta=1]" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        if (std::atoi(argv[1]) == 2)
            AdaptiveHistogramEqualization<itk::Image<float, 2>>(argc, argv);
        else if (std::atoi(argv[1]) == 3)
            AdaptiveHistogramEqualization<itk::Image<float, 3>>(argc, argv);
        else
        {
            std::stringstream s;
            s << "Unsupported image dimensions." << std::endl;
            throw std::runtime_error(s.str());
        }
    }
    catch (itk::ExceptionObject & err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
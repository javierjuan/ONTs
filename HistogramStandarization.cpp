/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2014 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Histogram standarization                                                 *
***************************************************************************/

#include <ITKUtils.hpp>
#include <itkImage.h>
#include <itkHistogramMatchingImageFilter.h>


template<class ImageType>
void HistogramMatching(int argc, char *argv[])
{
    using HistogramMatchingFilterType = itk::HistogramMatchingImageFilter<ImageType, ImageType>;

    // Read input image
    typename ImageType::Pointer imageSource = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[1]));
    // Read reference image
    typename ImageType::Pointer imageReference = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[2]));
    // Check consistency
    ITKUtils::AssertCompatibleImageAndMaskSizes<ImageType, ImageType>(imageSource, imageReference);
    // Get number of histogram bins
    unsigned int bins = 128;
    if (argc > 4)
        bins = (unsigned int) std::atoi(argv[4]);
    // Get number of match points
    unsigned int matchPoints = 10;
    if (argc > 5)
        matchPoints = (unsigned int) std::atoi(argv[5]);
    // Apply histogram matching filter
    typename HistogramMatchingFilterType::Pointer matcher = HistogramMatchingFilterType::New();
    matcher->SetSourceImage(imageSource);
    matcher->SetReferenceImage(imageReference);
    matcher->SetNumberOfHistogramLevels(bins);
    matcher->SetNumberOfMatchPoints(matchPoints);
    matcher->ThresholdAtMeanIntensityOn();
    matcher->Update();
    // Save image
    ITKUtils::WriteNIfTIImage<ImageType>(matcher->GetOutput(), std::string(argv[3]));
}


int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: HistogramStandarization sourceImage referenceImage outputImage [bins=128] [matchPoints=10]" << std::endl;
        return EXIT_FAILURE;
    }
    
    try
    {
        if (std::atoi(argv[1]) == 2)
            HistogramMatching<itk::Image<float, 2>>(argc, argv);
        else if (std::atoi(argv[1]) == 3)
            HistogramMatching<itk::Image<float, 3>>(argc, argv);
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
}
/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2020 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Truncate Negatives                                                       *
***************************************************************************/

#include <ITKUtils.hpp>
#include <itkImage.h>
#include <itkImageRegionIterator.h>


template<typename ImageType>
void TruncateNegatives(char *argv [])
{
    // Read image
    typename ImageType::Pointer image = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[1]));
    // Truncate negatives
    itk::ImageRegionIterator<ImageType> iterator(image, image->GetLargestPossibleRegion());
    iterator.GoToBegin();
    while (!iterator.IsAtEnd())
    {
        if (iterator.Get() < 0)
            iterator.Set(0);
        ++iterator;
    }
    // Save image
    ITKUtils::WriteNIfTIImage<ImageType>(image, std::string(argv[3]));
}


int main(int argc, char *argv [])
{
    if (argc != 3)
    {
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: TruncateNegatives inputImage outputImage" << std::endl;
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
            TruncateNegatives<itk::Image<float, 2>>(argv);
        else if (ImageDimension == 3)
            TruncateNegatives<itk::Image<float, 3>>(argv);
        else
            TruncateNegatives<itk::Image<float, 4>>(argv);
    }
    catch (itk::ExceptionObject & err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
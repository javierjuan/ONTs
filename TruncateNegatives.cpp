/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2020 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Truncate Negatives                                                       *
***************************************************************************/

#include <cstdlib>
#include <ITKUtils.hpp>
#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionIteratorWithIndex.h>


template<typename ImageType, typename MaskType>
void TruncateNegativesMask(int argc, char *argv [])
{
    // Image dimensions
    const unsigned int ImageDimension = ImageType::ImageDimension;
    const unsigned int MaskDimension = MaskType::ImageDimension;
    // Read image
    typename ImageType::Pointer image = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[1]));
    // Truncate value
    const typename ImageType::PixelType truncateValue = (typename ImageType::PixelType) std::atof(argv[3]);
    // Read mask
    typename MaskType::Pointer mask = ITKUtils::ReadNIfTIImage<MaskType>(std::string(argv[4]));
    // Inside mask truncate value
    typename ImageType::PixelType truncateValueMask = 0;
    if (argc > 5)
        truncateValueMask = (typename ImageType::PixelType) std::atof(argv[5]);
    // Truncate negatives
    itk::ImageRegionIteratorWithIndex<ImageType> iterator(image, image->GetLargestPossibleRegion());
    iterator.GoToBegin();
    while (!iterator.IsAtEnd())
    {
        itk::Index<ImageDimension> imageIndex = iterator.GetIndex();
        itk::Index<MaskDimension> maskIndex;
        // Need to do this because I can pass a 4D image and a 3D mask (or a 3D image and a 3D mask, where, therefore, imageIndex = maskIndex)
        for (int i = 0; i < MaskDimension; ++i)
            maskIndex[i] = imageIndex[i];
        const typename ImageType::PixelType imageValue = image->GetPixel(imageIndex);
        const typename MaskType::PixelType maskValue = mask->GetPixel(maskIndex);
        if (maskValue)
        {
            if (imageValue <= 0)
                iterator.Set(truncateValueMask);
        }
        else
        {
            if (imageValue <= 0)
                iterator.Set(truncateValue);
        }
        ++iterator;
    }
    // Save image
    ITKUtils::WriteNIfTIImage<ImageType>(image, std::string(argv[2]));
}


template<typename ImageType>
void TruncateNegatives(int argc, char *argv [])
{
    // Read image
    typename ImageType::Pointer image = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[1]));
    // Truncate value
    typename ImageType::PixelType truncateValue = 0;
    if (argc > 3)
        truncateValue = (typename ImageType::PixelType) std::atof(argv[3]);
    // Truncate negatives
    itk::ImageRegionIterator<ImageType> iterator(image, image->GetLargestPossibleRegion());
    iterator.GoToBegin();
    while (!iterator.IsAtEnd())
    {
        if (iterator.Get() <= 0)
            iterator.Set(truncateValue);
        ++iterator;
    }
    // Save image
    ITKUtils::WriteNIfTIImage<ImageType>(image, std::string(argv[2]));
}


int main(int argc, char *argv [])
{
    if (argc < 3 || argc > 6)
    {
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: TruncateNegatives inputImage outputImage [truncateValue=0] [maskImage] [insideMaskTruncateValue=0]" << std::endl;
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
            if (argc > 4)
                TruncateNegativesMask<itk::Image<double, 2>, itk::Image<unsigned char, 2>>(argc, argv);
            else
                TruncateNegatives<itk::Image<double, 2>>(argc, argv);
        else if (ImageDimension == 3)
            if (argc > 4)
                TruncateNegativesMask<itk::Image<double, 3>, itk::Image<unsigned char, 3>>(argc, argv);
            else
                TruncateNegatives<itk::Image<double, 3>>(argc, argv);
        else
            if (argc > 4)
                TruncateNegativesMask<itk::Image<double, 4>, itk::Image<unsigned char, 3>>(argc, argv);
            else
                TruncateNegatives<itk::Image<double, 4>>(argc, argv);
    }
    catch (itk::ExceptionObject & err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
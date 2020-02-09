/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2020 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Apply Mask                                                               *
***************************************************************************/

#include <ITKUtils.hpp>
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>


template<typename ImageType, typename MaskType>
void applyMask(char *argv [])
{
    ITKUtils::AssertCompatibleImageAndMaskTypes<ImageType, MaskType>();

    const unsigned int ImageDimension = ImageType::ImageDimension;
    const unsigned int MaskDimension = MaskType::ImageDimension;

    // Read image
    typename ImageType::Pointer image = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[2]));
    // Read mask
    typename MaskType::Pointer mask = ITKUtils::ReadNIfTIImage<MaskType>(std::string(argv[3]));
    // Check consistency
    ITKUtils::AssertCompatibleImageAndMaskSizes<ImageType, MaskType>(image, mask);
    // Get image size
    typename ImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();
    // Apply mask
    itk::ImageRegionConstIteratorWithIndex<MaskType> iterator(mask, mask->GetLargestPossibleRegion());
    iterator.GoToBegin();
    while (!iterator.IsAtEnd())
    {
        if (!iterator.Get())
        {
            itk::Index<MaskDimension> maskIndex = iterator.GetIndex();
            itk::Index<ImageDimension> imageIndex = ITKUtils::GetSpatialIndex<MaskDimension, ImageDimension>(maskIndex);

            image->SetPixel(imageIndex, 0);
            if (ImageDimension > MaskDimension)
            {
                for (int i = 0; i < imageSize[MaskDimension]; ++i)
                {
                    imageIndex[MaskDimension] = i;
                    image->SetPixel(imageIndex, 0);
                }    
            }
            
        }
        ++iterator;
    }
    // Save image
    ITKUtils::WriteNIfTIImage<ImageType>(image, std::string(argv[4]));
}


int main(int argc, char *argv [])
{
    if (argc != 5)
    {
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: ApplyMask dimensions inputImage maskImage outputImage" << std::endl;
        return EXIT_FAILURE;
    }

    // Typedefs
    using ScalarImageType = itk::Image<float, 3>;
    using ComponentsImageType = itk::Image<float, 4>;
    using MaskType = itk::Image<unsigned char, 3>;

    // Get dimension
    const unsigned int D = (unsigned int ) std::atoi(argv[1]);

    try
    {
        if (D == 3)
            applyMask<ScalarImageType, MaskType>(argv);
        else if (D == 4)
            applyMask<ComponentsImageType, MaskType>(argv);
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
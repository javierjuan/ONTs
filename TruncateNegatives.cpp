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
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>


template<typename ImageType>
void TruncateNegatives(char *argv [])
{
    // Read image
    typename ImageType::Pointer image = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[2]));
    // Truncate negatives
    itk::ImageRegionConstIteratorWithIndex<ImageType> iterator(image, image->GetLargestPossibleRegion());
    iterator.GoToBegin();
    while (!iterator.IsAtEnd())
    {
        if (iterator.Get() < 0)
            image->SetPixel(iterator.GetIndex(), 0);
        ++iterator;
    }
    // Save image
    ITKUtils::WriteNIfTIImage<ImageType>(image, std::string(argv[3]));
}


int main(int argc, char *argv [])
{
    if (argc != 4)
    {
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: TruncateNegatives dimensions inputImage outputImage" << std::endl;
        return EXIT_FAILURE;
    }

    // Typedefs
    using ScalarImageType = itk::Image<float, 3>;
    using ComponentsImageType = itk::Image<float, 4>;

    // Get dimension
    const unsigned int D = (unsigned int ) std::atoi(argv[1]);

    try
    {
        if (D == 3)
            TruncateNegatives<ScalarImageType>(argv);
        else if (D == 4)
            TruncateNegatives<ComponentsImageType>(argv);
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
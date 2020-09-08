/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2020 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Copy Header Information                                                  *
***************************************************************************/

#include <ITKUtils.hpp>
#include <itkImage.h>
#include <itkChangeInformationImageFilter.h>


template<class ImageType>
void CopyHeaderInformation(int argc, char *argv[])
{
    // Read input image
    typename ImageType::Pointer imageSource = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[1]));
    // Read reference image
    typename ImageType::Pointer imageReference = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[2]));
    // Check consistency
    ITKUtils::AssertCompatibleImageAndMaskSizes<ImageType, ImageType>(imageSource, imageReference);
    // Get copy spacing
    bool copySpacing = true;
    if (argc > 4)
        copySpacing = (bool) std::atoi(argv[4]);
    // Get copy origin
    bool copyOrigin = true;
    if (argc > 5)
        copyOrigin = (bool) std::atoi(argv[5]);
    // Get copy direction
    bool copyDirection = true;
    if (argc > 6)
        copyDirection = (bool) std::atoi(argv[6]);
    // Define the change image info filter type
    using ChangeInformationImageFilterType = itk::ChangeInformationImageFilter<ImageType>;
    typename ChangeInformationImageFilterType::Pointer changeInformationImageFilter = ChangeInformationImageFilterType::New();
    changeInformationImageFilter->SetInput(imageSource);
    if (copySpacing)
    {
        changeInformationImageFilter->SetOutputSpacing(imageReference->GetSpacing());
        changeInformationImageFilter->ChangeSpacingOn();
    }
    if (copyOrigin)
    {
        changeInformationImageFilter->SetOutputOrigin(imageReference->GetOrigin());
        changeInformationImageFilter->ChangeOriginOn();
    }
    if (copyDirection)
    {
        changeInformationImageFilter->SetOutputDirection(imageReference->GetDirection());
        changeInformationImageFilter->ChangeDirectionOn();
    }
    changeInformationImageFilter->Update();
    // Save image
    ITKUtils::WriteNIfTIImage<ImageType>(changeInformationImageFilter->GetOutput(), std::string(argv[3]));
}


int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: CopyHeaderInformation sourceImage referenceImage outputImage [copySpacing=1] [copyOrigin=1] [copyDirection=1]" << std::endl;
        return EXIT_FAILURE;
    }

    typename itk::ImageIOBase::Pointer sourceIO = ITKUtils::ReadImageInformation(std::string(argv[1]));
    typename itk::ImageIOBase::Pointer referenceIO = ITKUtils::ReadImageInformation(std::string(argv[2]));

    const unsigned int SourceImageDimension = sourceIO->GetNumberOfDimensions();
    const unsigned int ReferenceImageDimension = referenceIO->GetNumberOfDimensions();

    if (SourceImageDimension != ReferenceImageDimension)
    {
        std::cerr << "Incompatible image dimensions" << std::endl;
        return EXIT_FAILURE;
    }

    if (SourceImageDimension < 2 || SourceImageDimension > 4)
    {
        std::cerr << "Unsupported image dimensions" << std::endl;
        return EXIT_FAILURE;   
    }
    
    try
    {
        if (SourceImageDimension == 2)
            CopyHeaderInformation<itk::Image<float, 2>>(argc, argv);
        else if (SourceImageDimension == 3)
            CopyHeaderInformation<itk::Image<float, 3>>(argc, argv);
        else
            CopyHeaderInformation<itk::Image<float, 4>>(argc, argv);
    }
    catch (itk::ExceptionObject & err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
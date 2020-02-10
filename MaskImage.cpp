/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2020 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Mask Image                                                               *
***************************************************************************/

#include <ITKUtils.hpp>
#include <itkImage.h>
#include <itkMaskImageFilter.h>
#include <itkChangeInformationImageFilter.h>
#include <itkSliceBySliceImageFilter.h>


template<typename ImageType, typename MaskType>
void MaskImageEqualDimensions(char *argv[])
{
    // Read image
    typename ImageType::Pointer image = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[1]));
    // Read mask
    typename MaskType::Pointer mask = ITKUtils::ReadNIfTIImage<MaskType>(std::string(argv[2]));
    // Define the mask image filter type
    using MaskImageFilterType = itk::MaskImageFilter<ImageType, MaskType>;
    // Create mask image filter
    typename MaskImageFilterType::Pointer maskFilter = MaskImageFilterType::New();
    maskFilter->SetInput(image);
    maskFilter->SetMaskImage(mask);
    maskFilter->Update();
    // Save image
    ITKUtils::WriteNIfTIImage<ImageType>(maskFilter->GetOutput(), std::string(argv[3]));
}


template<typename ImageType, typename MaskType>
void MaskImageDifferentDimensions(char *argv[])
{
    // Read image
    typename ImageType::Pointer image = ITKUtils::ReadNIfTIImage<ImageType>(std::string(argv[1]));
    // Read mask
    typename MaskType::Pointer mask = ITKUtils::ReadNIfTIImage<MaskType>(std::string(argv[2]));
    // Declare slice by slice type
    using SliceBySliceImageFilterType = itk::SliceBySliceImageFilter<ImageType, ImageType>;
    // Define the change image info filter type with the internal type of slice to slice image filter
    using ChangeInformationImageFilterType = itk::ChangeInformationImageFilter<typename SliceBySliceImageFilterType::InternalInputImageType>;
    typename ChangeInformationImageFilterType::Pointer changeInformationImageFilter = ChangeInformationImageFilterType::New();
    changeInformationImageFilter->SetOutputSpacing(mask->GetSpacing());
    changeInformationImageFilter->ChangeSpacingOn();
    changeInformationImageFilter->SetOutputOrigin(mask->GetOrigin());
    changeInformationImageFilter->ChangeOriginOn();
    changeInformationImageFilter->SetOutputDirection(mask->GetDirection());
    changeInformationImageFilter->ChangeDirectionOn();
    // Define the mask image filter type with the internal type of slice to slice image filter
    using MaskImageFilterType = itk::MaskImageFilter<typename SliceBySliceImageFilterType::InternalInputImageType, MaskType>;
    typename MaskImageFilterType::Pointer maskFilter = MaskImageFilterType::New();
    maskFilter->SetMaskImage(mask);
    maskFilter->SetInput(changeInformationImageFilter->GetOutput());
    // Create slice to slice image filter
    typename SliceBySliceImageFilterType::Pointer sliceBySliceImageFilter = SliceBySliceImageFilterType::New();
    sliceBySliceImageFilter->SetInput(image);
    sliceBySliceImageFilter->SetInputFilter(changeInformationImageFilter);
    sliceBySliceImageFilter->SetOutputFilter(maskFilter);
    sliceBySliceImageFilter->Update();
    // Save image
    ITKUtils::WriteNIfTIImage<ImageType>(sliceBySliceImageFilter->GetOutput(), std::string(argv[3]));
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: MaskImage inputImage maskImage outputImage" << std::endl;
        return EXIT_FAILURE;
    }

    typename itk::ImageIOBase::Pointer imageIO = ITKUtils::ReadImageInformation(std::string(argv[1]));
    typename itk::ImageIOBase::Pointer maskIO = ITKUtils::ReadImageInformation(std::string(argv[2]));

    const unsigned int ImageDimension = imageIO->GetNumberOfDimensions();
    const unsigned int MaskDimension = maskIO->GetNumberOfDimensions();

    if (ImageDimension != MaskDimension && ImageDimension != (MaskDimension + 1))
    {
        std::cerr << "Incompatible image dimensions" << std::endl;
        return EXIT_FAILURE;
    }

    if (ImageDimension != 3 && ImageDimension != 4)
    {
        std::cerr << "Unsupported image dimensions" << std::endl;
        return EXIT_FAILURE;   
    }
    
    try
    {
        if (ImageDimension == 4)
        {
            if (MaskDimension == 4)
                MaskImageEqualDimensions<itk::Image<float, 4>, itk::Image<unsigned char, 4>>(argv);
            else
                MaskImageDifferentDimensions<itk::Image<float, 4>, itk::Image<unsigned char, 3>>(argv);
        }
        else
        {
            if (MaskDimension == 3)
                MaskImageEqualDimensions<itk::Image<float, 3>, itk::Image<unsigned char, 3>>(argv);
            else
                MaskImageDifferentDimensions<itk::Image<float, 3>, itk::Image<unsigned char, 2>>(argv);
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
/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2020 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Cast Image type                                                          *
***************************************************************************/

#include <ITKUtils.hpp>
#include <itkImage.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkCastImageFilter.h>


template<typename InputImageType, typename OutputImageType>
void _CastImage(int argc, char *argv [])
{
    bool rescaleIntensity = true;
    if (argc == 5)
        rescaleIntensity = (bool) std::atoi(argv[4]);
    
    // Read image
    typename InputImageType::Pointer image = ITKUtils::ReadNIfTIImage<InputImageType>(std::string(argv[1]));

    // Cast filter
    using FilterType = itk::CastImageFilter<InputImageType, OutputImageType>;
    typename FilterType::Pointer filter = FilterType::New();

    // Rescale filter if required
    if (rescaleIntensity)
    {
        using RescaleType = itk::RescaleIntensityImageFilter<InputImageType, InputImageType>;
        typename RescaleType::Pointer rescale = RescaleType::New();
        rescale->SetInput(image);
        rescale->SetOutputMinimum(0);
        rescale->SetOutputMaximum(itk::NumericTraits<typename OutputImageType::PixelType>::max());
        rescale->Update();
        filter->SetInput(rescale->GetOutput());
    }
    else
    {
        filter->SetInput(image);
    }
    filter->Update();
    // Save image
    ITKUtils::WriteNIfTIImage<OutputImageType>(filter->GetOutput(), std::string(argv[2]));
}


template<typename InputImageType>
void CastImage(int argc, char *argv [])
{
    // Read pixel type
    const unsigned int type = (unsigned int) std::atoi(argv[3]);

    if (type == 0)
        _CastImage<InputImageType, itk::Image<float, InputImageType::ImageDimension>>(argc, argv);
    else if (type == 1)
        _CastImage<InputImageType, itk::Image<unsigned char, InputImageType::ImageDimension>>(argc, argv);
    else if (type == 2)
        _CastImage<InputImageType, itk::Image<unsigned short, InputImageType::ImageDimension>>(argc, argv);
    else if (type == 3)
        _CastImage<InputImageType, itk::Image<unsigned int, InputImageType::ImageDimension>>(argc, argv);
    else if (type == 4)
        _CastImage<InputImageType, itk::Image<unsigned long, InputImageType::ImageDimension>>(argc, argv);
    else if (type == 5)
        _CastImage<InputImageType, itk::Image<char, InputImageType::ImageDimension>>(argc, argv);
    else if (type == 6)
        _CastImage<InputImageType, itk::Image<short, InputImageType::ImageDimension>>(argc, argv);
    else if (type == 7)
        _CastImage<InputImageType, itk::Image<int, InputImageType::ImageDimension>>(argc, argv);
    else if (type == 8)
        _CastImage<InputImageType, itk::Image<long, InputImageType::ImageDimension>>(argc, argv);
    else if (type == 9)
        _CastImage<InputImageType, itk::Image<double, InputImageType::ImageDimension>>(argc, argv);
    else
    {
        std::stringstream s;
        s << "Unsupported data type" << std::endl;
        throw std::runtime_error(s.str());
    }
}


int main(int argc, char *argv [])
{
    if (argc < 4)
    {
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: CastImage inputImage outputImage pixelType [rescaleIntensity=1]" << std::endl;
        std::cerr << "pixelType:\t0 -> float" << std::endl;
        std::cerr << "\t\t1 -> unsigned char" << std::endl;
        std::cerr << "\t\t2 -> unsigned short" << std::endl;
        std::cerr << "\t\t3 -> unsigned int" << std::endl;
        std::cerr << "\t\t4 -> unsigned long" << std::endl;
        std::cerr << "\t\t5 -> char" << std::endl;
        std::cerr << "\t\t6 -> short" << std::endl;
        std::cerr << "\t\t7 -> int" << std::endl;
        std::cerr << "\t\t8 -> long" << std::endl;
        std::cerr << "\t\t9 -> double" << std::endl;
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
            CastImage<itk::Image<float, 2>>(argc, argv);
        else if (ImageDimension == 3)
            CastImage<itk::Image<float, 3>>(argc, argv);
        else
            CastImage<itk::Image<float, 4>>(argc, argv);
    }
    catch (itk::ExceptionObject & err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
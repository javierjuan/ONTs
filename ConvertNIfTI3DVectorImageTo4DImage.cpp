/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2020 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Convert 3D Vector image to 4D image                                      *
***************************************************************************/

#include <cstdio>
#include <itkImage.h>
#include <itkNiftiImageIOFactory.h>
#include <itkNiftiImageIO.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkComposeImageFilter.h>
#include <itkVectorImage.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkVectorIndexSelectionCastImageFilter.h>

typedef itk::VectorImage<float, 3> VectorImageType;
typedef itk::Image<float, 3> Image3DType;
typedef itk::Image<float, 4> Image4DType;
typedef itk::ImageFileReader<Image4DType> Image4DReader;
typedef itk::ImageFileReader<VectorImageType> VectorImageReader;
typedef itk::ImageFileWriter<Image4DType> ImageWriter;
typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, Image3DType> IndexSelectionType;

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: ConvertNIfTI3DVectorImageTo4D inputFilePath outputFilePath" << std::endl;
		return EXIT_FAILURE;
	}

	// NIfTI IO
	itk::NiftiImageIO::Pointer NIfTIIO = itk::NiftiImageIO::New();
	
	if (!NIfTIIO->CanReadFile(argv[1]))
	{
		std::cerr << "Unable to read file: " << std::string(argv[1]) << std::endl;
		return EXIT_FAILURE;
	}

	// Read image header information
	try
	{
		NIfTIIO->SetFileName(argv[1]);
		NIfTIIO->ReadImageInformation();
	}
	catch (itk::ExceptionObject &err)
	{
		std::cerr << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;
		return EXIT_FAILURE;
	}

	// Get number of components and dimensions
	const int numberOfCompnents  = NIfTIIO->GetNumberOfComponents();
	const int numberOfDimensions = NIfTIIO->GetNumberOfDimensions();

	// NIfTI IO Factory
	itk::NiftiImageIOFactory::RegisterOneFactory();

	// Image 4D case
	if (numberOfCompnents == 1 && numberOfDimensions == 4)
	{
		// Just save the image with the corresponding name (to ensure nii.gz compression we should explicitly save the image)
		Image4DType::Pointer inputImage;		
		try
		{
			Image4DReader::Pointer reader = Image4DReader::New();
			reader->SetFileName(argv[1]);
			reader->Update();
			ImageWriter::Pointer writer = ImageWriter::New();
			writer->SetFileName(argv[2]);
			writer->SetInput(reader->GetOutput());
			writer->Update();
		}
		catch (itk::ExceptionObject & err)
		{
			std::cerr << "ExceptionObject caught !" << std::endl;
			std::cerr << err << std::endl;
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}
	// VectorImage 3D multi-component case
	else if (numberOfCompnents > 1 && numberOfDimensions == 3)
	{
		// Declare Vector image pointer
		VectorImageType::Pointer inputImage;

		// Read vector image
		VectorImageReader::Pointer reader = VectorImageReader::New();
		try
		{
			reader->SetFileName(argv[1]);
			reader->Update();
			inputImage = reader->GetOutput();
		}
		catch (itk::ExceptionObject &err)
		{
			std::cerr << "ExceptionObject caught !" << std::endl;
			std::cerr << err << std::endl;
			return EXIT_FAILURE;
		}

		// Copy metadata to 4D image
		VectorImageType::SpacingType inputSpacing = inputImage->GetSpacing();
		Image4DType::SpacingType outputSpacing;
		outputSpacing[0] = inputSpacing[0];
		outputSpacing[1] = inputSpacing[1];
		outputSpacing[2] = inputSpacing[2];
		outputSpacing[3] = 1;
		VectorImageType::PointType inputOrigin = inputImage->GetOrigin();
		Image4DType::PointType outputOrigin;
		outputOrigin[0] = inputOrigin[0];
		outputOrigin[1] = inputOrigin[1];
		outputOrigin[2] = inputOrigin[2];
		outputOrigin[3] = 0;
		VectorImageType::DirectionType inputDirection = inputImage->GetDirection();
		Image4DType::DirectionType outputDirection;
		outputDirection.SetIdentity();
		for (int i = 0; i < inputDirection.RowDimensions; ++i)
		for (int j = 0; j < inputDirection.ColumnDimensions; ++j)
			outputDirection(i, j) = inputDirection(i, j);

		// Create index selection type
		IndexSelectionType::Pointer indexSelectionFilter = IndexSelectionType::New();
		indexSelectionFilter->SetInput(inputImage);

		// Create 4D image
		VectorImageType::SizeType inputSize = inputImage->GetLargestPossibleRegion().GetSize();
		Image4DType::RegionType::SizeType outputSize;
		outputSize[0] = inputSize[0];
		outputSize[1] = inputSize[1];
		outputSize[2] = inputSize[2];
		outputSize[3] = inputImage->GetNumberOfComponentsPerPixel();
		Image4DType::RegionType outputRegion(outputSize);

		Image4DType::Pointer outputImage = Image4DType::New();
		outputImage->SetSpacing(outputSpacing);
		outputImage->SetOrigin(outputOrigin);
		outputImage->SetDirection(outputDirection);
		outputImage->SetRegions(outputRegion);
		outputImage->Allocate();

		// Compose 4D image
		for (unsigned int i = 0; i < inputImage->GetNumberOfComponentsPerPixel(); ++i)
		{
			indexSelectionFilter->SetIndex(i);
			indexSelectionFilter->Update();
			Image3DType::Pointer componentImage = indexSelectionFilter->GetOutput();

			itk::ImageRegionConstIteratorWithIndex<Image3DType> iterator(componentImage, componentImage->GetLargestPossibleRegion());
			iterator.GoToBegin();
			while (!iterator.IsAtEnd())
			{
				itk::Index<3> SpatialIndex = iterator.GetIndex();
				itk::Index<4> SpatioTemporalIndex;
				SpatioTemporalIndex[0] = SpatialIndex[0];
				SpatioTemporalIndex[1] = SpatialIndex[1];
				SpatioTemporalIndex[2] = SpatialIndex[2];
				SpatioTemporalIndex[3] = i;
				outputImage->SetPixel(SpatioTemporalIndex, iterator.Get());
				++iterator;
			}
		}

		try
		{
			ImageWriter::Pointer writer = ImageWriter::New();
			writer->SetFileName(argv[2]);
			writer->SetInput(outputImage);
			writer->Update();
		}
		catch (itk::ExceptionObject & err)
		{
			std::cerr << "ExceptionObject caught !" << std::endl;
			std::cerr << err << std::endl;
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}
}
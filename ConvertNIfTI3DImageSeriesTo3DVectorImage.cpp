/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2020 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Convert 3D NIfTI series to 3D vector image                               *
***************************************************************************/

#include <itkImage.h>
#include <itkDirectory.h>
#include <itkNiftiImageIOFactory.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkComposeImageFilter.h>
#include <itkVectorImage.h>

const char PathSeparator =
#ifdef _WIN32
'\\';
#else
'/';
#endif

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: ConvertNIfTI3DSeriesTo4D inputDirectory outputFileName [TR=1]" << std::endl;
		return EXIT_FAILURE;
	}

	typedef itk::VectorImage<float, 3> VectorImageType;
	typedef itk::Image<float, 3> ScalarImageType;
	typedef itk::ImageFileReader<ScalarImageType> ImageReader;
	typedef itk::ImageFileWriter<VectorImageType> ImageWriter;
	typedef itk::ComposeImageFilter<ScalarImageType> ImageToVectorImageFilterType;

	// Set directory
	std::string baseDirectory(argv[1]);

	// Load directory files
	itk::Directory::Pointer directoryReader = itk::Directory::New();
	if (!directoryReader->Load(baseDirectory.c_str()))
	{
		std::cerr << "Error! Cannot read directory: " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	// NIfTI IO Factory
	itk::NiftiImageIOFactory::RegisterOneFactory();

	// Load pointers for each 3D file and assign to imageToVectorImageFilter
	ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();
	int index = 0;
	for (int i = 0; i < directoryReader->GetNumberOfFiles(); i++)
	{
		// Read image
		try
		{
			std::string fileName(directoryReader->GetFile(i));
			if (fileName == "." || fileName == "..")
			{
				continue;
			}

			ImageReader::Pointer reader = ImageReader::New();
			reader->SetFileName(baseDirectory + PathSeparator + directoryReader->GetFile(i));
			reader->Update();
			imageToVectorImageFilter->SetInput(index, reader->GetOutput());
			index++;
		}
		catch (itk::ExceptionObject & err)
		{
			std::cerr << "ExceptionObject caught !" << std::endl;
			std::cerr << err << std::endl;
			return EXIT_FAILURE;
		}
	}
	imageToVectorImageFilter->Update();

	// Save image
	ImageWriter::Pointer writer = ImageWriter::New();
	try
	{
		writer->SetFileName(argv[2]);
		writer->SetInput(imageToVectorImageFilter->GetOutput());
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
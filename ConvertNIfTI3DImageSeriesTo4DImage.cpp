/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2020 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Convert 3D NIfTI series to 4D image                                      *
***************************************************************************/

#include <itkImage.h>
#include <itkNiftiImageIOFactory.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageSeriesReader.h>
#include <itkDirectory.h>

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
		std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: ConvertNIfTI3DSeriesTo4D inputDirectory outputFileName" << std::endl;
		return EXIT_FAILURE;
	}

	typedef itk::Image<float, 4> ImageType;
	typedef itk::ImageSeriesReader<ImageType> ImageReader;
	typedef itk::ImageFileWriter<ImageType> ImageWriter;

	// NIfTI IO Factory
	itk::NiftiImageIOFactory::RegisterOneFactory();

	// Set directory
	std::string baseDirectory(argv[1]);

	// Load directory files
	itk::Directory::Pointer directoryReader = itk::Directory::New();
	if (!directoryReader->Load(baseDirectory.c_str()))
	{
		std::cerr << "Error! Cannot read directory: " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	// Get file paths
	std::vector<std::string> imagesFilePaths;
	for (int i = 0; i < directoryReader->GetNumberOfFiles(); ++i)
	{
		std::string fileName(directoryReader->GetFile(i));
		if (fileName == "." || fileName == "..")
		{
			continue;
		}
		imagesFilePaths.push_back(baseDirectory + PathSeparator + fileName);
	}
    // Sort image file paths in ascending order
    std::sort(imagesFilePaths.begin(), imagesFilePaths.end());

	// Set image reader
	ImageReader::Pointer reader = ImageReader::New();
	reader->SetFileNames(imagesFilePaths);

	// Save image
	ImageWriter::Pointer writer = ImageWriter::New();
	try
	{
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
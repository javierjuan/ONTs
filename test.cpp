/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2014 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Global Principal Compoment Analysis filtering                            *
***************************************************************************/

#include <itkImage.h>
#include <itkNiftiImageIO.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <ITKUtils.hpp>
#include <cstdlib>

using namespace Eigen;

int main(int argc, char *argv [])
{
	if (argc < 5)
	{
		std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: GlobalPCADenoising <InputNIfTIFile> <MaskNIfTIFile> <variance> <OutputNIFTIFile> [minComponents=4] [maxComponents=8]" << std::endl;
		return EXIT_FAILURE;
	}
	// NIfTI IO
	itk::NiftiImageIO::Pointer NIfTIIO = itk::NiftiImageIO::New();

	// Typedefs
	using ComponentsImageType = itk::Image<float, 4>;
	using MaskType = itk::Image<unsigned char, 3>;

	using ComponentsImageReader = itk::ImageFileReader<ComponentsImageType>;
	using ComponentsImageWriter = itk::ImageFileWriter<ComponentsImageType>;
	using MaskReader = itk::ImageFileReader<MaskType>;
	using MaskWriter = itk::ImageFileWriter<MaskType>;

	// Get multidimensional PWI
	ComponentsImageType::Pointer PWI;
	ComponentsImageReader::Pointer PWIReader = ComponentsImageReader::New();
	PWIReader->SetImageIO(NIfTIIO);
	try
	{
		PWIReader->SetFileName(argv[1]);
		PWIReader->Update();
		PWI = PWIReader->GetOutput();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;
		return EXIT_FAILURE;
	}

	// Get mask
	MaskType::Pointer mask;
	MaskReader::Pointer maskReader = MaskReader::New();
	maskReader->SetImageIO(NIfTIIO);
	try
	{
		maskReader->SetFileName(argv[2]);
		maskReader->Update();
		mask = maskReader->GetOutput();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;
		return EXIT_FAILURE;
	}

	// Get variance
	const double variance = std::strtod(argv[3], NULL);

	// Default min and max number of components
	unsigned int minComponents = 4;
	unsigned int maxComponents = 8;

	// Get user min number of components
	if (argc >= 6)
		minComponents = std::atoi(argv[5]);

	// Get user max number of components
	if (argc >= 7)
		maxComponents = std::atoi(argv[6]);

	//MaskType::Pointer nonZerosMask = ITKUtils::NonZerosMaskIntersect<float, 4, 3>(PWI, mask);

	// Save new filtered image
	MaskWriter::Pointer maskWriter = MaskWriter::New();
	maskWriter->SetImageIO(NIfTIIO);
	try
	{
		maskWriter->SetFileName(argv[4]);
		maskWriter->SetInput(mask);
		maskWriter->Update();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

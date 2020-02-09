/***************************************************************************
/* Javier Juan Albarracin - jajuaal1@ibime.upv.es                         */
/* Universidad Politecnica de Valencia, Spain                             */
/*                                                                        */
/* Copyright (C) 2020 Javier Juan Albarracin                              */
/*                                                                        */
/***************************************************************************
* Global Principal Compoment Analysis filtering                            *
***************************************************************************/

#include <itkImage.h>
#include <Eigen/Dense>
#include <EigenITK.hpp>
#include <ITKUtils.hpp>
#include <PrincipalComponentAnalysis.hpp>
#include <cstdlib>
#include <cmath>

using namespace Eigen;

int main(int argc, char *argv [])
{
    if (argc < 5)
    {
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: GlobalPCADenoising <InputNIfTIFile> <MaskNIfTIFile> <variance> <OutputNIFTIFile> [minComponents=5% of number of components] [maxComponents=25% of number of components]" << std::endl;
        return EXIT_FAILURE;
    }

    // Typedefs
    typedef itk::Image<float, 4> ComponentsImageType;
    typedef itk::Image<unsigned char, 3> MaskType;
    try
    {
        // Get multidimensional PWI
        typename ComponentsImageType::Pointer PWI = ITKUtils::ReadNIfTIImage<ComponentsImageType>(std::string(argv[1]));
        // Get mask
        typename MaskType::Pointer mask = ITKUtils::ReadNIfTIImage<MaskType>(std::string(argv[2]));
        // Get variance
        const double variance = std::strtod(argv[3], NULL);
        // Default min and max number of components
        typename ComponentsImageType::SizeType imageSize = PWI->GetLargestPossibleRegion().GetSize();
        unsigned int minComponents = std::ceil(imageSize[3] * 0.05);
        unsigned int maxComponents = std::ceil(imageSize[3] * 0.25);
        // Get user min number of components
        if (argc >= 6)
            minComponents = std::atoi(argv[5]);
        // Get user max number of components
        if (argc >= 7)
            maxComponents = std::atoi(argv[6]);
        // Compute Non-Zeros mask
        typename MaskType::Pointer nonZerosMask = ITKUtils::ZerosMaskIntersect<ComponentsImageType, MaskType>(PWI, mask, true);
        // Convert to Eigen Matrix
        MatrixXf dataset(EigenITK::toEigen<ComponentsImageType, MaskType>(PWI, nonZerosMask));
        // Compute PCA filtering
        PrincipalComponentAnalysis pca;
        MatrixXf PWIPCARawdata = pca.filteringVarianceExplained(dataset, variance, minComponents, maxComponents);
        // Correct curves with negative values
        #pragma omp parallel for
        for (int i = 0; i < PWIPCARawdata.rows(); i++)
        {
            ArrayXf row = PWIPCARawdata.row(i);
            if ((row < 1).any())
                PWIPCARawdata.row(i) = (row - row.minCoeff() + 1);
        }
        // Convert to ITK image
        EigenITK::toITK<ComponentsImageType, MaskType>(PWIPCARawdata, nonZerosMask, PWI);
        // Save new filtered image
        ITKUtils::WriteNIfTIImage<ComponentsImageType>(PWI, std::string(argv[4]));
    }
    catch (itk::ExceptionObject & err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
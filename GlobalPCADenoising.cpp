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
        std::cerr << "Error! Invalid number of arguments!" << std::endl << "Usage: GlobalPCADenoising inputImage maskImage variance outputImage [minComponents=5% of number of components] [maxComponents=25% of number of components] [verbose=0]" << std::endl;
        return EXIT_FAILURE;
    }

    // Typedefs
    typedef itk::Image<float, 4> ComponentsImageType;
    typedef itk::Image<unsigned char, 3> MaskType;
    try
    {
        // Get PWI
        typename ComponentsImageType::Pointer PWI = ITKUtils::ReadNIfTIImage<ComponentsImageType>(std::string(argv[1]));
        // Get mask
        typename MaskType::Pointer mask = ITKUtils::ReadNIfTIImage<MaskType>(std::string(argv[2]));
        // Get variance
        const double variance = std::strtod(argv[3], NULL);
        // Default min and max number of components
        typename ComponentsImageType::SizeType imageSize = PWI->GetLargestPossibleRegion().GetSize();
        unsigned int minComponents = std::ceil(imageSize[3] * 0.0500);
        unsigned int maxComponents = std::ceil(imageSize[3] * 0.3333);
        // Get user min number of components
        if (argc > 5)
            minComponents = std::atoi(argv[5]);
        // Get user max number of components
        if (argc > 6)
            maxComponents = std::atoi(argv[6]);
        // Get user max number of components
        bool verbose = false;
        if (argc > 7)
            verbose = (bool) std::atoi(argv[7]);
        // Print configuration
        if (verbose)
        {
            std::cout << "CONFIGURATION" << std::endl;
            std::cout << "-------------" << std::endl;
            std::cout << "Variance explained" << std::endl;
            std::cout << "\tValue: " << variance << std::endl;
            std::cout << "Number of components" << std::endl;
            std::cout << "\tMinium: " << minComponents << std::endl;
            std::cout << "\tMaximum: " << maxComponents << std::endl;    
        }
        // Compute Non-Zeros mask
        typename MaskType::Pointer nonZerosMask = ITKUtils::ZerosMaskIntersect<ComponentsImageType, MaskType>(PWI, mask, true, false, 0.05);
        // Convert to Eigen Matrix
        MatrixXf dataset(EigenITK::toEigen<ComponentsImageType, MaskType>(PWI, nonZerosMask));
        // Compute PCA filtering
        PrincipalComponentAnalysis pca;
        MatrixXf PWIPCARawdata = pca.filteringVarianceExplained(dataset, variance, minComponents, maxComponents);
        if (verbose)
        {
            std::cout << "PCA" << std::endl;
            std::cout << "---" << std::endl;
            std::cout << "Reconstruction with " << pca.components() << " components out of " << imageSize[3] << std::endl;
        }
        // Correct curves with negative values
        #pragma omp parallel for
        for (int i = 0; i < PWIPCARawdata.rows(); i++)
        {
            ArrayXf row = PWIPCARawdata.row(i);
            if ((row <= 0).any())
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
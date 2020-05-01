cmake_minimum_required(VERSION 3.10)

# set the project name
project(onts VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# specify build type
set(CMAKE_BUILD_TYPE Release)

# Eigen support
find_package(ITK 5 REQUIRED)
include(${ITK_USE_FILE})

# Eigen support
find_package(Eigen3 3.3 REQUIRED)

# OpenMP support
find_package(OpenMP REQUIRED)

# set private library path
set(LIBRARY_DIR /home/javier/Library)

# add the library
add_executable(AdaptiveHistogramEqualization AdaptiveHistogramEqualization.cpp)
add_executable(MaskImage MaskImage.cpp)
add_executable(CastImage CastImage.cpp)
add_executable(ConvertNIfTI3DSeriesTo3DVectorImage ConvertNIfTI3DSeriesTo3DVectorImage.cpp)
add_executable(ConvertNIfTI3DSeriesTo4DImage ConvertNIfTI3DSeriesTo4DImage.cpp)
add_executable(ConvertNIfTI3DVectorImageTo4D ConvertNIfTI3DVectorImageTo4D.cpp)
add_executable(GlobalPCADenoising GlobalPCADenoising.cpp)
add_executable(HistogramStandardization HistogramStandarization.cpp)
add_executable(TruncateNegatives TruncateNegatives.cpp)

# set -fPIC
set_property(TARGET AdaptiveHistogramEqualization
	                MaskImage
	                CastImage
	                ConvertNIfTI3DSeriesTo3DVectorImage
	                ConvertNIfTI3DSeriesTo4DImage
	                ConvertNIfTI3DVectorImageTo4D
	                GlobalPCADenoising
	                HistogramStandardization
	                TruncateNegatives PROPERTY POSITION_INDEPENDENT_CODE ON)

# compile options
#target_compile_options(svfmm PRIVATE -Wall -Wextra -Wno-comment -Wno-unused-variable -Wno-unused-parameter)
add_compile_options(-Wall -Wextra -Wno-comment -Wno-unused-variable -Wno-unused-parameter)

target_include_directories(AdaptiveHistogramEqualization PRIVATE ${ITK_INCLUDE_DIRS} ${LIBRARY_DIR}/tools)
target_include_directories(MaskImage PRIVATE ${ITK_INCLUDE_DIRS} ${LIBRARY_DIR}/tools)
target_include_directories(CastImage PRIVATE ${ITK_INCLUDE_DIRS} ${LIBRARY_DIR}/tools)
target_include_directories(ConvertNIfTI3DSeriesTo3DVectorImage PRIVATE ${ITK_INCLUDE_DIRS})
target_include_directories(ConvertNIfTI3DSeriesTo4DImage PRIVATE ${ITK_INCLUDE_DIRS})
target_include_directories(ConvertNIfTI3DVectorImageTo4D PRIVATE ${ITK_INCLUDE_DIRS})
target_include_directories(GlobalPCADenoising PRIVATE ${ITK_INCLUDE_DIRS} ${LIBRARY_DIR}/tools ${LIBRARY_DIR}/decomposition)
target_include_directories(HistogramStandardization PRIVATE ${ITK_INCLUDE_DIRS} ${LIBRARY_DIR}/tools)
target_include_directories(TruncateNegatives PRIVATE ${ITK_INCLUDE_DIRS} ${LIBRARY_DIR}/tools)

target_link_libraries(AdaptiveHistogramEqualization PRIVATE ${ITK_LIBRARIES} OpenMP::OpenMP_CXX)
target_link_libraries(MaskImage PRIVATE ${ITK_LIBRARIES} OpenMP::OpenMP_CXX)
target_link_libraries(CastImage PRIVATE ${ITK_LIBRARIES} OpenMP::OpenMP_CXX)
target_link_libraries(ConvertNIfTI3DSeriesTo3DVectorImage PRIVATE ${ITK_LIBRARIES} OpenMP::OpenMP_CXX)
target_link_libraries(ConvertNIfTI3DSeriesTo4DImage PRIVATE ${ITK_LIBRARIES} OpenMP::OpenMP_CXX)
target_link_libraries(ConvertNIfTI3DVectorImageTo4D PRIVATE ${ITK_LIBRARIES} OpenMP::OpenMP_CXX)
target_link_libraries(GlobalPCADenoising PRIVATE ${ITK_LIBRARIES} Eigen3::Eigen OpenMP::OpenMP_CXX)
target_link_libraries(HistogramStandardization PRIVATE ${ITK_LIBRARIES} OpenMP::OpenMP_CXX)
target_link_libraries(TruncateNegatives PRIVATE ${ITK_LIBRARIES} OpenMP::OpenMP_CXX)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS AdaptiveHistogramEqualization
	            MaskImage
	            CastImage
	            ConvertNIfTI3DSeriesTo3DVectorImage
	            ConvertNIfTI3DSeriesTo4DImage
	            ConvertNIfTI3DVectorImageTo4D
	            GlobalPCADenoising
	            HistogramStandardization
	            TruncateNegatives
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin)
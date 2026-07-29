#include "itkIdentityTransform.h"
#include "itkImage.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"

using ImageType = itk::Image<double, 3>;

ImageType::Pointer ResampleImage(ImageType::Pointer inputImage)
{
    constexpr unsigned int Dimension = 3;

    using ResampleFilterType =
        itk::ResampleImageFilter<ImageType, ImageType>;

    using InterpolatorType =
        itk::LinearInterpolateImageFunction<ImageType, double>;

    using TransformType =
        itk::IdentityTransform<double, Dimension>;

    auto resampler = ResampleFilterType::New();
    auto interpolator = InterpolatorType::New();
    auto transform = TransformType::New();

    const auto inputRegion = inputImage->GetLargestPossibleRegion();
    const auto inputSize = inputRegion.GetSize();
    const auto inputSpacing = inputImage->GetSpacing();

    ImageType::SizeType outputSize = inputSize;
    outputSize[0] = 512;
    outputSize[1] = 512;
    outputSize[2] = inputSize[2];

    ImageType::SpacingType outputSpacing;

    for (unsigned int i = 0; i < Dimension; ++i)
    {
        outputSpacing[i] =
            inputSpacing[i] *
            static_cast<double>(inputSize[i]) /
            static_cast<double>(outputSize[i]);
    }

    std::cout << "metadata:" << std::endl;
    std::cout << outputSize << std::endl;
    std::cout << "\n";
    std::cout << outputSpacing << std::endl;

    resampler->SetInput(inputImage);
    std::cout << "1" << std::endl;
    resampler->SetTransform(transform);
    std::cout << "2" << std::endl;
    resampler->SetInterpolator(interpolator);
    std::cout << "3" << std::endl;

    resampler->SetSize(outputSize);
    std::cout << "4" << std::endl;
    resampler->SetOutputSpacing(outputSpacing);
    std::cout << "5" << std::endl;
    resampler->SetOutputOrigin(inputImage->GetOrigin());
    std::cout << "6" << std::endl;

    std::cout << "1";
    resampler->Update();
    std::cout << "2";

    auto outputImage = resampler->GetOutput();
    outputImage->DisconnectPipeline();

    return outputImage;
}

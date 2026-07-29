#include "itkIdentityTransform.h"
#include "itkImage.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"

using ImageType = itk::Image<float, 3>;

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
            static_cast<float>(inputSize[i]) /
            static_cast<float>(outputSize[i]);
    }

    resampler->SetInput(inputImage);
    resampler->SetTransform(transform);
    resampler->SetInterpolator(interpolator);
    resampler->SetSize(outputSize);
    resampler->SetOutputSpacing(outputSpacing);
    resampler->SetOutputOrigin(inputImage->GetOrigin());

    std::cout << "about to update";
    resampler->Update();
    std::cout << "updated";

    ImageType::Pointer outputImage = resampler->GetOutput();
    outputImage->DisconnectPipeline();

    return outputImage;
}

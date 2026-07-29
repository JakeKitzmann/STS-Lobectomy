#include <iostream>

#include "ResampleImage.h"

// ITK
#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"

int main( const int argc, const char * argv []){
    if(argc !=2){
        std::cout << "Usage: \n";
        std::cout << argv[0] << " <input dicom directory>\n";
    }

    
    using PixelType = double;
    constexpr unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;

    const std::string dicomDir = argv[1];

    using ImageIOType = itk::GDCMImageIO;
    using NamesGeneratorType = itk::GDCMSeriesFileNames;

    ImageIOType::Pointer gdcmIO = ImageIOType::New();
    NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();

    namesGenerator->SetUseSeriesDetails(true);
    namesGenerator->SetDirectory(dicomDir);

    try
    {
        // Get the series UIDs found in the directory
        using SeriesIdContainer = std::vector<std::string>;
        const SeriesIdContainer& seriesUIDs = namesGenerator->GetSeriesUIDs();

        if (seriesUIDs.empty())
        {
            std::cerr << "No DICOM series found in directory: " << dicomDir << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "Found " << seriesUIDs.size() << " series." << std::endl;

        // If multiple series exist, pick one (here: the first)
        const std::string seriesIdentifier = seriesUIDs.front();

        using FileNamesContainer = std::vector<std::string>;
        FileNamesContainer fileNames = namesGenerator->GetFileNames(seriesIdentifier);

        using ReaderType = itk::ImageSeriesReader<ImageType>;
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetImageIO(gdcmIO);
        reader->SetFileNames(fileNames);

        reader->Update();

        ImageType::Pointer image = reader->GetOutput();

        auto resampled = ResampleImage(image);
    }
    catch (const itk::ExceptionObject& ex)
    {
        std::cerr << "ITK Exception: " << ex << std::endl;
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

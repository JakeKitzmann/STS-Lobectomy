#include <iostream>

#include "ResampleImage.h"

// ITK
#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"

#include <itksys/SystemTools.hxx>

int main( const int argc, const char * argv []){
    if(argc !=3){
        std::cout << "Usage: \n";
        std::cout << argv[0] << " <input dicom directory> <output dicom directory>\n";
        return EXIT_FAILURE;
    }


    using PixelType = float;
    constexpr unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;
    using Image2DType = itk::Image<PixelType, Dimension - 1>;

    const std::string dicomDir = argv[1];
    const std::string outputDicomDir = argv[2];

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

        itksys::SystemTools::MakeDirectory(outputDicomDir);
        namesGenerator->SetOutputDirectory(outputDicomDir);

        using SeriesWriterType = itk::ImageSeriesWriter<ImageType, Image2DType>;
        SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
        seriesWriter->SetInput(resampled);
        seriesWriter->SetImageIO(gdcmIO);
        seriesWriter->SetFileNames(namesGenerator->GetOutputFileNames());
        seriesWriter->SetMetaDataDictionaryArray(reader->GetMetaDataDictionaryArray());

        seriesWriter->Update();

        std::cout << "Wrote resampled DICOM series to: " << outputDicomDir << std::endl;
    }
    catch (const itk::ExceptionObject& ex)
    {
        std::cerr << "ITK Exception: " << ex << std::endl;
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

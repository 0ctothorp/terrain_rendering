#include <vector>
#include <string>
#include <iostream>

#include <gdal/gdal_priv.h>
#include <gdal/cpl_conv.h>


class GeotiffReader {
private:
    short* data;
public:
    GeotiffReader(std::string filename) {
        GDALDataset *poDataset;
        GDALAllRegister();
        poDataset = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);
        if(poDataset == NULL)
            std::cerr << "Couldn't read the file\n";   

        double adfGeoTransform[6];
        std::cout << "Driver: " << poDataset->GetDriver()->GetDescription() << "/" 
                  << poDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME) << "\n";
        std::cout << "Size is " << poDataset->GetRasterXSize() << "x" 
                  << poDataset->GetRasterYSize() << "x" << poDataset->GetRasterCount() << "\n";
        if(poDataset->GetProjectionRef() != NULL)
            std::cout << "Projection is '" << poDataset->GetProjectionRef() << "'\n";
        if(poDataset->GetGeoTransform(adfGeoTransform) == CE_None) {
            std::cout << "Origin = (" << adfGeoTransform[0] << ", " << adfGeoTransform[3] << "\n";
            std::cout << "Pixel Size = (" << adfGeoTransform[1] << ", " << adfGeoTransform[5] 
                      << "\n";
        }

        GDALRasterBand* poBand;
        int             nBlockXSize, nBlockYSize;
        int             bGotMin, bGotMax;
        double          adfMinMax[2];
        poBand = poDataset->GetRasterBand(1);
        poBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
        printf("Block=%dx%d Type=%s, ColorInterp=%s\n",
               nBlockXSize, nBlockYSize,
               GDALGetDataTypeName(poBand->GetRasterDataType()),
               GDALGetColorInterpretationName(poBand->GetColorInterpretation()));
        adfMinMax[0] = poBand->GetMinimum(&bGotMin);
        adfMinMax[1] = poBand->GetMaximum(&bGotMax);
        if(!(bGotMin && bGotMax))
            GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
        printf("Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1]);

        short *pafScanline;
        int nXSize = poBand->GetXSize();
        int nYSize = poBand->GetYSize();
        std::cout << "nXSize " << nXSize << ", nYSize " << nYSize << std::endl;
        pafScanline = (short*)CPLMalloc(sizeof(short) * nXSize * nYSize);
        poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, GDT_Int16, 0, 0);

        data = pafScanline; 

        // std::cout << std::endl;
        // for(int i = 0; i < nXSize * nYSize; i++) {
        //     std::cout << "[" << i << "]" << pafScanline[i] << "\n";
        // }

        GDALClose(poDataset);
    }

    ~GeotiffReader() {
        CPLFree(data);
    }

    short* GetData() { return data; }
};
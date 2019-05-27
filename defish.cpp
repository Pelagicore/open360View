
#include <stdlib.h>
#define CL_TARGET_OPENCL_VERSION 120

#include <QDebug>
#include "defish.h"
using namespace cv;

Defish::Defish(uint32_t src_width, uint32_t src_height, uint32_t dest_width, uint32_t dest_height, uint32_t num_color_channels)
{
    qDebug("Construct Defish");
    UMat K, D, targetK, scaledK;
    this->origSize = Size(src_width, src_height);
    this->dewarpSize = Size(dest_width, dest_height);
    Size targetSize = this->dewarpSize;
    float scalingFactor = ((float) targetSize.width) / ((float) this->origSize.width);
    Mat(Matx33f(FX, 0.0f, CX, 0.0f, FY, CY, 0.0f, 0.0f, 1.0f)).copyTo(K);
    Mat(Matx33f(FX * scalingFactor, 0.0f, CX * scalingFactor, 0.0f, FY * scalingFactor, CY * scalingFactor, 0.0f, 0.0f, 1.0f)).copyTo(scaledK);
    Mat(Matx14f(-0.00616707789546795f, 0.055942168483669785f, -0.008456906825946709f, -0.0015652853262762868f)).copyTo(D);

    qDebug("estimate new matrix");
    fisheye::estimateNewCameraMatrixForUndistortRectify(scaledK, D, targetSize, cv::Matx33f::eye(), targetK, 0.0, targetSize, 0.4);

    qDebug("init undistort");
    fisheye::initUndistortRectifyMap(K, D, cv::Matx33f::eye(), targetK, targetSize, CV_16SC2, this->map1, this->map2 );
  
}

int Defish::fisheyeDewarp(cv::UMat *inMat, cv::UMat *outMat)
{
    qDebug("Entering %s", __func__);
    cv::remap(*inMat, *outMat, this->map1, this->map2, INTER_LINEAR, BORDER_CONSTANT);

    return 0;
}

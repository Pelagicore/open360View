
#include <stdlib.h>
#include <iostream>
#define CL_TARGET_OPENCL_VERSION 120

#include <QDebug>
#include "defish.h"

// https://trac.ffmpeg.org/wiki/RemapFilter
/* So, to get the x’,y’ position for the circular image we will have to first pass the
 * coordinates x,y from the rectangular output image to spherical coordinates using the
 * first coordinate system, then those to the second shown spherical coordinate system,
 * then those to the polar projection and then pass the polar system to cardinal x’,y’.
 */




double2 Defish::evaluatePixel_Front(double2 outCoord, double2 srcSize) {
  double2 o;
  double theta, phi;
  double3 sphericCoords;
  double phi2_over_pi;
  double theta2;
  double2 inCentered;

  // convert outcoords to radians (180 = pi, so half a sphere)
  o.x = outCoord.x / srcSize.x ;
  o.y = outCoord.y / srcSize.y ;
  theta = (1.0 - o.x) * M_PI;
  phi = o.y * M_PI;

  // Convert outcoords to spherical (x,y,z on unisphere)
  sphericCoords.x = cos(theta) * sin(phi);
  sphericCoords.y = sin(theta) * sin(phi);
  sphericCoords.z = cos(phi);

  // Convert spherical to input coordinates...
  theta2 = atan2(-sphericCoords.z, sphericCoords.x);
  phi2_over_pi = acos(sphericCoords.y) / (M_PI);

  inCentered.x = ((phi2_over_pi * cos(theta2)) + 0.5) * srcSize.x;
  inCentered.y = ((phi2_over_pi * sin(theta2)) + 0.5) * srcSize.y;
  //inCentered.x += 0.00025* (srcSize.y/2 - inCentered.y) * (srcSize.x/2 - inCentered.x);// + 0.5 * (srcSize.x/2 - inCentered.x);
  //inCentered.x += (0.005*(srcSize.y/2 - inCentered.y)) * (inCentered.x - srcSize.x/2);// + 0.5 * (srcSize.x/2 - inCentered.x);

  return inCentered;
}

void Defish::gen_front_maps(int cols, int rows, int width, int height, double crop, UMat* image_x, UMat* image_y) {
  int x, y;
  cv::Mat out_x (rows, cols, CV_32FC1);
  cv::Mat out_y (rows, cols, CV_32FC1);
  cv::UMat unfl_x;
  cv::UMat unfl_y;



  printf("Front proj...");
  for (y = 0; y < rows; y++) {
    for (x = 0; x < cols; x++) {
      struct double2 outCoord = {
          .x = (((double)x / cols) * ((width) - (2 * crop))),
          .y = (((double)y / rows) * ((height) - (2 * crop)))
      };
      double2 o = evaluatePixel_Front(
          outCoord,
          (double2){(width) - (2 * crop), height - (2 * crop)});
      out_x.at<float>(y,x)  = (o.x + crop)/2;
      out_y.at<float>(y,x)  = (o.y + crop)/2;
    }
  }
  out_x.copyTo(unfl_x);
  out_y.copyTo(unfl_y);
  flip(unfl_x,*image_x,-1);
  flip(unfl_y,*image_y,-1);
  printf("Done!\n");
}

void Defish::CorrectPerspective (UMat *inMat, UMat *outMat) {
    Point2f srcpoints[] = {{1480/4.0f,0},
                           {3*1480/4.0f,0},
                           {1480/4.0f+180,1480},
                           {3*1480/4.0f-180,1480},
                          };
    Point2f dstpoints[] = {{1480/4.0f-360,200},
                           {3*1480/4.0f+360,200},
                           {1480/4.0f-360,1480+200},
                           {3*1480/4.0f+360,1480+200},
                          };
    Mat ret = getPerspectiveTransform(srcpoints,dstpoints);
    warpPerspective(*inMat,*outMat,ret,Size(inMat->cols,inMat->rows),INTER_LANCZOS4);

}



Defish::Defish(uint32_t src_width, uint32_t src_height, uint32_t dest_width, uint32_t dest_height, uint32_t num_color_channels)
{
    qDebug("Construct Defish");
    //UMat K, D, targetK, scaledK;
    //this->origSize = Size(src_width, src_height);
    //this->dewarpSize = Size(dest_width, dest_height);
    //Size targetSize = this->dewarpSize;
    //float scalingFactor = ((float) targetSize.width) / ((float) this->origSize.width);
    //Mat(Matx33f(FX, 0.0f, CX, 0.0f, FY, CY, 0.0f, 0.0f, 1.0f)).copyTo(K);
    //Mat(Matx33f(FX * scalingFactor, 0.0f, CX * scalingFactor, 0.0f, FY * scalingFactor, CY * scalingFactor, 0.0f, 0.0f, 1.0f)).copyTo(scaledK);
    //Mat(Matx14f(-0.00616707789546795f, 0.055942168483669785f, -0.008456906825946709f, -0.0015652853262762868f)).copyTo(D);

    //qDebug("estimate new matrix");
    //fisheye::estimateNewCameraMatrixForUndistortRectify(scaledK, D, targetSize, cv::Matx33f::eye(), targetK, 0.0, targetSize, 0.4);

    //qDebug("init undistort");
    //fisheye::initUndistortRectifyMap(K, D, cv::Matx33f::eye(), targetK, targetSize, CV_16SC2, this->map1, this->map2 );

    gen_front_maps(src_width, src_height, dest_width, dest_height, 0 , &this->map1, &this->map2);
  
}

int Defish::fisheyeDewarp(cv::UMat *inMat, cv::UMat *outMat)
{
    UMat equir;
    qDebug("Entering %s", __func__);
    remap(*inMat, equir, this->map1, this->map2, INTER_LINEAR, BORDER_CONSTANT);
    CorrectPerspective(&equir,outMat);

    return 0;
}

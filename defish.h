#pragma once
#define FX 338.2471781498528f
#define FY 338.88427412567574f
#define CX 640.0133185801166f
#define CY 539.9839119428968f

#include <CL/cl.h>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"

class Defish {
  cv::Size origSize, dewarpSize;
  cv::UMat map1, map2;

public:
  Defish(uint32_t src_width, uint32_t src_height, uint32_t dest_width, uint32_t dest_height, uint32_t num_color_channels);
  int fisheyeDewarp(cv::UMat *inMat, cv::UMat *outMat);
  virtual ~Defish() = default;

};

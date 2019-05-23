#pragma once

#include <cstdint>
#include <opencv2/core/mat.hpp>

class Framegrabber
{
public:
    Framegrabber(uint32_t num_cameras, uint32_t width, uint32_t height, uint32_t num_color_channels);

    int update();
    cv::UMat *getUMatFromFramebuffers();

    const uint32_t width;
    const uint32_t height;
    const unsigned int framebufferSize;
    const uint32_t num_cameras;

private:

    uint8_t **camera_framebuffers;
};

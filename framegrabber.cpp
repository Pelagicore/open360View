#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <stropts.h>
#include <unistd.h>

#include "framegrabber.h"

#include <QDebug>

Framegrabber::Framegrabber(uint32_t num_cameras, uint32_t width, uint32_t height, uint32_t num_color_channels)
    :framebufferSize(width * height * num_color_channels), num_cameras(num_cameras)
{
    qDebug("Entering %s", __func__);
    uint32_t i;

    qDebug("Initialize camera_framebuffers");
    camera_framebuffers = new uint8_t* [num_cameras];
    for(i = 0; i < num_cameras; i++) {
        camera_framebuffers[i] = new uint8_t[framebufferSize];
    }

    qDebug("Initialize camera_umats");
    camera_umats = new cv::UMat[num_cameras];
    for (i = 0; i < num_cameras; i++) {
        cv::Mat(static_cast<int>(height),
                static_cast<int>(width),
                CV_8UC3,
                camera_framebuffers[i]
        ).copyTo(camera_umats[i]);
    }
}

int Framegrabber::update()
{
    qDebug("Entering %s", __func__);
    int framegrabber_fd;
    uint32_t camera_nr;
    int ret;
    ssize_t readSize;

    errno = 0;
    framegrabber_fd = open("/dev/framegrabber", O_RDONLY);

    if (framegrabber_fd == -1) {
            printf("Error while opening '/dev/framegrabber': %s\n", strerror(errno));
            return errno;
    }

    for (camera_nr = 0; camera_nr < num_cameras; camera_nr++) {

        printf("Selecting camera nr to %i\n", camera_nr);
        ret = ioctl(framegrabber_fd, 0, camera_nr);
        if (ret == -1) {
                printf("Failed to select camera nr %d: %s\n", camera_nr, strerror(errno));
                break;
        }

        printf("Reading image from selected camera...\n");
        readSize = read(framegrabber_fd, camera_framebuffers[camera_nr], framebufferSize);
        if (readSize < framebufferSize) {
                printf("Error while reading device: %s\n", strerror(errno));
                break;
        }
    }

    close(framegrabber_fd);
    return errno;
}

cv::UMat *Framegrabber::getUMatFromFramebuffers()
{
    return camera_umats;
}

#include <iostream>
#include <fstream>

#include "dummyinput.h"


#define NUM_PICTURES 4

using namespace std;
using namespace cv;

Dummyinput::Dummyinput()
{

}


cv::UMat *Dummyinput::getUMatFromFramebuffers()
{
    uint32_t i;
    cv::UMat *camera_umats = new cv::UMat[NUM_PICTURES];



    for (i = 0; i < NUM_PICTURES; i++) {

        // Read the ith argument or image
        // and push into the image array
        camera_umats[i] = imread(argv[i]);
        if (camera_umats[i].empty())
        {
            // Exit if image is not present
            cout << "Can't read image '" << argv[i] << "'\n";
            return -1;
        }
//        cv::Mat(static_cast<int>(height), static_cast<int>(width), CV_8UC3, camera_framebuffers[i]).copyTo(camera_umats[i]);
    }

    return camera_umats;
}

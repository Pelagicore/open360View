
#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <cstdio>
#include <cstring>

#define CL_TARGET_OPENCL_VERSION 120

#include "camerastream.h"
#include "opencv2/stitching.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"


#define WIDTH 1280
#define HEIGHT 1080
#define BORDER_ADD_X 100
#define BORDER_ADD_Y 200
#define NUM_CAMERAS 4
#define NUM_COLOR_CHANNELS 3

using namespace cv;

// Define mode for stitching as panoroma
// (One out of many functions of Stitcher)
cv::Stitcher::Mode mode = cv::Stitcher::PANORAMA;

void addBorders (UMat *inMat,UMat *outMat) {
    UMat withborders;
    copyMakeBorder( *inMat, *outMat, BORDER_ADD_Y, BORDER_ADD_Y, BORDER_ADD_X, BORDER_ADD_X, BORDER_CONSTANT,Scalar(0,0,0));
}

void CameraStream::read_images_fromfile()
{
    int i;
    char image_filename[20];
    void addBorders (UMat *inMat,UMat *outMat);
    UMat wbord;

    for (i = 0; i < NUM_CAMERAS; i++) {
        snprintf(image_filename, 20, "camera_%d.png", i);
        cv::imread(image_filename).copyTo(camera_in_mats[i]);
        addBorders (&camera_in_mats[i],&wbord);
        wbord.copyTo(camera_in_mats[i]);
    }
    //imread("picture_0_c.png").copyTo(camera_in_mats[0]);
    //imread("picture_1_c.png").copyTo(camera_in_mats[1]);
    //imread("picture_2_c.png").copyTo(camera_in_mats[2]);
    //imread("picture_3_c.png").copyTo(camera_in_mats[3]);

}

CameraStream::CameraStream(QQuickItem* parent)
    : QQuickItem(parent),
    defish(WIDTH+BORDER_ADD_X*2, HEIGHT+BORDER_ADD_Y*2, (WIDTH+BORDER_ADD_X*2) * 2, (HEIGHT+BORDER_ADD_Y*2) * 2, NUM_COLOR_CHANNELS),
    framegrabber(NUM_CAMERAS, WIDTH, HEIGHT, NUM_COLOR_CHANNELS)
{
    camera_in_mats = new cv::UMat[NUM_CAMERAS];
    camera_out_mats = new cv::UMat[NUM_CAMERAS];

    this->setFlag(QQuickItem::ItemHasContents, true);
    read_images_fromfile();
}

CameraStream::~CameraStream()
{
    delete m_image;
}

UMat CameraStream::stitch_mat(UMat* camera_mats) {
    std::vector<UMat> u_img_vector;
    for (int i=0; i<NUM_CAMERAS; i++) {
            u_img_vector.push_back(camera_mats[i]);
        }
    // Define object to store the stitched image
    UMat pano;
    UMat p[1];

    // Create a Stitcher class object with mode panoroma
    Ptr<Stitcher> stitcher = Stitcher::create(mode);

    // Command to stitch all the images present in the image array
    Stitcher::Status status = stitcher->stitch(u_img_vector, pano);

    if (status != Stitcher::OK)
    {
        // Check if images could not be stiched
        // status is OK if images are stiched successfully
        qDebug("Can't stitch images\n");
//        return -1;
    }
    //hconcat(camera_mats[0],camera_mats[1],pano);
    //hconcat(camera_mats[1],camera_mats[1],pano);
    return pano;
}

QSGNode* CameraStream::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
    int i;
    UMat pano;
    UMat pano2;
    QQuickWindow *win = QQuickItem::window();

    QSGSimpleTextureNode *textureNode = static_cast<QSGSimpleTextureNode *>(oldNode);
    if (!textureNode)
        textureNode = new QSGSimpleTextureNode();

    if (textureNode->texture()) {
        delete textureNode->texture();
    }

    for (i = 0; i < NUM_CAMERAS; i++) {
        defish.fisheyeDewarp(&camera_in_mats[i], &camera_out_mats[i]);
        //camera_out_mats[i] = camera_in_mats[i];
    }
    //imwrite("result0.jpg", camera_out_mats[0]);
    //imwrite("result1.jpg", camera_out_mats[1]);
    //imwrite("result2.jpg", camera_out_mats[2]);
    //imwrite("result3.jpg", camera_out_mats[3]);

    //UMat stitched = stitch_mat(camera_in_mats);

    hconcat(camera_out_mats[0],camera_out_mats[3],pano);
    hconcat(pano,camera_out_mats[2],pano2);
    hconcat(pano2,camera_out_mats[1],pano);
    //printf ("R = %d, C = %d, S = %d\n",pano.rows,pano.cols,pano.step);
    m_image = new QImage(pano.getMat(cv::AccessFlag::ACCESS_READ).data,
                         pano.cols,
                         pano.rows,
                         pano.step,
                         QImage::Format_RGB888);


    QImage resizedImage = m_image->scaled(width(), height(), Qt::KeepAspectRatio);

    auto texture = win->createTextureFromImage(resizedImage);
    textureNode->setTexture(texture);

    textureNode->markDirty(QSGNode::DirtyMaterial);

    textureNode->setRect(0, 0, width(), height());

    //m_framecount++;

    return textureNode;
}

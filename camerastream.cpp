
#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <cstdio>
#include <cstring>

#include "camerastream.h"
#include "opencv2/stitching.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"


#define WIDTH 1280
#define HEIGHT 1080
#define NUM_CAMERAS 4
#define NUM_COLOR_CHANNELS 3

using namespace cv;

// Define mode for stitching as panoroma
// (One out of many functions of Stitcher)
cv::Stitcher::Mode mode = cv::Stitcher::PANORAMA;

void CameraStream::read_images_fromfile()
{
    int i;
    char image_filename[20];

    for (i = 0; i < NUM_CAMERAS; i++) {
        snprintf(image_filename, 20, "camera_%d.png", i);
        cv::imread(image_filename).copyTo(camera_in_mats[i]);
    }

}

CameraStream::CameraStream(QQuickItem* parent)
    : QQuickItem(parent),
    defish(WIDTH, HEIGHT, WIDTH * 2, HEIGHT * 2, NUM_COLOR_CHANNELS),
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
    hconcat(camera_mats[0],camera_mats[1],pano);
    return pano;
}

QSGNode* CameraStream::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
    int i;
    QQuickWindow *win = QQuickItem::window();

    QSGSimpleTextureNode *textureNode = static_cast<QSGSimpleTextureNode *>(oldNode);
    if (!textureNode)
        textureNode = new QSGSimpleTextureNode();

    if (textureNode->texture()) {
        delete textureNode->texture();
    }

    for (i = 0; i < NUM_CAMERAS; i++) {
        defish.fisheyeDewarp(&camera_in_mats[i], &camera_out_mats[i]);
    }

    UMat stitched = stitch_mat(camera_out_mats);

    m_image = new QImage(stitched.getMat(cv::AccessFlag::ACCESS_READ).data,
                         stitched.cols,
                         stitched.rows,
                         stitched.step,
                         QImage::Format_RGB888);


    QImage resizedImage = m_image->scaled(width(), height(), Qt::KeepAspectRatio);

    auto texture = win->createTextureFromImage(resizedImage);
    textureNode->setTexture(texture);

    textureNode->markDirty(QSGNode::DirtyMaterial);

    textureNode->setRect(0, 0, width(), height());

    //m_framecount++;

    return textureNode;
}

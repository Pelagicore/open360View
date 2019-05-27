
#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <cstdio>
#include <cstring>

#include "camerastream.h"

#define WIDTH 1280
#define HEIGHT 1080
#define NUM_CAMERAS 4
#define NUM_COLOR_CHANNELS 3

void CameraStream::read_images_fromfile()
{
    int i;
    char image_filename[20];

    for (i = 0; i < NUM_CAMERAS; i++) {
        snprintf(image_filename, 20, "camera_%d.png", i);
        cv::imread(image_filename).copyTo(camera_in_mats[i]);
    }

}

void CameraStream::cameraVisibleChanged()
{
    if (isVisible()) {
        qDebug("is visible");
        m_renderTimer.start();
    } else {
        m_renderTimer.stop();
    }
}

CameraStream::CameraStream(QQuickItem* parent)
    : QQuickItem(parent),
    defish(WIDTH, HEIGHT, WIDTH * 2, HEIGHT * 2, NUM_COLOR_CHANNELS),
    framegrabber(NUM_CAMERAS, WIDTH, HEIGHT, NUM_COLOR_CHANNELS)
{
    m_renderTimer.setInterval(30);
    m_renderTimer.setSingleShot(false);

    connect(this, &QQuickItem::visibleChanged, this, &CameraStream::cameraVisibleChanged);
    connect(&m_renderTimer, &QTimer::timeout, this, &QQuickItem::update);

    camera_in_mats = new cv::UMat[NUM_CAMERAS];
    camera_out_mats = new cv::UMat[NUM_CAMERAS];

    this->setFlag(QQuickItem::ItemHasContents, true);
    read_images_fromfile();
    m_renderTimer.start();
}

void CameraStream::nextImage() {
    current_cam = (current_cam + 1) % NUM_CAMERAS;
}

CameraStream::~CameraStream()
{
    delete m_image;
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

    m_image = new QImage(camera_out_mats[current_cam].getMat(cv::AccessFlag::ACCESS_READ).data,
                         camera_out_mats[current_cam].cols,
                         camera_out_mats[current_cam].rows,
                         camera_out_mats[current_cam].step,
                         QImage::Format_RGB888);

    *m_image = m_image->rgbSwapped();
    QImage resizedImage = m_image->scaled(width(), height(), Qt::KeepAspectRatio);

    auto texture = win->createTextureFromImage(resizedImage);
    textureNode->setTexture(texture);

    textureNode->markDirty(QSGNode::DirtyMaterial);

    textureNode->setRect(0, 0, width(), height());

    //m_framecount++;

    return textureNode;
}

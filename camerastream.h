#pragma once

#include "defish.h"
#include "framegrabber.h"

#include <opencv2/core/mat.hpp>
#include <QImage>
#include <QQuickItem>
#include <QSGSimpleRectNode>
#include <QTimer>

class CameraStream : public QQuickItem
{
    Q_OBJECT

public:
    CameraStream(QQuickItem* parent = nullptr);
    virtual ~CameraStream();

    Q_INVOKABLE void nextImage();

protected:
    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;

private:
    void read_images_fromfile();

    cv::UMat *camera_in_mats;
    cv::UMat *camera_out_mats;

    QTimer m_renderTimer;

    Defish defish;
    Framegrabber framegrabber;

    int current_cam = 0;
    QImage *m_image;
    void cameraVisibleChanged();
};

#pragma once

#include "defish.h"
#include "framegrabber.h"

#include <opencv2/core/mat.hpp>
#include <QImage>
#include <QQuickItem>
#include <QSGSimpleRectNode>

class CameraStream : public QQuickItem
{
    Q_OBJECT

public:
    CameraStream(QQuickItem* parent = nullptr);
    virtual ~CameraStream();

protected:
    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;

private:
    void read_images_fromfile();

    cv::UMat *camera_in_mats;
    cv::UMat *camera_out_mats;

    Defish defish;
    Framegrabber framegrabber;

    QImage *m_image;
};

#include "camerastream.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


    qDebug("Register type CameraStream");
    qmlRegisterType<CameraStream>("surroundview", 0, 1, "CameraStream");
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/SurroundView.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    Defish defish(WIDTH, HEIGHT, WIDTH * 2, HEIGHT * 2, NUM_COLOR_CHANNELS);

    Framegrabber framegrabber(NUM_CAMERAS, WIDTH, HEIGHT, NUM_COLOR_CHANNELS);
    cv::UMat *camera_in_mats = framegrabber.getUMatFromFramebuffers();
    cv::UMat camera_out_mats[NUM_CAMERAS];

    for (i = 0; i < NUM_CAMERAS; i++) {
        defish.fisheyeDewarp(&camera_in_mats[i], &camera_out_mats[i]);
    }

    qDebug("Start application");
    return app.exec();
}

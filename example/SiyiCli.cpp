#include <thread>

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

#include "Siyi.h"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    // Initialize Siyi API with default IP and port
    siyi::CameraApi& siyiApi = siyi::CameraApi::instance();

    // Check initialization
    if (!siyiApi.initialized()) {
        // Check initialization status periodically up to 3 seconds
        int maxTimeout     = 3;
        int currentTimeout = 0;
        while (!siyiApi.initialized() && currentTimeout < maxTimeout) {
            ++currentTimeout;
            qDebug() << "Siyi API is not initialized. Waiting for initialization...";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    if (!siyiApi.initialized()) {
        qDebug() << "Cannot initialize Siyi API";
        return 1;
    }

    // Initialize command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Siyi camera and gimbal controller test application");
    parser.addHelpOption();

    // Custom options
    parser.addOption({"version", "Show camera and gimbal version"});
    parser.addOption({"take-picture", "Take a picture to SD card"});
    parser.addOption({"toggle-recording", "Toggle video recording to SD card"});
    parser.addOption({"center-gimbal", "Center the gimbal"});
    QCommandLineOption zoomOption(QStringList() << "zoom", "Use zoom: <in|out|stop>", "<option>");
    zoomOption.setValueName("in|out|stop");
    parser.addOption(zoomOption);

    // Process arguments
    parser.process(app);

    if (parser.isSet("version")) {
        auto cametaTypematcher = [](siyi::CameraApi::CameraType type) {
            switch (type) {
            case siyi::CameraApi::CameraType::ZR10:
                return "ZR10";
            case siyi::CameraApi::CameraType::A8Mini:
                return "A8Mini";
            case siyi::CameraApi::CameraType::A2Mini:
                return "A2Mini";
            case siyi::CameraApi::CameraType::ZR30:
                return "ZR30";
            case siyi::CameraApi::CameraType::ZT30:
                return "ZT30";
            case siyi::CameraApi::CameraType::Unknown:
            default:
                return "Unknown";
            }
        };
        qDebug() << QStringLiteral("Board version: %1\nFirmware version: %2\nZoom firmware version: %3\nCamera type: %4")
                        .arg(siyiApi.firmwareMessage.boardVersion,
                             siyiApi.firmwareMessage.gimbalFirmwareVersion,
                             siyiApi.firmwareMessage.zoomFirmwareVersion)
                        .arg(cametaTypematcher(siyiApi.cameraType()));
    } else if (parser.isSet("take-picture")) {
        std::ignore = siyiApi.takePhoto();
    } else if (parser.isSet("toggle-recording")) {
        std::ignore = siyiApi.toggleRecordingVideo();
    } else if (parser.isSet("zoom")) {
        QString zoomOptionValue = parser.value("zoom");
        if (zoomOptionValue == "in" || zoomOptionValue == "out" || zoomOptionValue == "stop") {
            int8_t zoomDirection = 0;
            if (zoomOptionValue == "in") {
                zoomDirection = 1;
            } else if (zoomOptionValue == "out") {
                zoomDirection = -1;
            }
            std::ignore = siyiApi.zoomDirection(zoomDirection);
        } else {
            qDebug() << "Invalid zoom option. Valid options are: in, out, stop.";
        }
    } else if (parser.isSet("center-gimbal")) {
        std::ignore = siyiApi.setGimbalCenter();
    } else {
        parser.showHelp(0);
    }

    return 0;
}

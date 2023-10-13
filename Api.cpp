#include "Api.h"

#include <thread>
#include <QThread>

namespace siyi {

namespace {
constexpr auto kGimbalAttitudeTimeout{100}; // Update gimbal timeout in ms
}

Api::Api(QObject* parent)
    : QObject(parent) {
    init();

    // Send messages about hardware ID and firmware
    auto message = _messageBuilder.buildHardwareIDRequestMessage();
    emit sendMessage(message);
}

Api::~Api() {
    _siyiConnection->thread()->quit();
    _siyiConnection->thread()->wait();
    delete _siyiConnection;
}

void Api::init() {
    // Create Connection
    _siyiConnection = new Connection(_messageBuilder);

    // Receive message for processing
    // connect(_siyiConnection, &Connection::messageReceived, this, &Api::processSdkMessage);
    connect(_siyiConnection, &Connection::messageReceived, [this](const QVariant& message, quint8 command) {
        processSdkMessage(message, command);
    });

    // Send message to camera
    connect(this, &Api::sendMessage, _siyiConnection, &Connection::sendMessage);

    // Create thread and move connection worker to it
    auto* connectionThread = new QThread(this);
    _siyiConnection->moveToThread(connectionThread);
    connect(connectionThread, &QThread::started, _siyiConnection, &Connection::init);
    connectionThread->start();

    // Start gimbal attitude timer
    _gimbalAttitudeTimer = startTimer(kGimbalAttitudeTimeout);
}

void Api::processSdkMessage(const QVariant& message, quint8 command) {
    switch (static_cast<Command>(command)) {
    case Command::UNKNOWN:
    case Command::AUTO_FOCUS:
    case Command::MANUAL_ZOOM:
    case Command::ABSOLUTE_ZOOM:
    case Command::GIMBAL_ROTATION:
    case Command::GIMBAL_CENTER:
    case Command::FUNC_FEEDBACK_INFO:
    case Command::PHOTO_VIDEO_HDR:
    case Command::GIMBAL_CONTROL_ANGLE:
        break;
    case Command::MANUAL_FOCUS: {
        manualFocusMessage = message.value<ManualFocusMessage>();
        break;
    }
    case Command::ACQUIRE_FW_VER: {
        firmwareMessage = message.value<FirmwareMessage>();
        break;
    }
    case Command::ACQUIRE_HW_ID: {
        hardwareIDMessage = message.value<HardwareIDMessage>();
        getCameraType();
        break;
    }
    case Command::ACQUIRE_GIMBAL_ATT: {
        gimbalAttitudeMessage = message.value<GimbalAttitudeMessage>();
        emit updateGimbalAngles();
        break;
    }
    case Command::ACQUIRE_GIMBAL_INFO:
        cameraStatusInfoMessage = message.value<CameraStatusInfoMessage>();
        break;
    }
}

bool Api::setAngles(float pan, float tilt) {
    auto message = _messageBuilder.buildSetGimbalControlAngleRequestMessage(static_cast<int16_t>(pan * 10),
                                                                            static_cast<int16_t>(tilt * 10));
    emit sendMessage(message);
    return true;
}

bool Api::setGimbalCenter() {
    auto message = _messageBuilder.buildGimbalCenterRequestMessage();
    emit sendMessage(message);
    return true;
}

bool Api::setRates(float panRate, float tiltRate) {
    auto message = _messageBuilder.buildGimbalRotationRequestMessage(static_cast<int8_t>(panRate),
                                                                     static_cast<int8_t>(tiltRate));
    emit sendMessage(message);
    return true;
}

bool Api::takePhoto() {
    auto message = _messageBuilder.buildTakePhotoRequestMessage();
    emit sendMessage(message);
    return true;
}

bool Api::toggleRecordingVideo() {
    auto message = _messageBuilder.buildStartStopRecordingRequestMessage();
    emit sendMessage(message);
    return true;
}

bool Api::setCameraMode(uint8_t mode) {
    switch (mode) {
    case 0: {
        return takePhoto();
    }
    case 2: {
        return toggleRecordingVideo();
    }
    case 3: {
        auto message = _messageBuilder.buildMotionLockModeRequestMessage();
        emit sendMessage(message);
        return true;
    }
    case 4: {
        auto message = _messageBuilder.buildMotionFollowModeRequestMessage();
        emit sendMessage(message);
        return true;
    }
    case 5: {
        auto message = _messageBuilder.buildMotionFPVModeRequestMessage();
        emit sendMessage(message);
        return true;
    }
    default:
        break;
    }
    return false;
}

bool Api::zoom(uint8_t zoomValue) {
    auto message = _messageBuilder.buildAbsoluteZoomRequestMessage(zoomValue);
    emit sendMessage(message);

    return true;
}

bool Api::zoomDirection(int8_t direction) {
    auto message = _messageBuilder.buildManualZoomRequestMessage(direction);
    emit sendMessage(message);
    return true;
}

bool Api::manualFocus(int8_t direction) {
    auto message = _messageBuilder.buildManualFocusShotRequestMessage(direction);
    emit sendMessage(message);
    return true;
}

void Api::timerEvent(QTimerEvent* e) {
    if (initialized() && e->timerId() == _gimbalAttitudeTimer) {
        emit sendMessage(_messageBuilder.buildAcquireGimbalAttitudeRequestMessage());
    }
}

void Api::getCameraType() {
    static QMap<uint16_t, CameraType> cameraTypeMap{
        {0x6B, CameraType::ZR10},
        {0x73, CameraType::A8Mini},
        {0x75, CameraType::A2Mini},
        {0x78, CameraType::ZR30},
        {0x7A, CameraType::ZT30},
    };
    _cameraType = cameraTypeMap.value(hardwareIDMessage.modelId, CameraType::Unknown);
}

} // namespace siyi

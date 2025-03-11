#include "CameraApi.h"

#include <QLoggingCategory>

#include "CommunicationWorker.h"
#include "MessageBuilder.h"

namespace siyi {

Q_LOGGING_CATEGORY(siyiSdkApi, "siyi.sdk.api")

namespace {
constexpr auto kGimbalAttitudeTimeout{100}; // Update gimbal timeout in ms
}

CameraApi::CameraApi(const QString& serverIp, quint16 port, QObject* parent)
    : QObject(parent)
    , _messageBuilder(std::make_shared<MessageBuilder>()) {
    init(serverIp, port);

    // Send messages about hardware ID and firmware
    auto message = _messageBuilder->buildHardwareIDRequestMessage();
    emit sendMessage(message);
}

CameraApi::~CameraApi() {
    _siyiCommunicationWorkerThread.quit();
    _siyiCommunicationWorkerThread.wait();
}

void CameraApi::init(const QString& serverIp, quint16 port) {
    // Create Connection
    _siyiCommunicationWorker = new CommunicationWorker(_messageBuilder, serverIp, port);

    // Receive message for processing
    // connect(_siyiConnection, &Connection::messageReceived, this, &Api::processSdkMessage);
    connect(_siyiCommunicationWorker, &CommunicationWorker::messageReceived, [this](const QVariant& message, quint8 command) {
        processSdkMessage(message, command);
    });

    // Send message to camera
    connect(this, &CameraApi::sendMessage, _siyiCommunicationWorker, &CommunicationWorker::sendMessage);

    // Create thread and move connection worker to it
    _siyiCommunicationWorker->moveToThread(&_siyiCommunicationWorkerThread);
    connect(&_siyiCommunicationWorkerThread, &QThread::finished, _siyiCommunicationWorker, &QObject::deleteLater);
    connect(&_siyiCommunicationWorkerThread, &QThread::started, _siyiCommunicationWorker, &CommunicationWorker::init);
    _siyiCommunicationWorkerThread.start();

    // Start gimbal attitude timer
    _gimbalAttitudeTimer = startTimer(kGimbalAttitudeTimeout);
}

void CameraApi::processSdkMessage(const QVariant& message, quint8 command) {
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

bool CameraApi::setAngles(float pan, float tilt) {
    auto message = _messageBuilder->buildSetGimbalControlAngleRequestMessage(static_cast<int16_t>(pan * 10),
                                                                             static_cast<int16_t>(tilt * 10));
    emit sendMessage(message);
    return true;
}

bool CameraApi::setGimbalCenter() {
    auto message = _messageBuilder->buildGimbalCenterRequestMessage();
    emit sendMessage(message);
    return true;
}

bool CameraApi::setRates(float panRate, float tiltRate) {
    auto message = _messageBuilder->buildGimbalRotationRequestMessage(static_cast<int8_t>(panRate), static_cast<int8_t>(tiltRate));
    emit sendMessage(message);
    return true;
}

bool CameraApi::takePhoto() {
    auto message = _messageBuilder->buildTakePhotoRequestMessage();
    emit sendMessage(message);
    return true;
}

bool CameraApi::toggleRecordingVideo() {
    auto message = _messageBuilder->buildStartStopRecordingRequestMessage();
    emit sendMessage(message);
    return true;
}

bool CameraApi::setCameraMode(uint8_t mode) {
    switch (mode) {
    case 0: {
        return takePhoto();
    }
    case 2: {
        return toggleRecordingVideo();
    }
    case 3: {
        auto message = _messageBuilder->buildMotionLockModeRequestMessage();
        emit sendMessage(message);
        return true;
    }
    case 4: {
        auto message = _messageBuilder->buildMotionFollowModeRequestMessage();
        emit sendMessage(message);
        return true;
    }
    case 5: {
        auto message = _messageBuilder->buildMotionFPVModeRequestMessage();
        emit sendMessage(message);
        return true;
    }
    default:
        break;
    }
    return false;
}

bool CameraApi::zoom(uint8_t zoomValue) {
    auto message = _messageBuilder->buildAbsoluteZoomRequestMessage(zoomValue);
    emit sendMessage(message);

    return true;
}

bool CameraApi::zoomDirection(int8_t direction) {
    auto message = _messageBuilder->buildManualZoomRequestMessage(direction);
    emit sendMessage(message);
    return true;
}

bool CameraApi::manualFocus(int8_t direction) {
    auto message = _messageBuilder->buildManualFocusShotRequestMessage(direction);
    emit sendMessage(message);
    return true;
}

void CameraApi::timerEvent(QTimerEvent* e) {
    if (initialized() && e->timerId() == _gimbalAttitudeTimer) {
        emit sendMessage(_messageBuilder->buildAcquireGimbalAttitudeRequestMessage());
    }
}

void CameraApi::getCameraType() {
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

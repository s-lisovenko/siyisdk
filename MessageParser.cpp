#include "MessageParser.h"

#include <QDataStream>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(siyiMessageParser, "siyi.messageParser")

namespace siyi {

QVariant FirmwareMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing firmware message " << data.toHex();

    FirmwareMessage firmwareMessage;
    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    stream >> firmwareMessage.boardVersion;
    stream >> firmwareMessage.gimbalFirmwareVersion;
    stream >> firmwareMessage.zoomFirmwareVersion;
    return QVariant::fromValue(firmwareMessage);
}

QVariant HardwareIDMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing hardware ID message " << data.toHex();

    HardwareIDMessage hardwareIDMessage;
    hardwareIDMessage.hardwareID = data.toHex();

    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    stream >> hardwareIDMessage.modelId;

    return QVariant::fromValue(hardwareIDMessage);
}

QVariant AutoFocusMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing auto focus message " << data.toHex();

    AutoFocusMessage autoFocusMessage;
    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    stream >> autoFocusMessage.success;
    return QVariant::fromValue(autoFocusMessage);
}

QVariant ManualZoomMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing manual zoom message " << data.toHex();

    ManualZoomMessage manualZoomMessage;
    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    stream >> manualZoomMessage.zoomLevel;
    return QVariant::fromValue(manualZoomMessage);
}

QVariant AbsoluteZoomMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing absolute zoom message " << data.toHex();

    AbsoluteZoomMessage absoluteZoomMessage;
    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    stream >> absoluteZoomMessage.absoluteMovementAsk;
    return QVariant::fromValue(absoluteZoomMessage);
}

QVariant ManualFocusMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing manual focus message " << data.toHex();

    ManualFocusMessage manualFocusMessage;
    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    stream >> manualFocusMessage.state;
    return QVariant::fromValue(manualFocusMessage);
}

QVariant GimbalRotationMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing gimbal rotation message " << data.toHex();

    GimbalRotationMessage gimbalRotationMessage;
    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    stream >> gimbalRotationMessage.state;
    return QVariant::fromValue(gimbalRotationMessage);
}

QVariant GimbalCenterMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing gimbal center message " << data.toHex();

    GimbalCenterMessage gimbalCenterMessage;
    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    stream >> gimbalCenterMessage.state;
    return QVariant::fromValue(gimbalCenterMessage);
}

QVariant FunctionFeedbackMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing function feedback message " << data.toHex();

    FunctionFeedbackMessage functionFeedbackMessage;
    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    stream >> functionFeedbackMessage.state;
    return QVariant::fromValue(functionFeedbackMessage);
}

QVariant GimbalAttitudeMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing gimbal attitude message " << data.toHex();

    GimbalAttitudeMessage gimbalAttitudeMessage;
    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    stream >> gimbalAttitudeMessage.pitch;
    stream >> gimbalAttitudeMessage.roll;
    stream >> gimbalAttitudeMessage.yaw;
    return QVariant::fromValue(gimbalAttitudeMessage);
}

QVariant GimbalControlAngleMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing gimbal control angle message " << data.toHex();

    GimbalControlAngleMessage gimbalControlAngleMessage;
    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    stream >> gimbalControlAngleMessage.pitch;
    stream >> gimbalControlAngleMessage.roll;
    stream >> gimbalControlAngleMessage.yaw;
    return QVariant::fromValue(gimbalControlAngleMessage);
}

QVariant CameraStatusInfoMessageParser::parse(const QByteArray& data) {
    qCDebug(siyiMessageParser) << "Parsing camera status info message " << data.toHex();

    CameraStatusInfoMessage cameraStatusInfoMessage;
    // Read the data from the stream into the struct fields
    QDataStream stream(data);
    uint8_t status;
    stream >> status; // reserved
    stream >> status; // hdr status
    cameraStatusInfoMessage.hdrOn = status == 1;
    stream >> status; // reserved
    stream >> status; // recording status
    cameraStatusInfoMessage.recordingStatus = static_cast<CameraStatusInfoMessage::RecordingStatus>(status);
    stream >> status; // gimbal motion mode
    cameraStatusInfoMessage.gimbalMotionMode = static_cast<CameraStatusInfoMessage::GimbalMotionMode>(status);
    stream >> status; // gimbal mounting
    cameraStatusInfoMessage.gimbalMounting = static_cast<CameraStatusInfoMessage::GimbalMounting>(status);
    stream >> status; // hdmi on cvbs off
    cameraStatusInfoMessage.hdmiOnCvbsOff = status == 0;

    return QVariant::fromValue(cameraStatusInfoMessage);
}

} // namespace siyi

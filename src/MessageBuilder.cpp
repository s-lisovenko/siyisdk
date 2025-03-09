#include "MessageBuilder.h"

#include <QDataStream>
#include <QIODevice>
#include <QLoggingCategory>
#include <QtEndian>

#include "Crc.h"

Q_LOGGING_CATEGORY(siyiMessageBuilder, "siyi.messageBuilder")

namespace siyi {

uint16_t MessageBuilder::_sequenceNumber{0};

QByteArray MessageBuilder::buildFirmwareRequestMessage() {
    return encode(Command::ACQUIRE_FW_VER);
}

QByteArray MessageBuilder::buildHardwareIDRequestMessage() {
    return encode(Command::ACQUIRE_HW_ID);
}

QByteArray MessageBuilder::buildAutoFocusRequestMessage() {
    QByteArray data;
    data.append(1);
    return encode(Command::AUTO_FOCUS, data);
}

QByteArray MessageBuilder::buildManualZoomRequestMessage(int8_t direction) {
    if (direction > 1) {
        direction = 1;
    }
    if (direction < -1) {
        direction = -1;
    }

    QByteArray data;
    data.append(direction);
    return encode(Command::MANUAL_ZOOM, data);
}

QByteArray MessageBuilder::buildAbsoluteZoomRequestMessage(uint8_t zoomLevel) {
    QByteArray data;
    data.append(static_cast<char>(zoomLevel));
    return encode(Command::ABSOLUTE_ZOOM, data);
}

QByteArray MessageBuilder::buildManualFocusShotRequestMessage(int8_t direction) {
    if (direction > 1) {
        direction = 1;
    }
    if (direction < -1) {
        direction = -1;
    }
    QByteArray data;
    data.append(direction);
    return encode(Command::MANUAL_FOCUS, data);
}

QByteArray MessageBuilder::buildGimbalRotationRequestMessage(int8_t yawSpeed, int8_t pitchSpeed) {
    QByteArray data;
    data.append(yawSpeed);
    data.append(pitchSpeed);
    return encode(Command::GIMBAL_ROTATION, data);
}

QByteArray MessageBuilder::buildGimbalCenterRequestMessage() {
    QByteArray data;
    data.append(1);
    return encode(Command::GIMBAL_CENTER, data);
}

QByteArray MessageBuilder::buildSetGimbalControlAngleRequestMessage(int16_t yawAngle, int16_t pitchAngle) {
    QByteArray  data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << yawAngle;
    stream << pitchAngle;
    return encode(Command::GIMBAL_CONTROL_ANGLE, data);
}

QByteArray MessageBuilder::buildAcquireGimbalAttitudeRequestMessage() {
    return encode(Command::ACQUIRE_GIMBAL_ATT);
}

QByteArray MessageBuilder::buildTakePhotoRequestMessage() {
    QByteArray data;
    data.append(static_cast<char>(0));
    return encode(Command::PHOTO_VIDEO_HDR, data);
}

QByteArray MessageBuilder::buildSwitchHDRRequestMessage() {
    QByteArray data;
    data.append(1);
    return encode(Command::PHOTO_VIDEO_HDR, data);
}

QByteArray MessageBuilder::buildStartStopRecordingRequestMessage() {
    QByteArray data;
    data.append(2);
    return encode(Command::PHOTO_VIDEO_HDR, data);
}

QByteArray MessageBuilder::buildMotionLockModeRequestMessage() {
    QByteArray data;
    data.append(3);
    return encode(Command::PHOTO_VIDEO_HDR, data);
}

QByteArray MessageBuilder::buildMotionFollowModeRequestMessage() {
    QByteArray data;
    data.append(4);
    return encode(Command::PHOTO_VIDEO_HDR, data);
}

QByteArray MessageBuilder::buildMotionFPVModeRequestMessage() {
    QByteArray data;
    data.append(5);
    return encode(Command::PHOTO_VIDEO_HDR, data);
}

QByteArray MessageBuilder::buildSetVideoOutputHDMIRequestMessage() {
    QByteArray data;
    data.append(6);
    return encode(Command::PHOTO_VIDEO_HDR, data);
}

QByteArray MessageBuilder::buildSetVideoOutputCVBSRequestMessage() {
    QByteArray data;
    data.append(7);
    return encode(Command::PHOTO_VIDEO_HDR, data);
}

QByteArray MessageBuilder::buildAcquireGimbalInfoRequestMessage() {
    return encode(Command::ACQUIRE_GIMBAL_INFO);
}

std::tuple<QByteArray, size_t, Command, uint16_t> MessageBuilder::decode(const QByteArray& message) {
    // Assuming the message structure is: header (2 bytes), control (1 byte),
    // data length (2 bytes), sequence number (2 bytes), command code (1 byte), data, CRC (2 bytes)

    // Check if the message has at least the minimum required length
    if (message.size() < 10) {
        // Handle error, the message is too short
        // You can throw an exception or return an error tuple indicating the failure
        // For simplicity, let's assume an empty data and zero values for other fields
        return std::make_tuple(QByteArray(), 0, Command::UNKNOWN, 0);
    }

    // Extract the header (revert the byte order) skip

    // Extract the control and data length
    auto control    = static_cast<uint8_t>(extractByte(message, 2));
    auto dataLength = static_cast<uint16_t>((static_cast<uint16_t>(extractByte(message, 4)) << 8)
                                            | static_cast<uint16_t>(extractByte(message, 3)));

    // Check message length
    if (dataLength >= message.size() - 5) {
        qCWarning(siyiMessageBuilder) << "Message length is not correct";
        return std::make_tuple(QByteArray(), 0, Command::UNKNOWN, 0);
    }

    // Extract the sequence number (revert the byte order)
    auto sequenceNumber = static_cast<uint16_t>((static_cast<uint16_t>(extractByte(message, 6)) << 8)
                                                | static_cast<uint16_t>(extractByte(message, 5)));

    // Extract the command code
    auto commandCode = static_cast<uint8_t>(extractByte(message, 7));

    // Extract the data
    QByteArray data = message.mid(8, dataLength);

    // Extract the CRC (revert the byte order)
    auto receivedCRC = static_cast<uint16_t>((static_cast<uint16_t>(extractByte(message, message.length() - 1)) << 8)
                                             | static_cast<uint16_t>(extractByte(message, message.length() - 2)));

    // Calculate the CRC for the message (excluding the CRC itself)
    uint16_t calculatedCRC = Crc::calculateCRC16(message.mid(0, message.length() - 2), 0);

    // Check if the received CRC matches the calculated CRC
    if (receivedCRC != calculatedCRC) {
        qCDebug(siyiMessageBuilder) << "CRC error";
        // Handle CRC error, the received CRC does not match the calculated CRC
        // You can throw an exception or return an error tuple indicating the failure
        // For simplicity, let's assume an empty data and zero values for other fields
        return std::make_tuple(QByteArray(), 0, Command::UNKNOWN, 0);
    }

    // Create the tuple with the extracted values and return it
    return std::make_tuple(data, dataLength, static_cast<Command>(commandCode), sequenceNumber);
}

QByteArray MessageBuilder::encode(Command command, const QByteArray& data) {
    uint16_t header         = revertBytes(0x6655);
    uint8_t  control        = 0x01;
    uint16_t dataLength     = revertBytes(data.length());
    uint16_t sequenceNumber = revertBytes(getSequenceNumber());
    uint8_t  commandCode    = static_cast<uint8_t>(command);

    QByteArray message;
    message.append(reinterpret_cast<const char*>(&header), sizeof(header));
    message.append(reinterpret_cast<const char*>(&control), sizeof(control));
    message.append(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));
    message.append(reinterpret_cast<const char*>(&sequenceNumber), sizeof(sequenceNumber));
    message.append(reinterpret_cast<const char*>(&commandCode), sizeof(commandCode));
    message.append(data);

    uint16_t crc         = Crc::calculateCRC16(message, 0);
    uint16_t crcReversed = revertBytes(crc);
    message.append(reinterpret_cast<const char*>(&crcReversed), sizeof(crcReversed));

    qCDebug(siyiMessageBuilder) << "MessageBuilder::encode: " << message.toHex();

    return message;
}

uint16_t MessageBuilder::getSequenceNumber() {
    auto returnNumber = _sequenceNumber;
    _sequenceNumber++;
    if (_sequenceNumber > 65535) {
        _sequenceNumber = 0;
    }

    return returnNumber;
}

uint16_t MessageBuilder::revertBytes(uint16_t value) {
    // Convert ToLittleEndian only if needed
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        return qToLittleEndian(value);
    }
    return value;
}

uint8_t MessageBuilder::extractByte(const QByteArray& data, size_t index) {
    if (data.size() > index) {
        return static_cast<uint8_t>(data[static_cast<int>(index)]);
    }
    return 0;
}

} // namespace siyi

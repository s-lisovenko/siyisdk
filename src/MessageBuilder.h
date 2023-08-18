#ifndef MESSAGEBUILDER_H
#define MESSAGEBUILDER_H

#include <cstdint>
#include <tuple>
#include <QByteArray>

#include "Command.h"

namespace siyi {

class MessageBuilder
{
public:
    QByteArray buildFirmwareRequestMessage();
    QByteArray buildHardwareIDRequestMessage();

    // Zoom
    QByteArray buildManualZoomRequestMessage(int8_t direction);
    QByteArray buildAbsoluteZoomRequestMessage(uint8_t zoomLevel);

    // Focus
    QByteArray buildAutoFocusRequestMessage();
    QByteArray buildManualFocusShotRequestMessage(int8_t direction);

    /**
     *  Build gimbal rotation request message
     * @param yawSpeed yae speed [-100, 100]
     * @param pitchSpeed Zoom value [-100, 100]
     * -100~0~100: Negative and positive represent two directions, higher or lower the number is away from
        0, faster the rotation speed is. Send 0 when released from control command and gimbal stops rotation.
     * @return Absolute zoom request message
     */
    QByteArray buildGimbalRotationRequestMessage(int8_t yawSpeed, int8_t pitchSpeed);

    QByteArray buildGimbalCenterRequestMessage();

    /**
     * Build gimbal set angle request message
     * @param yawAngle Target yaw angle
     * @param pitchAngle Target pitch angle
     * @return Set gimbal control angle request message
     */
    QByteArray buildSetGimbalControlAngleRequestMessage(int16_t yawAngle, int16_t pitchAngle);

    /**
     * Gimbal attitude request
     * @return Gimbal attitude request  message
     */
    QByteArray buildAcquireGimbalAttitudeRequestMessage();

    // Photo and Video
    QByteArray buildTakePhotoRequestMessage();
    QByteArray buildSwitchHDRRequestMessage();
    QByteArray buildStartStopRecordingRequestMessage();
    QByteArray buildMotionLockModeRequestMessage();
    QByteArray buildMotionFollowModeRequestMessage();
    QByteArray buildMotionFPVModeRequestMessage();
    QByteArray buildSetVideoOutputHDMIRequestMessage();
    QByteArray buildSetVideoOutputCVBSRequestMessage();

    QByteArray buildAcquireGimbalInfoRequestMessage();

    /**
     * @brief Decode incoming data
     * @param message Data to decode
     * @return Tuple containing decoded data {data, data length, command, sequence number}
     */
    [[nodiscard]] std::tuple<QByteArray, size_t, Command, uint16_t> decode(const QByteArray& message);

private:
    /**
     * @brief Encode outgoing data
     * @param command Command to encode
     * @param data Data to encode
     * @return Encoded data
     */
    [[nodiscard]] QByteArray encode(Command command, const QByteArray& data = {});

    /**
     * Increments sequence number by one.
     * @return Sequence number
     */
    uint16_t getSequenceNumber();

    /**
     * Reverts bytes in a 16-bit value.
     * @param value Value to revert
     * @return Reverted value
     */
    uint16_t revertBytes(uint16_t value);

    /**
     * Extracts byte from QByteArray at specified index.
     * @param data Data to extract byte from
     * @param index Index of byte to extract
     * @return Extracted byte
     */
    uint8_t extractByte(const QByteArray& data, size_t index);

private:
    static uint16_t _sequenceNumber;
};

} // namespace siyi

#endif // MESSAGEBUILDER_H

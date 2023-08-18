#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <tuple>
#include <QByteArray>
#include <QVariant>

#include "Command.h"
#include "Message.h"

namespace siyi {

/**
 * Response message
 */
struct ResponseMessageParser
{
    virtual ~ResponseMessageParser() = default;
    /**
     * Parse message
     * @param data
     */
    virtual QVariant parse(const QByteArray& data) = 0;
    [[nodiscard]] virtual Command command() const = 0;

    bool success{false};
};

/**
 * The FirmwareMessage
 */
struct FirmwareMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::ACQUIRE_FW_VER; }
};

/**
 * The HardwareIDMessage
 */
struct HardwareIDMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::ACQUIRE_HW_ID; }
};

/**
 * The AutoFocusMessage
 */
struct AutoFocusMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::AUTO_FOCUS; }
};

/**
 * The ManualZoomMessage
 */
struct ManualZoomMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::MANUAL_ZOOM; }
};

/**
 * The AbsoluteZoomMessage
 */
struct AbsoluteZoomMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::ABSOLUTE_ZOOM; }
};

/**
 * The ManualFocusMessage
 */
struct ManualFocusMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::MANUAL_FOCUS; }
};

/**
 * The GimbalRotationMessage
 */
struct GimbalRotationMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::GIMBAL_ROTATION; }
};

/**
 * The CenterMessage
 */

struct GimbalCenterMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::GIMBAL_CENTER; }
};

/**
 * The FunctionFeedbackMessage
 */
struct FunctionFeedbackMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::FUNC_FEEDBACK_INFO; }

    // 0: Success
    // 1: Fail to take a photo (Please check
    // if TF card is inserted)
    // 2: HDR ON
    // 3: HDR OFF
    // 4: Fail to record a video (Please check
    // if TF card is inserted)
    uint8_t state{0};
};

/**
 * The GimbalAttitudeMessage
 */
struct GimbalAttitudeMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::ACQUIRE_GIMBAL_ATT; }
};

/**
 * The GimbalControlAngleMessage
 */
struct GimbalControlAngleMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::GIMBAL_CONTROL_ANGLE; }
};

struct CameraStatusInfoMessageParser : public ResponseMessageParser
{
    QVariant parse(const QByteArray& data) override;
    [[nodiscard]] Command command() const override { return Command::ACQUIRE_GIMBAL_INFO; }
};

} // namespace siyi

#endif // MESSAGEPARSER_H

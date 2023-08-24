#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <QMetaType>

/**
 * The FirmwareMessage
 */
struct FirmwareMessage
{
    uint32_t boardVersion{0};
    uint32_t gimbalFirmwareVersion{0};
    uint32_t zoomFirmwareVersion{0};
};

/**
 * The HardwareIDMessage
 */
struct HardwareIDMessage
{
    QString hardwareID;
    uint16_t modelId;
};

/**
 * The AutoFocusMessage
 */
struct AutoFocusMessage
{
    bool success{false};
};

/**
 * The ManualZoomMessage
 */
struct ManualZoomMessage
{
    [[nodiscard]] float actualZoom() const { return static_cast<float>(zoomLevel) / 10.0f; }

    uint16_t zoomLevel{0};
};

/**
 * The AbsoluteZoomMessage
 */

struct AbsoluteZoomMessage
{
    uint8_t absoluteMovementAsk{0};
};

/**
 * The ManualFocusMessage
 */
struct ManualFocusMessage
{
    uint8_t state{0}; // 1: Success, 0: Fail
};

/**
 * The GimbalRotationMessage
 */
struct GimbalRotationMessage
{
    uint8_t state{0}; // 1: Success, 0: Fail
};

/**
 * The CenterMessage
 */

struct GimbalCenterMessage
{
    uint8_t state{0}; // 1: Success, 0: Fail
};

/**
 * The FunctionFeedbackMessage
 */
struct FunctionFeedbackMessage
{
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
struct GimbalAttitudeMessage
{
    [[nodiscard]] float actualYaw() const { return static_cast<float>(yaw) / 10.0f; }
    [[nodiscard]] float actualPitch() const { return static_cast<float>(pitch) / 10.0f; }
    [[nodiscard]] float actualRoll() const { return static_cast<float>(roll) / 10.0f; }
    [[nodiscard]] float actualYawVelocity() const { return static_cast<float>(yawVelocity) / 10.0f; }
    [[nodiscard]] float actualPitchVelocity() const { return static_cast<float>(pitchVelocity) / 10.0f; }
    [[nodiscard]] float actualRollVelocity() const { return static_cast<float>(rollVelocity) / 10.0f; }

    int16_t yaw{0};
    int16_t pitch{0};
    int16_t roll{0};
    int16_t yawVelocity{0};
    int16_t pitchVelocity{0};
    int16_t rollVelocity{0};
};

/**
 * The GimbalControlAngleMessage
 */
struct GimbalControlAngleMessage
{
    [[nodiscard]] float actualYaw() const { return static_cast<float>(yaw) / 10.0f; }
    [[nodiscard]] float actualPitch() const { return static_cast<float>(pitch) / 10.0f; }
    [[nodiscard]] float actualRoll() const { return static_cast<float>(roll) / 10.0f; }

    int16_t yaw{0};
    int16_t pitch{0};
    int16_t roll{0};
};

// Camera status information
struct CameraStatusInfoMessage
{
    enum RecordingStatus {
        RecordingOn = 0,
        RecordingOff = 1,
        TFCardSlotEmpty = 2,
        DataLoss = 3,
        Undefined,
    };

    enum class GimbalMotionMode {
        Lock = 0,
        Follow = 1,
        FPV = 2,
        Undefined,
    };

    enum class GimbalMounting {
        Reserved = 0,
        Normal = 1,
        UpsideDown = 2,
        Undefined,
    };

    bool hdrOn{false};
    RecordingStatus recordingStatus{RecordingStatus::Undefined};
    GimbalMotionMode gimbalMotionMode{GimbalMotionMode::Undefined};
    GimbalMounting gimbalMounting{GimbalMounting::Undefined};
    bool hdmiOnCvbsOff{false};
};

Q_DECLARE_METATYPE(FirmwareMessage)
Q_DECLARE_METATYPE(HardwareIDMessage)
Q_DECLARE_METATYPE(AutoFocusMessage)
Q_DECLARE_METATYPE(ManualZoomMessage)
Q_DECLARE_METATYPE(AbsoluteZoomMessage)
Q_DECLARE_METATYPE(ManualFocusMessage)
Q_DECLARE_METATYPE(GimbalRotationMessage)
Q_DECLARE_METATYPE(GimbalCenterMessage)
Q_DECLARE_METATYPE(FunctionFeedbackMessage)
Q_DECLARE_METATYPE(GimbalAttitudeMessage)
Q_DECLARE_METATYPE(GimbalControlAngleMessage)
Q_DECLARE_METATYPE(CameraStatusInfoMessage)

#endif // MESSAGE_H

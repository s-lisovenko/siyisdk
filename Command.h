#ifndef COMMAND_H
#define COMMAND_H

namespace siyi {

enum class Command : quint8 {
    UNKNOWN = 0x00,
    ACQUIRE_FW_VER = 0x01,
    ACQUIRE_HW_ID = 0x02,
    AUTO_FOCUS = 0x04,
    MANUAL_ZOOM = 0x05,
    ABSOLUTE_ZOOM = 0x0F,
    MANUAL_FOCUS = 0x06,
    GIMBAL_ROTATION = 0x07,
    GIMBAL_CENTER = 0x08,
    ACQUIRE_GIMBAL_INFO = 0x0a,
    FUNC_FEEDBACK_INFO = 0x0b,
    PHOTO_VIDEO_HDR = 0x0c,
    ACQUIRE_GIMBAL_ATT = 0x0d,
    GIMBAL_CONTROL_ANGLE = 0x0E
};

} // namespace siyi

#endif // COMMAND_H

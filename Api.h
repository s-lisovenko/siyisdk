#ifndef API_H
#define API_H

#include <QObject>
#include <QTimerEvent>

#include "Connection.h"

namespace siyi {

class Api : public QObject
{
    Q_OBJECT

public:
    enum class CameraType {
        ZR10,
        A8Mini,
        A2Mini,
        ZR30,
        ZT30,
        Unknown,
    };

    FirmwareMessage firmwareMessage;
    HardwareIDMessage hardwareIDMessage;
    ManualZoomMessage manualZoomMessage;
    GimbalAttitudeMessage gimbalAttitudeMessage;
    ManualFocusMessage manualFocusMessage;
    CameraStatusInfoMessage cameraStatusInfoMessage;

public:
    // Instance singleton
    static Api& instance() {
        static Api instance;
        return instance;
    }

    ~Api() override;

    /**
     * @brief Check if Siyi API is initialized
     * @return True if Siyi API is initialized, false otherwise
     */
    [[nodiscard]] bool initialized() const {
        return _cameraType != CameraType::Unknown || !hardwareIDMessage.hardwareID.isEmpty();
    }

    /**
     * @brief Set gimbal angles
     * @param pan Pan angle
     * @param tilt Tilt angle
     * @return True if angles were set, false otherwise
     */
    [[nodiscard]] bool setAngles(float pan, float tilt);

    /**
     * @brief Set gimbal center
     * @return True if center was set, false otherwise
     */
    [[nodiscard]] bool setGimbalCenter();

    /**
     * @brief Set gimbal rates
     * @param panRate Pan rate
     * @param tiltRate Tilt rate
     * @return True if rates were set, false otherwise
     */
    [[nodiscard]] bool setRates(float panRate, float tiltRate);

    /**
     * @brief Send take photo message
     * @return True if message was sent, false otherwise
     */
    [[nodiscard]] bool takePhoto();

    /**
     * @brief Send start recording video message
     * @return True if message was sent, false otherwise
     */
    [[nodiscard]] bool toggleRecordingVideo();

    // Zoom messages
    bool zoom(uint8_t zoomValue);
    bool zoomDirection(int8_t direction);

    // Focus message
    bool manualFocus(int8_t direction);

    // Camera type
    [[nodiscard]] CameraType cameraType() const { return _cameraType; };

signals:
    /**
     * Signal about gimbal angles need to update
     */
    void updateGimbalAngles();

protected:
    void timerEvent(QTimerEvent* e) override;

private:
    explicit Api(QObject* parent = nullptr);

    /**
     * @brief Initialize Siyi API
     * @return True if initialization was successful, false otherwise
     */
    void init();

    // Additional message handlers that need to be called after hardware ID message parsing
    void getCameraType();

private slots:
    /**
     * @brief Process SDK message
     * @param message Message
     * @param command Command
     */
    void processSdkMessage(const QVariant& message, quint8 command);

signals:
    /**
     * Send message to camera signal
     * @param message
     */
    void sendMessage(const QByteArray& message);

private:
    Connection* _siyiConnection{nullptr};
    MessageBuilder _messageBuilder;
    int _gimbalAttitudeTimer{-1};
    CameraType _cameraType{CameraType::Unknown};
};

} // namespace siyi

#endif // API_H

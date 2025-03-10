#pragma once

#include <QMap>
#include <QUdpSocket>

#include "MessageBuilder.h"
#include "MessageParser.h"

namespace siyi {

class CommunicationWorker : public QObject {
    Q_OBJECT

public:
    explicit CommunicationWorker(std::shared_ptr<MessageBuilder>& messageBuilder,
                        const QString&                   serverIp = "192.168.144.25",
                        quint16                          port     = 37260,
                        QObject*                         parent   = nullptr);
    ~CommunicationWorker() override;

signals:
    // Emit received message and command
    void messageReceived(const QVariant& message, quint8 command);

public slots:
    /**
     * Init connection
     */
    void init();

    /**
     * Send message to camera
     * @param message Message to send
     */
    void sendMessage(const QByteArray& message);

private slots:
    void readPendingDatagrams();

private:
    /**
     * Add parser
     * @param parser
     */
    void addParser(ResponseMessageParser* parser);

private:
    bool                                  _connected{false};
    std::shared_ptr<MessageBuilder>       _messageBuilder;
    QHostAddress                          _cameraAddress;
    quint16                               _port;
    QUdpSocket*                           _socket{nullptr};
    QMap<Command, ResponseMessageParser*> _parsers;
};

} // namespace siyi

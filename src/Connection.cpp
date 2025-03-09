#include "Connection.h"

#include <QLoggingCategory>

#include "MessageParser.h"

Q_LOGGING_CATEGORY(siyiSdk, "siyi.sdk")

namespace siyi {

Connection::Connection(std::shared_ptr<MessageBuilder>& messageBuilder, const QString& serverIp, quint16 port, QObject* parent)
    : QObject(parent)
    , _messageBuilder(messageBuilder)
    , _cameraAddress(serverIp)
    , _port(port) {
    // Fill parsers map
    addParser(new FirmwareMessageParser);
    addParser(new HardwareIDMessageParser);
    addParser(new AutoFocusMessageParser);
    addParser(new ManualZoomMessageParser);
    addParser(new AbsoluteZoomMessageParser);
    addParser(new ManualFocusMessageParser);
    addParser(new GimbalRotationMessageParser);
    addParser(new GimbalCenterMessageParser);
    addParser(new FunctionFeedbackMessageParser);
    addParser(new GimbalAttitudeMessageParser);
    addParser(new GimbalControlAngleMessageParser);
    addParser(new CameraStatusInfoMessageParser);
}

Connection::~Connection() {
    qDeleteAll(_parsers);
}

void Connection::readPendingDatagrams() {
    while (_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(static_cast<int>(_socket->pendingDatagramSize()));
        _socket->readDatagram(datagram.data(), datagram.size());
        // Decode message
        const auto [data, dataLength, command, sequenceNumber] = _messageBuilder->decode(datagram);
        // Notify about message received only if parser available
        if (_parsers.contains(command)) {
            auto message = _parsers.value(command)->parse(data);
            emit messageReceived(message, static_cast<quint8>(command));
        } else {
            qCWarning(siyiSdk) << "No parser for command" << static_cast<int>(command);
        }
    }
}

void Connection::sendMessage(const QByteArray& message) {
    if (!_connected) {
        qCWarning(siyiSdk) << "Not connected to camera";
        return;
    }
    auto bytesSent = _socket->writeDatagram(message, _cameraAddress, _port);
    if (bytesSent == -1) {
        qCWarning(siyiSdk) << "Failed to send data via UDP.";
    }
}

void Connection::addParser(ResponseMessageParser* parser) {
    _parsers.insert(parser->command(), parser);
}

void Connection::init() {
    _socket = new QUdpSocket(this);
    if (_socket->bind(QHostAddress::Any, _port)) {
        _connected = _socket->state() == QUdpSocket::SocketState::BoundState;
        connect(_socket, &QUdpSocket::readyRead, this, &Connection::readPendingDatagrams);
    } else {
        qCWarning(siyiSdk) << "Failed to bind to port";
    }
}

} // namespace siyi

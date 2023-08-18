#ifndef CRC_H
#define CRC_H

#include <cstdint>
#include <QByteArray>

namespace siyi {

class Crc
{
public:
    [[nodiscard]] static uint16_t calculateCRC16(const QByteArray& data, uint16_t crc_init);
};

} // namespace siyi

#endif // CRC_H

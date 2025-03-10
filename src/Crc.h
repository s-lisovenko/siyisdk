#pragma once

#include <cstdint>

#include <QByteArray>

namespace siyi {

class Crc {
public:
    [[nodiscard]] static uint16_t calculateCRC16(const QByteArray& data, uint16_t crc_init);
};

} // namespace siyi

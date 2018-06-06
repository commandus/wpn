#include <string>
#include "nayuki/QR-Code-generator/BitBuffer.hpp"
#include "nayuki/QR-Code-generator/QrCode.hpp"

using qrcodegen::QrCode;
using qrcodegen::QrSegment;

std::string qr2string(const std::string &value, const bool invert = false);

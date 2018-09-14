#include <sstream>
#include "utilqr.h"

// Prints the given QR Code to the console.
static void qr2stream(
	std::ostream &retval, 
	const QrCode &qr,
	const std::string &foreground,
	const std::string &background
) 
{
	int border = 4;
	for (int y = -border; y < qr.getSize() + border; y++) 
	{
		for (int x = -border; x < qr.getSize() + border; x++) 
		{
			bool b = qr.getModule(x, y);
			retval << (b ? background : foreground);
		}
		retval << std::endl;
	}
	retval << std::endl;
}

std::string qr2string
(
	const std::string &value,
	const std::string &foreground,
	const std::string &background
)
{
	const QrCode qr = QrCode::encodeText(value.c_str(), QrCode::Ecc::LOW);
	std::stringstream ss;
	qr2stream(ss, qr, foreground, background);
	return ss.str();
}

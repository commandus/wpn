#include <sstream>
#include "utilqr.h"

const std::string sym = "\u2588\u2588";
const std::string emp = "  ";

// Prints the given QR Code to the console.
static void qr2stream(
	std::ostream &retval, 
	const QrCode &qr,
	bool invert
) 
{
	int border = 4;
	for (int y = -border; y < qr.getSize() + border; y++) 
	{
		for (int x = -border; x < qr.getSize() + border; x++) 
		{
			bool b = qr.getModule(x, y);
			if (invert)
				b = !b;
			retval << (b ? emp : sym);
		}
		retval << std::endl;
	}
	retval << std::endl;
}

std::string qr2string
(
	const std::string &value,
	const bool invert
)
{
	const QrCode qr = QrCode::encodeText(value.c_str(), QrCode::Ecc::LOW);
	std::stringstream ss;
	qr2stream(ss, qr, invert);
	return ss.str();
}

/*
 * SHT31.cpp
 *
 *  Created on: Mar 2, 2019
 *      Author: pete
 */

#include "sht31.h"

SHT31::SHT31(int bus, int address)
{
	m_bus = bus;
	m_address = address;
	m_handle = 0;
	m_open = false;
}

SHT31::~SHT31()
{
	if (m_open)
		close(m_handle);
}

bool SHT31::deviceOpen()
{
	char bus[32];
	memset (bus, '\0', 32);

	snprintf(bus, 32, "/dev/i2c-%d", m_bus);

	if((m_handle = open(bus, O_RDWR)) < 0)
	{
		std::cerr << __PRETTY_FUNCTION__ << ": Unable to open device at " << bus << std::endl;
		return false;
	}
	// Get I2C device, SHT31 I2C address is 0x44(68)
	ioctl(m_handle, I2C_SLAVE, m_address);

	m_open = true;
	return m_open;
}

void SHT31::deviceClose()
{
	if (m_open)
		close(m_handle);
}

bool SHT31::query(double *tempc, double *tempf, double *humidity)
{
	uint8_t config[2] = {0};
	uint8_t data[6] = {0};
	int count = 0;
	uint16_t tvalue = 0;
	uint16_t hvalue = 0;

	config[0] = 0x2C;
	config[1] = 0x0D;

	if (m_open) {
		write(m_handle, config, 2);

		while (read(m_handle, data, 6) != 6) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			std::cout << ".";
			if (count++ == 10) {
				std::cerr << __PRETTY_FUNCTION__ << ": Unable to read temp data" << std::endl;
				return false;
			}
		}

		if (crcCheck(&data[0], 2, data[2])) {
			tvalue = ((data[0] << 8) | (data[1] & 0xFF));
		}
		if (crcCheck(&data[3], 2, data[5])) {
			hvalue = ((data[3] << 8) | (data[4] & 0xFF));
		}
		*tempc = -45.0 + (175.0 * (tvalue / (2^16 - 1)));
		*tempf = -49.0 + (315.0 * (tvalue / (2^16 - 1)));
		*humidity = 100.0 * (hvalue / (2^16 - 1));

		return true;
	}
	return false;
}

bool SHT31::crcCheck(uint8_t *data, int len, uint8_t csum)
{
	const uint8_t POLYNOMIAL = 0x31;
	uint8_t crc = 0xFF;

	for (int j = len; j; --j ) {
		crc ^= *data++;

		for (int i = 8; i; --i ) {
			crc = ( crc & 0x80 )
				? (crc << 1) ^ POLYNOMIAL
				: (crc << 1);
		}
	}
	if (crc != csum) {
		std::cerr << __PRETTY_FUNCTION__ << ": checksum mismatch. Got " << crc << " but I expected " << csum << std::endl;
	}
	return (crc == csum);
}

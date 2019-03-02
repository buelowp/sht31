/*
 * sht31.cpp
 *
 *  Created on: Mar 2, 2019
 *      Author: pete
 */

#include "sht31.h"

sht31::sht31(int bus, int address)
{
	m_bus = bus;
	m_address = address;
	m_handle = 0;
	m_open = false;
}

sht31::~sht31()
{
	if (m_open)
		close(m_handle);
}

bool sht31::deviceOpen()
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

void sht31::deviceClose()
{
	if (m_open)
		close(m_handle);
}

bool sht31::query(double *tempc, double *tempf, double *humidity)
{
	char config[2] = {0};
	char data[6] = {0};
	int count = 0;

	config[0] = 0x2C;
	config[1] = 0x06;

	if (m_open) {
		write(m_handle, config, 2);

		while (read(m_handle, data, 6) != 6) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			if (count++ == 10) {
				std::cerr << __PRETTY_FUNCTION__ << ": Unable to read temp data";
				return false;
			}
		}

		*tempc = (((data[0] * 256) + data[1]) * 175.0) / 65535.0  - 45.0;
		*tempf = (((data[0] * 256) + data[1]) * 315.0) / 65535.0 - 49.0;
		*humidity = (((data[3] * 256) + data[4])) * 100.0 / 65535.0;

		return true;
	}
	return false;
}






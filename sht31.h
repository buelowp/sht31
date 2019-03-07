/*
 * SHT31.h
 *
 *  Created on: Mar 2, 2019
 *      Author: Peter Buelow
 */

#ifndef SHT31_H_
#define SHT31_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

class SHT31 {
public:
	SHT31(int, int);
	virtual ~SHT31();

	bool deviceOpen();
	void deviceClose();
	bool isOpen();
	bool query(double*, double*, double*);

private:
	bool crcCheck(uint8_t*, int, uint8_t);
	int m_bus;
	int m_address;
	int m_handle;
	bool m_open;
};

#endif /* SHT31_H_ */

/*
 * sht31.h
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

class sht31 {
public:
	sht31(int, int);
	virtual ~sht31();

	bool deviceOpen();
	void deviceClose();
	bool isOpen();
	bool query(double*, double*, double*);

private:
	int m_bus;
	int m_address;
	int m_handle;
	bool m_open;
};

#endif /* SHT31_H_ */

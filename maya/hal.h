/*
 * hardware abstract layer
 *
 * Copyright (c) 2015, longfeng.xiao <xlongfeng@126.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __HAL_H__
#define __HAL_H__

#define APM_AC_OFFLINE			0
#define APM_AC_ONLINE			1
#define APM_AC_BACKUP			2
#define APM_AC_UNKNOWN			0xff

#define APM_BATTERY_STATUS_HIGH		0
#define APM_BATTERY_STATUS_LOW		1
#define APM_BATTERY_STATUS_CRITICAL	2
#define APM_BATTERY_STATUS_CHARGING	3
#define APM_BATTERY_STATUS_NOT_PRESENT	4
#define APM_BATTERY_STATUS_UNKNOWN	0xff

#define APM_BATTERY_FLAG_HIGH		(1 << 0)
#define APM_BATTERY_FLAG_LOW		(1 << 1)
#define APM_BATTERY_FLAG_CRITICAL	(1 << 2)
#define APM_BATTERY_FLAG_CHARGING	(1 << 3)
#define APM_BATTERY_FLAG_NOT_PRESENT	(1 << 7)
#define APM_BATTERY_FLAG_UNKNOWN	0xff

#define APM_UNITS_MINS			0
#define APM_UNITS_SECS			1
#define APM_UNITS_UNKNOWN		-1

class Hal
{
public:
	static Hal *instance();

	void setLcdBacklight(int brightness);
	int getLcdBacklight(void);

	void setAlarmBlueLed(bool state);
	void setAlarmRedLed(bool state);
	void setAlarmYellowLed(bool state);

	int ecgStart();
	int ecgStop();
	int ecgOut(short *data, int len);

	int saveEeprom(const char *data, int len);
	int loadEeprom(char *data, int len);

	void apmPowerStatus(int &acLineStatus, int &batteryStatus, int &batteryLife);

	void suspend();
	void resume();
	void poweroff();
	void setNurseCall(bool value);
	void setBootIBP(bool value);
	void setWDTIBP(bool value);
	void setBootF0(bool value);
	void setWDTF0(bool value);
	void setBootF1(bool value);
	void setWDTF1(bool value);

    const char *machine();
    const char *bootVersion();
    const char *kernelVersion();
    const char *rootVersion();

private:
	Hal();
	Hal(const Hal &);
	Hal &operator=(const Hal &);
	int puts(const char *file, const char *str);
	int gets(const char *file, char *str, int len);
	int putb(const char *file, const char *buf, int len);
	int getb(const char *file, char *buf, int len);

private:
	static Hal *self;

};

#endif /* __HAL_H__ */

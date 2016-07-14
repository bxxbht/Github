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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "hal.h"

static const char *pwm_backlight_2 = "/sys/class/backlight/pwm-backlight.2/brightness";
static const char *pwm_backlight_3 = "/sys/class/backlight/pwm-backlight.3/brightness";

static const char *alarm_blue_led = "/sys/class/leds/alarm-blue-led/brightness";
static const char *alarm_red_led = "/sys/class/leds/alarm-red-led/brightness";
static const char *alarm_yellow_led = "/sys/class/leds/alarm-yellow-led/brightness";

static const char *dac7311_start = "/sys/devices/platform/dac7311.3/start";
static const char *dac7311_stop = "/sys/devices/platform/dac7311.3/stop";
static const char *dac7311_output = "/sys/devices/platform/dac7311.3/output";

static const char *eeprom = "/sys/bus/i2c/drivers/at24/3-0050/eeprom";

static const char *apm = "/proc/apm";

static const char *tps65910_dev_off = "/sys/devices/platform/imx-i2c.2/i2c-2/2-002d/dev_off";

static const char *nurse_call_output= "/sys/class/gpio/gpio54/value";
static const char *running_state= "/sys/class/gpio/gpio67/value";
static const char *boot_ibp = "/sys/class/gpio/gpio74/value";
static const char *wdt_ibp = "/sys/class/gpio/gpio75/value";
static const char *boot_f0 = "/sys/class/gpio/gpio77/value";
static const char *wdt_f0 = "/sys/class/gpio/gpio76/value";
static const char *boot_f1 = "/sys/class/gpio/gpio79/value";
static const char *wdt_f1 = "/sys/class/gpio/gpio78/value";

Hal *Hal::self = NULL;

Hal::Hal()
{

}

Hal *Hal::instance()
{
	if (!self) {
		self = new Hal();
	}
	return self;
}

void Hal::setLcdBacklight(int brightness)
{
	char str[32];

	if (brightness < 0)
		brightness = 0;
	if (brightness > 248)
		brightness = 248;

	sprintf(str, "%d", brightness);

	puts(pwm_backlight_2, str);
	puts(pwm_backlight_3, str);
}

int Hal::getLcdBacklight(void)
{
	char str[32];

	int ret = gets(pwm_backlight_2, str, sizeof(str));

	if (ret > 0) {
		return atoi(str);
	}

	return -1;
}

void Hal::setAlarmBlueLed(bool state)
{
	puts(alarm_blue_led, state ? "255" : "0"); 
}

void Hal::setAlarmRedLed(bool state)
{
	puts(alarm_red_led, state ? "255" : "0"); 
}

void Hal::setAlarmYellowLed(bool state)
{
	puts(alarm_yellow_led, state ? "255" : "0"); 
}

int Hal::ecgStart()
{
	return puts(dac7311_start, "1");
}

int Hal::ecgStop()
{
	return puts(dac7311_stop, "1");
}

int Hal::ecgOut(short *data, int len)
{
	return putb(dac7311_output, (char *)data, len * 2);
}

int Hal::saveEeprom(const char *data, int len)
{
	return putb(eeprom, (char *)data, len);
}

int Hal::loadEeprom(char *data, int len)
{
	return getb(eeprom, data, len);
}


void Hal::apmPowerStatus(int &acLineStatus, int &batteryStatus, int &batteryLife)
{
	char str[64];
	char *p;

	acLineStatus = APM_AC_UNKNOWN;
	batteryStatus = APM_BATTERY_STATUS_UNKNOWN;
	batteryLife = -1;

	if (gets(apm, str, sizeof(str)) < 0)
		return;

	p = str;

	// 1.2
	p = strchr(p, ' ');
	p++;

	// APM_32_BIT_SUPPORT
	p = strchr(p, ' ');
	p++;

	// ac_line_status
	p = strchr(p, ' ');
	p++;
	acLineStatus = strtoul(p, NULL, 0);
	printf(p);

	// battery_status
	p = strchr(p, ' ');
	p++;
	batteryStatus = strtoul(p, NULL, 0);

	// battery_flag
	p = strchr(p, ' ');
	p++;

	// battery_life
	p = strchr(p, ' ');
	p++;
	batteryLife = strtoul(p, NULL, 0);
}

void Hal::suspend()
{
	puts(running_state, "0");
}

void Hal::resume()
{
	puts(running_state, "1");
}

void Hal::poweroff()
{
	puts(tps65910_dev_off, "1");
}

void Hal::setNurseCall(bool value)
{
	puts(nurse_call_output, value ? "1" : "0");
}

void Hal::setBootIBP(bool value)
{
	puts(boot_ibp, value ? "1" : "0");
}

void Hal::setWDTIBP(bool value)
{
	puts(wdt_ibp, value ? "1" : "0");
}

void Hal::setBootF0(bool value)
{
	puts(boot_f0, value ? "1" : "0");
}

void Hal::setWDTF0(bool value)
{
	puts(wdt_f0, value ? "1" : "0");
}

void Hal::setBootF1(bool value)
{
	puts(boot_f1, value ? "1" : "0");
}

void Hal::setWDTF1(bool value)
{
	puts(wdt_f1, value ? "1" : "0");
}

const char *Hal::machine()
{
    static char mach[64];
    char buf[256];

    memset(buf, 0, sizeof(buf));

    if (gets("/proc/cmdline", buf, sizeof(buf)) < 0)
        return NULL;

    char *start = strstr(buf, "machine=");
    if (start) {
        start += strlen("machine=");
        char *end = strstr(start, " ");
        strncpy(mach, start, end - start);
    }

    return mach;

}

const char *Hal::bootVersion()
{
    static char ver[64];
    char buf[256];

    memset(buf, 0, sizeof(buf));

    if (gets("/proc/cmdline", buf, sizeof(buf)) < 0)
        return NULL;

    char *start = strstr(buf, "u_boot_version=");
    if (start) {
        start += strlen("u_boot_version=");
        char *end = strstr(start, " ");
        strncpy(ver, start, end - start);
    }

    return ver;
}

const char *Hal::kernelVersion()
{
    static char ver[64];
    char buf[256];

    memset(buf, 0, sizeof(buf));

    if (gets("/proc/version", buf, sizeof(buf)) < 0)
        return NULL;

    char *start = strstr(buf, "-");
    if (start) {
        start += strlen("-");
        char *end = strstr(start, "-");
        end += 1;
        end = strstr(end, "-");
        strncpy(ver, start, end - start);
    }

    return ver;
}

const char *Hal::rootVersion()
{
    static char ver[64];
    char buf[256];

    memset(buf, 0, sizeof(buf));

    if (gets("/etc/version", buf, sizeof(buf)) < 0)
        return NULL;

    char *p = ver;
    int i = 0;
    char ch;
    while ((ch = buf[i++]) != '\0') {
        if (ch != ' ' && ch != '\r' && ch != '\n' && ch != '\t')
            *p++ = ch;
    }

    return ver;
}

int Hal::puts(const char *file, const char *str)
{
	return putb(file, str, strlen(str));
}

int Hal::gets(const char *file, char *str, int len)
{
	return getb(file, str, len);
}

int Hal::putb(const char *file, const char *buf, int len)
{
	if (!file || len <= 0)
		return -1;

	int fd = open(file, O_WRONLY);
	if (fd < 0)
		return -1;

	len = write(fd, buf, len);

	close(fd);

	return len;
}

int Hal::getb(const char *file, char *buf, int len)
{
	if (!file || len <= 0)
		return -1;

	int fd = open(file, O_RDONLY);
	if (fd < 0)
		return -1;

	len = read(fd, buf, len);

	close(fd);

	return len;
}

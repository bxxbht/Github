#ifndef _LINUX_CRC_CCITT_H
#define _LINUX_CRC_CCITT_H

#include <QtGlobal>

extern quint16 const crc_ccitt_table[256];

extern quint16 crc_ccitt(quint16 crc, const uchar *buffer, size_t len);

static inline quint16 crc_ccitt_byte(quint16 crc, const uchar c)
{
	return (crc >> 8) ^ crc_ccitt_table[(crc ^ c) & 0xff];
}

#endif /* _LINUX_CRC_CCITT_H */

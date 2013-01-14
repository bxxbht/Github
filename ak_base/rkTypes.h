
#ifndef RKTYPES_H_2002_03_12
#define RKTYPES_H_2002_03_12

#include <sys/types.h>
#include <errno.h>

typedef unsigned long       DWORD;	//双字型(32位无符号)
typedef unsigned char       BYTE;	//字节型
typedef unsigned short      WORD;	//单字型
typedef float               FLOAT;	//浮点型
typedef unsigned int		UINT;	//无符号整型
typedef UINT				WPARAM;	//W参数
typedef DWORD				LPARAM;	//L参数

/*
	以下的两种长度类型用于表示缓冲区容量和字符串长度
	SIZE型变量表示缓冲区字节容量,是一个缓冲区的实际大小
	LEN型变量表示一个字符串的长度,不包含字符串结尾的NULL字符(即其值+1才是该字符串占用的缓冲区容量)
*/
typedef UINT SIZE;					//缓冲区字节容量
typedef UINT LEN;					//字符串长度


typedef void* HANDLE;				//句柄
typedef void* PVOID;				//空指针

typedef long				LONG;	//有符号长整型
typedef long*				PLONG;	//有符号长整型指针
typedef unsigned long		ULONG;	//无符号长整型
typedef ULONG*				PULONG;	//无符号长整型指针
typedef unsigned short		USHORT;	//无符号短整型
typedef USHORT*				PUSHORT;//无符号短整型指针
typedef unsigned char		UCHAR;	//无符号字符型
typedef UCHAR*				PUCHAR;	//无符号字符型指针
typedef char*				PSZ;	//字符串指针

#define SUCCESS 0

#endif// #ifndef RKTYPES_H_2002_03_12

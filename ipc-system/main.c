/*
 * ipc system
 *
 * Copyright (C) 2012 GoldWay INC.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/time.h>

#define IPC_SYSTEM_MSG_KEY 	3845
#define BUF_SIZE		256

enum {
	IPC_SYSTEM_NONE,
	IPC_SYSTEM_ACK,
	IPC_SYSTEM_NOSYNC,
	IPC_SYSTEM_SYNC,
};

struct ipc_system_t {
	long int msg_type;
	int flag;
	char cmd[BUF_SIZE];
};

static int ipcSystemMsgInitialed;
static int ipcSystemMsgClientId;

static int ipcSystemMsgClientInit(void)
{
	ipcSystemMsgInitialed = 1;
	ipcSystemMsgClientId = msgget((key_t)IPC_SYSTEM_MSG_KEY, 0666);
	if (ipcSystemMsgClientId == -1) {
		ipcSystemMsgInitialed = 0;
		fprintf(stderr, "Can't create ipc msg from system <%d>\n", __LINE__);
	}
	return 0;
}

static int ipcSystem(char *cmd)
{
	struct ipc_system_t ipcSystemMsg;
	int len;

	if (!cmd)
		return -1;

	if (!ipcSystemMsgInitialed) {
		fprintf(stderr, "!!! IPC system msg is not initialed\n");
		return system(cmd);
	}

	memset(&ipcSystemMsg, 0, sizeof(struct ipc_system_t));

	len = strlen(cmd);
	if (len > (BUF_SIZE - 1)) {
		fprintf(stderr, "!!!CMD <%s>  is too long\n", cmd);
		return -1;
	}

	while (msgrcv(ipcSystemMsgClientId, (void *)&ipcSystemMsg,
		sizeof(struct ipc_system_t) - sizeof(long int), 0, IPC_NOWAIT) > 0);

	ipcSystemMsg.msg_type = 'c'; 
	ipcSystemMsg.flag = strrchr(cmd, '&') ? IPC_SYSTEM_NOSYNC : IPC_SYSTEM_SYNC;

	strcpy(ipcSystemMsg.cmd, cmd);

	if (ipcSystemMsg.flag == IPC_SYSTEM_NOSYNC) {
		if (msgsnd(ipcSystemMsgClientId, (void *)&ipcSystemMsg,
			sizeof(struct ipc_system_t) - sizeof(long int), 0) == -1)
			fprintf(stderr, "!!! Send ipc message failed, %s:%d\n", __func__, __LINE__);
	} else {
		int timeout = 20 * 10;

		if (msgsnd(ipcSystemMsgClientId, (void *)&ipcSystemMsg,
			sizeof(struct ipc_system_t) - sizeof(long int), 0) == -1) {
			fprintf(stderr, "!!! Send ipc message failed, %s:%d\n", __func__, __LINE__);
			return -1;
		}

		while (msgrcv(ipcSystemMsgClientId, (void *)&ipcSystemMsg,
			sizeof(struct ipc_system_t) - sizeof(long int), 's', IPC_NOWAIT) == -1) {
			usleep(1000*50);
			if (timeout-- == 0) {
				fprintf(stderr, "!!! Wait ipc message sync failed, %s:%d\n", __func__, __LINE__);
				return -1;
			}
		}
	}

	return ipcSystemMsg.flag;
}

/* -------------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
	if (argc == 2) {
		ipcSystemMsgClientInit();
		return ipcSystem(argv[1]);
	}

	int ipcSystemMsgServerId;
	struct ipc_system_t ipcSystemMsg;
	char *cmd;
	int rc;

	ipcSystemMsgServerId = msgget((key_t)IPC_SYSTEM_MSG_KEY, 0666 | IPC_CREAT);

	if (ipcSystemMsgServerId == -1) {
		fprintf(stderr, "Can't create ipc msg from system <%d>\n", __LINE__);
		return 1;
	}

	while (1) {
		if (msgrcv(ipcSystemMsgServerId, (void *)&ipcSystemMsg,
			sizeof(struct ipc_system_t) - sizeof(long int), 'c', 0) == -1)
			continue;

		rc = system(ipcSystemMsg.cmd);

		if (ipcSystemMsg.flag == IPC_SYSTEM_SYNC) {
			ipcSystemMsg.msg_type = 's';
			ipcSystemMsg.flag = rc;
			if (msgsnd(ipcSystemMsgServerId, (void *)&ipcSystemMsg,
				sizeof(struct ipc_system_t) - sizeof(long int), 0) == -1)
				fprintf(stderr, "!!! Send ipc message ack failed, %s:%d\n", __func__, __LINE__);
		}
	}

	return 0;
}

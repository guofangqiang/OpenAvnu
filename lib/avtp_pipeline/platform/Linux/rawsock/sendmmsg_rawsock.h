/*************************************************************************************************************
Copyright (c) 2012-2015, Symphony Teleca Corporation, a Harman International Industries, Incorporated company
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS LISTED "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS LISTED BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Attributions: The inih library portion of the source code is licensed from
Brush Technology and Ben Hoyt - Copyright (c) 2009, Brush Technology and Copyright (c) 2009, Ben Hoyt.
Complete license and copyright information can be found at
https://github.com/benhoyt/inih/commit/74d2ca064fb293bc60a77b0bd068075b293cf175.
*************************************************************************************************************/

#ifndef SENDMMSG_RAWSOCK_H
#define SENDMMSG_RAWSOCK_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/socket.h>

#include "rawsock_impl.h"

#define MSG_COUNT 8
#define MAX_FRAME_SIZE 1024
#define USE_LAUNCHTIME 0


// State information for raw socket
//
typedef struct {
	base_rawsock_t base;

	// the underlying socket
	int sock;

	// count of total buffers available for messages
	int frameCount;

	// count of buffers taken by senders
	int buffersOut;

	// count of buffers ready to send
	int buffersReady;

	// buffer for receiving frames
	U8 rxBuffer[1518];

	struct mmsghdr mmsg[MSG_COUNT];

	struct iovec miov[MSG_COUNT];

	unsigned char pktbuf[MSG_COUNT][MAX_FRAME_SIZE];
#if USE_LAUNCHTIME
	unsigned char cmsgbuf[MSG_COUNT][CMSG_SPACE(sizeof(uint64_t))];
#endif
} sendmmsg_rawsock_t;

// Open a rawsock for TX or RX
void* sendmmsgRawsockOpen(sendmmsg_rawsock_t *rawsock, const char *ifname, bool rx_mode, bool tx_mode, U16 ethertype, U32 frame_size, U32 num_frames);

// Close the rawsock
void sendmmsgRawsockClose(void *pvRawsock);

// Get a buffer from the simple to use for TX
U8* sendmmsgRawsockGetTxFrame(void *pvRawsock, bool blocking, unsigned int *len);

// Set the Firewall MARK on the socket
// The mark is used by FQTSS to identify AVTP packets in kernel.
// FQTSS creates a mark that includes the AVB class and stream index.
bool sendmmsgRawsockTxSetMark(void *pvRawsock, int mark);

// Pre-set the ethernet header information that will be used on TX frames
bool sendmmsgRawsockTxSetHdr(void *pvRawsock, hdr_info_t *pHdr);

// Release a TX frame, and mark it as ready to send
bool sendmmsgRawsockTxFrameReady(void *pvRawsock, U8 *pBuffer, unsigned int len, U64 timeNsec);

// Send all packets that are ready (i.e. tell kernel to send them)
int sendmmsgRawsockSend(void *pvRawsock);

// Get a RX frame
U8* sendmmsgRawsockGetRxFrame(void *pvRawsock, U32 timeout, unsigned int *offset, unsigned int *len);

// Setup the rawsock to receive multicast packets
bool sendmmsgRawsockRxMulticast(void *pvRawsock, bool add_membership, const U8 addr[ETH_ALEN]);

// Get the socket used for this rawsock; can be used for poll/select
int  sendmmsgRawsockGetSocket(void *pvRawsock);

#endif

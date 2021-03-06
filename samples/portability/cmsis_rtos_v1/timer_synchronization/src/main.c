/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Synchronization demo using CMSIS RTOS V1 APIs.
 */

#include <zephyr.h>
#include <cmsis_os.h>

/* specify delay between greetings (in ms); compute equivalent in ticks */
#define TIMER_TICKS  50
#define STACKSIZE  512
#define MSGLEN 12
#define Q_LEN 1
#define INITIAL_DATA_VALUE 5

u32_t data;

void read_msg_callback(void const *arg);

osTimerDef(myTimer, read_msg_callback);

osMessageQDef(messageq, Q_LEN, u32_t);
osMessageQId message_id;

void read_msg_callback(void const *arg)
{
	osEvent evt;
	u32_t read_msg = 0;

	evt = osMessageGet(message_id, 0);
	read_msg = evt.value.v;
	if (read_msg == 0) {
		printk("\n**Error reading message from message queue**\n");
	} else {
		printk("Read from message queue: %d\n\n", read_msg);
	}
}

void send_msg_thread(void)
{
	osStatus status;

	status = osMessagePut(message_id, data, osWaitForever);
	if (status != osOK) {
		printk("\n**Error sending message to message queue**\n");
	} else {
		printk("Wrote to message queue: %d\n", data);
	}
}

void main(void)
{
	osTimerId timer_id;
	u32_t counter = 10;

	data = INITIAL_DATA_VALUE;
	timer_id = osTimerCreate(osTimer(myTimer), osTimerPeriodic, NULL);

	message_id = osMessageCreate(osMessageQ(messageq), NULL);

	/* Send first message */
	send_msg_thread();
	osTimerStart(timer_id, TIMER_TICKS);

	while (--counter) {
		data++;
		send_msg_thread();
	}
	osDelay(TIMER_TICKS);

	osTimerStop(timer_id);
	osTimerDelete(timer_id);

	if (counter == 0) {
		printk("Sample execution successful\n");
	} else {
		printk("Error in execution\n");
	}
}

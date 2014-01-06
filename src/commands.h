#ifndef COMMANDS_H
#define COMMANDS_H

// Commands to be run on the phone
typedef enum {
	CMD_GET_WEATHER = 0 /* Get weather data*/,
	CMD_SEND_ALERT /* Alert to phone */,
	CMD_SEND_SMS /* Send SMS */,
	CMD_SEND_ALERT_TO_WATCH,
	CMD_GET_SMS_RECIPIENT /* Have phone send watch the SMS recipient */,
	NOOP /* Stay Frosty */,
	NUM_COMMANDS
} command_t;

#endif

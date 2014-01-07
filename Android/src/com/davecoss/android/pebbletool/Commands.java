package com.davecoss.android.pebbletool;

import android.content.Context;
import android.content.Intent;

public class Commands {

// Commands 
    public static final String ANDROID_APP_GET_LAST_WEATHER = "LASTWEATHER";
    public static final String ANDROID_APP_COMMAND = "COMMAND";
    public static final String ANDROID_APP_SMS_RECIPIENT = "SMSRECIPIENT";
    public static final String COMMAND_ARG_TYPE = "ARG";
    public enum CommandCode {SEND_WEATHER_DATA,
	    SEND_ALERT_TO_PHONE,
	    SEND_SMS,
	    SEND_ALERT_TO_WATCH,
	    SEND_SMS_RECIPIENT,
	    NOOP,
	    NUM_COMMANDS};
 
    
    public static Intent createIntent(Context context, Class _class, CommandCode code, String cmd_args) {
		Intent intent = new Intent(context, _class);
		intent.putExtra(ANDROID_APP_COMMAND, code.ordinal());
		if(cmd_args != null)
		    intent.putExtra(COMMAND_ARG_TYPE, cmd_args);
		return intent;
    }

    public static Intent createIntent(Context context, Class _class, String commandString, String cmd_args) {
    	Intent intent = new Intent(context, _class);
		intent.putExtra(commandString, cmd_args);
		return intent;
    }

    public static Intent createIntent(Context context, Class _class, CommandCode code) {
    	return createIntent(context, _class, code, null);
    }
    
   
    public static CommandCode getCommandCode(int intcode) {
		if(intcode == -1 || intcode >= CommandCode.values().length)
		    return null;
		
		return CommandCode.values()[intcode];
    }

    public static CommandCode getCommandCode(Intent intent) {
		int idx = intent.getIntExtra(ANDROID_APP_COMMAND, -1);
		return getCommandCode(idx);
    }
   
}
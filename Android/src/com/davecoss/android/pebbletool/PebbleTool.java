package com.davecoss.android.pebbletool;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.davecoss.android.lib.Notifier;

import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;

public class PebbleTool extends Activity {
	protected Notifier notifier;
    
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_pebble_tool);
		
		notifier = new Notifier(getApplicationContext());
	}
	
	@Override
    protected void onResume() {
        super.onResume();
    }

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.pebble_tool, menu);
		return true;
	}
	
	public void updateWeather(View view) {
		try {
			Intent intent = new Intent(this, PebbleService.class);
			intent.putExtra(PebbleService.ANDROID_APP_COMMAND, PebbleService.Commands.SEND_WEATHER_DATA.ordinal());
			startService(intent);
		} catch(RuntimeException re) {
			notifier.log_exception("PebbleTool.onCreate", re.getMessage(), re);
		}
	}
	
	public void setMsgText(String msg) {
		EditText msgbox = (EditText)findViewById(R.id.txtMessage);
		if(msgbox == null)
			return;
		msgbox.setText(msg);
	}
	
	public void sendMsgClick(View btn) {
		EditText msgbox = (EditText)findViewById(R.id.txtMessage);
		if(msgbox == null)
			return;
		String msg = msgbox.getText().toString();
		if(msg == null)
			return;
		msg = msg.trim();
		if(msg.length() == 0)
			return;
		Intent intent = new Intent(this, PebbleService.class);
		intent.putExtra(PebbleService.ANDROID_APP_COMMAND, PebbleService.Commands.SEND_ALERT_TO_WATCH.ordinal());
		intent.putExtra(PebbleService.COMMAND_ARG_TYPE, msg);
		startService(intent);
	}

}

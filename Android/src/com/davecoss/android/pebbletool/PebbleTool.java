package com.davecoss.android.pebbletool;

import java.text.DateFormat;
import java.util.Date;
import com.davecoss.android.lib.Notifier;

import android.os.Bundle;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

public class PebbleTool extends Activity {
	protected Notifier notifier;
	
	public static final String PEBBLE_TOOL_RECEIVER = "Pebble Tool Receiver";
    
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_pebble_tool);
		
		notifier = new Notifier(getApplicationContext());
		

		TextView weatherUpdate = (TextView)findViewById(R.id.txtLastUpdate);
		weatherUpdate.setText("");
	        
	}
	
	@Override
    protected void onResume() {
        super.onResume();
		 // Register Receiver for listening to the Service.
		LocalBroadcastManager.getInstance(this).registerReceiver(serviceReceiver,
				new IntentFilter(PEBBLE_TOOL_RECEIVER));
		showLastWeatherData();

		// Ensure that if the Activity has been started, the service has been started also.
		Intent wakeUp = Commands.createIntent(this, PebbleService.class, Commands.CommandCode.NOOP);
		startService(wakeUp);
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		// Unregister service receiver
		LocalBroadcastManager.getInstance(this).unregisterReceiver(serviceReceiver);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.pebble_tool, menu);
		return true;
	}
	
	public void updateWeather(View view) {
		try {
		    Intent intent = Commands.createIntent(this, PebbleService.class, Commands.CommandCode.SEND_WEATHER_DATA);
		    startService(intent);
		    showLastWeatherData();
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
		Intent intent = Commands.createIntent(this, PebbleService.class, PebbleService.Commands.SEND_ALERT_TO_WATCH.ordinal(), msg);
		startService(intent);
	}
	
	private void showLastWeatherData() {
	    Intent intent = Commands.createIntent(this, PebbleService.class,Commands.CommandCode.ANDROID_APP_GET_LAST_WEATHER, "");
	    startService(intent);
	}

	// handler for received Intents for the "my-event" event 
	private BroadcastReceiver serviceReceiver = new BroadcastReceiver() {
	  @Override
	  public void onReceive(Context context, Intent intent) {
		Log.d("PebbleTool.serviceReceiver", "Received message");
		if(!intent.hasExtra("temperature"))
			return;
		double temp = intent.getDoubleExtra("temperature", -42);
		
		Date lastUpdateDate = null;
		
		try {
			lastUpdateDate = (Date) intent.getSerializableExtra("timestamp");
		} catch(ClassCastException cce) {
			lastUpdateDate = null;
			Log.e("PebbleTool.serviceReceiver.onReceive", "Could not cast serializable to Date", cce);
		}

		String weatherData = Double.toString(temp) + " deg F ";
		if(lastUpdateDate != null)
		{
			DateFormat df = DateFormat.getDateTimeInstance();
			weatherData += "\n" + df.format(lastUpdateDate);
		}
		
		if(weatherData.length() != 0) {
			TextView tv = (TextView)findViewById(R.id.txtLastUpdate);
			tv.setText(weatherData);
		}
	  }
	};

}

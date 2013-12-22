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
import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

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
	// the tuple key corresponding to the weather icon displayed on the watch
    private static final int ICON_KEY = 0;
    // the tuple key corresponding to the temperature displayed on the watch
    private static final int TEMP_KEY = 1;
    // This UUID identifies the weather app
    private static final UUID WEATHER_UUID = UUID.fromString("4D4A4DC2-2077-4AF6-B3DA-60DB398932F3");

    public enum WeatherType { CLEAR, RAIN, SNOW, PARTLY_CLOUDY, MOSTLY_CLOUDY};
    
    private PebbleKit.PebbleDataReceiver msgHandler = null;
    
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
        final Handler handler = new Handler();

        // To receive data back from the sports watch-app, Android
        // applications must register a "DataReceiver" to operate on the
        // dictionaries received from the watch.
        //
        // In this example, we're registering a receiver to listen for
        // changes in the activity state sent from the watch, allowing
        // us the pause/resume the activity when the user presses a
        // button in the watch-app.
        msgHandler = new PebbleKit.PebbleDataReceiver(WEATHER_UUID) {
            @Override
            public void receiveData(Context context, int transactionId, PebbleDictionary data) {
            	
            	String msg = data.getString(0);
            	Long cmd = data.getInteger(1);
                
                PebbleKit.sendAckToPebble(context, transactionId);
                if(msg != null) {
	                PebbleTool.this.setMsgText(msg);
	                sendAlertToPebble("Pong");
                }
                if(cmd != null) {
                	doWeatherUpdate();
                }
            }
        };
        PebbleKit.registerReceivedDataHandler(this, msgHandler);
    }

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.pebble_tool, menu);
		return true;
	}
	
	public void updateWeather(View view) {
		try {
			doWeatherUpdate();
		} catch(RuntimeException re) {
			notifier.log_exception("PebbleTool.onCreate", re.getMessage(), re);
		}
	}
	
	public void doWeatherUpdate() {
       try {
        	String url = String.format("http://api.wunderground.com/api/%s/conditions/q/%s/%s.json",
        			getString(R.string.APIKEY),
                    getString(R.string.STATE),
                    getString(R.string.CITY));
        	URL u = new URL(url);

        	double temperature = 0;
        	WeatherType wtype = WeatherType.CLEAR;
            HttpURLConnection urlConnection = (HttpURLConnection) u.openConnection();
            try {
                BufferedReader reader =
                        new BufferedReader(new InputStreamReader(urlConnection.getInputStream(), "UTF-8"));
                String json = "";
                String line = null;
                while((line = reader.readLine()) != null) {
                	json += line + "\n";
                }
                

                JSONObject jsonObject = new JSONObject(json);
                JSONObject obs = jsonObject.getJSONObject("current_observation");
                temperature = obs.getDouble("temp_f");
                try {
                	wtype = WeatherType.valueOf(obs.getString("weather").toUpperCase());
                } catch(IllegalArgumentException iae) {
                	wtype = WeatherType.CLEAR;
                }
            } catch(Exception e) {
            	notifier.log_exception("PebbleTool", "Weather Retrieval Error: " + e.getMessage() + " from " + url , e);
            	temperature = -1.0;
            	wtype = WeatherType.CLEAR;
            } finally {
                urlConnection.disconnect();
            }
            
            int weatherIcon = getIconFromWeatherId(wtype);
            int temp = (int) (temperature);

            sendWeatherDataToWatch(weatherIcon, temp);
        	
           notifier.toast_message("Weather: " + String.format("%s %f", wtype.toString(), temperature));
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
	
	public void sendWeatherDataToWatch(int weatherIconId, int temperatureCelsius) {
        // Build up a Pebble dictionary containing the weather icon and the current temperature in degrees celsius
        PebbleDictionary data = new PebbleDictionary();
        data.addUint8(ICON_KEY, (byte) weatherIconId);
        data.addString(TEMP_KEY, String.format("%d\u00B0C", temperatureCelsius));

        // Send the assembled dictionary to the weather watch-app; this is a no-op if the app isn't running or is not
        // installed
        PebbleKit.sendDataToPebble(getApplicationContext(), WEATHER_UUID, data);
    }

	
	private int getIconFromWeatherId(WeatherType weather) {
        switch(weather) {
        case SNOW:
        	return 3;
        case RAIN:
        	return 2;
        case PARTLY_CLOUDY: case MOSTLY_CLOUDY:
        	return 1;
        case CLEAR: default:
        	return 0;
        }
    }
	
	public void sendAlertToPebble(String msg) {
		if(msg == null)
			return;
		
	    final Intent i = new Intent("com.getpebble.action.SEND_NOTIFICATION");

	    final Map<String, String> data = new HashMap<String, String>();
	    data.put("title", "Android Message");
	    data.put("body", msg);
	    final JSONObject jsonData = new JSONObject(data);
	    final String notificationData = new JSONArray().put(jsonData).toString();

	    i.putExtra("messageType", "PEBBLE_ALERT");
	    i.putExtra("sender", "MyAndroidApp");
	    i.putExtra("notificationData", notificationData);

	    sendBroadcast(i);
	    notifier.toast_message("Message sent to pebble.");
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
		sendAlertToPebble(msg);
	}

}

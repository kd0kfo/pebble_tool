package com.davecoss.android.pebbletool;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import android.annotation.SuppressLint;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.TaskStackBuilder;
import android.support.v4.content.LocalBroadcastManager;
import android.telephony.SmsManager;
import android.util.Log;

public class PebbleService extends Service {
    	IBinder binder = new PebbleBinder();
	
    // the tuple key corresponding to the weather icon displayed on the watch
    private static final int ICON_KEY = 0;
    // the tuple key corresponding to the temperature displayed on the watch
    private static final int TEMP_KEY = 1;
    // This UUID identifies the weather app
    private static final UUID WEATHER_UUID = UUID.fromString("4D4A4DC2-2077-4AF6-B3DA-60DB398932F3");

    protected static final int NOTIFIER_ID = 0;
    
    private static final String SMS_SENT_RECEIVER = "PEBBLESERVICE_SMS_SENT";

    public enum WeatherType { CLEAR, RAIN, SNOW, PARTLY_CLOUDY, MOSTLY_CLOUDY, OVERCAST};
    
    private PebbleKit.PebbleDataReceiver msgHandler = null;
    
    protected String lastMessageReceived = "";
    protected WeatherData lastWeatherData = null;
    protected String SMSRecipient = null;
    
    @Override
	public void onCreate() {
    	// To receive data back from the sports watch-app, Android
        // applications must register a "DataReceiver" to operate on the
        // dictionaries received from the watch.
        //
        // In this example, we're registering a receiver to listen for
        // changes in the activity state sent from the watch, allowing
        // us the pause/resume the activity when the user presses a
        // button in the watch-app.
    	if(msgHandler == null)
    	{
    		msgHandler = createMsgHandlerInstance();
    		PebbleKit.registerReceivedDataHandler(this, msgHandler);
    	}

	// Let the watch know we're here.
    	if(msgHandler != null)
	    sendAlertToPebble("Pebble Service Started");
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
    	
    	if(intent.hasExtra(Commands.ANDROID_APP_GET_LAST_WEATHER))
    	{
    		Intent replyIntent = new Intent(PebbleTool.PEBBLE_TOOL_RECEIVER);
    		if(lastWeatherData != null){
    			replyIntent.putExtra("temperature", lastWeatherData.temperature);
    			replyIntent.putExtra("timestamp", lastWeatherData.measureDateTime);
    		  LocalBroadcastManager.getInstance(this).sendBroadcast(replyIntent);
    		}
    	}

    	Commands.CommandCode cmd = Commands.getCommandCode(intent);
    	
    	if(cmd != null)
	   {
	       runCommand(cmd, intent.getStringExtra(Commands.COMMAND_ARG_TYPE));
	   }
    	
        return START_STICKY;
    }
    
    
    @Override
    public void onDestroy() {
        // Destroy service
    	if(msgHandler != null)
    	{
    		sendAlertToPebble("Disconnecting Pebble Service");
    		unregisterReceiver(msgHandler);
    		msgHandler = null;
    	}
    }
    
    protected PebbleKit.PebbleDataReceiver createMsgHandlerInstance() {
    	return new PebbleKit.PebbleDataReceiver(WEATHER_UUID) {
            @Override
			public void receiveData(Context context, int transactionId,
					PebbleDictionary data) {
				PebbleKit.sendAckToPebble(context, transactionId);

				// PebbleDictionary needs a getKeys function :-(
				JSONArray jsondata;
				try {
					jsondata = new JSONArray(data.toJsonString());
					if(jsondata == null)
						Log.i("PebbleService.receiveData", "NULL pointer for Received Data");
					else
						Log.i("PebbleService.receiveData", jsondata.toString());
				} catch (JSONException e) {
					Log.e("PebbleService.receiveData", "Error creating json object: " + e.getMessage(), e);
					return;
				}
				for(int arrayidx = 0;arrayidx < jsondata.length();arrayidx++) {
					JSONObject currJsonObj = null;
					try {
						currJsonObj = jsondata.getJSONObject(arrayidx);
						int keyidx = currJsonObj.getInt("key");
						Commands.CommandCode cmd = Commands.getCommandCode(keyidx);
						if(cmd == null)
						    return;
						String arg = currJsonObj.getString("value");
						runCommand(cmd, arg);
					} catch (JSONException e) {
						Log.e("PebbleService.receiveData", "Broken JSON Data from Pebble: " + e.getMessage(), e);
						return;
					}
				}// end json array for loop
			}
		};
    }
    
	@SuppressLint("DefaultLocale")
	public void doWeatherUpdate() {
		// If last weather data is less than an hour old, don't update.
		if(lastWeatherData != null) {
			Date now = new Date();
			if(now.getTime() - lastWeatherData.measureDateTime.getTime() < 3600000L)
			{
				sendWeatherDataToWatch(lastWeatherData);
				return;
			}
		}
		
		// If there, weather data needs to be updated
       try {
        	String url = String.format("http://api.wunderground.com/api/%s/conditions/q/%s/%s.json",
        			getString(R.string.APIKEY),
                    getString(R.string.STATE),
                    getString(R.string.CITY));
        	URL u = new URL(url);

        	WeatherData wdata = new WeatherData(-1.0, WeatherType.MOSTLY_CLOUDY);
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
                double temperature = obs.getDouble("temp_f");
                WeatherType wtype = WeatherType.MOSTLY_CLOUDY;
                try {
                	wtype = WeatherType.valueOf(obs.getString("weather").toUpperCase());
                } catch(IllegalArgumentException iae) {
                	wtype = WeatherType.CLEAR;
                }
                wdata = new WeatherData(temperature, wtype);
            } catch(Exception e) {
            	wdata = new WeatherData(-1.0, WeatherType.MOSTLY_CLOUDY);
            } finally {
                urlConnection.disconnect();
            }
            
            sendWeatherDataToWatch(wdata);
        	lastWeatherData = wdata;
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
	
	public void sendWeatherDataToWatch(WeatherData wdata) {
        // Build up a Pebble dictionary containing the weather icon and the current temperature in degrees celsius
        PebbleDictionary data = new PebbleDictionary();
        data.addUint8(ICON_KEY, (byte) wdata.getIconFromWeatherId());
        data.addString(TEMP_KEY, String.format("%d\u00B0F", (int)wdata.temperature));

        // Send the assembled dictionary to the weather watch-app; this is a no-op if the app isn't running or is not
        // installed
        PebbleKit.sendDataToPebble(getApplicationContext(), WEATHER_UUID, data);
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
	}
	
	public class WeatherData {
		double temperature;
		WeatherType wtype;
		Date measureDateTime = null;

		WeatherData(double temperature, WeatherType wtype) {
			this.temperature = temperature;
			this.wtype = wtype;
			this.measureDateTime = new Date();
		}
		
		public int getIconFromWeatherId() {
	        switch(wtype) {
	        case SNOW:
	        	return 3;
	        case RAIN:
	        	return 2;
	        case PARTLY_CLOUDY: case MOSTLY_CLOUDY: case OVERCAST:
	        	return 1;
	        case CLEAR: default:
	        	return 0;
	        }
	    }
	}
	
	@Override
	public IBinder onBind(Intent arg0) {
		return binder;
	}
    
	public class PebbleBinder extends Binder {
		PebbleService getService() {
			return PebbleService.this;
		}
	}
	

	private String getSMSRecipient() {
		if(SMSRecipient == null)
			return getResources().getString(R.string.SMS_RECIPIENT);
		return SMSRecipient;
	}
	
	private void runCommand(Commands.CommandCode command, String arg) {
		if (command == null)
			return;
		switch (command) {
		case SEND_SMS_RECIPIENT:
			sendAlertToPebble("Recipient: " + getSMSRecipient());
			break;
		case SEND_WEATHER_DATA:
			doWeatherUpdate();
			break;
		case SEND_ALERT_TO_PHONE: {
			String msg = arg;
			if (msg == null || msg.length() == 0)
				break;
			if (msg.equals("ping"))
				sendAlertToPebble("Pong");
			else
				postNotification(msg, "Message from Watch", PebbleTool.class);
			break;
		}
		case SEND_SMS: {
			String recipient = getSMSRecipient();

			if (arg == null || arg.length() == 0) {
				sendAlertToPebble("Cannot send empty SMS");
				break;
			}
			if (recipient == null || recipient.length() == 0) {
				sendAlertToPebble("Missing SMS Recipient");
			}
			sendSMS(recipient, arg);
			break;
		}
		case SEND_ALERT_TO_WATCH: {
			String msg = arg;
			if (msg == null || msg.length() == 0)
				break;
			sendAlertToPebble(msg);
			break;
		}
		case NOOP:
		default:
			break;
		}
	}

    private int postNotification(String msg, String title, Class notifyingClass) {
		NotificationCompat.Builder nbuilder =
		    new NotificationCompat.Builder(this)
		    .setSmallIcon(R.drawable.ic_launcher)
		    .setContentTitle(title)
		    .setContentText(msg)
		    .setAutoCancel(true);
	
		// Creates an explicit intent for an Activity in your app
		Intent intent = new Intent(this, notifyingClass);
	
		TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
		stackBuilder.addParentStack(notifyingClass);
		// Adds the Intent that starts the Activity to the top of the stack
		stackBuilder.addNextIntent(intent);
		PendingIntent resultPendingIntent =
		    stackBuilder.getPendingIntent(0,PendingIntent.FLAG_UPDATE_CURRENT);
		nbuilder.setContentIntent(resultPendingIntent);
		NotificationManager nm = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
		nm.notify(NOTIFIER_ID, nbuilder.build());
	
		return NOTIFIER_ID;
    }
    
   private void sendSMS(String phoneNumber, String message)
    {        
        PendingIntent deliveredReceiver = PendingIntent.getBroadcast(this, 0, new Intent(SMS_SENT_RECEIVER), 0);
 
        SmsManager sms = SmsManager.getDefault();
        sms.sendTextMessage(phoneNumber, null, message, null, deliveredReceiver); 
        Log.d("PebbleService.sendSMS", "Sent SMS");
        postNotification("Sent Message to " + phoneNumber, "Watch SMS", PebbleTool.class);
 	   	sendAlertToPebble("Message sent");
    }

}

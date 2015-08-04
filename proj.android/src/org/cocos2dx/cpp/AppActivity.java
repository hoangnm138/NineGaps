/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package org.cocos2dx.cpp;

import google.example.games.basegameutils.BaseGameActivity;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;

import org.tinywood.brain.ancientschool.R;

import android.annotation.TargetApi;
import android.app.AlertDialog;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Point;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Display;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.widget.FrameLayout;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.InterstitialAd;
import com.google.android.gms.analytics.GoogleAnalytics;
import com.google.android.gms.analytics.HitBuilders;
import com.google.android.gms.analytics.Logger;
import com.google.android.gms.analytics.Tracker;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.games.Games;

public class AppActivity extends BaseGameActivity {
	private static AppActivity _appActiviy;
	private AdView adView;
	private static InterstitialAd adView_full;
	private static final String AD_UNIT_ID_BANNER = "ca-app-pub-9220182316892247/5797692411";
	private static final String AD_UNIT_ID_INTERSTITIAL = "ca-app-pub-9220182316892247/7274425610";
	public static final String filename = "screenshot.png";
	public static final String folderSavePath = "AncientSchool";
	// ID's
	private static final int REQUEST_ACHIEVEMENTS = 10000;
	private static final int REQUEST_LEADERBOARDS = 10001;
	private static final int REQUEST_LEADERBOARD = 10002;
	public static final String folderSystemSavePath = Environment
			.getExternalStorageDirectory()
			+ File.separator
			+ folderSavePath
			+ File.separator + filename;
	public static final String folderSystemSave = Environment
			.getExternalStorageDirectory() + File.separator + folderSavePath;

	/**
	 * Enum used to identify the tracker that needs to be used for tracking.
	 * 
	 * A single tracker is usually enough for most purposes. In case you do need
	 * multiple trackers, storing them all in Application object helps ensure
	 * that they are created only once per application instance.
	 */
	public enum TrackerName {
		APP_TRACKER, // Tracker used only in this app.
		GLOBAL_TRACKER, // Tracker used by all the apps from a company. eg:
						// roll-up tracking.
		ECOMMERCE_TRACKER, // Tracker used by all ecommerce transactions from a
							// company.
	}

	HashMap<TrackerName, Tracker> mTrackers = new HashMap<TrackerName, Tracker>();
	// The following line should be changed to include the correct property id.
	private static final String PROPERTY_ID = "UA-56039608-2";
	private static Tracker t;
	
	protected synchronized Tracker getTracker(TrackerName trackerId) {
		if (!mTrackers.containsKey(trackerId)) {

			GoogleAnalytics analytics = GoogleAnalytics.getInstance(this);
			analytics.getLogger().setLogLevel(Logger.LogLevel.VERBOSE);
			analytics.enableAutoActivityReports(getApplication());
			Tracker t = (trackerId == TrackerName.APP_TRACKER) ? analytics
					.newTracker(PROPERTY_ID)
					: (trackerId == TrackerName.GLOBAL_TRACKER) ? analytics
							.newTracker(R.xml.global_tracker) : analytics
							.newTracker(R.xml.ecommerce_tracker);
			mTrackers.put(trackerId, t);
			t.enableAdvertisingIdCollection(true);
			t.enableExceptionReporting(true);
		}
		return mTrackers.get(trackerId);
	}

	/**
	 * Send Unexception to server
	 * 
	 * @param t
	 * @param e
	 */
	public static void sendUnexceptionServer(Tracker t, Exception e) {
		t.send(new HitBuilders.ExceptionBuilder()
				.setDescription(Log.getStackTraceString(e.fillInStackTrace()))
				.setFatal(true).build());
	}

	// Helper get display screen to avoid deprecated function use
	private Point getDisplaySize(Display d) {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
			return getDisplaySizeGE11(d);
		}
		return getDisplaySizeLT11(d);
	}

	@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
	private Point getDisplaySizeGE11(Display d) {
		Point p = new Point(0, 0);
		d.getSize(p);
		return p;
	}

	private Point getDisplaySizeLT11(Display d) {
		try {
			Method getWidth = Display.class.getMethod("getWidth",
					new Class[] {});
			Method getHeight = Display.class.getMethod("getHeight",
					new Class[] {});
			return new Point(
					((Integer) getWidth.invoke(d, (Object[]) null)).intValue(),
					((Integer) getHeight.invoke(d, (Object[]) null)).intValue());
		} catch (NoSuchMethodException e2) // None of these exceptions should
											// ever occur.
		{
			return new Point(-1, -1);
		} catch (IllegalArgumentException e2) {
			return new Point(-2, -2);
		} catch (IllegalAccessException e2) {
			return new Point(-3, -3);
		} catch (InvocationTargetException e2) {
			return new Point(-4, -4);
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		int width = getDisplaySize(getWindowManager().getDefaultDisplay()).x;

		final FrameLayout.LayoutParams adViewLayoutParams = new FrameLayout.LayoutParams(
				width, FrameLayout.LayoutParams.WRAP_CONTENT, Gravity.CENTER
						| Gravity.BOTTOM);
		adView = new AdView(this);
		adView.setAdSize(AdSize.BANNER);
		adView.setAdUnitId(AD_UNIT_ID_BANNER);

		AdRequest adRequest = new AdRequest.Builder().addTestDevice(
				"11210D93A6C5D3D0F9293E595F172202").build();
		adView.loadAd(adRequest);
		adView.setBackgroundColor(Color.BLACK);
		adView.setBackgroundColor(0);
		addContentView(adView, adViewLayoutParams);

		adView_full = new InterstitialAd(this);
		adView_full.setAdUnitId(AD_UNIT_ID_INTERSTITIAL);
		// adView_full.loadAd(adRequest);
		adView_full.setAdListener(new AdListener() {
			@Override
			public void onAdLoaded() {
				super.onAdLoaded();
				if (adView_full.isLoaded())
					adView_full.show();
			}
		});
		_appActiviy = this;

		// Get tracker.
		t = getTracker(TrackerName.APP_TRACKER);
		// Enable Advertising Features.
		t.enableAdvertisingIdCollection(true);
		// Send a screen view.
		t.setScreenName("Main Screen");
		t.send(new HitBuilders.AppViewBuilder().build());
		final Thread.UncaughtExceptionHandler defaultUncaughtExceptionHandler = Thread
				.getDefaultUncaughtExceptionHandler();
		Thread.setDefaultUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
			@Override
			public void uncaughtException(Thread thread, Throwable throwable) {
				defaultUncaughtExceptionHandler.uncaughtException(thread,
						throwable);
			}
		});
	}


	@Override
	protected void onStart() {
		super.onStart();
		// Get an Analytics tracker to report app starts & uncaught exceptions
		// etc.
		GoogleAnalytics.getInstance(this).reportActivityStart(this);
	}

	@Override
	protected void onStop() {
		super.onStop();
		// Stop the analytics tracking
		GoogleAnalytics.getInstance(this).reportActivityStop(this);
	}
	
	public static void displayInterstitial() {
		_appActiviy.runOnUiThread(new Runnable() {
			public void run() {
				AdRequest adRequest = new AdRequest.Builder().addTestDevice(
						"11210D93A6C5D3D0F9293E595F172202").build();
				adView_full.loadAd(adRequest);
			}
		});
	}

	public static void hideAd() {
		_appActiviy.runOnUiThread(new Runnable() {

			@Override
			public void run() {
				if (_appActiviy.adView.isEnabled())
					_appActiviy.adView.setEnabled(false);
				if (_appActiviy.adView.getVisibility() != 4)
					_appActiviy.adView.setVisibility(View.INVISIBLE);
			}
		});

	}

	public static void showAd() {
		_appActiviy.runOnUiThread(new Runnable() {

			@Override
			public void run() {
				if (!_appActiviy.adView.isEnabled())
					_appActiviy.adView.setEnabled(true);
				if (_appActiviy.adView.getVisibility() == 4)
					_appActiviy.adView.setVisibility(View.VISIBLE);
			}
		});

	}

	@Override
	protected void onResume() {
		super.onResume();
	}

	@Override
	protected void onPause() {
		super.onPause();
	}

	@Override
	protected void onDestroy() {
		adView.destroy();
		super.onDestroy();
	}

	public static void transferData() {
		final String fullPath = "/data/data/"
				+ ((AppActivity) getContext()).getPackageName() + "/files/";
		File dir = new File(fullPath);
		if (!dir.exists()) {
			dir.mkdir();
		}
		((AppActivity) getContext()).runOnUiThread(new Runnable() {

			@Override
			public void run() {

				InputStream in = null;
				OutputStream out = null;
				try {
					in = new FileInputStream(fullPath + filename);
					out = new FileOutputStream(folderSystemSavePath);
					byte[] buffer = new byte[1024];
					int read;
					try {
						while ((read = in.read(buffer)) != -1) {
							out.write(buffer, 0, read);
						}
						in.close();
						in = null;
						out.flush();
						out.close();
						out = null;
					} catch (IOException e) {
						e.printStackTrace();
						sendUnexceptionServer(t,e);
					}
				} catch (FileNotFoundException e) {
					e.printStackTrace();
					sendUnexceptionServer(t,e);
				}
			}
		});
	}

	public static void share() {
		File file = new File(folderSystemSave);
		if (!file.exists())
			file.mkdirs();
		File file_share = new File(folderSystemSave, filename);
		final Uri uri = Uri.fromFile(file_share); // get uri from file
		((AppActivity) getContext()).runOnUiThread(new Runnable() {

			@Override
			public void run() {
				Intent shareIntent = new Intent();
				shareIntent.setAction(Intent.ACTION_SEND);
				if (uri != null) {
					shareIntent.putExtra(Intent.EXTRA_STREAM, uri);
					shareIntent.setType("image/*");
				} else {
					shareIntent.putExtra(Intent.EXTRA_TEXT,
							"Congratulation! Share your score to friend ^^");
					shareIntent.setType("text/plain");
				}
				((AppActivity) getContext()).startActivity(Intent
						.createChooser(shareIntent, "Share a photo to friend!"));
			}
		});

	}

	public static void gameServicesSignIn() {
		_appActiviy.runOnUiThread(new Runnable() {
			public void run() {
				_appActiviy.beginUserInitiatedSignIn();
			}
		});

	}

	public void signOutGooglePlay() {
		signOut();
	}

	/**
	 * Google Play Games Services Sign Out
	 */
	public static void gameServicesSignOut() {
		_appActiviy.runOnUiThread(new Runnable() {

			@Override
			public void run() {
				if (_appActiviy.isSignedIn())
					_appActiviy.signOutGooglePlay();
			}
		});
	}

	public GoogleApiClient getCustomApiClient() {
		return getApiClient();
	}

	/**
	 * 
	 * @param message
	 */
	public static void displayAlert(final String message) {
		AlertDialog.Builder builder = new AlertDialog.Builder(_appActiviy);
		builder.setMessage(message);
		builder.setNeutralButton(
				_appActiviy.getResources().getString(android.R.string.ok), null);
		builder.create().show();
	}

	/**
	 * Submit a score in a leaderboard.
	 * 
	 * @param leaderboardID
	 * @param score
	 */
	public static void submitScore(final String leaderboardID, final long score) {
		_appActiviy.runOnUiThread(new Runnable() {

			@Override
			public void run() {
				if (_appActiviy.isSignedIn()) {
					Log.d("AppActivity", "Submit score " + score + " to "
							+ leaderboardID);

					Games.Leaderboards.submitScore(
							_appActiviy.getCustomApiClient(), leaderboardID,
							score);
				} else {
					String message = _appActiviy.getResources().getString(
							R.string.fail_submit_score_leaderboard);
					message = message.replace("{score}", score + "");
					message = message.replace("{leaderboardID}", leaderboardID);
					displayAlert(message);
				}
			}
		});
	}

	/**
	 * Show all leaderboard.
	 */
	public static void showLeaderboards() {

		_appActiviy.runOnUiThread(new Runnable() {

			@Override
			public void run() {
				if (_appActiviy.isSignedIn())
					_appActiviy.startActivityForResult(Games.Leaderboards
							.getAllLeaderboardsIntent(_appActiviy
									.getCustomApiClient()),
							REQUEST_LEADERBOARDS);
				else {
					String message = _appActiviy.getResources().getString(
							R.string.fail_show_leaderboards);
					displayAlert(message);
				}
			}
		});

	}

	/**
	 * Show single leaderboard.
	 */
	public static void showLeaderboard(final String leaderboardID) {

		_appActiviy.runOnUiThread(new Runnable() {

			@Override
			public void run() {
				if (_appActiviy.isSignedIn())
					_appActiviy.startActivityForResult(Games.Leaderboards
							.getLeaderboardIntent(
									_appActiviy.getCustomApiClient(),
									leaderboardID), REQUEST_LEADERBOARD);
				else {
					String message = _appActiviy.getResources().getString(
							R.string.fail_show_leaderboard);
					message = message.replace("{leaderboardID}", leaderboardID);
					displayAlert(message);
				}
			}
		});

	}

	@Override
	public void onSignInFailed() {
		// TODO Auto-generated method stub
		Log.w("AppActivity", "LeaderboardsActivity :: onSignInFailed");
	}

	@Override
	public void onSignInSucceeded() {
		// TODO Auto-generated method stub
		Log.w("AppActivity", "LeaderboardsActivity :: onSignInSucceeded");
	}
}

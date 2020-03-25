package com.numworks.calculator;

import java.util.Locale;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;

import com.google.android.gms.analytics.GoogleAnalytics;
import com.google.android.gms.analytics.Tracker;
import com.google.android.gms.analytics.HitBuilders;

import org.libsdl.app.SDLActivity;

public class EpsilonActivity extends SDLActivity {
  private static GoogleAnalytics sAnalytics;
  private static Tracker sTracker;

  protected String[] getLibraries() {
    return new String[] {
      "epsilon"
    };
  }

  @Override
  protected String[] getArguments() {
    Locale currentLocale = getResources().getConfiguration().locale;
    String[] arguments = {"--language", currentLocale.getLanguage()};
    return arguments;
  }

  public Bitmap retrieveBitmapAsset(String identifier) {
    Bitmap bitmap = null;
    try {
      bitmap = BitmapFactory.decodeStream(
        this.getResources().getAssets().open(identifier)
      );
    } catch (Exception e) {
      Log.w("LoadTexture", "Coundn't load a file:" + identifier);
    }
    return bitmap;
  }

  public void telemetryInit() {
    sAnalytics = GoogleAnalytics.getInstance(this);
    sTracker = sAnalytics.newTracker("UA-93775823-3");
  }

  public void telemetryScreen(String screenName) {
    sTracker.setScreenName(screenName);
    sTracker.send(new HitBuilders.ScreenViewBuilder().build());
  }

  public void telemetryEvent(String category, String action, String label) {
    sTracker.send(new HitBuilders.EventBuilder()
      .setCategory(category)
      .setAction(action)
      .setLabel(label)
      .build()
    );
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    /* This is done to hide the status bar and the bottom navigation buttons.
     *
     * In SDLActivity::onCreate, setWindowStyle(false) is called, which means
     * the fullscreen mode is put to false. We call again the method here with
     * true, in order not to modify the external sources.
     *
     * TODO: This was not needed for v12 of Epsilon, even though
     * setWindowStyle(false) was already called in SDLActivity::onCreate. Find
     * out why and make a proper fix? */
    super.onCreate(savedInstanceState);
    setWindowStyle(true);
  }

}

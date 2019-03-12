package com.numworks.calculator;

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
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    sAnalytics = GoogleAnalytics.getInstance(this);
    sTracker = sAnalytics.newTracker("UA-93775823-3");
  }

  @Override
  public void onResume() {
    super.onResume();
    sTracker.setScreenName("Calculator");
    sTracker.send(new HitBuilders.ScreenViewBuilder().build());
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
}

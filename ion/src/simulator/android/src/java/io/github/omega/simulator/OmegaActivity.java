package io.github.omega.simulator;

import java.util.Locale;

import android.app.Activity;
import android.content.Context;
import android.content.ContentResolver;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;

import com.google.android.gms.analytics.GoogleAnalytics;
import com.google.android.gms.analytics.Tracker;
import com.google.android.gms.analytics.HitBuilders;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDL;

public class OmegaActivity extends SDLActivity {
  protected String[] getLibraries() {
    return new String[] {
      "epsilon"
    };
  }

  public Bitmap retrieveBitmapAsset(String identifier) {
    Bitmap bitmap = null;
    try {
      bitmap = BitmapFactory.decodeStream(
        this.getResources().getAssets().open(identifier)
      );
    } catch (Exception e) {
      Log.w("LoadTexture", "Couldn't load a file:" + identifier);
    }
    return bitmap;
  }

  public String retrieveLanguage() {
    return getResources().getConfiguration().locale.getLanguage();
  }

  public boolean hapticFeedbackIsEnabled() {
    ContentResolver contentResolver = SDL.getContext().getContentResolver();
    int val = Settings.System.getInt(contentResolver, Settings.System.HAPTIC_FEEDBACK_ENABLED, 0);
    return val != 0;
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
    if (!mBrokenLibraries) {
      /* If mBrokenLibraries, SDL is not initialized by onCreate in
       * SDLActivity.java. */
      setWindowStyle(true);
    }
  }
}

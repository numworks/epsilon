package io.github.omega.simulator;

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

public class OmegaActivity extends SDLActivity {
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

  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    if (!mBrokenLibraries) {
      setWindowStyle(true);
    }
  }
}

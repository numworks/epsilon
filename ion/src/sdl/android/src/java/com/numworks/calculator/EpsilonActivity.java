package com.numworks.calculator;

import android.graphics.Bitmap;
//import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.app.Activity;
import android.util.Log;

import org.libsdl.app.SDLActivity;

public class EpsilonActivity extends SDLActivity {
  protected String[] getLibraries() {
    return new String[] {
      "SDL2",
      "epsilon"
    };
  }

  public Bitmap retrieveBitmapAsset(String identifier) {
    Log.w("LoadTexture", "Retrieving bitmap asset");
    Log.w("LoadTexture", "Asset is " + identifier);
    Bitmap bitmap = null;
    try {
      Log.w("LoadTexture", "Rez is " + this.getResources().getAssets().open(identifier));
      bitmap = BitmapFactory.decodeStream(
        this.getResources().getAssets().open(identifier)
      );
    } catch (Exception e) {
      Log.w("LoadTexture", "Coundn't load a file:" + identifier);
    }

    return bitmap;
  }
}

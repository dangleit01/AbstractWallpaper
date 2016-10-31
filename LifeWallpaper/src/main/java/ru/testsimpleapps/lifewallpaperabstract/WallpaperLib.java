package ru.testsimpleapps.lifewallpaperabstract;

import android.content.res.AssetManager;

public class WallpaperLib {
    public static String TAG = "SimpleWallpaper";

    static{
        System.loadLibrary("SimpleWallpaper");
    }

    public static native void init(AssetManager assetManager, PNGManager pngManager, int width, int height, int color, int form);
    public static native void step();
    public static native void destroyPrevious();
    public static native void action(float x, float y);
    public static native void setSettings(int color, int form);
    public static native void exit();
}
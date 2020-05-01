package xmusic;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.io.File;
import android.net.Uri;
import android.content.Intent;

import android.os.Bundle;

import android.os.PowerManager;

import android.net.ConnectivityManager;
import android.net.NetworkInfo;

public class XActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    private PowerManager.WakeLock wakeLock = null;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        PowerManager powerManager = (PowerManager)getSystemService(POWER_SERVICE);
        wakeLock = powerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK
            | PowerManager.ACQUIRE_CAUSES_WAKEUP, "xmusicWakelock");
        wakeLock.acquire();
    }

    public boolean isMobileConnected()
    {
        ConnectivityManager connManager = (ConnectivityManager)getSystemService(CONNECTIVITY_SERVICE);
        NetworkInfo networkInfo = connManager.getActiveNetworkInfo();
        if (networkInfo != null && networkInfo.getType() == ConnectivityManager.TYPE_MOBILE)
        {
            return networkInfo.isAvailable();
        }

        return false;
    }

    public static void installApk(String filePath, QtActivity activity)
    {
        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_VIEW);
        intent.setDataAndType(Uri.fromFile(new File(filePath)),"application/vnd.android.package-archive");
        activity.startActivity(intent);
    };
};

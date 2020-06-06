package xmusic;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.io.File;
import android.net.Uri;
import android.content.Intent;

import android.os.Bundle;

import android.os.PowerManager;

import android.net.ConnectivityManager;
import android.net.NetworkInfo;

import android.Manifest;

import android.content.pm.PackageManager;

import android.os.Build;

public class XActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    private PowerManager.WakeLock wakeLock = null;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        getWindow().setFlags(0x400, 0x400);

        PowerManager powerManager = (PowerManager)getSystemService(POWER_SERVICE);
        wakeLock = powerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK
            | PowerManager.ACQUIRE_CAUSES_WAKEUP, "xmusicWakelock");
        wakeLock.acquire();

        /*//安卓6以上需要运行时申请权限
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
        {
            if (PackageManager.PERMISSION_GRANTED != checkCallingOrSelfPermission("android.permission.WRITE_EXTERNAL_STORAGE"))
            {
                requestPermissions(new String[]{"android.permission.WRITE_EXTERNAL_STORAGE"}, 1);
            }
        }*/
    }

    public boolean checkMobileConnected()
    {
        ConnectivityManager connManager = (ConnectivityManager)getSystemService(CONNECTIVITY_SERVICE);
        NetworkInfo networkInfo = connManager.getActiveNetworkInfo();
        if (networkInfo != null && networkInfo.getType() == ConnectivityManager.TYPE_MOBILE)
        {
            if (networkInfo.isAvailable())
            {
                return true;
            }
        }

        return false;
    }

    public void installApk(String filePath)
    {
        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_VIEW);
        intent.setDataAndType(Uri.fromFile(new File(filePath)),"application/vnd.android.package-archive");
        startActivity(intent);
    };
};

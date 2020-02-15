package xmusic;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.io.File;
import android.net.Uri;
import android.content.Intent;

import android.os.Bundle;

import android.os.PowerManager;

public class XActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    public static void installApk(String filePath, QtActivity activity)
    {
        Intent intent = new Intent();
        // 执行动作
        intent.setAction(Intent.ACTION_VIEW);
        File file = new File(filePath);
        // 执行的数据类型
        intent.setDataAndType(Uri.fromFile(file),"application/vnd.android.package-archive");
        activity.startActivity(intent);
    };

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
};

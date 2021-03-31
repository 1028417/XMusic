package xmusic;

import org.qtproject.qt5.android.bindings.QtActivity;

import android.os.Bundle;

import android.os.Build;

import android.os.PowerManager;

import java.io.File;
import android.net.Uri;
import android.content.Intent;

import android.net.ConnectivityManager;
import android.net.NetworkInfo;

import android.Manifest;

import android.content.pm.PackageManager;

import android.support.v4.content.FileProvider;

import android.hardware.SensorManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;

import android.view.Window;
import android.view.WindowManager.LayoutParams;
import android.view.View;
import android.graphics.Color;

import android.widget.Toast;

public class XActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    private PowerManager.WakeLock wakeLock = null;

    private native void accelerometerNotify(int x,int y,int z);

    private SensorEventListener eventListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent event) {
            int type = event.sensor.getType();
            if (type == Sensor.TYPE_ACCELEROMETER) {
                float[] values = event.values;
                float x = values[0];
                float y = values[1];
                float z = values[2];
                accelerometerNotify((int)x, (int)y, (int)z);
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        fullScreen(true);

        PowerManager powerManager = (PowerManager)getSystemService(POWER_SERVICE);
        wakeLock = powerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK
            | PowerManager.ACQUIRE_CAUSES_WAKEUP, "xmusicWakelock");
        wakeLock.acquire();

        SensorManager mSensorManager = ((SensorManager)getSystemService(SENSOR_SERVICE));
        if (mSensorManager != null) {
            Sensor mAccelerometerSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            if (mAccelerometerSensor != null) {
                mSensorManager.registerListener(eventListener, mAccelerometerSensor, SensorManager.SENSOR_DELAY_UI);
            }
        }

        /*//安卓6(API 23)以上需要运行时申请权限
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
        {
            if (PackageManager.PERMISSION_GRANTED != checkCallingOrSelfPermission("android.permission.WRITE_EXTERNAL_STORAGE"))
            {
                requestPermissions(new String[]{"android.permission.WRITE_EXTERNAL_STORAGE"}, 1);
            }
        }*/
    }

    public void fullScreen(boolean bSet) //普通全屏控制，qt WindowFullScreen就已经可以实现
    {
        Window window = getWindow();
        if (bSet)
        {
            window.addFlags(LayoutParams.FLAG_FULLSCREEN); //window.setFlags(LayoutParams.FLAG_FULLSCREEN, LayoutParams.FLAG_FULLSCREEN);
        }
        else
        {
            window.clearFlags(LayoutParams.FLAG_FULLSCREEN); //window.setFlags(LayoutParams.FLAG_FULLSCREEN, 0);
        }
    }

    public void showTranslucentStatusBar(boolean bShow) //Qt WindowMaximized状态下，显示半透明状态栏，实际上只需调用一次true
    {
        Window window = getWindow();
        if (bShow)
        {
            window.addFlags(LayoutParams.FLAG_TRANSLUCENT_STATUS);
            window.getDecorView().setFitsSystemWindows(true);
        }
        else
        {
            window.getDecorView().setFitsSystemWindows(false);
            window.clearFlags(LayoutParams.FLAG_TRANSLUCENT_STATUS);
        }
    }

    public void showTransparentStatusBar(boolean bShow) //Qt WindowMaximized状态下，显示透明状态栏
    {
        int visibility = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) // SYSTEM_UI_FLAG_LIGHT_STATUS_BAR状态栏字体设置为深色
        {
            visibility |= View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR;
        }

        Window window = getWindow();
        if (!bShow)
        {
            window.clearFlags(LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            visibility = window.getDecorView().getSystemUiVisibility() & (~visibility);
            window.getDecorView().setSystemUiVisibility(visibility);
            return;
        }

        // 在Qt不设全屏时，jni实现半沉浸全屏
        //window.clearFlags(LayoutParams.FLAG_TRANSLUCENT_STATUS);
        visibility |= window.getDecorView().getSystemUiVisibility();
        window.getDecorView().setSystemUiVisibility(visibility);

        // 部分机型会有半透明黑色背景，所以用透明色绘制
        window.addFlags(LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        window.setStatusBarColor(Color.TRANSPARENT);
    }

    public void showQuitToast()
    {
        Toast.makeText(getApplicationContext(), "再按一次退出程序", Toast.LENGTH_SHORT).show();
    }

    public boolean checkMobileConnected()
    {
        ConnectivityManager connManager = (ConnectivityManager)getSystemService(CONNECTIVITY_SERVICE);
        NetworkInfo networkInfo = connManager.getActiveNetworkInfo();
        if (networkInfo == null)
        {
            return false;
        }

        if (!networkInfo.isAvailable())
        {
            return false;
        }

        return networkInfo.getType() == ConnectivityManager.TYPE_MOBILE;
    }

    public void installApk(String filePath)
    {
        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_VIEW);

        File file = new File(filePath);
        Uri uri;
        // 安卓7(API 24)需要通过FileProvider共享路径
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N)
        {
            uri = FileProvider.getUriForFile(this, "com.musicrossoft.xmusic.fileProvider", file);
            intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        }
        else
        {
            uri = Uri.fromFile(file);
        }
        intent.setDataAndType(uri, "application/vnd.android.package-archive");

        //intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

        startActivity(intent);
    };
};

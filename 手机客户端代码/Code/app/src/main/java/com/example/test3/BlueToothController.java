package com.example.test3;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

import java.util.ArrayList;

public class BlueToothController {

    private BluetoothAdapter mAapter;

    public BlueToothController() { mAapter = BluetoothAdapter.getDefaultAdapter();}   //获取本地蓝牙适配器

    public BluetoothAdapter getmAapter() {return mAapter;}

    /**
     * 检查支持
     */
    public boolean isSupportBlueTooth(){
        if (mAapter != null){
            return true;
        }
        else {
            return false;
        }
    }

    /**
     * 打开蓝牙
     */

    @SuppressLint("MissingPermission")
    public boolean turnOnBlueTooth(Activity activity,int requestCode) {
        assert (mAapter != null);
        if (!mAapter.isEnabled()){
            Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);

            activity.startActivityForResult(intent, requestCode);

            return true;
        }
        Toast.makeText(activity,"已开启",Toast.LENGTH_SHORT).show();
        return false;
    }

    /**
     * 关闭蓝牙
     */
    @SuppressLint("MissingPermission")
    public void turnOffBlueTooth(){
        assert (mAapter != null);
        if(mAapter.isEnabled()){
            mAapter.disable();

        }
    }

    /**
     * 打开蓝牙可见性  300s
     */
    @SuppressLint("MissingPermission")
    public void enableVisibily(Context context){
        if(mAapter == null)return;
        Intent disCoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
        disCoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION,300);
        context.startActivity(disCoverableIntent);
    }

    /**
     * 查找设备
     */
    @SuppressLint("MissingPermission")
    public Boolean findDevice(){
        assert (mAapter != null);
        if(mAapter.isDiscovering()){
            mAapter.cancelDiscovery();
            return false;
        }
        else {
            return mAapter.startDiscovery();
        }
    }

    public BluetoothDevice find_device(String addr){
        return mAapter.getRemoteDevice(addr);
    }

    /**
     * 停止查找设备
     */
    @SuppressLint("MissingPermission")
    public void cancelSearch() {mAapter.cancelDiscovery();
    }

    /**
     * 获取已绑定设备
     */
    @SuppressLint("MissingPermission")
    public ArrayList<BluetoothDevice> getBondedDeviceList(){
        return new ArrayList<BluetoothDevice>(mAapter.getBondedDevices());
    }
}

package com.example.test3;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.lang.reflect.Method;
import java.util.ArrayList;

public class connect extends AppCompatActivity {
    public static int REQUEST_CODE = 0;
    public int once = 1;
    private Toast mToast;
    private ProgressBar progressBar;
    public ArrayAdapter mAdapter;
    public BlueToothController mController = new BlueToothController();

    public ArrayList<String> requestList = new ArrayList<>();               //申请权限

    public ArrayList<String> arrayList=new ArrayList<>();                //地址列表
    public ArrayList<String> deviceName=new ArrayList<>();               //设备列表
    private ArrayList<BluetoothDevice> mDeviceList = new ArrayList<>();       //设备列表

    IntentFilter foundFilter=new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_connect);

        InitUI();

        registerBluetoothReceiver();
        if (once == 1) {
            once = 0;
            findBT();
        }
    }

    private void InitUI(){
        getPermision();
        if (!mController.isSupportBlueTooth()) {
            showToast("不支持蓝牙");
        }

        progressBar = (ProgressBar)findViewById(R.id.progressBar);
        progressBar.setVisibility(View.INVISIBLE);

        mAdapter = new ArrayAdapter(this, R.layout.item,deviceName);

        ListView listView=(ListView) findViewById(R.id.list_view);
        listView.setAdapter(mAdapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @SuppressLint("MissingPermission")
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                CharSequence content = ((TextView) view).getText();
                String con = content.toString();
                String[] conArray = con.split("\n");
                String rightStr = conArray[1].substring(5, conArray[1].length());
                BluetoothDevice device = mController.find_device(rightStr);

                if (device.getBondState() == BluetoothDevice.BOND_NONE){
                    mController.cancelSearch();
                    String s = "设备名：" + device.getName() + "\n" + "设备地址：" + device.getAddress() + "\n" + "连接状态：未配对"  + "\n";
                    deviceName.remove(s);
                    device.createBond();

                    s = "设备名：" + device.getName() + "\n" + "设备地址：" + device.getAddress() + "\n" + "连接状态：已配对"  + "\n";
                    deviceName.add(s);
                    mAdapter.notifyDataSetChanged();
                    showToast("配对: " + device.getName());
                }
                else {
                    mController.cancelSearch();
                    String s = "设备名：" + device.getName() + "\n" + "设备地址：" + device.getAddress() + "\n" + "连接状态：已配对"  + "\n";
                    deviceName.remove(s);
                    device.createBond();
                    s = "设备名：" + device.getName() + "\n" + "设备地址：" + device.getAddress() + "\n" + "连接状态：未配对"  + "\n";
                    mAdapter.notifyDataSetChanged();
                    deviceName.add(s);
                    unpairDevice(device);
                }
            }
        });
    }

    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    public boolean onOptionsItemSelected(MenuItem item) {
        int itemID = item.getItemId();
        BluetoothDevice device = null;
        if (itemID == R.id.OpenBlueTooth) {
            mController.turnOnBlueTooth(this, REQUEST_CODE);
        } else if (itemID == R.id.CloseBlueTooth) {
            mController.turnOffBlueTooth();
        } else if (itemID == R.id.enable_visibility) {
            mController.enableVisibily(this);
        } else if (itemID == R.id.find_device) {
            findBT();
        } else if (itemID == R.id.bonded_device) {

            Intent intent = new Intent(connect.this,Talk.class);
            startActivity(intent);
//            finish();
        }
        return super.onOptionsItemSelected(item);
    }

    private void unpairDevice(BluetoothDevice device) {
        try {
            Method m = device.getClass()
                    .getMethod("removeBond", (Class[]) null);
            m.invoke(device, (Object[]) null);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private BroadcastReceiver receiver = new BroadcastReceiver(){
        @SuppressLint("MissingPermission")
        @Override
        public void onReceive(Context context, Intent intent) {
            /////////////搜索蓝牙
            int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, -1);
            switch (state) {
                case BluetoothAdapter.STATE_OFF:
                    showToast("STATE_OFF");
                    break;
                case BluetoothAdapter.STATE_ON:
                    showToast("STATE_ON");
                    break;
                case BluetoothAdapter.STATE_TURNING_OFF:
                    showToast("STATE_TURNING_OFF");
                    break;
                case BluetoothAdapter.STATE_TURNING_ON:
                    showToast("STATE_TURNING_ON");
                    break;
                default:
                    showToast("UnKnow STATE");
                    unregisterReceiver(this);
                    break;
            }

        }
    };

    public void registerBluetoothReceiver() {
        //注册广播


        foundFilter.addAction(BluetoothDevice.ACTION_FOUND);
// 远程设备的绑定状态发生变化
        foundFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
// 第一次检索远程设备的友好名称，或自上次检索后更改
        foundFilter.addAction(BluetoothDevice.ACTION_NAME_CHANGED);
// 远程设备的蓝牙类已更改
        foundFilter.addAction(BluetoothDevice.ACTION_CLASS_CHANGED);
// 用于在获取远程设备后将其UUID 作为ParcelUuid远程设备的包装进行广播
        foundFilter.addAction(BluetoothDevice.ACTION_UUID);
// 操作配对请求
        foundFilter.addAction(BluetoothDevice.ACTION_PAIRING_REQUEST);
// 蓝牙状态改变
        foundFilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
// 开始搜索蓝牙
        foundFilter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
// 蓝牙搜索完成
        foundFilter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
// 本地Adapter的蓝牙扫描模式发生了变化
        foundFilter.addAction(BluetoothAdapter.ACTION_SCAN_MODE_CHANGED);
// 本地蓝牙适配器更改蓝牙名称
        foundFilter.addAction(BluetoothAdapter.ACTION_LOCAL_NAME_CHANGED);
// 请求本地蓝牙可被其它蓝牙扫描到
        foundFilter.addAction(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
// 显示请求可发现模式的系统活动;
        foundFilter.addAction(BluetoothAdapter.ACTION_REQUEST_ENABLE);
// 连接状态改变
        foundFilter.addAction(BluetoothAdapter.ACTION_CONNECTION_STATE_CHANGED);

        registerReceiver(receiver,foundFilter);
    }

    private BroadcastReceiver bluetoothReceiver = new BroadcastReceiver() {
        @SuppressLint("MissingPermission")
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if(BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)){
                showToast("开始搜索");
                progressBar.setVisibility(View.VISIBLE);
            }
            else if(BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)){
                showToast("搜索完成");
                progressBar.setVisibility(View.INVISIBLE);
                unregisterReceiver(this);
            }
            else if (BluetoothDevice.ACTION_FOUND.equals(action)){
                String s;
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if (device.getBondState() == BluetoothDevice.BOND_BONDED) {
                    s = "设备名：" + device.getName() + "\n" + "设备地址：" + device.getAddress() + "\n" + "连接状态：已配对" + "\n";
                }
                else if (device.getBondState() == BluetoothDevice.BOND_NONE){
                    s = "设备名：" + device.getName() + "\n" + "设备地址：" + device.getAddress() + "\n" + "连接状态：未配对" +"\n";
                }else{
                    s = "设备名：" + device.getName() + "\n" + "设备地址：" + device.getAddress() + "\n" + "连接状态：未知" + "\n";
                }
                if (!deviceName.contains(s)) {
                    deviceName.add(s);//将搜索到的蓝牙名称和地址添加到列表。
//                if (!mDeviceList.contains(device)) {
//                    mDeviceList.add(device);//将搜索到的蓝牙名称和地址添加到列表。
                    arrayList.add(device.getAddress());//将搜索到的蓝牙地址添加到列表。
                    mAdapter.notifyDataSetChanged();//更新
                }
            }
            else if(BluetoothDevice.ACTION_BOND_STATE_CHANGED.equals(action)) {
                BluetoothDevice remoteDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if(remoteDevice == null) {
                    showToast("无设备");
                    return;
                }
                int status = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, 0);
                if(status == BluetoothDevice.BOND_BONDED) {
                    showToast("已绑定" + remoteDevice.getName());
                } else if(status == BluetoothDevice.BOND_BONDING) {
                    showToast("正在绑定" + remoteDevice.getName());
                } else if(status == BluetoothDevice.BOND_NONE) {
                    showToast("未绑定" + remoteDevice.getName());
                }
            }
        }
    };

    @SuppressLint("MissingPermission")
    public void findBT(){
        registerReceiver(bluetoothReceiver ,foundFilter);

        mController.turnOnBlueTooth(this, REQUEST_CODE);

        mController.enableVisibily(this);

        arrayList.clear();

        deviceName.clear();

        mAdapter.notifyDataSetChanged();

        mController.findDevice();

        mDeviceList = mController.getBondedDeviceList();

        for(int i = 0; i < mDeviceList.size(); i++){
            BluetoothDevice device = mDeviceList.get(i);

            arrayList.add(device.getAddress());

            if(device.getBondState() == BluetoothDevice.BOND_BONDED){
                deviceName.add("设备名：" + device.getName() + "\n" + "设备地址：" + device.getAddress() + "\n" + "连接状态：已配对" + "\n");
            }
            else if (device.getBondState() == BluetoothDevice.BOND_NONE){
                deviceName.add("设备名：" + device.getName() + "\n" + "设备地址：" + device.getAddress() + "\n" + "连接状态：未配对" + "\n");
            }else{
                deviceName.add("设备名：" + device.getName() + "\n" + "设备地址：" + device.getAddress() + "\n" + "连接状态：未知" + "\n");
            }

            mAdapter.notifyDataSetChanged();
        }
    }


    public void getPermision(){
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.S){
            requestList.add(android.Manifest.permission.BLUETOOTH_SCAN);
            requestList.add(android.Manifest.permission.BLUETOOTH_ADVERTISE);
            requestList.add(android.Manifest.permission.BLUETOOTH_CONNECT);
            requestList.add(android.Manifest.permission.ACCESS_FINE_LOCATION);
            requestList.add(android.Manifest.permission.ACCESS_COARSE_LOCATION);
            requestList.add(Manifest.permission.BLUETOOTH);
        }
        if(requestList.size() != 0){
            ActivityCompat.requestPermissions(this, requestList.toArray(new String[0]), 1);
        }
    }

    public void showToast(String text){

        // 若Toast控件未初始化
        if( mToast == null){
            // 则初始化
            mToast = Toast.makeText(this, text, Toast.LENGTH_SHORT);
        }
        // 否则
        else{
            // 修改显示文本
            mToast.setText(text);
        }
        // 显示
        mToast.show();
    }
}

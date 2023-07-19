package com.example.test3;


import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.util.UUID;

public class TalkActivity extends Activity {

    public BluetoothAdapter mBluetoothAdapter;

    public String UUID_LEGACY = "00001101-0000-1000-8000-00805F9B34FB";

    private final BroadcastReceiver mRecevier = new BroadcastReceiver() {
        @SuppressLint("MissingPermission")
        @Override
        public void onReceive(Context context, Intent intent) {

                String action = intent.getAction();
                if (BluetoothDevice.ACTION_FOUND.equals(action)) {

                    BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                    String name = device.getAddress();

                    if (name != null && name.equals("98:D3:32:31:8E:D9")) {


//                if(name != null){

                        new Thread(new ClientThread(device)).start();
                    }

            }
        }
    };

    public class ClientThread extends Thread { private BluetoothDevice device;
        public ClientThread (BluetoothDevice device) {
            this.device = device;
        }

        @SuppressLint("MissingPermission")
        @Override
        public void run() {

                while (true) {
                    BluetoothSocket socket = null;


                    try {
                        socket = device.createRfcommSocketToServiceRecord(UUID.fromString(UUID_LEGACY));
                        socket.connect();
                        readDataFromServer(socket);

                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

        }
        StringBuilder stringBuilder = new StringBuilder();
        TextView textViewReceived = findViewById(R.id.textViewReciced);
        private void readDataFromServer(BluetoothSocket socket) {
            byte[] buffer = new byte[64];
            try {

                    InputStream is = socket.getInputStream();
                    int cnt = is.read(buffer);
//                    is.close();

                    runOnUiThread(() -> {
                        String s = new String(buffer, 0, cnt);
                        stringBuilder.append(s);
                        textViewReceived.setText(stringBuilder.toString());
                    });

            }catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    Handler mMainHandler;

    @SuppressLint("MissingPermission")
    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.talk);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(mRecevier, filter);
        mBluetoothAdapter.startDiscovery();


//        Toast.makeText(TalkActivity.this,"OK1",Toast.LENGTH_SHORT).show();
    }


    @Override
    protected void onDestroy(){
        super.onDestroy();
        unregisterReceiver(mRecevier);
    }


}
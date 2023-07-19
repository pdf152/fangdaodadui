package com.example.test3;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
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
import android.widget.Button;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {


        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initUI();


    }

    public class mainListener implements View.OnClickListener {
        public void onClick(View view) {
            if (view.getId() == R.id.Conn_But) {
                Intent intent = new Intent(MainActivity.this, connect.class);
                startActivity(intent);
//                finish();
            }
            if (view.getId() == R.id.Open) {
                Intent intent = new Intent(MainActivity.this, Talk.class);
                startActivity(intent);
                Talk.qwq = 0;

//                finish();
            }
            if (view.getId() == R.id.Quick) {
                Intent intent = new Intent(MainActivity.this, LoginActivity.class);
                startActivity(intent);
//                finish();
            }
        }
    }


    private void initUI() {
        Button Conn_But = (Button) findViewById(R.id.Conn_But);
        Button Open = (Button) findViewById(R.id.Open);
        Button Quick = (Button) findViewById(R.id.Quick);

        Conn_But.setOnClickListener(new mainListener());
        Open.setOnClickListener(new mainListener());
        Quick.setOnClickListener(new mainListener());
    }

}




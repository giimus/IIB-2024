package com.example.controlesp32


import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import java.io.IOException
import java.util.UUID


class MainActivity : AppCompatActivity() {

    private val bluetoothAdapter: BluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
    private var bluetoothSocket: BluetoothSocket? = null
    private val esp32UUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB") // UUID padrão para Bluetooth serial

    private lateinit var statusText: TextView
    private lateinit var btnConnect: Button
    private lateinit var btnLed1On: Button
    private lateinit var btnLed1Off: Button
    private lateinit var btnLed2On: Button
    private lateinit var btnLed2Off: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Inicializar a variável de status
        statusText = findViewById(R.id.statusText)
        btnConnect = findViewById(R.id.btnConnect)
        btnLed1On = findViewById(R.id.btnLed1On)
        btnLed1Off = findViewById(R.id.btnLed1Off)
        btnLed2On = findViewById(R.id.btnLed2On)
        btnLed2Off = findViewById(R.id.btnLed2Off)

        // Verificar permissões de Bluetooth
        checkBluetoothPermissions()

        // Configurar os listeners dos botões
        btnLed1On.setOnClickListener { sendCommand("A") } // Liga o LED 1
        btnLed1Off.setOnClickListener { sendCommand("B") } // Desliga o LED 1
        btnLed2On.setOnClickListener { sendCommand("C") } // Liga o LED 2
        btnLed2Off.setOnClickListener { sendCommand("D") } // Desliga o LED 2

    }

    private fun checkBluetoothPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) { // Android 12 ou superior
            if (ContextCompat.checkSelfPermission(
                    this,
                    Manifest.permission.BLUETOOTH_CONNECT
                ) != PackageManager.PERMISSION_GRANTED ||
                ContextCompat.checkSelfPermission(
                    this,
                    Manifest.permission.BLUETOOTH_SCAN
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                ActivityCompat.requestPermissions(
                    this,
                    arrayOf(
                        Manifest.permission.BLUETOOTH_CONNECT,
                        Manifest.permission.BLUETOOTH_SCAN
                    ),
                    1
                )
            }
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) { // Android 6 ou superior
            if (ContextCompat.checkSelfPermission(
                    this,
                    Manifest.permission.ACCESS_FINE_LOCATION
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                ActivityCompat.requestPermissions(
                    this,
                    arrayOf(Manifest.permission.ACCESS_FINE_LOCATION),
                    1
                )
            }
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == 1 && grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            connectToESP32()
        } else {
            showPermissionDeniedMessage()
        }
    }

    private fun showPermissionDeniedMessage() {
        Toast.makeText(this, "Permissão negada para usar Bluetooth ou Localização", Toast.LENGTH_LONG).show()
    }

    private fun connectToESP32() {
        try {
            val pairedDevices: Set<BluetoothDevice> = bluetoothAdapter.bondedDevices
            val esp32 = pairedDevices.find { it.name == "ESP32_LED" }

            if (esp32 != null) {
                bluetoothSocket = esp32.createRfcommSocketToServiceRecord(esp32UUID)
                bluetoothSocket?.connect()
                statusText.text = "Conectado"
            } else {
                statusText.text = "Dispositivo não encontrado"
            }
        } catch (e: SecurityException) {
            e.printStackTrace()
            statusText.text = "Permissão negada para conectar ao dispositivo"
        } catch (e: IOException) {
            e.printStackTrace()
            statusText.text = "Falha na Conexão"
        }
    }

    private fun sendCommand(command: String) {
        try {
            bluetoothSocket?.outputStream?.write(command.toByteArray())
        } catch (e: IOException) {
            e.printStackTrace()
            statusText.text = "Erro ao enviar comando"
        }
    }
}


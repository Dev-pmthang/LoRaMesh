#ifndef LORA_FUNCTIONS_H
#define LORA_FUNCTIONS_H

#include "main.h"

String LoRaMessage;
String LoRaData;
String data;
String node1Data = "";
String node2Data = "";
String node3Data = "";
String er = "ERORR";

int nodeID;
int expectedNode = 1; // Initialize expected node to 1 (Node 1)

unsigned long currentMillis = 0;  // Biến lưu thời gian hiện tại
const long interval = 60 * 1000;  // Khoảng thời gian giữa các lần gọi (1 phút)
const long retryInterval = 10000; // Khoảng thời gian giữa mỗi lần thử (5 giây)

const int maxRetryCount = 5;    // Số lần thử tối đa cho mỗi node
int currentRetry = 0;           // Số lần đã thử cho node hiện tại
unsigned long lastSendTime = 0; // Biến lưu thời gian của lần gửi gần nhất

bool dataSent = false;
int sendnodeCount[4] = {0}; // Retry count array for each node

unsigned long lastSendTime_gui1s = 0;
const long interval_gui1s = 1 * 60 * 1000; // Khoảng thời gian giữa các lần gọi (1 phút)
int node_count_send = 0;

void print_and_notify_error(int nodeID)
{
    Serial.println("SENSOR " + String(nodeID) + " ERROR!");
    String path = "Node " + String(nodeID) + "/" + init_date() + "/" + init_time();
    String errorString = "SENSOR " + String(nodeID) + " ERROR!";
    sendString(path, errorString);
}

void print_data()
{
    if (nodeID == 1)
    {
        // Parsing the received data string
        int pos2 = data.indexOf('@');
        int pos3 = data.indexOf('*');
        int pos4 = data.indexOf('#');
        float temperature = data.substring(0, pos2).toFloat();
        float humidity = data.substring(pos2 + 1, pos3).toFloat();
        int soilMoisture = data.substring(pos3 + 1, pos4).toInt();
        int co2 = data.substring(pos4 + 1).toInt();

        // Sending parsed data to Firebase
        sendString("Node 1/temp", String(temperature));
        sendString("Node 1/humi", String(humidity));
        sendString("Node 1/SoilMoisture", String(soilMoisture));
        sendString("Node 1/CO2", String(co2));

        // Khởi tạo một JSON writer
        String dataKey = "Node 1/" + init_date();
        String jsonString = String("{\"temp\":") + temperature +
                            ",\"humi\":" + humidity +
                            ",\"soilMoisture\":" + soilMoisture +
                            ",\"CO2\":" + co2 +
                            ",\"time\":\"" + init_time() + "\"" +
                            "}";
        Database.push<object_t>(aClient, dataKey, jsonString);
        Database.set<int>(aClient, "STT NODE ERROR/1", 1);
        fb_node1_on();
    }
    else if (nodeID == 2)
    {
        int pos2 = data.indexOf('@');
        int pos3 = data.indexOf('*');
        float temperature = data.substring(0, pos2).toFloat();
        float humidity = data.substring(pos2 + 1, pos3).toFloat();
        int UV = data.substring(pos3 + 1).toInt();

        sendString("Node 2/temp", String(temperature));
        sendString("Node 2/humi", String(humidity));
        sendString("Node 2/UV", String(UV));

        String dataKey2 = "Node 2/" + init_date();
        // Khởi tạo một JSON writer
        String jsonString = String("{\"temp\":") + temperature +
                            ",\"humi\":" + humidity +
                            ",\"UV\":" + UV +
                            ",\"time\":\"" + init_time() + "\"" +
                            "}";
        Database.push<object_t>(aClient, dataKey2, jsonString);
        Database.set<int>(aClient, "STT NODE ERROR/2", 1);
        fb_node2();
    }
    else if (nodeID == 3)
    {
        digitalWrite(LED3, 0);

        int pos2 = data.indexOf('@');
        int pos3 = data.indexOf('*');
        float temperature = data.substring(0, pos2).toFloat();
        float humidity = data.substring(pos2 + 1, pos3).toFloat();
        int soilMoisture = data.substring(pos3 + 1).toInt();

        sendString("Node 3/temp", String(temperature));
        sendString("Node 3/humi", String(humidity));
        sendString("Node 3/SoilMoisture", String(soilMoisture));

        String dataKey3 = "Node 3/" + init_date();
        String jsonString = String("{\"temp\":") + temperature +
                            ",\"humi\":" + humidity +
                            ",\"soilMoisture\":" + soilMoisture +
                            ",\"time\":\"" + init_time() + "\"" +
                            "}";
        Database.push<object_t>(aClient, dataKey3, jsonString);
        Database.set<int>(aClient, "STT NODE ERROR/3", 1);
        fb_node3();
    }
}

void getLoRaData()
{
    Serial.print("LoRa packet received: ");
    while (LoRa.available())
    {
        LoRaData = LoRa.readString();
        Serial.println(LoRaData);
        int errorNode = -1; // Initialize error node ID to -1 (no error)
        if (LoRaData.indexOf("ERROR") != -1)
        {
            errorNode = LoRaData.substring(0, LoRaData.indexOf("/")).toInt();
            print_and_notify_error(errorNode);
            node_count_send++;
        }
        else if (LoRaData.indexOf("^")!= -1)
        {
            digitalWrite(LED1, 0);
            if (Database.set<bool>(aClient, "/Device1", true))
            {
                Serial.println("Successfully updated Driver1 to ON in Firebase.");
                Database.set<bool>(aClient, "led1", true);
            }
            else
            {
                Serial.println("Failed to update Driver1 in Firebase.");
            }
        }
        else if (LoRaData.indexOf("~")!= -1)
        {
            digitalWrite(LED1, 1);
            if (Database.set<bool>(aClient, "/Device1", false))
            {
                Serial.println("Successfully updated Driver1 to OFF in Firebase.");
                Database.set<bool>(aClient, "led1", false);
            }
            else
            {
                Serial.println("Failed to update Driver1 in Firebase.");
            }
        }
        else
        {
            int pos1 = LoRaData.indexOf('/');
            nodeID = LoRaData.substring(0, pos1).toInt();
            data = LoRaData.substring(pos1 + 1);
            if (nodeID == expectedNode)
            {
                print_data();                          // Assuming you have a function for complete parsing
                expectedNode = (expectedNode % 3) + 1; // Update expected node (cyclic)
                sendnodeCount[nodeID - 1] = 0;         // Reset retry count for received node
                currentRetry = 0;
                node_count_send++;
            }
        }
        int rssi = LoRa.packetRssi();
        Serial.print(" with RSSI ");
        Serial.println(rssi);
    }
}

void send_node(int node, String data)
{
    LoRaMessage = "N" + String(node);
    LoRa.beginPacket();
    LoRa.print(LoRaMessage);
    LoRa.endPacket();
    Serial.print("Sending data to NODE ");
    Serial.println(node);
}

void lora_init()
{
    Serial.println("LoRa Gateway");
    LoRa.setPins(NSS, RST, DI0);
    if (!LoRa.begin(433E6))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }
    LoRa.setTxPower(13);
    LoRa.setSpreadingFactor(12);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();
    LoRa.setSyncWord(0xF2);
    Serial.println("LoRa Initializing Successful!");
}

void sendnode1()
{
    String msg = "N1";
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
}

void sendnode2()
{
    String msg = "N2";
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
}

void sendnode3()
{
    String msg = "N3";
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
}

void sendon1()
{
    String msg = "N1ON";
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
    Serial.println(msg);
}

void sendon2()
{
    String msg = "N2ON";
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
}

void sendon3()
{
    String msg = "N3ON";
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
}

void sendoff1()
{
    String msg = "N1OFF";
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
    Serial.println(msg);
}

void sendoff2()
{
    String msg = "N2OFF";
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
}

void sendoff3()
{
    String msg = "N3OFF";
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
}

#endif
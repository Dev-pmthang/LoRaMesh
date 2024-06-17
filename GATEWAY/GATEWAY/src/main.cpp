
#include "main.h"

char flag_gui1s = 0;
unsigned long sendDataPrevMillis = 0;
unsigned long GetDatapreviousMillis = 0; // Biến lưu milis
const long GetDatainterval = 1000;       // Biến lưu khoản thời gian thực hiện
void setup()
{
    init_setup();
}
void loop()
{
    stopWiFiClient();
    authHandler();
    app.loop();
    Database.loop();

    usebutton();
    button0.tick();
    blink_led();
    unsigned long currentTime = millis(); // Thời gian hiện tại

    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        getLoRaData();
    }
    if (mode)
    {
        // Nếu đã đủ thời gian giữa các lần gửi
        if (millis() - lastSendTime_gui1s >= interval_gui1s)
        {
            flag_gui1s = 1;
            digitalWrite(LED1, HIGH);
            digitalWrite(LED2, HIGH);
            digitalWrite(LED3, HIGH);
            lastSendTime_gui1s = millis();
            Serial.println("SEND");
            send_node(expectedNode, "");
            currentRetry++;
        }
        if (flag_gui1s)
        {
            // Gửi dữ liệu cho node tiếp theo
            // Nếu đã thử đủ số lần tối đa cho node hiện tại
            if (node_count_send > 2)
            {
                flag_gui1s = 0;
                node_count_send = 0;
            }
            // Nếu đã đủ thời gian giữa các lần thử cho node hiện tại
            if ((currentRetry < maxRetryCount) && (millis() - lastSendTime >= retryInterval))
            {
                // Đặt lại thời gian lần gửi gần nhất
                lastSendTime = millis();
                Serial.println("RESEND");
                send_node(expectedNode, "");
                // Tăng biến đếm số lần thử cho node hiện tại
                currentRetry++;
            }
            // Nếu đã thử đủ số lần tối đa cho node hiện tại
            else if (currentRetry >= maxRetryCount)
            {
                Serial.print("NO DATA FROM NODE ");
                Serial.println(expectedNode);
                Database.set<int>(aClient, "STT NODE ERROR/" + String(expectedNode), 0);
                String datekey = "Node " + String(expectedNode) + " ERROR";

                JsonWriter writerJson;
                writerJson.create(error, "time", init_time());
                pushJSon(datekey + "/" + init_date(), error);
                if (expectedNode == 1)
                {
                    digitalWrite(LED1, 0);
                }
                else if (expectedNode == 2)
                {
                    digitalWrite(LED2, 0);
                }
                else if (expectedNode == 3)
                {
                    digitalWrite(LED3, 0);
                }
                currentRetry = 0;
                expectedNode = (expectedNode % 3) + 1;

                flag_gui1s = 0;
                node_count_send++;
            }
        }
    }
    else
    {
        if (millis() - GetDatapreviousMillis >= GetDatainterval && app.ready())
        {
            GetDatapreviousMillis = millis();
            bool getdata = Database.get<bool>(aClient, "led1");
            if (datacurrent != getdata)
            {
                Serial.print("\nDữ liệu thay đổi: ");
                Serial.println(getdata);
                if (getdata)
                {
                    digitalWrite(LED1, LOW);
                    sendon1();
                }
                else
                {
                    digitalWrite(LED1, HIGH);
                    sendoff1();
                }
                datacurrent = getdata;
                Serial.println(getdata);
            }
        }
    }
    if (millis() - GetModepreviousMillis >= 3000 && app.ready())
    {
        GetModepreviousMillis = millis();
        mode = Database.get<bool>(aClient, "/MODE/");
        Serial.print("MODE: ");
        Serial.println(mode);
    }
    delay(10);
}

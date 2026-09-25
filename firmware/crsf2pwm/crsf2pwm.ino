#include <Arduino.h>
#include <Servo.h>
#include "CrsfSerial.h"
#include "median.h"
#include "target.h"

//定义通道数量
#define NUM_OUTPUTS 8

//PWM与CRSF通道对应关系表
constexpr int OUTPUT_MAP[NUM_OUTPUTS] = { 1, 2, 3, 4, 5, 6, 7, 8 };

//失控保护定义
constexpr int OUTPUT_FAILSAFE[NUM_OUTPUTS] = {
    1500, 1500, 988, 1500,                  // ch1-ch4
    fsaHold, fsaHold, fsaHold, fsaNoPulses  // ch5-ch8
    };

//PWM硬件端口对应
constexpr PinName OUTPUT_PINS[NUM_OUTPUTS] = { OUTPUT_PIN_MAP };

#define PWM_FREQ_HZ     50
#define VBAT_INTERVAL   500 //电压获取间隔比例
#define VBAT_SMOOTH     5   //电压计算次数
// Scale used to calibrate or change to CRSF standard 0.1 scale
#define VBAT_SCALE      1.1

static HardwareSerial  CrsfSerialStream(DPIN_CRSF_RX,DPIN_CRSF_TX);

static CrsfSerial crsf(CrsfSerialStream);
static int g_OutputsUs[NUM_OUTPUTS];

//static Servo *g_Servos[NUM_OUTPUTS];

uint32_t _lastPacket = 0;

static struct tagConnectionState {
    uint32_t lastVbatRead;
    MedianAvgFilter<unsigned int, VBAT_SMOOTH>vbatSmooth;
    unsigned int vbatValue;

    char serialInBuff[64];
    uint8_t serialInBuffLen;
    bool serialEcho;
} g_State;

static void crsfOobData(uint8_t b)
{
    // A shifty byte is usually just log messages from ELRS
    //
    Serial.write(b);
}

static void crsfLinkUp()
{
    digitalWrite(DPIN_LED, HIGH ^ LED_INVERTED);
}

/**
 * @brief: Initialize a servo pin output for the first time
*/
static void servoPlatformBegin(unsigned int servo)
{
  /*
    Servo *s = new Servo();
    s->attach(OUTPUT_PINS[servo], CRSF_ELIMIT_US_MIN, CRSF_ELIMIT_US_MAX);
    g_Servos[servo] = s;
    */



    //analogWriteFrequency(OUTPUT_PINS[servo], PWM_FREQ_HZ);
    pin_function(OUTPUT_PINS[servo], AIR_PIN_DATA(AIR_MODE_OUTPUT_PP, GPIO_NOPULL, 0));

}

/**
 * @brief: Set an already initialized servo to a usec value
*/
static void servoPlatformSet(unsigned int servo, int usec)
{
  /*
    if (g_Servos[servo] == nullptr)
    {
        Servo *s = new Servo();
        s->attach(OUTPUT_PINS[servo], CRSF_ELIMIT_US_MIN, CRSF_ELIMIT_US_MAX, usec);
        g_Servos[servo] = s;
    }
    else
    {
        g_Servos[servo]->writeMicroseconds(usec);
        if(servo == 4)
        {
          Serial.println(usec);
        }
    }
    */
    pwm_start(OUTPUT_PINS[servo], PWM_FREQ_HZ, usec, MICROSEC_COMPARE_FORMAT);
}

static void servoPlatformEnd(unsigned int servo)
{
    /*
     Servo *s = g_Servos[servo];
     g_Servos[servo] = nullptr;
     s->detach();
     delete s;
     */
    pwm_stop(OUTPUT_PINS[servo]);
    pin_function(OUTPUT_PINS[servo], AIR_PIN_DATA(AIR_MODE_INPUT, GPIO_PULLDOWN, 0));
}

static void servoSetUs(unsigned int servo, int usec)
{
    if (usec > 0)
    {
        // 0 means it was disabled previously, enable OUTPUT mode
        if (g_OutputsUs[servo] == 0)
            servoPlatformBegin(servo);
        servoPlatformSet(servo, usec);
    }
    else
    {
        servoPlatformEnd(servo);
    }
    g_OutputsUs[servo] = usec;
}

static void crsfLinkDown()
{
    digitalWrite(DPIN_LED, LOW ^ LED_INVERTED);

    // Perform the failsafe action
    for (unsigned int out=0; out<NUM_OUTPUTS; ++out)
    {
        if (OUTPUT_FAILSAFE[out] == fsaNoPulses)
            servoSetUs(out, 0);
        else if (OUTPUT_FAILSAFE[out] != fsaHold)
            servoSetUs(out, OUTPUT_FAILSAFE[out]);
        // else fsaHold does nothing, keep the same value
    }
 }

static void packetLinkStatistics(crsfLinkStatistics_t *link)
{
  /*
  Serial.print(link->uplink_RSSI_1, DEC);
  Serial.println("dBm");
  */
}

static void packetChannels()
{
  //--------------------------------------------------------------------
    for (unsigned int out=0; out<NUM_OUTPUTS; ++out)
    {
        const int chInput = OUTPUT_MAP[out];
        int usOutput;
        if (chInput > 0)
            usOutput = crsf.getChannel(chInput);
        else
        {
            // if chInput is negative, invert the channel output
            usOutput = crsf.getChannel(-chInput);
            // (1500 - usOutput) + 1500
            usOutput = 3000 - usOutput;
        }
        servoSetUs(out, usOutput);
    }
    //

    //-----测试接收
    uint32_t t = millis();
    if(  t - _lastPacket > 500)
    {
      _lastPacket = t;
      for (unsigned int ch=1; ch<=8; ++ch)
      {
        Serial.write(ch < 10 ? '0' + ch : 'A' + ch - 10);
        Serial.write('=');
        Serial.print(crsf.getChannel(ch), DEC);
        Serial.write(' ');
      }
      Serial.println();
    }
    //
}

static void checkVbatt()
{
#if defined(APIN_VBAT)
    if (millis() - g_State.lastVbatRead < (VBAT_INTERVAL / VBAT_SMOOTH))
        return;
    g_State.lastVbatRead = millis();

    unsigned int idx = g_State.vbatSmooth.add(analogRead(APIN_VBAT));
    if (idx != 0)
        return;

    unsigned int adc = g_State.vbatSmooth;
    g_State.vbatValue = 500U * adc * (VBAT_R1 + VBAT_R2) / VBAT_R2 / ((1 << 12) - 1) *VBAT_SCALE;
    //g_State.vbatValue = adc /  ((1 << 12) - 1) * VBAT_R2 /

    crsf_sensor_battery_t crsfbatt = { 0 };
    uint16_t scaledVoltage = g_State.vbatValue ;
    // Values are MSB first (BigEndian)
    crsfbatt.voltage = htobe16(scaledVoltage);
    crsf.queuePacket(CRSF_SYNC_BYTE, CRSF_FRAMETYPE_BATTERY_SENSOR, &crsfbatt, sizeof(crsfbatt));

    Serial.print("ADC="); Serial.print(adc, DEC);
    Serial.print(" "); Serial.print(g_State.vbatValue, DEC); Serial.println("V");
#endif // APIN_VBAT
}

static void setupCrsf()
{
    crsf.onLinkUp = &crsfLinkUp;
    crsf.onLinkDown = &crsfLinkDown;
    crsf.onOobData = &crsfOobData;
    crsf.onPacketChannels = &packetChannels;
    crsf.onPacketLinkStatistics = &packetLinkStatistics;
    crsf.begin();
}


static void setupGpio()
{
    pinMode(DPIN_LED, OUTPUT);
    digitalWrite(DPIN_LED, LOW ^ LED_INVERTED);
    analogReadResolution(12);
    //analogWriteResolution(12);


    // The servo outputs are initialized when the
    // first channels packet comes in and sets the PWM
    // output value, to prevent them from jerking around
    // on startup
}

void setup() {
  // put your setup code here, tod:\ZY\RC\elrs\Crsf2PWM\crsf2pwm\CrsfSerial\CrsfSerial.h run once:
  Serial.begin(115200);
  Serial.println("CRSF2PWM(PY32F002A) Start!");

  setupGpio();
  setupCrsf();
}

void loop() {
  // put your main code here, to run repeatedly:
    crsf.loop();
    //---------------------------------------------
    //电池电压检测
    checkVbatt();
    //debug uart口的命令参数处理
    //checkSerialIn();
}

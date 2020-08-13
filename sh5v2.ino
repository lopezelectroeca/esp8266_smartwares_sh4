
#define RX 5

int error = 25;
int _numBits = 32;
int remoteId;
int _channel;
boolean state;
unsigned long _message;
bool _messageReady = false;
int pulsoPre;

boolean ICACHE_RAM_ATTR _checkPulse(long pulseLength, long comp) {
  if ((pulseLength < comp + error) && (pulseLength > comp - error)) {
    return true;
  }
  else {
    return false;
  }

}

void ICACHE_RAM_ATTR _pulseReceived() {

  unsigned long _now = micros();
  static unsigned long lastChange = 0;
  static unsigned int lastPulseLength = 0;
  static int  pulseCounter = 0;
  static int bitCounter = 0;
  static boolean started = false;
  static unsigned long message = 0;

  unsigned long pulseWidth =  _now - lastChange;
  //Serial.println(pulseWidth);

  if (started) {
    if (pulseCounter == 0) {
      pulseCounter++;

      if (_checkPulse(pulseWidth, 265 * 2)) { // 0
        message <<= 1;
      }
      else if (_checkPulse(pulseWidth, 250 * 6)) { // 1
        message <<= 1;
        message |= 1;
      }
      else  {
        started = false;
        bitCounter = 0;
        lastChange = _now;
        pulseCounter = 0;
        message = 0;
        _messageReady = false;
        return;
      }

      if (bitCounter == _numBits - 1) {
        remoteId = message >> 16;
        state = message >> 4 & 1;
        _channel = message & 15;
        bitCounter++;
      } else {
        //rawPulse[bitCounter] = pulseWidth;
        bitCounter++;
      }
    }
    else {
      if (bitCounter == 32) {
        started = false;
        bitCounter = 0;
        message = 0;
        _messageReady = true;
      }
      lastChange = _now;
      pulseCounter = 0;
      return;
    }



  }
  else {
    if (_checkPulse(pulseWidth, 250 * 11)) {
      started = true;
      lastChange = _now;
    }
    else {
      started = false;
      lastChange = _now;
    }
  }


}

void setup() {
  Serial.begin(115200);

  pinMode (RX, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RX), _pulseReceived, RISING);

}

void loop() {

  if (_messageReady) {
    _messageReady = false;
    enviardatos();
  }
}

void enviardatos() {
  Serial.print("Id : ");
  Serial.print(remoteId);
  Serial.print(" / Canal : ");
  Serial.print(_channel);
  Serial.print(" / Estado : ");
  Serial.println(state);

}

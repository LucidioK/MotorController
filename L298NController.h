
#define L298N_MOTOR_A 0
#define L296N_MOTOR_B 1

class L298NController {
public:
  
  L298NController(int enApin, int in1pin, int enBpin, int in2pin) {
    _enApin = enApin;
    _in1pin = in1pin;
    _enBpin = enBpin;
    _in2pin = in2pin;
  }

  L298NController(int enApin, int in1pin) {
    _enApin = enApin;
    _in1pin = in1pin;
    _enBpin = -1;
    _in2pin = -1;
  }

  void Start(int motor) { Start(motor, 255); }

  void Start(int motor, int power) { Start(motor, power, 0); }

  void Start(int motor, int power, int durationInMilliseconds) { Start(motor, power, durationInMilliseconds, true); }

  void Start(int motor, int power, int durationInMilliseconds, bool keepRunning) {
    serialSSI("L298NController.Start", "motor", motor);
    serialSSI("L298NController.Start", "power", power);
    serialSSI("L298NController.Start", "durationInMilliseconds", durationInMilliseconds);
    serialSSI("L298NController.Start", "keepRunning", keepRunning ? 1 : 0);
    digitalWriteInternal(getIN(motor), HIGH);
    analogWriteInternal (getEN(motor), power);
    delay(durationInMilliseconds);
    if (!keepRunning) {
      Stop(motor);
    }
  }

  void Stop(int motor) {
    serialSSI("L298NController.Stop", "motor", motor);
    digitalWriteInternal(getIN(motor), LOW);
    analogWriteInternal (getEN(motor), 0);    
  }

private:
  void digitalWriteInternal(int pin, int value) {
    if (pin >= 0) {
      digitalWrite(pin, value);
    }
  }

  void analogWriteInternal(int pin, int value) {
    if (pin >= 0) {
      analogWrite(pin, value);
    }
  }
  
  int _enApin, _in1pin, _enBpin, _in2pin;
  int getEN(int motor) { return motor == L298N_MOTOR_A ? _enApin : _enBpin; }
  int getIN(int motor) { return motor == L298N_MOTOR_A ? _in1pin : _in2pin; }
};

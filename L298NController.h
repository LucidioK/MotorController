
#define L298N_MOTOR_A 0
#define L296N_MOTOR_B 1

class L298NController {
public:
  
  L298NController(int enApin, int in1pin, int enBpin, int in2pin) {
    _enApin = enApin;
    _in1pin = inApin;
    _enBpin = enBpin;
    _in2pin = in2pin;
  }

  void Start(int motor) {
    Start(motor, 255);
  }

  void Start(int motor, int power) {
    Start(motor, power, 0);
  }

  void Start(int motor, int power, int durationInMilliseconds) {
    Start(motor, power, durationInMilliseconds, true);
  }

  void Start(int motor, int power, int durationInMilliseconds, bool keepRunning) {
    digitalWrite(getIN(motor), HIGH);
    analogWrite (getEN(motor), power);
    delay(durationInMilliseconds);
    if (!keepRunning) {
      Stop(motor);
    }
  }

  void Stop(int motor) {
    digitalWrite(getIN(motor), LOW);
    analogWrite (getEN(motor), 0);    
  }

private:
  int _enApin, _in1pin, _enBpin, _in2pin;
  int getEN(int motor) { return motor == L298N_MOTOR_A ? _enApin : _enBpin; }
  int getIN(int motor) { return motor == L298N_MOTOR_A ? _in1pin : _in2pin; }
}


// Ultrasonic Class
class Ultrasonic {
  public:
    int trigger_pin;
    int echo_pin;
    Ultrasonic(int TRIGGER_PIN, int ECHO_PIN) {
      trigger_pin = TRIGGER_PIN;
      echo_pin = ECHO_PIN;
    }
   

    int get_distance() {
      long duration, ult_distance;

      // clear trigger pin
      digitalWrite(trigger_pin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigger_pin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigger_pin, LOW);

      // get duration in miliseconds
      duration = pulseIn(echo_pin, HIGH, 20000);

      // converet to cm
      ult_distance = (duration / 29) / 2;  // Calculating the ult_distance

      return ult_distance;
    }
};

// Decide which ultrasonic (left, right or neither) to use
String ultrasonic_side_chooser(int left_distance, int right_distance, std::vector<int> left_distance_history, std::vector<int> right_distance_history, int FIELD_WIDTH, int ULTRASONIC_TO_ROBOT) {
    bool sensor_blocked;
    
    // determine if one of the sensors is being blocked by an obstical
    if ((left_distance + right_distance + 2 * ULTRASONIC_TO_ROBOT) < FIELD_WIDTH - 30) {
      sensor_blocked = true;
    }

    else {
      sensor_blocked = false;
    }

    if (sensor_blocked) {
      
      // average left and right distances

      int common_left_distance = Average(left_distance_history);
      int common_right_distance = Average(right_distance_history);

      // determine which (if any) sensor to believe

      // right sensor blocked
      if (right_distance < common_right_distance - 10 and not (left_distance < common_left_distance - 10)) {
        return "LEFT";
      }

      // left sensor blocked
      else if (left_distance < common_left_distance - 10 and not (right_distance < common_right_distance - 10)) {
        return "RIGHT";
      }

      // both blocked
      else {
        return "NONE";
      }

    }
    
    // if sensor is not blocked return sensor with shortest ult_distance
    else {
      if (left_distance < right_distance) {
        return "LEFT";
      }

      else {
        return "RIGHT";
      }
    }
}

// find true ult_distance due to tilt
int Get_True_Distance(int ult_distance, float triangle_tilt) {
  return round(cos(Deg_To_Rad(triangle_tilt)) * ult_distance);
}


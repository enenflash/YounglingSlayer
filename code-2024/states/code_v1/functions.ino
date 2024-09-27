// check if value is in vector
bool in(std::vector<int> var, int element) {
  std::vector<int>::iterator it = find(var.begin(), var.end(), element);

  if (it != var.end()) {
    return true;
  }

  else {
    return false;
  }
}

// find most common value in vector
int Most_Common(std::vector<int> vec){
  int Count = 0;
  int item = 0;

  for (unsigned i = 0; i<vec.size(); i++) {
    if (count(vec.begin(), vec.end(), vec[i]) > Count) {
      item = vec[i];
      Count = count(vec.begin(), vec.end(), vec[i]);
    }
  }
  return item;
  
}

// find average of vector
int Average(std::vector<int> vec) {
  
  float sum = accumulate(vec.begin(), vec.end(), 0.0f);

  return round(sum/vec.size());
}

// convert between degrees and radians
float Deg_To_Rad(float degree) {
  return (degree*M_PI)/180;
}

vector<float> get_xy(int direction, int ball_distance, int ROBOT_TO_BALL, int ROBOT_TO_BALL_MULTIPLY, std::map<int, int> ir_angles) {
  float x = 0;
  float y = 0;

  if (direction == 0) {
    return {0, 0};
  }

  // if ball behind robot move to side instead
  if (in({ 2, 10, 11, 1, 3, 9 }, direction)) {
    if (ball_distance < MIN_BALL_DISTANCE || in({ 3, 9 }, direction)) {
      y = sin(Deg_To_Rad(ir_angles[direction])) * ball_distance - ROBOT_TO_BALL;
    }
    
    else {
      y = sin(Deg_To_Rad(ir_angles[direction])) * ball_distance - round(ROBOT_TO_BALL / 1.5);
    }
  }
  else {
    y = sin(Deg_To_Rad(ir_angles[direction])) * ball_distance;
  }

  if (in({ 4, 5, 6 }, direction)) {
    x = cos(Deg_To_Rad(ir_angles[direction])) * ball_distance + (ROBOT_TO_BALL * ROBOT_TO_BALL_MULTIPLY);
  }
  else if (in({ 7, 8 }, direction)) {
    x = cos(Deg_To_Rad(ir_angles[direction])) * ball_distance - (ROBOT_TO_BALL * ROBOT_TO_BALL_MULTIPLY);
  }
  else if (in({ 3, 9, 10, 2, 1, 11 }, direction)) {
    x = cos(Deg_To_Rad(ir_angles[direction])) * ball_distance;
  }

  else if (direction == 12) {
    x = 0;
  }

  return { x, y };
}

bool getIdle() {
  int switchState = digitalRead(SWITCH_PIN);
  // return true when idle switch is off (as in the robot should be idle)
  // return false when idle switch is on (as in the robot should run)
  return (switchState == LOW);
};



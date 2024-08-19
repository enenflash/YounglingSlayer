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

// find max value
float find_max(float array[], int size) {
  float max = array[0];
  for (int i = 1; i < size; ++i) {
    if (array[i] > max) {
      max = array[i];
    }
  }

    return max;
}

// find minimum value
float find_min(float array[], int size) {
  float min = array[0];
  for (int i = 1; i < size; ++i) {
    if (array[i] < min) {
      min = array[i];
    }
  }

    return min;
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

int Average(std::vector<int> vec) {
  int total = 0;
  for (unsigned i; i = 0; i < vec.size()) {
    total = total + vec[i];
  }

  return round(total/3.0);
}

// convert between degrees and radians
float Deg_To_Rad(float degree) {
  return (degree*M_PI)/180;
}

float Rad_To_Deg(float radians) {
  return (radians*180)/M_PI;
}

// get the x,y coordinants to head
vector<float> get_xy(int direction, int ball_distance, int ROBOT_TO_BALL, int ROBOT_TO_BALL_MULTIPLY, std::map<int, int> ir_angles) {
  float x = 0;
  float y = 0;

  // if ball behind robot move to side instead
  if (in({ 2, 10, 11, 1, 3, 9 }, direction)) {
    if (ball_distance < 23 or in({ 3, 9 }, direction)) {
      y = sin(Deg_To_Rad(ir_angles[direction])) * ball_distance - ROBOT_TO_BALL;
    }

    else {
      y = sin(Deg_To_Rad(ir_angles[direction])) * ball_distance - (ROBOT_TO_BALL / 1.5);
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

  else if (direction == 0) {
    x = 0;
    y = 0;
  }

  else if (direction == 12) {
    x = 0;
  }

  return { x, y };
}

// check status of idle switch
bool getIdle() {
  int switchState = digitalRead(SWITCH_PIN);
  // return true when idle switch is off (as in the robot should be idle)
  // return false when idle switch is on (as in the robot should run)
  return (switchState == LOW);
};

// return string position of robot
String Find_Position(int distance, int FIELD_WIDTH) {
  int Half_width = FIELD_WIDTH / 2;

  if (distance >= Half_width - 22 and distance <= Half_width + 22) {return "CENTER";}
  else if (distance < Half_width - 22) {return "LEFT";}
  else {return "RIGHT";}
}

// calculate how much to tilt to goal
float Get_Goal_Tilt(int true_back_distance, int distance, String pos, int FIELD_HEIGHT, int ULTRASONIC_TO_ROBOT, int FIELD_WIDTH) {
  if (pos == "CENTER") {
    return 0;
  }
  
  else if (pos == "RIGHT") {
    int front_distance = FIELD_HEIGHT - true_back_distance;
    int distance_from_center = (FIELD_WIDTH / 2) - (FIELD_WIDTH - 2*ULTRASONIC_TO_ROBOT - distance);
    float goal_rad_angle = atan(distance_from_center/front_distance);
    float goal_deg_angle = Rad_To_Deg(goal_rad_angle);
    return goal_deg_angle;

  }

  else {
    int front_distance = FIELD_HEIGHT - true_back_distance;
    int distance_from_center = FIELD_WIDTH - distance;
    float goal_rad_angle = atan(distance_from_center/front_distance);
    float goal_deg_angle = Rad_To_Deg(goal_rad_angle);
    return -goal_deg_angle; 

  }
}
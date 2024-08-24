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

float Rad_To_Deg(float radians) {
  return (radians*180)/M_PI;
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


// return string position of robot
String Find_Position(int distance, int FIELD_WIDTH, int ULTRASONIC_TO_ROBOT) {
  int Half_width = FIELD_WIDTH / 2;

  if ((distance >= Half_width - 2 * ULTRASONIC_TO_ROBOT and distance <= Half_width + 2 * ULTRASONIC_TO_ROBOT) or distance == 808) {return "CENTER";}
  else if (distance < Half_width - 2 * ULTRASONIC_TO_ROBOT) {return "LEFT";}
  else {return "RIGHT";}
}

// calculate how much to tilt to goal
float Get_Goal_Tilt(int true_back_distance, int distance, String pos, int FIELD_HEIGHT, int ULTRASONIC_TO_ROBOT, int FIELD_WIDTH, int direction, int strength) {
  if (direction == 0) {
    return 0;
  }
  
  if (pos == "CENTER" or strength > 28 or in({9,8,7,6,5,4,3}, direction) or distance == 808) {
    return 0;
  }
  
  else if (pos == "RIGHT" and strength < 28 and in({1,2,12}, direction)) {
    int front_distance = FIELD_HEIGHT - true_back_distance;
    int distance_from_center = (FIELD_WIDTH / 2) - (FIELD_WIDTH - 2 * ULTRASONIC_TO_ROBOT - distance);
    float goal_rad_angle = atan(distance_from_center/(front_distance + 0.0));
    float goal_deg_angle = Rad_To_Deg(goal_rad_angle);
    return -goal_deg_angle;
  }

  else if (pos == "LEFT" and strength < 28 and in({11,10,12}, direction)) {
    int front_distance = FIELD_HEIGHT - true_back_distance;
    int distance_from_center = (FIELD_WIDTH/2) - distance;
    float goal_rad_angle = atan(distance_from_center/(front_distance + 0.0));
    float goal_deg_angle = Rad_To_Deg(goal_rad_angle);
    return goal_deg_angle; 
  }

  else {
    return 0;
  }
}

// stay within lines
float Line_Check(int distance, float x) {
  // error distance
  //if (distance == 808) {return x;}
  
  // left side
  if (distance < 35 and x > 0 and distance != 0) {
      return -x;
  }

  // right side
  else if (distance > 145 and x < 0) {
    return -x;
  }

  // neither
  return x;
}



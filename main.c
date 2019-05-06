//SailingQuarks^2 ISS FINAL v2.2 - with tugging integration
 
#define TUGGING 1
#define HOOKING 0
#define MOVING_TIME 5
 
void hook_angle(){
    float hookEuler[3];
    hookEuler[0] =-otherEulerState[6]+3.14159268f;
    hookEuler[1] =-otherEulerState[7];
    hookEuler[2] = otherEulerState[8]+3.14159268f+0.03;
    if(getDist(targetPos,&myEulerState[0])<.03 || !firstPhaseHook)
    {
        hookEuler[0] =-otherEulerState[6]+3.14159268f;
        hookEuler[1] =-otherEulerState[7];
        hookEuler[2] = otherEulerState[8]+3.14159268f;
        float holder_a[3], holder_b[3];
        for(int i=0;i<3; i++)
        {
            holder_a[i] = myZRState[i+3];
            holder_b[i] = otherZRState[i+3];
        }
        float holder_c[3];
        mathVecSubtract(holder_c, holder_a,holder_b,3);
        if(fabs(myEulerState[9] + otherEulerState[9]) < 0.0065 && targetCount>=4 && getDist(targetPos, myPos) < .007 
        && mathVecMagnitude(holder_c,3)<.01)
        {
            secondPhaseHook=false;
            phase = TUGGING;
            DEBUG(("SUPERHERE"));
        }
        DEBUG(("HERE"));
    }
    //api.setPositionTarget(startP);
    game.setEulerTarget(hookEuler);
}
void updateMovingAverage(){
     for(int i = 0; i < 3; i++)
     {
       movingOtherPositionAverage[i][api.getTime()%5] = otherZRState[i];
       movingOtherPositionAverage[i][5] = 0;
       for(int j = 0; j < 5; j++)
       {
         movingOtherPositionAverage[i][5] += movingOtherPositionAverage[i][j];
       }
       movingOtherPositionAverage[i][5] /= 5;
     }
}
void setHooking0Pos(){
    float position[5][3] = {
                              {0.38, 0.02, 0.005},
                              {0.37, 0.04, 0.005},
                              {0.32, 0.04, 0.005},
                              {0.32, 0.005, 0.00},
                              {0.32, 0.005, 0.00}
                            };
 
    for(int i = 0; i < 3; i++)
      targetPos[i] = position[targetCount][i];
    targetPos[2] = targetPos[2] + 0.003;
    inTermsOfTarget(targetPos);
 
 
    if(getDist(targetPos, myPos) < 0.03)
      targetCount++;
 
    if(targetCount > 4)
      targetCount = 4;
 
    moveTo(targetPos, 0.006f);
}
void inTermsOfTarget(float v[3]){
    DEBUG(("------------"));
    DEBUG(("Target %d", targetCount));
 
    rot(v, otherEulerState[6], otherEulerState[7], otherEulerState[8]);
    for(int i = 0; i < 3; i++)
    {
      v[i] += movingOtherPositionAverage[i][5];
    }
}
void rot(float v[3], float angleYZ, float angleXZ, float angleXY){
    float xN;
    float yN;
    float zN;
    float xO = v[0];
    float yO = v[1];
    float zO = v[2];
 
    float savedCos;
    float savedSin;
 
    savedCos = cosf(angleYZ);
    savedSin = sinf(angleYZ);
    yN = (yO * savedCos) - (zO * savedSin);
    zN = (yO * savedSin) + (zO * savedCos);
    yO = yN;
    zO = zN;
 
    savedCos = cosf(angleXZ);
    savedSin = sinf(angleXZ);
    zN = (zO * savedCos) - (xO * savedSin);
    xN = (zO * savedSin) + (xO * savedCos);
    zO = zN;
    xO = xN;
 
    savedCos = cosf(angleXY);
    savedSin = sinf(angleXY);
    xN = (xO * savedCos) - (yO * savedSin);
    yN = (xO * savedSin) + (yO * savedCos);
    xO = xN;
    yO = yN;
 
    v[0] = xN;
    v[1] = yN;
    v[2] = zN;
}
void tow(){
   
    float speed = 0.08;
    if(towClock >= 5 && otherZRState[4] < 0.015) //y vel
    {
        init();
        return;
    }
   
    float target[3];
   
    /*if(towStart)
    {
        speed = 0.03;
        towStart = false;
    }*/
   
    target[0] = 0;
    target[1] = 0.2;
    target[2] = 0;
    float hookEuler[3];
 
    hookEuler[0] =-otherEulerState[6]+3.14159268f;
    hookEuler[1] =-otherEulerState[7];
    hookEuler[2] = otherEulerState[8]+3.14159268f;
    /*if(myZRState[1] > 0.05)
    {
        speed = 0.035;
    }*/
   
    if(myZRState[1] > 0.2)
    {
        target[0] = 0;
        target[1] = 0;
        target[2] = -1;
       rot(target, myEulerState[6], myEulerState[7], myEulerState[8]);
        target[1] = otherZRState[4]-.075;
    }
    api.setVelocityTarget(target);
    float holderAttRate[3];
    for(int i=0;i<3;i++)
    {
        holderAttRate[i] = -otherZRState[i+9];
    }
    api.setAttRateTarget(holderAttRate);
    //game.setEulerTarget(hookEuler);
 
    towClock++;
}
void moveTo(float target[3], float speed){
   
    float myState[12];//Our information
    float futureToTarget[3];//vector between target and future position
    float vectToSetMag;//multiplier, function of distance over time.
   
    api.getMyZRState(myState);
   
    for(int y=0;y<3;y++){
        futureToTarget[y]=myState[y]+myState[y+3];//myposition+myVelocity
    }
   
    mathVecSubtract(futureToTarget, target, futureToTarget, 3);//target-futureposition=futuretotarget
    vectToSetMag = fabsf(sqrt(speed*mathVecNormalize(futureToTarget, 3)));//function that we got from graphing SPHERE movement
    //can change number(0.007) to get faster but more fuel, or slower and less fuel.
    for(int i=0;i<3;i++){
        futureToTarget[i]*=vectToSetMag;
    }
    api.setVelocityTarget(futureToTarget);
}
float getDist(float a[3], float b[3]){ //distance
    float c[3];
    mathVecSubtract(c, a, b, 3);
    return mathVecMagnitude(c, 3);
}
 
//Immediate towing fix after hooking by Jeffrey
 
float movingOtherPositionAverage[3][6];
//
int targetCount;
 
 
//General
float myZRState[12];
float otherZRState[12];
float myEulerState[13];
float otherEulerState[13];
int phase;
bool mode;
//phase 1&2
 
 
//hooking helper
float savedHookEuler[3];
 
//phase hook
bool beganHooking;
float myPos[3];
//moving average
float targetPos[3];
float movingAverage[MOVING_TIME];
//PID
/*float previous_error;
 
float error;
float integral;
float derivative;*/
 
//hook_MS
bool firstPhaseHook;
bool secondPhaseHook;
//
 
//hook_pH
 
 
//phase tow
float hookEuler[3];
bool towStart;
int towClock;
//
 
void init(){
    //general
       api.getMyZRState(myZRState);
       api.getOtherZRState(otherZRState);
       game.getMyEulerState(myEulerState);
       game.getOtherEulerState(otherEulerState);
       phase = 0;
       for(int i = 0; i < 3; i++)
       myPos[i] = myZRState[i];
 
    for(int i = 0; i < 6; i++)
      for(int j = 0; j < 3; j++)
      {
        movingOtherPositionAverage[j][i] = otherZRState[j];
        targetPos[j] =0;
      }
   
    //phase hook
            //PID
            /*integral = 0;
            previous_error = 0;
            derivative = 0;*/
           
    //hook_MS
    //
    //hook_pH
   
    beganHooking= false;
    targetCount = 0;
    mode = false;
    firstPhaseHook = true;
    secondPhaseHook = true;
    //phase tow
    towClock = 0;
   
}
 
void loop(){
    api.getMyZRState(myZRState);
    api.getOtherZRState(otherZRState);
    game.getMyEulerState(myEulerState);
    game.getOtherEulerState(otherEulerState);
    updateMovingAverage();
    for(int i = 0; i < 3; i++)
      myPos[i] = myZRState[i];
    /*movingAverage[(api.getTime()-1)%MOVING_TIME] = otherZRState[9];
    if(api.getTime()==10)
    {
        if(fabsf(otherZRState[0]+otherZRState[1]+otherZRState[2]+.05)<.01)//assumes max is  
            mode =true;
    }
    if(!mode)
    {
        DEBUG(("light"));
       
    }
    else
    {
        DEBUG(("bird image"));
        hook_mode1();
    }*/
    if(phase == 0)
    {
        hook_angle();
        setHooking0Pos();
    }
    else
    {
        tow();
    }
}


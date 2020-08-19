//Begin page hooking
void hook()
{
    float k;
    k=2.5f;
    float otherZRState[12];

    float myEulerState[13];
    float otherEulerState[13];
    float me[12];
    
    api.getOtherZRState(otherZRState);
    api.getMyZRState(me);
    
    game.getMyEulerState(myEulerState);
    game.getOtherEulerState(otherEulerState);
    
    float hookEuler[3];
    float startP[3];
    
    float speed;
    
    hookEuler[0] = -otherEulerState[6] -otherZRState[9]*k;
    hookEuler[1] = -otherEulerState[7];
    hookEuler[2] = otherEulerState[8] + 3.14159268f;
    
    float correctEuler[3];

    correctEuler[0] = 0.0f;
    correctEuler[1] = 0.15f;
    correctEuler[2] = 0.0f;
    
    
    if(getDist(me, otherZRState) < (2*0.17095f + 0.1f))// - 0.03 * 5;
    {
        startP[0] = otherEulerState[0] + otherZRState[6] * (2*0.17095f - 0.013f);
        startP[1] = otherEulerState[1] + otherZRState[7] * (2*0.17095f - 0.013f);
        startP[2] = otherEulerState[2] + otherZRState[8] * (2*0.17095f - 0.013f);
        
        correctEuler[1] = (getDist(me, otherZRState) - 2*0.17095f) * 1.304;
        if(correctEuler[1]<0.0f  || !firstPhaseHook)
        {
            correctEuler[1] = 0.0f;
            
            startP[0] = otherEulerState[0] + otherZRState[6] * (2*0.17095f);
            startP[1] = otherEulerState[1] + otherZRState[7] * (2*0.17095f);
            startP[2] = otherEulerState[2] + otherZRState[8] * (2*0.17095f);
            
            firstPhaseHook=false;
        }
        
        
        speed = 0.003;
        
        moveTo(startP, speed);
    }
    
    DEBUG(( "correctEuler[1] %f",  correctEuler[1]));
    
    float hookQuat[4], correctQuat[4];

    game.eulerToQuaternion(hookEuler, hookQuat);
    
    game.eulerToQuaternion(correctEuler, correctQuat);

    float wynikQuat[4];
    quatMult(wynikQuat, hookQuat, correctQuat);

    float wynikEuler[3];
    game.quaternionToEuler(wynikQuat, wynikEuler);
    
    game.setEulerTarget(wynikEuler);
}
//End page hooking
//Begin page main
//SailingQuarks^2 ISS FINAL v0.1

//sneakymove
float dCCounter;

//hook_MS
bool firstPhaseHook, secondPhaseHook;
int deltaTime;
//tow
bool towStart;

void init()
{
    //sneakymove
    dCCounter = 0;
    //hook_MS
    firstPhaseHook = true;
    secondPhaseHook = true;
    towStart = true;
}

void loop() 
{
       
    switch(game.getGamePhase())
    {
        case 3:
        {
            SneakyMove();
            hook();
            break;
        }
        case 4:
        {
            tow();
            break;
        }
    }
}
//End page main
//Begin page other
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
//End page other
//Begin page sneaky move
void SneakyMove()
{
    float myZRState[12];
    float otherZRState[12];
    float otherEulerState[13];
    
    float dest[3] = {0.0f, 0.0f, 0.0f};
    
    api.getMyZRState(myZRState);
    api.getOtherZRState(otherZRState);
    game.getOtherEulerState(otherEulerState);
        
    float speed = 0.02;//0.025
    
    if(myZRState[1] < 0.4)//0.2 Idealnie dla popszedniego
    {
        speed = 0.007;
        
        dest[0] = (otherEulerState[0] + otherZRState[6] * (2*0.17095f));
        dest[1] = -0.155f;
        dest[2] = (otherEulerState[2] + otherZRState[8] * (2*0.17095f));
    }
    
    moveTo(dest, speed);
}
//End page sneaky move
//Begin page towing
void tow()
{
    float myZRState[12];
    float otherEulerState[13];
    
    api.getMyZRState(myZRState);
    game.getOtherEulerState(otherEulerState);
    
    float target[3];
    float speed = 0.2;
    
    
    if(towStart)
    {
        speed = 0.1;
        towStart = false;
    }
    
    
    target[0] = myZRState[0] + myZRState[6] * (2*0.17095f);
    target[1] = 0.4;
    target[2] = myZRState[2] + myZRState[8] * (2*0.17095f);
    
    if(myZRState[1] > 0.05)
    {
        speed = 0.1;
    }
    
    if(myZRState[1] > target[1])
    {
        target[0] = fabsf(otherEulerState[6]) / -otherEulerState[6];
        target[1] = 1;
        target[2] = 1;
        
        speed = 0.25;
    }
    
    moveTo(target, speed);

}
//End page towing





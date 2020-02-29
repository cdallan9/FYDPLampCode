//------------------------------------------//
//                Pin setup
//------------------------------------------//

// Digital input pin
int ambientFocusSwitch = A4;

//Analog input pin
int brightness = A3;

// Pull these pin high for on, low for off
int rgbOnOff = A2;
int coldOnOff = A1;
int warmOnOff = A0;

// These control values as PWM
int blueBrightness = D1;
int otherBrightness = D0;

//------------------------------------------//
//               Custom Types
//------------------------------------------//

// Current supported timezones
enum timezoneType
{
    NONE,
    EST,
    PST,
    AST,
};

//------------------------------------------//
//              Global Variables
//------------------------------------------//

// Start this as invalid timezone
timezoneType userTimezone = NONE;

//------------------------------------------//
//              Init functions
//------------------------------------------//

void setupPins()
{
    // Analog input
    pinMode(brightness, AN_INPUT);
    
    // Digital input
    pinMode(ambientFocusSwitch, INPUT_PULLUP);

    // Digital outputs
    pinMode(rgbOnOff, OUTPUT);
    pinMode(coldOnOff, OUTPUT);
    pinMode(warmOnOff, OUTPUT);
    
    // PWM outputs
    pinMode(blueBrightness, OUTPUT);
    pinMode(otherBrightness, OUTPUT);
    
    // FOR DEBUGGING PURPOSES
    pinMode(D7, OUTPUT);
}

int cloudSetTimezone(String timezone)
{
    // Expand this to support more timezones
    if (timezone == "EST")
    {
        userTimezone = EST;
        
        // FOR DEBUGGING PURPOSES
        digitalWrite(D7, HIGH);
        return 1;
    }
    else 
    {
        // FOR DEBUGGING PURPOSES
        digitalWrite(D7, LOW);
        return 0;
    }
}

void setupCloudFunctions()
{
    Particle.function("SetTimezone", cloudSetTimezone);
}

//------------------------------------------//
//              Main init
//------------------------------------------//
void setup()
{
    // Set everything up
    setupPins();
    setupCloudFunctions();
    
    digitalWrite(rgbOnOff, HIGH);
    digitalWrite(coldOnOff, HIGH);
    digitalWrite(warmOnOff, HIGH);
    
    // Initialize all global values to their starting values
}

int blueVal = 0;

int prevARead = 0;

int prevDRead = HIGH;
int lastDRead = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int brightnessVal = 0;

int state = 0;

//------------------------------------------//
//              Main loop
//------------------------------------------//
void loop() 
{
    int aRead = analogRead(brightness);
    //Serial.printlnf("aRead: %d prevARead %d\n delta: %d", aRead, prevARead, abs(aRead - prevARead));

    if (abs(aRead - prevARead) > 60)
    {
        prevARead = aRead;
        float norm = aRead / 4095.0;
        brightnessVal = (int)(255 * norm);
        
        analogWrite(otherBrightness, brightnessVal, 200);
        analogWrite(blueBrightness, brightnessVal, 200);
        
        Serial.printlnf("Setting brigntness to %d\n", brightnessVal);
    }
    
    int dRead = digitalRead(ambientFocusSwitch);
    
    if (dRead != lastDRead)
    {
        lastDebounceTime = millis();
    }
    
    if ((millis() - lastDebounceTime) > debounceDelay) 
    {
        if (dRead != prevDRead) 
        {
            prevDRead = dRead;
            
            if (prevDRead == 0)
            {
                state++;
                
                state = state % 3;
                Serial.printlnf("Switching state to %d\n", brightnessVal);
            }
        }
    }
    
    lastDRead = dRead;
    
    if (state == 0)
    {
        digitalWrite(coldOnOff, HIGH);
        digitalWrite(warmOnOff, HIGH);

        digitalWrite(rgbOnOff, LOW);
    }
    else if(state == 1)
    {
        digitalWrite(warmOnOff, HIGH);
        digitalWrite(rgbOnOff, HIGH);

        digitalWrite(coldOnOff, LOW);
    }
    else if(state == 2)
    {
        digitalWrite(coldOnOff, HIGH);
        digitalWrite(rgbOnOff, HIGH);

        digitalWrite(warmOnOff, LOW);
    }
}
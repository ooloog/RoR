#define LED 13
#define BAUDRATE 57600
#define DEBUGOUTPUT 0

#define GREENLED1  3
#define YELLOWLED1  4
#define REDLED1  5
#define REDLED2 6
#define FAN1 7
#define FAN2 8
#define FAN3 9


#define powercontrol 10

// checksum variables
byte generatedChecksum = 0;
byte checksum = 0; 
int payloadLength = 0;
byte payloadData[64] = {0};
byte poorQuality = 0;
byte attention = 0;
byte meditation = 0;

// system variables
long lastReceivedPacket = 0;
boolean bigPacket = false;


void setup() 

{
  pinMode(GREENLED1, OUTPUT);
  pinMode(YELLOWLED1, OUTPUT);
  pinMode(REDLED1, OUTPUT);
  pinMode(REDLED2, OUTPUT);
  pinMode(FAN1, OUTPUT);
  pinMode(FAN2, OUTPUT);
  pinMode(FAN3, OUTPUT);


  pinMode(LED, OUTPUT);
  Serial.begin(BAUDRATE);           // USB
     
            digitalWrite(GREENLED1,LOW);
            digitalWrite(YELLOWLED1,LOW);
            digitalWrite(REDLED1,LOW);
            digitalWrite(REDLED2,LOW);
            digitalWrite(FAN1,LOW);
            digitalWrite(FAN2,LOW);
            digitalWrite(FAN3,LOW);
}


byte ReadOneByte() 

{
  int ByteRead;
  while(!Serial.available());
  ByteRead = Serial.read();

#if DEBUGOUTPUT  
  Serial.print((char)ByteRead);   // echo the same byte out the USB serial (for debug purposes)
#endif

  return ByteRead;
}

/////////////
//MAIN LOOP//
/////////////
void loop() 

{
  // Look for sync bytes
  if(ReadOneByte() == 170) 
  {
    if(ReadOneByte() == 170) 
    {
        payloadLength = ReadOneByte();
      
        if(payloadLength > 169)                      //Payload length can not be greater than 169
        return;
        generatedChecksum = 0;        
        for(int i = 0; i < payloadLength; i++) 
        {  
        payloadData[i] = ReadOneByte();            //Read payload into memory
        generatedChecksum += payloadData[i];
        }   

        checksum = ReadOneByte();                      //Read checksum byte from stream      
        generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum

        if(checksum == generatedChecksum) 
        {    
          poorQuality = 200;
          attention = 0;
          meditation = 0;

          for(int i = 0; i < payloadLength; i++) 
          {                                          // Parse the payload
          switch (payloadData[i]) 
          {
          case 2:
            i++;            
            poorQuality = payloadData[i];
            bigPacket = true;            
            break;
          case 4:
            i++;
            attention = payloadData[i];                        
            break;
          case 5:
            i++;
            meditation = payloadData[i];
            break;
          case 0x80:
            i = i + 3;
            break;
          case 0x83:
            i = i + 25;      
            break;
          default:
            break;
          } // switch
        } // for loop

#if !DEBUGOUTPUT

        // *** Add your code here ***

        if(bigPacket) 
        {
          if(poorQuality == 0)
             digitalWrite(LED, HIGH);
          else
            digitalWrite(LED, LOW);
          
          Serial.print("PoorQuality: ");
          Serial.print(poorQuality, DEC);
          Serial.print(" Attention: ");
          Serial.print(attention, DEC);
          Serial.print(" Time since last packet: ");
          Serial.print(millis() - lastReceivedPacket, DEC);
          lastReceivedPacket = millis();
          Serial.print("\n");
  
            if (attention>0 && attention<30)//start here
            {
            digitalWrite(GREENLED1,HIGH);
            digitalWrite(YELLOWLED1,LOW);
            digitalWrite(REDLED1,LOW);
            digitalWrite(REDLED2,LOW);
            digitalWrite(FAN1,HIGH);
            digitalWrite(FAN2,HIGH);
            digitalWrite(FAN3,HIGH);
            }
            else if (attention>=30 && attention<45){
            digitalWrite(GREENLED1,LOW);
            digitalWrite(YELLOWLED1,HIGH);
            digitalWrite(REDLED1,LOW);
            digitalWrite(REDLED2,LOW);
            digitalWrite(FAN1,LOW);
            digitalWrite(FAN2,HIGH);
            digitalWrite(FAN3,HIGH);
            }
            else if (attention>=45 && attention<=60){
            digitalWrite(GREENLED1,LOW);
            digitalWrite(YELLOWLED1,LOW);
            digitalWrite(REDLED1,HIGH);
            digitalWrite(REDLED2,LOW);
            digitalWrite(FAN1,HIGH);
            digitalWrite(FAN2,LOW);
            digitalWrite(FAN3,HIGH);
            }
            else if (attention>60 && attention<=75)
            {
            digitalWrite(GREENLED1,LOW);
            digitalWrite(YELLOWLED1,LOW);
            digitalWrite(REDLED1,LOW);
            digitalWrite(REDLED2,HIGH);
            digitalWrite(FAN1,HIGH);
            digitalWrite(FAN2,HIGH);
            digitalWrite(FAN3,LOW);
            }
            else if (attention>75 && attention<=85)
            {
            digitalWrite(GREENLED1,LOW);
            digitalWrite(YELLOWLED1,LOW);
            digitalWrite(REDLED1,HIGH);
            digitalWrite(REDLED2,HIGH);
            digitalWrite(FAN1,HIGH);
            digitalWrite(FAN2,LOW);
            digitalWrite(FAN3,LOW);
            }
            else if (attention>85 && attention<=100)
            {
            digitalWrite(GREENLED1,LOW);
            digitalWrite(YELLOWLED1,HIGH);
            digitalWrite(REDLED1,HIGH);
            digitalWrite(REDLED2,HIGH);
            digitalWrite(FAN1,LOW);
            digitalWrite(FAN2,LOW);
            digitalWrite(FAN3,LOW);
            }               
        }
#endif        
        bigPacket = false;        
      }
      else {
        // Checksum Error
      }  // end if else for checksum
    } // end if read 0xAA byte
  } // end if read 0xAA byte
}

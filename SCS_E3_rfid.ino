/* Author: Benjamin Low
 *
 * Description:
 *
 *  Code adapted from http://bildr.org/2011/02/rfid-arduino/
 *
 *  Uses the Sparkfun ID-20 RFID reader connected to an Arduino. See above
 *  URL link for the wiring connections. There are four LEDs soldered on board.
 *  which you can blink if any of the pre-registered RFID tags are read.
 *
 * Last updated: 2 Dec 2015
 */

//used to identify the reader: 1 = audio, 2 = video, 3 = watchout
//MUST ENTER THIS NUMBER DIFFERENTLY FOR THE 3 ARDUINOS
const int THIS_READER_NUM = 1;

//Register your RFID tags here
char tag_A1[13] = "4D006A6E4D04";
char tag_A2[13] = "50009EF2023E";
char tag_A3[13] = "4B0082BBA7D5";
char tag_V1[13] = "50009EC3676A";
char tag_V2[13] = "4B0082C35359";
char tag_V3[13] = "50009EC32E23";
char tag_W1[13] = "50009EC6E1E9";
char tag_W2[13] = "50009E94FDA7";
char tag_W3[13] = "50009EEA91B5";

String message_out, prev_message_out; //the string sent out to the Processing bridge

const long LED_DELAY = 250; //blink speed
const long RESET_DELAY = 150; //refresh rate of the reader

long curr_led1_time, curr_led2_time, curr_led3_time, curr_led4_time;
boolean is_led1_on, is_led2_on, is_led3_on, is_led4_on;

int RFIDResetPin = 13;

void setup() {
  Serial.begin(9600);

  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);

  //LEDs
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
}

void loop() {

  char tagString[13];
  int index = 0;
  boolean reading = false;

  while (Serial.available()) {

    int readByte = Serial.read(); //read next available byte

    if (readByte == 2) reading = true; //begining of tag
    if (readByte == 3) reading = false; //end of tag

    if (reading && readByte != 2 && readByte != 10 && readByte != 13) {
      //store the tag
      tagString[index] = readByte;
      index ++;
    }
  }

  checkTag(tagString); //Check if it is a match and updates the message_out string

  if (message_out != prev_message_out) {
    Serial.println(message_out);
    prev_message_out = message_out;
  }

  clearTag(tagString); //Clear the char of all value

  resetReader(); //reset both RFID readers
}

void checkTag(char tag[]) {
  /*
  Check the read tag against known tags
  You can use lightLED(pin_number) to test for the tags.
  Use pins 2,3,4,5 for this.
  */

  //Serial.println(tag);

  if (strlen(tag) == 0) {

    formulate_message("00"); //no tag is on the reader

  } else if (compareTag(tag, tag_A1)) {

    formulate_message("a1");

  } else if (compareTag(tag, tag_A2)) {

    formulate_message("a2");

  } else if (compareTag(tag, tag_A3)) {

    formulate_message("a3");

  } else if (compareTag(tag, tag_V1)) {

    formulate_message("v1");

  } else if (compareTag(tag, tag_V2)) {

    formulate_message("v2");

  } else if (compareTag(tag, tag_V3)) {

    formulate_message("v3");

  } else if (compareTag(tag, tag_W1)) {

    formulate_message("w1");

  } else if (compareTag(tag, tag_W2)) {

    formulate_message("w2");

  } else if (compareTag(tag, tag_W3)) {

    formulate_message("w3");

  } else {

    message_out = "unknown tag: ";
    message_out += tag; //read out any unknown tag
  }
}

void formulate_message(String my_string) {
  message_out = "r";
  message_out += THIS_READER_NUM;
  message_out += my_string;
}

void lightLED(int pin) {
  ///////////////////////////////////
  //Turn on LED on pin "pin" for 250ms
  ///////////////////////////////////
  if (pin == 2) {
    if (is_led2_on == false) {
      digitalWrite(2, HIGH);
      curr_led1_time = millis();
      is_led1_on = true;
    }
    else if (millis() - curr_led1_time > LED_DELAY) {
      digitalWrite(2, LOW);
      is_led1_on = false;
    }
  }

  if (pin == 3) {
    if (is_led2_on == false) {
      digitalWrite(3, HIGH);
      curr_led2_time = millis();
      is_led2_on = true;
    }
    else if (millis() - curr_led2_time > LED_DELAY) {
      digitalWrite(3, LOW);
      is_led2_on = false;
    }
  }

  if (pin == 4) {
    if (is_led3_on == false) {
      digitalWrite(4, HIGH);
      curr_led3_time = millis();
      is_led3_on = true;
    }
    else if (millis() - curr_led3_time > LED_DELAY) {
      digitalWrite(4, LOW);
      is_led3_on = false;
    }
  }

  if (pin == 5) {
    if (is_led4_on == false) {
      digitalWrite(5, HIGH);
      curr_led4_time = millis();
      is_led4_on = true;
    }
    else if (millis() - curr_led4_time > LED_DELAY) {
      digitalWrite(5, LOW);
      is_led4_on = false;
    }
  }
}

void turn_off_leds() {
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  is_led1_on = false;
  is_led2_on = false;
  is_led3_on = false;
  is_led4_on = false;
}

void resetReader() {
  ///////////////////////////////////
  //Reset the RFID reader to read again.
  ///////////////////////////////////
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  delay(RESET_DELAY);
}

void clearTag(char one[]) {
  ///////////////////////////////////
  //clear the char array by filling with null - ASCII 0
  //Will think same tag has been read otherwise
  ///////////////////////////////////
  for (int i = 0; i < strlen(one); i++) {
    one[i] = 0;
  }
}

boolean compareTag(char one[], char two[]) {
  ///////////////////////////////////
  //compare two value to see if same,
  //strcmp not working 100% so we do this
  ///////////////////////////////////

  if (strlen(one) == 0) return false; //empty

  for (int i = 0; i < 12; i++) {
    if (one[i] != two[i]) return false;
  }

  return true; //no mismatches
}

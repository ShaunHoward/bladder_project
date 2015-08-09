#include <StandardCplusplus.h>
#include <vector>
#include <wavelet.h>
#include <SoftwareSerial.h>
#include <stdio.h>
#include <stdlib.h>
#include <cat.h>

/*
 *	This class implements the CAT algorithm.
 *	CAT stands for Context Aware Thresholding.
 *	
 *	It is a novel, parameterized, user-tunable algorithmic framework capable of
 *	real-time classification of bladder events, such as detrusor contractions, from
 *	single-sensor bladder pressure data.
 *
 *	This algorithm includes EMA filtering, DWT, and adaptive thresholding of samples.
 *
 *	Relative to other methods, Context Aware Thresholding (CAT) is fast, robust, and
 *      highly accurate, noise-tolerant, and amenable to energy-efficient hardware implementation, which 
 *	is important for mapping to an implant device.
 *
 *      This code is intended for use on the Teensy Aruduino platform but should be compatible with
 *	most other Arduino devices.
 *
 *	@author Shaun Howard
 */
 
int TRIAL_COUNT = 40;

int curr_trial = 0;

vector<double> contractions(100);
vector<double> detail_event_log(100);
vector<double> sum_log(TRIAL_COUNT);
vector<double> dwt_output(TRIAL_COUNT);

int incoming_signal = 0;
int led = 13;

CAT signal_processor(contractions, detail_event_log, sum_log, dwt_output, TRIAL_COUNT);

// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(9600);
  
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT); 
   
  Serial.println("Setup arduino");
}

void print_vector(vector<double>& print_me) {
  Serial.println(print_me.size());
  for (int i = 0; i < print_me.size(); i++) {
    if (i < print_me.size() - 1) {
      Serial.print(print_me.at(i));
      Serial.print(", ");
    } else {
      Serial.println(print_me.at(i));
    }
  }
}

// the loop routine runs over and over again forever:
void loop() {
  if (Serial.available() > 0) {
    if (curr_trial >= TRIAL_COUNT) {
      curr_trial = 0;
    } else {
      curr_trial += 1;
    }
    
    //blink LED here
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    
    //read latest incoming data
    incoming_signal = Serial.parseFloat();
    Serial.println(incoming_signal);
    //flush out junk
    //Serial.flush();    
    
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);  
    
    signal_processor.read_and_process_signal(incoming_signal, curr_trial);
    
    print_vector(sum_log);
    print_vector(dwt_output);
  }
  
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
  delay(1000);
}




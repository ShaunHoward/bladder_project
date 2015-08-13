#include <StandardCplusplus.h>
#include <vector>
#include <SoftwareSerial.h>
#include <stdio.h>
#include <stdlib.h>
#include <cat.h>
#include <wavelet.h>
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
 
int TRIAL_COUNT = 50;

int curr_trial = 0;

double* contractions = (double*) malloc(100 * sizeof(double));
double* detail_event_log = (double*) malloc(100 * sizeof(double));
double* sum_log = (double*) malloc(TRIAL_COUNT * sizeof(double));
double* dwt_output = (double*) malloc(TRIAL_COUNT * sizeof(double));
double* dwt_sum_log = (double*) malloc(TRIAL_COUNT * sizeof(double));
double* db2_flags = (double*) malloc(2 * sizeof(double));
int* db2_lengths = (int*) malloc(2 * sizeof(double));
double* hp1 = (double*)malloc(4 * sizeof(double));
double* lp1 = (double*)malloc(4 * sizeof(double));

int incoming_signal = 0;
int led = 13;

CAT signal_processor(contractions, detail_event_log, sum_log, dwt_sum_log, db2_flags, db2_lengths, dwt_output, hp1, lp1, TRIAL_COUNT);

// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(9600);
  
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT); 
   
  Serial.println("Setup arduino");
  initialize_array(contractions, 100);
  initialize_array(detail_event_log, 100);
  initialize_array(sum_log, TRIAL_COUNT);
  initialize_array(dwt_output, TRIAL_COUNT);
  initialize_array(dwt_sum_log, TRIAL_COUNT);
  initialize_array(db2_flags, 2);
  initialize_array(db2_lengths, 2);
  initialize_array(hp1, 4);
  initialize_array(lp1, 4);
  return;
}

void print_array(double* print_me, int my_size) {
  for (int i = 0; i < my_size; i++) {
    if (i < my_size - 1) {
      Serial.print(print_me[i]);
      Serial.print(", ");
    } else {
      Serial.println(print_me[i]);
    }
  }
  return;
}

void initialize_array(int* do_me, int my_size) {
  for (int i = 0; i < my_size; i++) {
    do_me[i] = 0;
  }
  return;
}

void initialize_array(double* do_me, int my_size) {
  for (int i = 0; i < my_size; i++) {
    do_me[i] = 0;
  }
  return;
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
    delay(250);               // wait for .25 second
    
    //read latest incoming data
    incoming_signal = 3; // Serial.parseFloat();
    //Serial.println(incoming_signal);
    //flush out junk
    //Serial.flush();    
    
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    //delay(250);  
    
    signal_processor.read_and_process_signal(incoming_signal, curr_trial);
    
    print_array(sum_log, TRIAL_COUNT);
    print_array(dwt_sum_log, TRIAL_COUNT);
  }
  
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
  delay(1000);
  return;
}




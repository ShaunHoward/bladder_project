#include <StandardCplusplus.h>

#include <stdio.h>
#include <stdlib.h>
#include <wavelet2d.h>
#include <math.h>

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

//cutoff frequency in hertz for low pass filter (EMA)
float cutoff_frequency = 0.01;  

//sample rate (10 Hz) in seconds
float dt = 0.1;  				  

//length of window to analyze samples in seconds
int sample_window = 90;

// calculate filtering coefficient
float filter_base = 2.7182818; //e
float filter_exponent = -(cutoff_frequency / dt) * 3.1415926 * 2.0;
double filter_coefficient = 1 - pow(filter_base, filter_exponent); //coA
double Pves = 1;

int TRIAL_COUNT = 30;

vector<double> contractions;
vector<double> detail_event_log;

int main (int argc, char **argv) {
  read_and_process_signal(TRIAL_COUNT);
}

void read_and_process_signal(int number_of_trials) {

  int n = number_of_trials;

  vector<float> contractions;
  vector<float> detail_event_log;
  vector<double> sum_log;
  
  for (int i = 0; i < n; i++) {
    //initialize sum_log values
    sum_log.at(i) = 0;
  }
  
  while (true) {
    /*
     * DWT (Discrete Wavelet Transform)
     * Uses a CPP library implementation.
     * A portion of the sum_log buffer is transformed.
     */
    for (int index = 0; index < n; index++){
  
    //  FILE * f;
  
    //  f = fopen (argv[1], "r");
      for (int i = 0; i < n; i++) {
        //initialize sum_log values
        //sum_log.at(i) = 0;
      }
  
      for (int i = 0; i < n; i++) {
        sum_log.at(i) = sum_log.at(i-1) + filter_coefficient * (Pves - sum_log.at(i-1));
      }
      //fclose (f);
  
      vector<double> dwt_output, flag;
  
      // perform J-Level DWT
      int J = 5;
      vector<int> length;
  
      dwt_sym(sum_log, J, dwt_output, flag, length);
  
      for (unsigned int i = 0; i < dwt_output.size(); i++){
  	//dwtout << dwt_output[i] << endl;
      }
  
      int capture_status = capture_signal_window(0.95, 0.95);
  
      for (int i = 0; i < n; i++) {
        printf ("%g\n", dwt_output.at(i));
      }
    }
  }
}

vector<double> quantile(vector<double> &data_set, float percentile) {
  return data_set;
}

double median(vector<double> &data_set) {
  std::sort(data_set.begin(), data_set.end());
  int mid_point = (data_set.size() / 2) - 1;
  return data_set.at(mid_point);
}

int capture_signal_window(float be_percentile, float ma_percentile) {

  // wavelet transform level
  int transform_level = 5;

  // frequency of wavelet transformation; initialized to 10 Hz
  float transform_freq = dt;

  // divide resolution in half (downsample by 2 each level)
  for (int i = 1; i < transform_level; i++) {
      transform_freq = transform_freq / 2;
  }

  int transform_length = 30;
  int sigLen = 60;

  /*
   * Extracts a portion of the EMA-filtered and DWT-transformed data
   * as the transform window length.
   * wtWindowLen = length of window (in samples) of the transformed data
   */
  int transform_window_length = ceil(transform_freq * sample_window);
  float transform_max = transform_length - transform_window_length;

  // process signal with adaptive thresholding
  int transform_window_end = sigLen  + transform_window_length;

  // window signal (approximation) and details
  vector<double> signal_in_window;
  vector<double> detail_in_window;

  /*
   * Calculate quantiles (approximation quantiles)
   * be_percentile = bladder event percentile, a double from 0 to 1 (e.g.
   * 0.95, where 95% of data is below that value
   */
  vector<double> approximation_quantile = quantile(signal_in_window, be_percentile);
  if (signal_in_window.at(transform_window_length) > median(approximation_quantile)
  		&& signal_in_window.at(transform_window_length) > 5) {
    contractions.at(transform_window_end) = 1;
  } else {
    contractions.at(transform_window_end) = 0;
  }

  vector<double> detail_quantile = quantile(detail_in_window, ma_percentile);
  if(detail_in_window.at(transform_window_length) > median(detail_quantile)) {
    detail_event_log.at(transform_window_end) = 1;
  } else {
    detail_event_log.at(transform_window_end) = 0;
  }

  /*
   * This gives us our classification of bladder events
   * (contractions and artifacts). Instead of logging in a buffer,
   * a time stamp will be recorded (the teensy has a fairly accurate RTC)
   * Later, we will go through the data and see if it matches up with
   * sensations the subject was having.
   */
   return 0;
}

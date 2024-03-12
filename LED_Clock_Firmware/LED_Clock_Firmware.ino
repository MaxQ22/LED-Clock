//Include the Adafruit Library to control the NeoPixl LEDs
#include <Adafruit_NeoPixel.h>
//Include the Time library for timekeeping
#include <time.h>
//Include the WIFI Library for WIFI connection
#include <ESP8266WiFi.h> 

//Include the local configuration
#include "config.h"

#define PIN D8 //Pin, to which the Neopixls are connected.

#define NUMDIGITS 4 //How many digits does the clock have?
#define NUMPIXELSPERDIGIT 21 //How many Neopixl LEDs does each display have? Only multiples of 7 allowed!!!!!!
#define NUMPIXELS NUMPIXELSPERDIGIT*NUMDIGITS + 2 //Number of Neopixl in total + 2 due to the colon

//Initialize the Adadfruit Neopixl Driver
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Initialize the SSID and the Password for your local WIFI to connect to the NTP server
const char * ssid = STASSID;
const char * password = STAPSK;

//Some global variables for timekeeping
time_t current_time;
tm local_time;

void setup() 
{
  //Initialization code

  //Start the Debug connection with a Baud Rate of 115200
  Serial.begin(115200);
  Serial.println("LED Clock startup");

  //Configure the NTP server. NOTE: the NTP server will be automatically started after WIFI connection is established
  configTime(TIMEZONE, NTPSERVER);

  // Connect to wIFI
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print ( "." );
  }
  Serial.println("\nWiFi connected");

  pixels.begin(); // Innitialize NeoPixel strip object 
}

void loop() {
  static unsigned long previousMillisColon = 0;   // Stores the last time the function was called
  static unsigned long previousMillisUpdate = 0;   // Stores the last time the function was called
  static bool colon_status = false; //Current status of the colon
  const long COLON_BLINK_INTERVAL = 2000;         // Interval in milliseconds for blinking the colon on and off(2 seconds)
  const long UPDATE_INTERVAL = 100;         // Interval in milliseconds for updating the time

   //Blink the Colon on and off 
   unsigned long currentMillis = millis();
   if (currentMillis - previousMillisColon >= COLON_BLINK_INTERVAL)
   {
    // Save the last time the function was called
    previousMillisColon = currentMillis;
    colon_status = !colon_status;
   }

   //Update the time
   currentMillis = millis();
   
   if (currentMillis - previousMillisUpdate >= UPDATE_INTERVAL)
   {
    // Save the last time the function was called
    previousMillisUpdate = currentMillis;
    time(&current_time);  //Get the current time                 
    localtime_r(&current_time, &local_time); //Convert the current timestamp to a local time struct

    //Update the Neopixls
    UpdatePixels(local_time.tm_hour * 100 + local_time.tm_min, colon_status); 
   }
}

void GeneratePixelsfromNumer(bool * p_pixels, unsigned char number)
{
  /*This is an array of bools, indicating, if the segment is on or off.
  The addresses in the array correspond to the digits as follows:

      5
    4   6
      3  
    2   7
      1
*/
  
  bool segments[7];

  //Use a look up table, to get to know, which segments are on for a given number:
  switch(number)
  {
    case 0: 
      segments[0] = 1;
      segments[1] = 1;
      segments[2] = 0;
      segments[3] = 1;
      segments[4] = 1;
      segments[5] = 1;
      segments[6] = 1;
    break;
    case 1: 
      segments[0] = 0;
      segments[1] = 0;
      segments[2] = 0;
      segments[3] = 0;
      segments[4] = 0;
      segments[5] = 1;
      segments[6] = 1;
    break;
    case 2: 
      segments[0] = 1;
      segments[1] = 1;
      segments[2] = 1;
      segments[3] = 0;
      segments[4] = 1;
      segments[5] = 1;
      segments[6] = 0;
    break;
    case 3: 
      segments[0] = 1;
      segments[1] = 0;
      segments[2] = 1;
      segments[3] = 0;
      segments[4] = 1;
      segments[5] = 1;
      segments[6] = 1;
    break;
    case 4: 
      segments[0] = 0;
      segments[1] = 0;
      segments[2] = 1;
      segments[3] = 1;
      segments[4] = 0;
      segments[5] = 1;
      segments[6] = 1;
    break;
    case 5: 
      segments[0] = 1;
      segments[1] = 0;
      segments[2] = 1;
      segments[3] = 1;
      segments[4] = 1;
      segments[5] = 0;
      segments[6] = 1;
    break;
    case 6: 
      segments[0] = 1;
      segments[1] = 1;
      segments[2] = 1;
      segments[3] = 1;
      segments[4] = 1;
      segments[5] = 0;
      segments[6] = 1;
    break;
    case 7: 
      segments[0] = 0;
      segments[1] = 0;
      segments[2] = 0;
      segments[3] = 0;
      segments[4] = 1;
      segments[5] = 1;
      segments[6] = 1;
    break;
    case 8: 
      segments[0] = 1;
      segments[1] = 1;
      segments[2] = 1;
      segments[3] = 1;
      segments[4] = 1;
      segments[5] = 1;
      segments[6] = 1;
    break;
    case 9: 
      segments[0] = 1;
      segments[1] = 0;
      segments[2] = 1;
      segments[3] = 1;
      segments[4] = 1;
      segments[5] = 1;
      segments[6] = 1;
    break;
  }

  //Each segment consists of three pixls, so convert the segment lookup table to a neopixel array
  for(unsigned int i=0; i <7; i++)
  {
  
    p_pixels[i*3] = segments[i];
    p_pixels[i*3 + 1] = segments[i];
    p_pixels[i*3 + 2] = segments[i];
  }
}

unsigned char GetLightIntensity(unsigned short adc_input, const unsigned short *adc_input_table, const unsigned char *illumination_table, size_t table_size) {
    //Get the desired light intensity of the neopixls based on the ADC -> Neopixl light intensity lookup table
    // Check if adc_input is within the range of the table
    if (adc_input <= adc_input_table[0])
    {
        // If adc_input is smaller than the smallest value in the table,
        // return the corresponding illumination value directly
        return illumination_table[0];
    } 
    else if (adc_input >= adc_input_table[table_size - 1])
    {
        // If adc_input is larger than the largest value in the table,
        // return the corresponding illumination value directly
        return illumination_table[table_size - 1];
    } 
    else
    {
        // Find the two nearest points for interpolation
        size_t idx = 0;
        while (adc_input_table[idx] < adc_input)
        {
            idx++;
        }
        // Linear interpolation
        float proportion = (float)(adc_input - adc_input_table[idx - 1]) / (adc_input_table[idx] - adc_input_table[idx - 1]);
        unsigned char estimated_illumination = (unsigned char)(illumination_table[idx - 1] + proportion * (illumination_table[idx] - illumination_table[idx - 1]));
        return estimated_illumination;
    }
}

unsigned short ADCFilter(unsigned short adc_value) {
    const float sampling_time = 0.1; // Sampling time in seconds (100 milliseconds)
    const float cutoff_frequency = 0.3; // Cutoff frequency in Hz

    // Define static variables for filter state
    static float alpha;
    static float y_prev = 0;

    // Calculate alpha using the bilinear transformation with frequency prewrapping
    float omega = 2 * 3.14159 * cutoff_frequency;
    float T = 1 / sampling_time;
    float tan_half_omega_T = 2*atanf(0.5 * omega * T);
    alpha = (tan_half_omega_T - 1) / (tan_half_omega_T + 1);

    // Apply the filter
    float y = alpha * adc_value + (1 - alpha) * y_prev;
    y_prev = y;

    return (unsigned short)y;
}

void LightIntensityFilter(unsigned char *red, unsigned char *green, unsigned char *blue, unsigned char red_stepoint, unsigned char green_stepoint, unsigned char blue_stepoint) {
    const float sampling_time = 0.1; // Sampling time in seconds (100 milliseconds)
    const float cutoff_frequency = 0.2; // Cutoff frequency in Hz

    // Define static variables for filter states
    static float alpha_red, alpha_green, alpha_blue;
    static float red_prev = 0, green_prev = 0, blue_prev = 0;

   // Calculate alpha using the bilinear transformation with frequency prewrapping
    float omega = 2 * 3.14159 * cutoff_frequency;
    float T = 1 / sampling_time;
    float tan_half_omega_T = 2*atanf(0.5 * omega * T);
    alpha_red = (tan_half_omega_T - 1) / (tan_half_omega_T + 1);
    alpha_green = alpha_red; // Use the same cutoff frequency for all channels
    alpha_blue = alpha_red; // Use the same cutoff frequency for all channels

    // Apply the filter for each color channel
    *red = alpha_red * red_stepoint + (1 - alpha_red) * red_prev;
    red_prev = *red;

    *green = alpha_green * green_stepoint + (1 - alpha_green) * green_prev;
    green_prev = *green;

    *blue = alpha_blue * blue_stepoint + (1 - alpha_blue) * blue_prev;
    blue_prev = *blue;
}

void UpdatePixels(unsigned int time_number, bool colon_status)
{
  unsigned char red = 0;
  unsigned char green = 0;
  unsigned char blue = 0;
  unsigned char light_intensity = 0;
  unsigned short adc_value = 0;
  unsigned int adc_accumulator = 0;
  const unsigned short ADC_CYCLES = 10;
  bool pixeldata[NUMPIXELSPERDIGIT * NUMDIGITS + 2];

  //Generate the Data for the digits
  GeneratePixelsfromNumer(pixeldata, time_number/1000 % 10);
  GeneratePixelsfromNumer(&pixeldata[NUMPIXELSPERDIGIT], time_number/100 % 10);
  GeneratePixelsfromNumer(&pixeldata[NUMPIXELSPERDIGIT*2 + 2], time_number/10 % 10);
  GeneratePixelsfromNumer(&pixeldata[NUMPIXELSPERDIGIT*3 + 2], time_number % 10);


   //Blink the colon 
   pixeldata[NUMPIXELSPERDIGIT * 2] = colon_status;
   pixeldata[NUMPIXELSPERDIGIT * 2 + 1] = colon_status;

  //Read the analog value of the phototransistor
  for (unsigned short n=0;n<ADC_CYCLES;n++)
  {
      adc_value = analogRead(A0);
      adc_accumulator += adc_value;
  }
  //Get the average of several ADC readings
  adc_value = adc_accumulator / ADC_CYCLES;
  adc_value = ADCFilter(adc_value);

  //Some Debug Information
  Serial.print("ADC Value: ");
  Serial.println(adc_value);
  light_intensity = GetLightIntensity(adc_value, adc_input_table, illumination_table, table_size);
  Serial.print("Light Intensity: ");
  Serial.println(light_intensity);

  //Determine if it is dark or not, to make the display red, when it is dark for better visibility 
  if(light_intensity > NIGHTTHRESHOLD)
  {
    LightIntensityFilter(&red, &green, &blue, 0, light_intensity, light_intensity);
  }
  else
  {
    LightIntensityFilter(&red, &green, &blue, light_intensity, 0, 0);
  }

  //Transfer the data to the Neopixls
  for(unsigned int i=0;i < NUMPIXELSPERDIGIT * NUMDIGITS + 2; i++)
  {
    if(pixeldata[i])
    {
        pixels.setPixelColor(i, red, green, blue);
    }
    else
    {
      pixels.setPixelColor(i, 0, 0, 0);
    }
  }
  pixels.show();
}

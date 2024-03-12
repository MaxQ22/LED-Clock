#define STASSID  "YOURSSID" //Change this to your WIFI SSID
#define STAPSK  "YOURPASSWORD" //Change this to your WIFI password
#define NTPSERVER "at.pool.ntp.org" //URL to the NTP server           
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3" //Berlin Time Zone. Change this to your local timezone (https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)

//Lookup table to determine the Light Intensity, based on the Light Sensor Value
 const unsigned short adc_input_table[] = {0, 20, 30, 60}; // ADC input values
 const unsigned char illumination_table[] = {5, 5, 100, 255}; // Corresponding illumination values
 size_t table_size = sizeof(adc_input_table) / sizeof(adc_input_table[0]); // Calculate the size of the lookup table
 #define NIGHTTHRESHOLD 120 //Threshold, to determine night mode. If the illumination is below this threshold, then the clock will switch to night mode

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
//mpu address
const static uint8_t MPU_ADDRESS = 0x68;
 
static void mpu6050_reset() {
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    //uint8_t buffer [] = {(uint8_t)65, (uint8_t)66};
    //i2c_read_blocking(i2c_default, MPU_ADDRESS, buffer[0], 8, false);
    //i2c_read_blocking(i2c_default, MPU_ADDRESS, buffer[1], 8, false);
    //printf("b1: %d b2: %d \n", buffer[0], buffer[1]);
 
    uint8_t buf[] = {0x6B, 0x00};
    i2c_write_blocking(i2c_default, MPU_ADDRESS, buf, 1, false);
    //i2c_read_blocking(i2c_default, MPU_ADDRESS, buffer[0], 8, false);
    //i2c_read_blocking(i2c_default, MPU_ADDRESS, buffer[1], 8, false);
   
}
 
static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.
 
    uint8_t buffer[6];
 
    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_default, MPU_ADDRESS, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c_default, MPU_ADDRESS, buffer, 6, false);
    printf("buffer accel, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d \n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }
 
    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(i2c_default, MPU_ADDRESS, &val, 1, true);
    i2c_read_blocking(i2c_default, MPU_ADDRESS, buffer, 6, false);  // False - finished with bus
    printf("buffer gyro, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d \n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);;
    }
    
 
    val = 0x41;
    i2c_write_blocking(i2c_default, MPU_ADDRESS, &val, 1, true);
    i2c_read_blocking(i2c_default, MPU_ADDRESS, buffer, 2, false);  // False - finished with bus
    printf("temp buf: %hd, %hd\n", buffer[0], buffer [1]);
    *temp = buffer[0] << 8 | buffer[1];
}
 
int main() {
    
    stdio_init_all();
    const uint SWITCH_POWER = 0;
    const uint SWITCH_POWER_IN = 2;
    gpio_init(SWITCH_POWER);
    gpio_init(SWITCH_POWER_IN);
    gpio_set_dir(SWITCH_POWER, GPIO_OUT);
    gpio_set_dir(SWITCH_POWER_IN, GPIO_IN);
    gpio_put(SWITCH_POWER, true);

    //check that pico is powered and running
    const uint LED = PICO_DEFAULT_LED_PIN;
    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);
    gpio_put(LED, 1);
 
    //check that pico can use serial output
    printf("hello world \n");
   
 
    //setup i2c buffer address stuff
    uint8_t MPU_ACCELRATION_BUFFER = 0x3B;
    uint8_t MPU_GRYOSCOPE_BUFFER = 0x43;
 
    //turn on MPU power
    const uint MPU_POWER = 1;
    gpio_init(MPU_POWER);
    gpio_set_dir(MPU_POWER, GPIO_OUT);
    gpio_put(MPU_POWER, 1);
 
    i2c_init(i2c0, 400 * 1000);
  
 
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    //uint baudrate = i2c_set_baudrate (i2c_default, 9600); 	
    
 
    //while (1){
    mpu6050_reset();
    //}
    int16_t acceleration[3], gyro[3], temp;
   
    while (1) {
        mpu6050_reset();
        //if(gpio_get(SWITCH_POWER_IN) == true){
            printf("writing... \n");
            //printf("baudrate: %hd \n", baudrate);
            mpu6050_read_raw(acceleration, gyro, &temp);
 
            // These are the raw numbers from the chip, so will need tweaking to be really useful.
            // See the datasheet for more information
            printf("Acc. X = %hd, Y = %hd, Z = %hd\n", acceleration[0], acceleration[1], acceleration[2]);
            printf("Gyro. X = %hd, Y = %hd, Z = %hd\n", gyro[0], gyro[1], gyro[2]);
            printf("Temp. = %f\n", (temp / 340.0) + 36.53);
            sleep_ms(100);
        //}
        //else 
        //{
            //mpu6050_reset();
            //printf("not writing... \n");
        //}
    }
    //return 0;
}
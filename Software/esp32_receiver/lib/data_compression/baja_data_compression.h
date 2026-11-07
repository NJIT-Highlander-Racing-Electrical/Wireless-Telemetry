#include <stdint.h>
#include <stddef.h>

#ifndef BAJA_DATA_COMPRESSION_H
#define BAJA_DATA_COMPRESSION_H

#define SENT_DATA_POINTS_BITS 11
typedef uint16_t sent_data_points_t;

#define CVT_DATA           0b1
#define WHEEL_SPEEDS       0b10
#define SUS_DISPLACEMENTS  0b100
#define PEDAL_DATA         0b1000
#define ACCEL_DATA         0b10000
#define GYRO_DATA          0b100000
#define GPS_LAT_LONG       0b1000000
#define GPS_OTHER_DATA     0b10000000
#define BATTERY_DATA       0b100000000
#define SD_LOGGING_DATA    0b1000000000

#define CVT_DATA_TOTAL_BITS 42
#define CVT_RPM_BITS 9
#define CVT_TEMP_BITS 8
typedef struct {
	uint16_t primary : CVT_RPM_BITS;
	uint16_t secondary : CVT_RPM_BITS;
	uint8_t temperature;
	uint8_t secondaryTemperature;
	uint8_t beltTemperature;
} cvt_data_t;

#define WHEEL_SPEEDS_TOTAL_BITS 40
#define WHEEL_SPEED_BITS 10
typedef struct {
	uint16_t fl : WHEEL_SPEED_BITS;
	uint16_t fr : WHEEL_SPEED_BITS;
	uint16_t rl : WHEEL_SPEED_BITS;
	uint16_t rr : WHEEL_SPEED_BITS;
} wheel_speeds_t;

#define SUS_DISPLACEMENTS_TOTAL_BITS 32
#define SUS_DISPLACEMENT_BITS 8
typedef struct {
	uint8_t fl : SUS_DISPLACEMENT_BITS;
	uint8_t fr : SUS_DISPLACEMENT_BITS;
	uint8_t rl : SUS_DISPLACEMENT_BITS;
	uint8_t rr : SUS_DISPLACEMENT_BITS;
} suspension_displacements_t;

#define PEDAL_DATA_TOTAL_BITS 38
#define PEDAL_BITS 7
#define PRESSURE_BITS 12
typedef struct {
	uint8_t gas : PEDAL_BITS;
	uint8_t brake : PEDAL_BITS;
	uint16_t frontPressure : PRESSURE_BITS;
	uint16_t rearPressure : PRESSURE_BITS;
} pedal_data_t;

#define ACCEL_DATA_TOTAL_BITS 27
#define ACCEL_BITS 9
typedef struct {
	int16_t x : ACCEL_BITS;
	int16_t y : ACCEL_BITS;
	int16_t z : ACCEL_BITS;
} accelerometer_data_t;

#define GYRO_DATA_TOTAL_BITS 42
#define GYRO_BITS 14
typedef struct {
	int16_t yaw : GYRO_BITS;
	int16_t pitch : GYRO_BITS;
	int16_t roll : GYRO_BITS;
} gyroscope_data_t;

#define GPS_POS_TOTAL_BITS 50
#define GPS_POS_BITS 25
typedef struct {
	int32_t latitude : GPS_POS_BITS;
	int32_t longitude : GPS_POS_BITS;
} gps_lat_long_t;

#define BATTERY_DATA_TOTAL_BITS 7
#define BATTERY_BITS 7
typedef struct {
	uint8_t percentage : BATTERY_BITS;
} battery_data_t;

#define SD_LOGGING_DATA_TOTAL_BITS 1
typedef struct {
	uint8_t active : 1;
} sd_logging_data_t;

typedef struct {
	cvt_data_t cvt_data;
	wheel_speeds_t wheel_speeds;
	suspension_displacements_t sus_displacements;
	pedal_data_t pedal_data;
	accelerometer_data_t accel_data;
	gyroscope_data_t gyro_data;
	gps_lat_long_t gps_pos;
	battery_data_t battery_data;
	sd_logging_data_t sd_logging_data;
} baja_data_t;

size_t pack_data(char* buffer, const baja_data_t* baja_data, const sent_data_points_t sent_data_points);
sent_data_points_t unpack_data(const char* buffer, baja_data_t* baja_data);

#endif
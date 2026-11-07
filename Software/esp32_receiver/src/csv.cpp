#include <stdio.h>
#include <stddef.h>
#include "util.h"
#include "baja_data_compression.h"
#include "csv.h"

#define CSV_APPEND(fmt, ...) \
    do { \
        size_t _w = snprintf(buf, bufLen, fmt, ##__VA_ARGS__); \
        buf += _w; bufLen -= _w; \
    } while(0)

int make_csv(char* buf, size_t bufLen, const rxdata_t rxdata) {

	baja_data_t sortedData = {};
	sent_data_points_t newDataPoints = unpack_data(rxdata.data, &sortedData);

	// indices 1-3: time
	CSV_APPEND(",,,");

	// index 4: time from start
	CSV_APPEND(",");

	// index 5: screenshot flag
	CSV_APPEND(",");

	// indices 6-7: battery voltage (no source), battery level
	CSV_APPEND(",");
	if (newDataPoints & BATTERY_DATA)
		CSV_APPEND("%u,", sortedData.battery_data.percentage);
	else
		CSV_APPEND(",");

	// indices 8-10: accelerometer
	if (newDataPoints & ACCEL_DATA)
		CSV_APPEND("%i,%i,%i,", sortedData.accel_data.x, sortedData.accel_data.y, sortedData.accel_data.z);
	else
		CSV_APPEND(",,,");

	// indices 11-13: unused
	CSV_APPEND(",,,");

	// indices 14-15: GPS lat/lon
	if (newDataPoints & GPS_LAT_LONG)
		CSV_APPEND("%.6f,%.6f,", sortedData.gps_pos.latitude / 100000.0f, sortedData.gps_pos.longitude / 100000.0f);
	else
		CSV_APPEND(",,");

	// indices 16-20: GPS status fields
	CSV_APPEND(",,,,,");

	// indices 21-22: CVT RPM
	if (newDataPoints & CVT_DATA)
		CSV_APPEND("%u,%u,", sortedData.cvt_data.primary, sortedData.cvt_data.secondary);
	else
		CSV_APPEND(",,");

	// indices 23-25: CVT temperatures
	if (newDataPoints & CVT_DATA)
		CSV_APPEND("%u,%u,%u,", sortedData.cvt_data.temperature, sortedData.cvt_data.secondaryTemperature, sortedData.cvt_data.beltTemperature);
	else
		CSV_APPEND(",,,");

	// indices 26-29: wheel speeds
	if (newDataPoints & WHEEL_SPEEDS)
		CSV_APPEND("%u,%u,%u,%u,", sortedData.wheel_speeds.fl, sortedData.wheel_speeds.fr, sortedData.wheel_speeds.rl, sortedData.wheel_speeds.rr);
	else
		CSV_APPEND(",,,,");

	// indices 30-33: suspension displacements
	if (newDataPoints & SUS_DISPLACEMENTS)
		CSV_APPEND("%u,%u,%u,%u,", sortedData.sus_displacements.fl, sortedData.sus_displacements.fr, sortedData.sus_displacements.rl, sortedData.sus_displacements.rr);
	else
		CSV_APPEND(",,,,");

	// indices 34-35: brake pressures
	if (newDataPoints & PEDAL_DATA)
		CSV_APPEND("%u,%u,", sortedData.pedal_data.frontPressure, sortedData.pedal_data.rearPressure);
	else
		CSV_APPEND(",,");

	// index 36: unused
	CSV_APPEND(",");

	// index 37: SD logging
	if (newDataPoints & SD_LOGGING_DATA)
		CSV_APPEND("%u", sortedData.sd_logging_data.active);

	return 1;
}
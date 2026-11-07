#include "baja_data_compression.h"
#include "data_compression.h"
#include <stdlib.h>

size_t pack_data(char* buffer, const baja_data_t* baja_data, const sent_data_points_t sent_data_points) {

	int data_size_bits = SENT_DATA_POINTS_BITS;
	if (sent_data_points & CVT_DATA)
		data_size_bits += CVT_DATA_TOTAL_BITS;
	if (sent_data_points & WHEEL_SPEEDS)
		data_size_bits += WHEEL_SPEEDS_TOTAL_BITS;
	if (sent_data_points & SUS_DISPLACEMENTS)
		data_size_bits += SUS_DISPLACEMENTS_TOTAL_BITS;
	if (sent_data_points & PEDAL_DATA)
		data_size_bits += PEDAL_DATA_TOTAL_BITS;
	if (sent_data_points & ACCEL_DATA)
		data_size_bits += ACCEL_DATA_TOTAL_BITS;
	if (sent_data_points & GYRO_DATA)
		data_size_bits += GYRO_DATA_TOTAL_BITS;
	if (sent_data_points & GPS_LAT_LONG)
		data_size_bits += GPS_POS_TOTAL_BITS;
	if (sent_data_points & BATTERY_DATA)
		data_size_bits += BATTERY_DATA_TOTAL_BITS;
	if (sent_data_points & SD_LOGGING_DATA)
		data_size_bits += SD_LOGGING_DATA_TOTAL_BITS;

	char* bits = (char*)malloc(data_size_bits);
	if (!bits) return 0;
	int i = 0;

	load_uintx(bits, &i, SENT_DATA_POINTS_BITS, sent_data_points);

	if (sent_data_points & CVT_DATA) {
		load_uintx(bits, &i, CVT_RPM_BITS, baja_data->cvt_data.primary);
		load_uintx(bits, &i, CVT_RPM_BITS, baja_data->cvt_data.secondary);
		load_uintx(bits, &i, CVT_TEMP_BITS, baja_data->cvt_data.temperature);
		load_uintx(bits, &i, CVT_TEMP_BITS, baja_data->cvt_data.secondaryTemperature);
		load_uintx(bits, &i, CVT_TEMP_BITS, baja_data->cvt_data.beltTemperature);
	}

	if (sent_data_points & WHEEL_SPEEDS) {
		load_uintx(bits, &i, WHEEL_SPEED_BITS, baja_data->wheel_speeds.fl);
		load_uintx(bits, &i, WHEEL_SPEED_BITS, baja_data->wheel_speeds.fr);
		load_uintx(bits, &i, WHEEL_SPEED_BITS, baja_data->wheel_speeds.rl);
		load_uintx(bits, &i, WHEEL_SPEED_BITS, baja_data->wheel_speeds.rr);
	}

	if (sent_data_points & SUS_DISPLACEMENTS) {
		load_uintx(bits, &i, SUS_DISPLACEMENT_BITS, baja_data->sus_displacements.fl);
		load_uintx(bits, &i, SUS_DISPLACEMENT_BITS, baja_data->sus_displacements.fr);
		load_uintx(bits, &i, SUS_DISPLACEMENT_BITS, baja_data->sus_displacements.rl);
		load_uintx(bits, &i, SUS_DISPLACEMENT_BITS, baja_data->sus_displacements.rr);
	}

	if (sent_data_points & PEDAL_DATA) {
		load_uintx(bits, &i, PEDAL_BITS, baja_data->pedal_data.gas);
		load_uintx(bits, &i, PEDAL_BITS, baja_data->pedal_data.brake);
		load_uintx(bits, &i, PRESSURE_BITS, baja_data->pedal_data.frontPressure);
		load_uintx(bits, &i, PRESSURE_BITS, baja_data->pedal_data.rearPressure);
	}

	if (sent_data_points & ACCEL_DATA) {
		load_intx(bits, &i, ACCEL_BITS, baja_data->accel_data.x);
		load_intx(bits, &i, ACCEL_BITS, baja_data->accel_data.y);
		load_intx(bits, &i, ACCEL_BITS, baja_data->accel_data.z);
	}

	if (sent_data_points & GYRO_DATA) {
		load_intx(bits, &i, GYRO_BITS, baja_data->gyro_data.yaw);
		load_intx(bits, &i, GYRO_BITS, baja_data->gyro_data.pitch);
		load_intx(bits, &i, GYRO_BITS, baja_data->gyro_data.roll);
	}

	if (sent_data_points & GPS_LAT_LONG) {
		load_intx(bits, &i, GPS_POS_BITS, baja_data->gps_pos.latitude);
		load_intx(bits, &i, GPS_POS_BITS, baja_data->gps_pos.longitude);
	}

	if (sent_data_points & BATTERY_DATA) {
		load_uintx(bits, &i, BATTERY_BITS, baja_data->battery_data.percentage);
	}

	if (sent_data_points & SD_LOGGING_DATA) {
		load_uintx(bits, &i, SD_LOGGING_DATA_TOTAL_BITS, baja_data->sd_logging_data.active);
	}

	pack_bits(buffer, bits, data_size_bits);
	free(bits);

	if (data_size_bits % 8 == 0)
		return data_size_bits / 8;
	return (data_size_bits / 8) + 1;
}

sent_data_points_t unpack_data(const char* buffer, baja_data_t* baja_data) {

	int i = 0;
	char bits_initial[SENT_DATA_POINTS_BITS] = { 0 };
	unpack_bits(buffer, bits_initial, SENT_DATA_POINTS_BITS);
	sent_data_points_t recv_data_points = read_uintx(bits_initial, &i, SENT_DATA_POINTS_BITS);

	int data_size_bits = SENT_DATA_POINTS_BITS;
	if (recv_data_points & CVT_DATA)
		data_size_bits += CVT_DATA_TOTAL_BITS;
	if (recv_data_points & WHEEL_SPEEDS)
		data_size_bits += WHEEL_SPEEDS_TOTAL_BITS;
	if (recv_data_points & SUS_DISPLACEMENTS)
		data_size_bits += SUS_DISPLACEMENTS_TOTAL_BITS;
	if (recv_data_points & PEDAL_DATA)
		data_size_bits += PEDAL_DATA_TOTAL_BITS;
	if (recv_data_points & ACCEL_DATA)
		data_size_bits += ACCEL_DATA_TOTAL_BITS;
	if (recv_data_points & GYRO_DATA)
		data_size_bits += GYRO_DATA_TOTAL_BITS;
	if (recv_data_points & GPS_LAT_LONG)
		data_size_bits += GPS_POS_TOTAL_BITS;
	if (recv_data_points & BATTERY_DATA)
		data_size_bits += BATTERY_DATA_TOTAL_BITS;
	if (recv_data_points & SD_LOGGING_DATA)
		data_size_bits += SD_LOGGING_DATA_TOTAL_BITS;

	char* bits = (char*)malloc(data_size_bits);
	if (!bits) return 0;

	unpack_bits(buffer, bits, data_size_bits);

	i = 0;
	read_uintx(bits, &i, SENT_DATA_POINTS_BITS);

	if (recv_data_points & CVT_DATA) {
		baja_data->cvt_data.primary = read_uintx(bits, &i, CVT_RPM_BITS);
		baja_data->cvt_data.secondary = read_uintx(bits, &i, CVT_RPM_BITS);
		baja_data->cvt_data.temperature = read_uintx(bits, &i, CVT_TEMP_BITS);
		baja_data->cvt_data.secondaryTemperature = read_uintx(bits, &i, CVT_TEMP_BITS);
		baja_data->cvt_data.beltTemperature = read_uintx(bits, &i, CVT_TEMP_BITS);
	}

	if (recv_data_points & WHEEL_SPEEDS) {
		baja_data->wheel_speeds.fl = read_uintx(bits, &i, WHEEL_SPEED_BITS);
		baja_data->wheel_speeds.fr = read_uintx(bits, &i, WHEEL_SPEED_BITS);
		baja_data->wheel_speeds.rl = read_uintx(bits, &i, WHEEL_SPEED_BITS);
		baja_data->wheel_speeds.rr = read_uintx(bits, &i, WHEEL_SPEED_BITS);
	}

	if (recv_data_points & SUS_DISPLACEMENTS) {
		baja_data->sus_displacements.fl = read_uintx(bits, &i, SUS_DISPLACEMENT_BITS);
		baja_data->sus_displacements.fr = read_uintx(bits, &i, SUS_DISPLACEMENT_BITS);
		baja_data->sus_displacements.rl = read_uintx(bits, &i, SUS_DISPLACEMENT_BITS);
		baja_data->sus_displacements.rr = read_uintx(bits, &i, SUS_DISPLACEMENT_BITS);
	}

	if (recv_data_points & PEDAL_DATA) {
		baja_data->pedal_data.gas = read_uintx(bits, &i, PEDAL_BITS);
		baja_data->pedal_data.brake = read_uintx(bits, &i, PEDAL_BITS);
		baja_data->pedal_data.frontPressure = read_uintx(bits, &i, PRESSURE_BITS);
		baja_data->pedal_data.rearPressure = read_uintx(bits, &i, PRESSURE_BITS);
	}

	if (recv_data_points & ACCEL_DATA) {
		baja_data->accel_data.x = read_intx(bits, &i, ACCEL_BITS);
		baja_data->accel_data.y = read_intx(bits, &i, ACCEL_BITS);
		baja_data->accel_data.z = read_intx(bits, &i, ACCEL_BITS);
	}

	if (recv_data_points & GYRO_DATA) {
		baja_data->gyro_data.yaw = read_intx(bits, &i, GYRO_BITS);
		baja_data->gyro_data.pitch = read_intx(bits, &i, GYRO_BITS);
		baja_data->gyro_data.roll = read_intx(bits, &i, GYRO_BITS);
	}

	if (recv_data_points & GPS_LAT_LONG) {
		baja_data->gps_pos.latitude = read_intx(bits, &i, GPS_POS_BITS);
		baja_data->gps_pos.longitude = read_intx(bits, &i, GPS_POS_BITS);
	}

	if (recv_data_points & BATTERY_DATA) {
		baja_data->battery_data.percentage = read_uintx(bits, &i, BATTERY_BITS);
	}

	if (recv_data_points & SD_LOGGING_DATA) {
		baja_data->sd_logging_data.active = read_uintx(bits, &i, SD_LOGGING_DATA_TOTAL_BITS);
	}

	free(bits);
	return recv_data_points;
}
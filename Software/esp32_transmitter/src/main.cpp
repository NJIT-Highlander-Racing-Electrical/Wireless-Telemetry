#include <Arduino.h>

#include <heltec_unofficial.h>
#include <baja_data_compression.h>
#include <BajaCAN.h>

#define FREQUENCY           905.2
#define BANDWIDTH           500
#define SPREADING_FACTOR    7
#define TRANSMIT_POWER      22

//#define RADIO_TEST

baja_data_t all_data;
sent_data_points_t current_recv_data_points = 0;

union crc_value
{
	uint16_t intVal;
	char strVal[2];
};

void setup()
{
	Serial.begin(115200);

	heltec_setup();
	heltec_ve(true);

	RadioLibCRCInstance.size = 16;
	RadioLibCRCInstance.poly = 0x755B;
	RadioLibCRCInstance.init = 0xFFFF;
	RadioLibCRCInstance.out = 0x0000;

	both.println("Radio init");
	radio.begin(FREQUENCY, BANDWIDTH, SPREADING_FACTOR, 7, 0x13, 0, 8);
	both.println("FAST");

	if (radio.setCRC(0) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION)
	{
		both.println("CRC config error");
	}

	radio.setOutputPower(TRANSMIT_POWER);

	setupCAN(BASE_STATION);

	twai_status_info_t status;
	twai_get_status_info(&status);
	Serial.printf("TWAI state: %d, rx: %d, tx: %d\n", status.state, status.msgs_to_rx, status.msgs_to_tx);
}

int a = 0;

int16_t send_packet(sent_data_points_t sd, const baja_data_t* bt)
{
	char buf[100];
	memset(buf, 0, 100);

	size_t byteCount = pack_data(buf, bt, sd);

	union crc_value checksum;
	checksum.intVal = RadioLibCRCInstance.checksum((const uint8_t*)buf, byteCount);

	char* fullStr = (char*)malloc(byteCount + 2);
	if (!fullStr)
	{
		both.println("malloc failed");
		return RADIOLIB_ERR_UNKNOWN;
	}

	memcpy((void*)fullStr, (const void*)buf, byteCount);
	fullStr[byteCount] = checksum.strVal[0];
	fullStr[byteCount + 1] = checksum.strVal[1];

	int16_t status = radio.transmit((const uint8_t*)fullStr, byteCount + 2, 0);

	free(fullStr);
	return status;
}

void populate_all_data()
{
	memset(&all_data, 0, sizeof(all_data));

	all_data.cvt_data.primary = (uint16_t)primaryRPM;
	all_data.cvt_data.secondary = (uint16_t)secondaryRPM;
	all_data.cvt_data.temperature = (uint8_t)primaryTemperature;
	all_data.cvt_data.secondaryTemperature = (uint8_t)secondaryTemperature;
	all_data.cvt_data.beltTemperature = (uint8_t)beltTemperature;

	all_data.wheel_speeds.fl = (uint16_t)roundf(frontLeftWheelSpeed);
	all_data.wheel_speeds.fr = (uint16_t)roundf(frontRightWheelSpeed);
	all_data.wheel_speeds.rl = (uint16_t)roundf(rearLeftWheelSpeed);
	all_data.wheel_speeds.rr = (uint16_t)roundf(rearRightWheelSpeed);

	all_data.pedal_data.gas = (uint8_t)gasPedalPercentage;
	all_data.pedal_data.brake = (uint8_t)brakePedalPercentage;
	all_data.pedal_data.frontPressure = (uint16_t)frontBrakePressure;
	all_data.pedal_data.rearPressure = (uint16_t)rearBrakePressure;

	all_data.accel_data.x = (int16_t)roundf(accelerationX * 100.0f);
	all_data.accel_data.y = (int16_t)roundf(accelerationY * 100.0f);
	all_data.accel_data.z = (int16_t)roundf(accelerationZ * 100.0f);

	all_data.gyro_data.roll = (int16_t)roundf(gyroscopeRoll * 10.0f);
	all_data.gyro_data.pitch = (int16_t)roundf(gyroscopePitch * 10.0f);
	all_data.gyro_data.yaw = (int16_t)roundf(gyroscopeYaw * 10.0f);

	all_data.gps_pos.latitude = (int32_t)roundf(gpsLatitude * 100000.0f);
	all_data.gps_pos.longitude = (int32_t)roundf(gpsLongitude * 100000.0f);

	all_data.battery_data.percentage = (uint8_t)batteryPercentage;
	all_data.sd_logging_data.active = (uint8_t)sdLoggingActive;
}

void loop()
{
	heltec_loop();

	#ifdef RADIO_TEST
	if (button.isSingleClick())
	{
		both.printf("Button %i\n", ++a);

		baja_data_t bt;
		sent_data_points_t sd = 0;
		sd |= CVT_DATA;
		sd |= PEDAL_DATA;
		sd |= GPS_LAT_LONG;

		for (int i = 0; i < 500; ++i)
		{
			bt.gps_pos.latitude = 1612345;
			bt.gps_pos.longitude = -1665432;
			bt.cvt_data.primary = 22;
			bt.cvt_data.secondary = i;
			bt.cvt_data.temperature = 37;
			bt.wheel_speeds.fr = i * 2;

			bt.pedal_data.gas = 1;
			bt.pedal_data.brake = 2;
			bt.pedal_data.frontPressure = 3;
			bt.pedal_data.rearPressure = 4;

			if (i % 10 == 0)
				sd |= WHEEL_SPEEDS;

			if (send_packet(sd, &bt) != RADIOLIB_ERR_NONE)
			{
				both.println("TX failed");
			}

			if (sd & WHEEL_SPEEDS)
				sd ^= WHEEL_SPEEDS;
		}
	}
	#else
	current_recv_data_points = CVT_DATA
		| WHEEL_SPEEDS
		| PEDAL_DATA
		| ACCEL_DATA
		| GYRO_DATA
		| GPS_LAT_LONG
		| BATTERY_DATA
		| SD_LOGGING_DATA;

	delay(100);

	Serial.println("Attempting to send packet");
	Serial.print("sd = ");
	Serial.println(current_recv_data_points);

	populate_all_data();
	Serial.printf("battery = %d\n", batteryPercentage);
	Serial.printf("primaryRPM = %d\n", primaryRPM);
	Serial.printf("secondaryRPM = %d\n", secondaryRPM);


	if (send_packet(current_recv_data_points, &all_data) != RADIOLIB_ERR_NONE)
	{
		both.println("TX failed");
	}
	#endif
}
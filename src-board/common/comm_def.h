typedef struct {
	int16_t x, y, z;
} accelerations_t;

typedef struct {
	uint16_t marker; //Must be 0x FIXME выбрать маркер пакета

	uint16_t number;

	uint32_t pressure;
	int32_t temperature_bmp;

	int16_t temperature_ds18;

	int16_t temperature_dht;
	uint16_t humidity;

	accelerations_t accelerations[10];

	uint16_t luminosity1, luminosity2, luminosity3;

	uint16_t temperature_soil1, temperature_soil2, temperature_soil3;

	struct {
		uint16_t adc;
		uint16_t digipot;
	} soilresist_data[3];

	float latitude, longtitude, height;

	uint32_t time;

	uint32_t checksumm;
} gr_telemetry_t;

typedef struct {
	uint16_t size;
	accelerations_t * data;
} gr_accelerationspack_t; //FIXME подумать над названием
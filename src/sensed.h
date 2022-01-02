#ifndef SENSED_H
#define SENSED_H

#ifdef SENSED_USE_EXTENSIONS
#include "sensed_extensions.h"
#undef SENSED_USE_EXTENSIONS
#endif 


#ifndef SENSED_MAX_CARGO
// Adjusted to make fit in a NRF24 32-bit packet (up to 4 uint32/floats or 8 uint16)
#define SENSED_MAX_CARGO  16  
#endif

#define SENSED_SENSOR_NAME_LENGTH    4
#define CURRENT_SENSED_VERSION       3

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif


#define sensor_class(x) (x? 1+x/16: 0)
/*
 ^ Class 0: no value or string value (depending on msgglen)
 * Class 1: String message
 * Class 2: 1 float value
 * Class 3: n float values (n = datalen/sizeof(float))
 * Class 4..9: Should be == class 3, to allow for more sensor types
 * Class A...  TBD
 *
 */
typedef enum {
	// Class 0
	SS_NOT_SET          =  0x00, // Class 0
	
	// Class 1 
	SS_GENERIC           = 0x01, // Generic, message
	
	// Class 2
	SS_GEIGER_CPM       =  0x10, // GEIGER COUNTS PER MINUTE    
	SS_THERMOMETER      =  0x11, // THERMOMETER
	// ...
	SS_TEST             =  0x1F, // TEST RANDOM / GENERIC 1 FLOAT
	// ..
	// 
	SS_CLASS2_GENERIC   =  0x20, // GEERIC CLASS2 X,Y,Z VALUES
	SS_COMBINED_TPH     =  0x21, // TEMPERATURE PRESSURE HUMIDITY
	SS_QUATERNION       =  0x22, // 3-AXIS ACCELEROMETER ROTATION QUATERNION
	SS_HPR              =  0x23, // HEADING-PITCH-ROLL
	SS_COMBINED_HT      =  0x24  // TEMPERATURE, HUMIDITY
	// ETC
} sensortype_t;


#pragma pack(push, 1)  // Align to 1 byte 
typedef struct {   // Bytes  
  uint8_t        checksum;								// 1  *** MUST be the first byte ***
  uint8_t        version;								// 1 
  uint8_t        sensortype;							// 1
  uint16_t       sensorid;								// 2
  char           name[SENSED_SENSOR_NAME_LENGTH];       // SENSED_SENSOR_NAME_LENGTH = 4
  unsigned long  elapsed;								// sizeof(unsigned long)
  uint8_t        datalen;								// 1 
  byte           data[SENSED_MAX_CARGO];				// SENSED_MAX_CARGO
} sensed_buf_t;
#pragma pack(pop) // Return alignment to whatever it wa

#define SENSED_HDR_SIZE  (sizeof(sensed_buf_t) - SENSED_MAX_CARGO)

#define SENSED_MAX_FLOATS_PER_MESSGE  (SENSED_MAX_CARGO / sizeof(float))


class sensed {
  
  private:
	unsigned long 	since;
    bool            have_name;  
    bool            have_time;
	byte            buf_len;
    sensed_buf_t    buf;
	byte            fence[33 - sizeof(sensortype_t)];

	inline void set_elapsed() { if (! have_time) begin(); buf.elapsed = micros() - since; };
	inline void set_data(void *data, unsigned int len) {
		set_elapsed();
		 if (len <=  SENSED_MAX_CARGO) {
			memcpy(buf.data, data, len); 
			buf.datalen = len;
		} else {
			buf.datalen = 0;
		}
	}	
	unsigned char xor_buf(int from);
	
  public:
	void init(sensortype_t type, unsigned int id=0);
	void set_name(const char *name);
	sensed();
    sensed(sensortype_t type);
    sensed(sensortype_t type, unsigned int id);
    sensed(sensortype_t type, const char *name, unsigned int id);

	inline unsigned int len() { return buf.datalen; };
	inline unsigned int msglen() { return (SENSED_HDR_SIZE+buf.datalen); };
	inline unsigned int buflen() { buf_len = msglen() + sizeof(buf_len); return buf_len; };
	inline unsigned int bufsize() { return sizeof(buf); }

	inline unsigned char *bufptr() { return (unsigned char *) &buf; };
	// x_bufptr devuelve un puntero a un buffer con la longitud por delante
	inline unsigned char *x_bufptr() { unsigned int x = buflen(); return (unsigned char *) &buf_len; };

	inline char *name() { buf.name[sizeof(buf.name) - 1] = 0;  return buf.name; };
	inline sensortype_t type() { return (sensortype_t) buf.sensortype; };
	inline unsigned long elapsed() { return buf.elapsed; };
	inline unsigned char version() { return buf.version; };
	inline uint16_t id() { return buf.sensorid; };

	
	inline float get_float() {float f; memcpy (&f, buf.data, sizeof(float)); return f; };
	inline int get_int() {int i; memcpy (&i, buf.data, sizeof(int)); return i; };
	inline char *get_str() { return (char *) buf.data; };
    inline void move_bytes(void *data, unsigned int num) { memcpy(data, buf.data, num); };
		
    inline void set(float value) { set_data((void *) &value, sizeof(float)); };
    inline void set(int value)   { set_data((void *) &value, sizeof(int)); };
	inline void set(char *value) { set_data((void *) value, strlen(value)+1); };
	inline void set(void *data, unsigned int size)  { set_data(data, size); };

	inline void begin() { have_time=true; since = micros(); buf.datalen = 0; };
	
	unsigned char set_checksum();
	unsigned char check_checksum();
    
	void dump(Stream *tty);
};
 
 
 
 
 
 
#endif // #sensed_H
  

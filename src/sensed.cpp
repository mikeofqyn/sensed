#include <sensed.h>


sensed::sensed(sensortype_t type, const char *name, unsigned int id)
{
	this->init(type, id);
	this->set_name(name);
}

sensed::sensed(sensortype_t type, unsigned int id) 
{
	this->init(type, id);
	set_name("");
}

sensed::sensed(sensortype_t type) 
{
	this->init(type, 0);
	set_name("");
}

sensed::sensed()
{
	this->init(SS_NOT_SET, 0);
	this->set_name("");
}


void sensed::init(sensortype_t type, unsigned int id) 
{
	memset (&buf, 0, sizeof(buf));
	buf.sensortype = (uint8_t) type;
	buf.sensorid = id;
	buf.version = CURRENT_SENSED_VERSION;
}

void sensed::set_name(const char *name) 
{
	if (name) {
		const char *namr = (name && *name)? name: "N/A";
		int l = strlen(namr);
		int m = l > 3 ? 3 : l;
		memcpy (buf.name, namr, m);
		buf.name[3] = 0;
		if (name && *name) have_name = true;
	}
}

/* To cumpute checksum an XOR of all bytes except chechum byte is performed. 
 * To check parity the  XOR of all bytes including computed checksum must be 0
 *
 * a   =  0 0 0 0 0 1 0 1  = 5
 * b   =  0 0 0 0 1 0 0 1  = 9
 * a^b =  0 0 0 0 1 1 0 0  = 12    c = a XOR b
 * CHECK  0 0 0 0 0 0 0 0  = 0     Checksum must be 0      
 * 
 * Checksum byte MUST be the first in the structure  
 */

 unsigned char sensed::xor_buf(int from) { 
	int len = SENSED_HDR_SIZE + buf.datalen;
	unsigned char *barr = (unsigned char *) &buf;
	unsigned char chk = barr[from] ^barr[from+1];
	for (int i=from+2; i<len; i++) 
		chk = chk ^ barr[i];
	return chk;
}
 
 unsigned char sensed::set_checksum() { 
	buf.checksum = this->xor_buf(1);
	return buf.checksum;
}

unsigned char sensed::check_checksum() { 
	return  this->xor_buf(0);
}

void sensed::dump(Stream *tty) {
	tty->println();
	tty->print(F("Since(us): ")); tty->println(since);
	tty->print(F("has name?: ")); tty->println(have_name); 
	tty->print(F("has time?: ")); tty->println(have_time);
	tty->println();
	tty->print(F("BUFLEN...: ")); tty->println(buf_len);
	tty->print(F("Checksum : ")); tty->println(buf.checksum);
	tty->print(F("Version  : ")); tty->println(buf.version);
	tty->print(F("Class    : ")); tty->println((int)sensor_class(buf.sensortype));
	tty->print(F("Type     : ")); tty->println((int)buf.sensortype);
	tty->print(F("Name:    : ")); tty->println(buf.name);
	tty->print(F("Sensor ID: ")); tty->println(buf.sensorid);
	tty->print(F("Elapsed  : ")); tty->println(buf.elapsed);
	tty->print(F("Length   : ")); tty->println(buf.datalen);

	tty->print(F("HEX DATA : "));
	for (int i=0; i<buf.datalen; i++) {
		tty->print(buf.data[i], HEX);
		tty->print(" ");
	}
	tty->println();
	byte *c = (byte *)&buf;
	for (unsigned int j=0; j<msglen(); j++) {
		tty->print(*c, HEX); 
		c++;
		tty->print(" ");
	}
	tty->println();
}

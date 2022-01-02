typedef enum sensed_vtype_e
{
	sensed_notset = 0,
	sensed_str,
	sensed_flt,
	sensed_int16
} sensed_vtype_t;

typedef struct sensed_trentry_s
{
  uint8_t          sensortype;
  sensed_vtype_t   valuetype;
  uint8_t          numvalues;
  const char*      tname;
  const char*      vnames[4];
} sensed_trentry_t;

const char *nulstr = "";

static sensed_trentry_t sensed_trtab[] = 
{
//  stype vtype         nVal tname           vnames
//  ===== ====          ==== ==========      ==================================================
//CLASS 0
  { 0x00, sensed_notset, 1,  "*UNDEFINED*",  { nulstr, nulstr, nulstr, nulstr }               },
//CLASS 1
  { 0x01, sensed_str,    1,  "MESSAGE",      { "Message", nulstr, nulstr, nulstr }            },
//CLASS 2
  { 0x10, sensed_flt,    1,  "GEIGER",       { "CPM", nulstr, nulstr, nulstr }                },
  { 0x11, sensed_flt,    1,  "THERMOMETER",  { "Temp", nulstr, nulstr, nulstr }               },
  { 0x1F, sensed_flt,    1,  "RANDOM",       { "RANDOM", nulstr, nulstr, nulstr }             },
//CLASS 3
  { 0X20, sensed_flt,    3,  "XYZ",          { "X", "Y", "Z", nulstr }                        },
  { 0x21, sensed_flt,    3,  "DHTP",         { "Temperature", "Humidity", "Pressure", nulstr} },
  { 0x22, sensed_flt,    4,  "QUATERNION",   { "q0", "q1", "q2", "q3" }                       },
  { 0x23, sensed_flt,    3,  "AHRS",         { "Heading", "Pitch", "Roll", nulstr }           },
  { 0x24, sensed_flt,    2,  "DHT",          { "Temperature", "Humidity", nulstr, nulstr }    }
};

const uint8_t SENSED_NUMSENSORS = (sizeof(sensed_trtab) / sizeof(sensed_trentry_t));

static uint8_t sensed_typeindex(uint8_t stype) 
{
	uint8_t pos = 0;
	for (uint8_t i=0; i<SENSED_NUMSENSORS; i++) 
	{
		if (stype==sensed_trtab[i].sensortype)
		{
			return i;
		}
	}
	return 0;
}

static sensed_vtype_t sensed_vtype(uint8_t index) 
{
	if (index && index<SENSED_NUMSENSORS)
		return sensed_trtab[index].valuetype;
	return sensed_notset;
}

static uint8_t sensed_nvalues(uint8_t index) 
{
	if (index && index<SENSED_NUMSENSORS)
		return sensed_trtab[index].numvalues;
	return 0;
}


static const char* sensed_typename(uint8_t index) 
{
  if (index && index<SENSED_NUMSENSORS)
    return sensed_trtab[index].tname;
  return nulstr;
}

static const char* sensed_valuename(uint8_t index, uint8_t vnum)  // vnum: 0..n-1
{
  if (index && index<SENSED_NUMSENSORS)
    if (vnum < sensed_trtab[index].numvalues)
      return sensed_trtab[index].vnames[vnum];
  return nulstr;
}


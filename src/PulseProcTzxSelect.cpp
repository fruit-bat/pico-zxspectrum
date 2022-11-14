#include "PulseProcTzxSelect.h"
#include <cstring>

PulseProcTzxSelect::PulseProcTzxSelect() 
{}
  
void PulseProcTzxSelect::init(PulseProc *nxt) {
  next(nxt);
}

/**
 * ID 28 - Select block
 * 
 * length: [00,01]+02
 * 
 * Offset	Value	Type	Description
 * 0x00	-	WORD	Length of the whole block (without these two bytes)
 * 0x02	N	BYTE	Number of selections
 * 0x03	-	SELECT[N]	List of selections
 */
int32_t __not_in_flash_func(PulseProcTzxSelect::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  
  DBG_PULSE("PulseProcTzxSelect: About to read TZX select\n");

  const int8_t l[] = {-2, 1};
  uint32_t n;
  if (is->decodeLsbf(&n, l, 2) < 0) {
    DBG_PULSE("PulseProcTzxSelect: failed to read number of options\n");
    return PP_ERROR;
  }
  DBG_PULSE("PulseProcTzxSelect: number of options %ld\n", n);

  /**
  * SELECT structure format
  * 
  * Offset	Value	Type	Description
  * 0x00	-	WORD	Relative Offset
  * 0x02	L	BYTE	Length of description text
  * 0x03	-	CHAR[L]	Description text (please use single line and max. 30 chars)
  */
  _offsets.clear();  
  while (n-- > 0) {
    const int8_t lo[] = {
      2,
      1
    };
    uint32_t h[2];
    if (is->decodeLsbf(h, lo, 2) < 0) {
      DBG_PULSE("PulseProcTzxSelect: failed to read option header\n");
      return PP_ERROR;
    }   
    DBG_PULSE("PulseProcTzxSelect: block offset %ld\n", h[0]);
    DBG_PULSE("PulseProcTzxSelect: option text length %ld\n", h[1]);
    _offsets.push_back(h[0]);
    uint8_t text[64];
    memset(text, 0, sizeof(text));
    for(uint32_t i = 0; i < h[1]; ++i) {
      int32_t r = is->readByte();
      if (r < 0) {
        DBG_PULSE("PulseProcTzxSelect: Error (%ld) reading option text\n", r);
        return PP_ERROR;
      }      
      if (i < 63) text[i] = r;
    }
    DBG_PULSE("PulseProcTzxSelect: option text %s\n", text);
  }
  *top = next();
  return PP_PAUSE;
}

int32_t PulseProcTzxSelect::offset(uint32_t choice) {
  return choice < _offsets.size() ? _offsets[choice] : 1;
}


#include "PulseProcSymdefs.h"

PulseProcSymdefs::PulseProcSymdefs(std::vector<uint16_t>* sd) :
  _sd(sd),
  _np(0),
  _as(0)
{
}

void PulseProcSymdefs::init(
  PulseProc *nxt, 
  uint32_t np,
  uint32_t as
) {
  next(nxt);
  _np = np;
  _as = as;
}

int32_t PulseProcSymdefs::advance(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  _sd->clear();
  
  int32_t r;
  
  for (uint32_t i = 0; i < _as; ++i) {
    
    r = is->readByte(); 
    
    if (r < 0) {
      DBG_PULSE("PulseProcSymdefs: Error (%ld) reading symbol type\n", r);      
      return PP_ERROR;
    }
    DBG_PULSE("PulseProcSymdefs: symbol type %ld\n", r);      

    // read the type byte
    _sd->push_back(static_cast<uint16_t>(r));
    
    // read the symbol data
    for (uint32_t j = 0; j < _np; ++j) {
      r = is->readWord(); 
      if (r < 0) {
        DBG_PULSE("PulseProcSymdefs: Error (%ld) reading symbol pulse\n", r);      
        return PP_ERROR;
      }
      DBG_PULSE("PulseProcSymdefs: symbol pulse %ld\n", r);      

      // read the pulse width byte
      _sd->push_back(static_cast<uint16_t>(r));
    }
  }
  
  return PP_COMPLETE;
}

#include "ZxSpectrumSnapList.h"

ZxSpectrumSnapList::ZxSpectrumSnapList() : _curr(0) {}

void ZxSpectrumSnapList::add(ZxSpectrumSnap *snap) {
  if (_curr) snap->link(_curr);
  _curr = snap;
}

void ZxSpectrumSnapList::next(ZxSpectrum *specy) {
  if (_curr) {
     _curr->load(specy);
    _curr = _curr->next();
  }
}
 
void ZxSpectrumSnapList::prev(ZxSpectrum *specy) {
  if (_curr) {
    _curr->load(specy);
    _curr = _curr->prev();
  }
}

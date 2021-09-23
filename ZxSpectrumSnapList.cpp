#include "ZxSpectrumSnapList.h"

ZxSpectrumSnapList::ZxSpectrumSnapList() : _curr(0) {}

void ZxSpectrumSnapList::add(ZxSpectrumSnap *snap) {
  if (_curr) snap->link(_curr);
  _curr = snap;
}

void ZxSpectrumSnapList::next(ZxSpectrum *specy) {
  if (_curr) {
    _curr = _curr->next();
     _curr->load(specy);
  }
}
 
void ZxSpectrumSnapList::prev(ZxSpectrum *specy) {
  if (_curr) {
    _curr = _curr->prev();
    _curr->load(specy);
  }
}

void ZxSpectrumSnapList::curr(ZxSpectrum *specy) {
  if (_curr) {
    _curr->load(specy);
  }
}

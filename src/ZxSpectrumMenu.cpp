#include <algorithm>

#include "ZxSpectrumMenu.h"
#include "ZxSpectrum.h"
#include "PicoPen.h"
#include "FatFsSpiDirReader.h"
#include "FatFsSpiInputStream.h"
#include "BufferedInputStream.h"
#include "hardware/clocks.h"

#define SAVED_SNAPS_DIR "/zxspectrum/snapshots"
#define SAVED_QUICK_DIR "/zxspectrum/quicksaves"
#define SAVED_TAPES_DIR "/zxspectrum/tapes"

#if PCS_COLS == 40
#define SZ_WIZ_COLS 40
#define SZ_WIZ_ML 0
#define SZ_WIZ_CW1 12
#define SZ_WIZ_CW2 18
#else
#define SZ_WIZ_COLS 70
#define SZ_WIZ_ML 5
#define SZ_WIZ_CW1 16
#define SZ_WIZ_CW2 40
#endif

ZxSpectrumMenu::ZxSpectrumMenu(SdCardFatFsSpi* sdCard, ZxSpectrum *zxSpectrum, QuickSave *quickSave) :
 PicoWin(0, 0, PCS_COLS, PCS_ROWS),
  _sdCard(sdCard),
  _zxSpectrum(zxSpectrum),
  _k1('1'), _k2('2'), _k3('3'), _k4('4'), _k5('5'), _k6('6'), 
  _wiz(SZ_WIZ_ML, 6, SZ_WIZ_COLS, 18),
  _main(0, 0, SZ_WIZ_COLS, 6, 3),
  _quickSavesOp("Quick saves"),
  
  _snapMgr(0, 0, SZ_WIZ_COLS, 6, 3),
  _snapLoadOp("Load"),
  _snapRenameOp("Rename"),
  _snapDeleteOp("Delete"),
  
  _tapePlayer(0, 0, SZ_WIZ_COLS, 6, 3),

  _chooseTapeOp("Choose a tape"),
  _ejectTapeOp("Eject tape"),
  _chooseTape(0, 0, SZ_WIZ_COLS, 9, 2),
  _chooseSnap(0, 0, SZ_WIZ_COLS, 9, 2),
  
  _reset(0, 0, SZ_WIZ_COLS, 6, 3),
  _reset48kOp("Reset 48K ZX Spectrum"),
  _reset128kOp("Reset 128K ZX Spectrum"),

  _message(0, 0, SZ_WIZ_COLS, 12),
  _confirm(0, 0, SZ_WIZ_COLS, 6, 3),
  _confirmNo("No"),
  _confirmYes("Yes"),
  
  _quickSaves(0, 0, SZ_WIZ_COLS, 12, 1),
  
  _quickSave(0, 0, SZ_WIZ_COLS, 6, 3),
  _quickSaveLoadOp("Load"),
  _quickSaveToSnapOp("Save as SNAP"),
  _quickSaveClearOp("Clear"),
  
  _quickSaveHelper(quickSave),
  
  _fileName(0, 0,SZ_WIZ_COLS, 64)
{
  addChild(&_wiz, true);
  _wiz.push(&_main, [](PicoPen *pen){ pen->printAt(0, 0, false, "Main menu"); }, true);
  _wiz.onKeydown([=](uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
     return (ascii != 27) || !_wiz.pop(true);
   });
   
  _confirm.addOption(_confirmNo.addQuickKey(&_k1));
  _confirm.addOption(_confirmYes.addQuickKey(&_k2));
  _confirm.enableQuickKeys();

  _main.addOption(_snapOp.addQuickKey(&_k1));
  _main.addOption(_tapePlayerOp.addQuickKey(&_k2));
  _main.addOption(_quickSavesOp.addQuickKey(&_k3));
  _main.addOption(_freqOp.addQuickKey(&_k4));
  _main.addOption(_muteOp.addQuickKey(&_k5));
  _main.addOption(_resetOp.addQuickKey(&_k6));

  _main.enableQuickKeys();
  _snapOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Snapshot", SZ_WIZ_CW2, _snapName.c_str());
  });
  _snapOp.toggle([=]() {
    _wiz.push(
      &_snapMgr, 
      [=](PicoPen *pen){ 
        pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Snapshot", SZ_WIZ_CW2, _snapName.c_str());
      },
      true);
  });
  _tapePlayerOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Tape player", SZ_WIZ_CW2, _tapeName.c_str());
  });
  _tapePlayerOp.toggle([=]() {
    _wiz.push(
      &_tapePlayer, 
      [=](PicoPen *pen){ 
        pen->printAtF(0, 0, false,"Tape player [ %-*s]", SZ_WIZ_CW2, _tapeName.c_str());
      }, 
      true);
  });
  _freqOp.toggle([=]() {
    _zxSpectrum->toggleModerate();
    _main.repaint();
  });
  _freqOp.onPaint([=](PicoPen *pen){
    pen->clear();
    const char *m;
    switch(_zxSpectrum->moderate()) {
      case 9: m = "3.5 Mhz" ; break;
      case 8: m = "4.0 Mhz" ; break;
      case 0: m = "Unmoderated" ; break;
      default: m = "Unknown" ; break;
    }
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "CPU Speed", SZ_WIZ_CW2, m);
  });
  _muteOp.toggle([=]() {
    _zxSpectrum->toggleMute();
    _main.repaint();
  });
  _muteOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Audio", SZ_WIZ_CW2, _zxSpectrum->mute() ? "off" : "on");
  });
  _resetOp.onPaint([=](PicoPen *pen){
    pen->clear();
    const char *m;
    switch(_zxSpectrum->type()) {
      case ZxSpectrum48k: m = "48k ZX Spectrum" ; break;
      case ZxSpectrum128k: m = "128k ZX Spectrum" ; break;
      default: m = "" ; break;
    }
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Reset", SZ_WIZ_CW2, m);
  });
  _resetOp.toggle([=]() {
    _wiz.push(
      &_reset, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Reset options"); }, 
      true);
  });
  
  _snapMgr.addOption(_snapLoadOp.addQuickKey(&_k1));
  _snapMgr.addOption(_snapRenameOp.addQuickKey(&_k2));
  _snapMgr.addOption(_snapDeleteOp.addQuickKey(&_k3));
  _snapMgr.enableQuickKeys();
  
  _snapLoadOp.toggle([=]() {
    _wiz.push(
      &_chooseSnap, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Choose snapshot to load"); },
      true);
      
    loadDirAlphabetical(SAVED_SNAPS_DIR, &_chooseSnap);

    _chooseSnap.onToggle([=](PicoOption *option) {
      PicoOptionText *textOption = (PicoOptionText *)option;
      std::string fname(SAVED_SNAPS_DIR);
      fname.append("/");
      fname.append(textOption->text());
      _snapName = textOption->text();
      FatFsSpiInputStream *is = new FatFsSpiInputStream(_sdCard, fname.c_str());
      _zxSpectrum->loadZ80(is);
      delete is;
      _wiz.pop(true);
    });
  });
  
  _snapRenameOp.toggle([=]() {
    _wiz.push(
      &_chooseSnap, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Choose snapshot to rename"); },
      true);
      
    loadDirAlphabetical(SAVED_SNAPS_DIR, &_chooseSnap);

    _chooseSnap.onToggle([=](PicoOption *option) {
      PicoOptionText *textOption = (PicoOptionText *)option;
      _fileName.clear();
      _fileName.onenter([=](const char* name) {
        std::string fname;
        snapName(fname, name);
        if (checkExists(fname.c_str())) {
          showError([=](PicoPen *pen) {
            pen->printAtF(0, 0, false, "Error: already exists '%s'", name);
          });
        }
        else {
          std::string fnameo;
          snapName(fnameo, textOption->text());

          if(renameSave(fnameo.c_str(), fname.c_str())) {
            loadDirAlphabetical(SAVED_SNAPS_DIR, &_chooseSnap);
            _wiz.pop(true);
          }
          else {
            showError([=](PicoPen *pen) {
              pen->printAtF(0, 0, false, "Error: failed to rename to '%s'", name);
            });
          }
        }
      });
      _wiz.push(
        &_fileName, 
        [=](PicoPen *pen){ 
          pen->printAtF(0, 0, false, "Enter new name for [ %s ]", textOption->text()); 
        },
        true);
    });
  });
  
  _snapDeleteOp.toggle([=]() {
    _wiz.push(
      &_chooseSnap, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Choose snapshot to delete"); },
      true);
      
    loadDirAlphabetical(SAVED_SNAPS_DIR, &_chooseSnap);

    _chooseSnap.onToggle([=](PicoOption *option) {
      PicoOptionText *textOption = (PicoOptionText *)option;
      confirm(
        [=](PicoPen *pen){
          pen->printAtF(0, 0, false, "Delete snapshot '%s'?", textOption->text());
        },
        [=]() {
          deleteSave(SAVED_SNAPS_DIR, textOption->text());
          loadDirAlphabetical(SAVED_SNAPS_DIR, &_chooseSnap);
        }
      );
    });
  });
  
  _reset.addOption(_reset48kOp.addQuickKey(&_k1));
  _reset.addOption(_reset128kOp.addQuickKey(&_k2));
  _reset.enableQuickKeys();

  _reset48kOp.toggle([=]() {
    _snapName.clear();
    _zxSpectrum->reset(ZxSpectrum48k);
    _wiz.pop(true);
  });
  _reset128kOp.toggle([=]() {
    _snapName.clear();
    _zxSpectrum->reset(ZxSpectrum128k);
    _wiz.pop(true);
  });

  _tapePlayer.addOption(_chooseTapeOp.addQuickKey(&_k1));
  _tapePlayer.addOption(_ejectTapeOp.addQuickKey(&_k2));
  _tapePlayer.addOption(_pauseTapeOp.addQuickKey(&_k3));
  _tapePlayer.enableQuickKeys();

  _chooseTapeOp.toggle([=]() {
    _wiz.push(
      &_chooseTape, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Choose tape:"); },
      true);
    loadDirAlphabetical(SAVED_TAPES_DIR, &_chooseTape);
  });

  _ejectTapeOp.toggle([=]() {
    ejectTape();
    _wiz.repaint();
  });
  
  _pauseTapeOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Motor", SZ_WIZ_CW2, _zxSpectrum->tapePaused() ? "off" : "on");
  });
  _pauseTapeOp.toggle([=]() {
    _zxSpectrum->togglePauseTape();
  });

  _chooseTape.onToggle([=](PicoOption *option) {
    PicoOptionText *textOption = (PicoOptionText *)option;
    ejectTape();
    std::string fname(SAVED_TAPES_DIR);
    fname.append("/");
    fname.append(textOption->text());
    _tapeName = textOption->text();
    _tis = new FatFsSpiInputStream(_sdCard, fname.c_str());
    _zxSpectrum->loadTap(_tis);
    _wiz.pop(true);
  });
  
  _quickSavesOp.toggle([=]() {
    for(int i = 0; i <12; ++i) {
      _quickSaveSlotUsed[i] = _quickSaveHelper->used(i);
    }
    _wiz.push(
      &_quickSaves, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Quick save"); }, 
      true);
  });
  
  for(int i = 0; i < 12; ++i) {
    PicoOption *qs = &_quickOps[i]; 
    _quickSaves.addOption(qs);
    qs->onPaint([=](PicoPen *pen){
      pen->clear();
      pen->printAtF(0, 0, false,"Slot %2d%-9s[ %-4s ]", i + 1, "", _quickSaveSlotUsed[i] ? "used" : "");
    });
    qs->toggle([=]() {
      if (_quickSaveHelper->used(i)) {
        _wiz.push(
          &_quickSave, 
          [=](PicoPen *pen){ pen->printAtF(0, 0, false, "Quick save slot %d", i + 1); }, 
          true);
        _quickSaveSlot = i;
      }
    });
  }

  _quickSave.addOption(_quickSaveLoadOp.addQuickKey(&_k1));
  _quickSave.addOption(_quickSaveClearOp.addQuickKey(&_k2));
  _quickSave.addOption(_quickSaveToSnapOp.addQuickKey(&_k3));
  _quickSave.enableQuickKeys();

  _quickSaveLoadOp.toggle([=]() {
    _quickSaveHelper->load(_zxSpectrum, _quickSaveSlot);
    _wiz.pop(true);
  });
  
  _quickSaveToSnapOp.toggle([=]() {
    _fileName.clear();
    _wiz.push(
      &_fileName, 
      [=](PicoPen *pen){ pen->printAtF(0, 0, false, "Quick save slot %d", _quickSaveSlot + 1); }, 
      true);
    _fileName.onenter([=](const char* name) {
      std::string fname;
      snapName(fname, name);
      if (checkExists(fname.c_str())) {
        confirm(
          [=](PicoPen *pen){
            pen->printAtF(0, 0, false, "'%s' exits. Overwrite?", name);
          },
          [=]() {
            quickSaveToSnap(_quickSaveSlot, name, fname.c_str());
          }
        );
      }
      else {
        quickSaveToSnap(_quickSaveSlot, name, fname.c_str());
      }
    });
  });

  _quickSaveClearOp.toggle([=]() {
    confirm(
      [=](PicoPen *pen){
        pen->printAt(0, 0, false, "Remove quick save ?");
      },
      [=]() {
        if (_quickSaveHelper->clear(_quickSaveSlot)) {
           _quickSaveSlotUsed[_quickSaveSlot] = false;
        }
        _wiz.pop(true);
      }
    );
  });
  
  onPaint([](PicoPen *pen) {
     pen->printAt(0, 0, false, "ZX Spectrum 48K/128K emulator");
     pen->printAtF(0, 1, false, "on RP2040 Pico Pi at %3.1fMhz", (float)clock_get_hz(clk_sys) / 1000000.0);
     pen->printAt(0, 2, false, "Menu System version 0.2");

     pen->printAt(0, 29, false, "F1 to exit menu");
     pen->printAt(SZ_WIZ_COLS-14, 29, false, "ESC to go back");
   }); 
}

void ZxSpectrumMenu::snapName(std::string &fname, const char *name) {
  fname.clear();
  fname.append(SAVED_SNAPS_DIR);
  fname.append("/");
  fname.append(name);
  auto nl = strlen(name);
  if (nl < 4 || !((strncmp(name + nl - 4, ".z80", 4) == 0) || (strncmp(name + nl - 4, ".Z80", 4) == 0))) {
    fname.append(".z80");
  }
}

void ZxSpectrumMenu::quickSaveToSnap(int i, const char *name, const char *fname) {
  if (!_quickSaveHelper->copy(i, fname)) {
    showError([=](PicoPen *pen) {
      pen->printAtF(0, 0, false, "Error: failed to create SNAP '%s'", name);
    });
  }
  else {
    _wiz.pop(true);
  }
}

void ZxSpectrumMenu::ejectTape() {
  // TODO ask the spectrum for the input stream
  if (_tis) {
    _zxSpectrum->loadTap(0);
    _tapeName.clear();
    delete _tis;
    _tis = 0;
  }
}

bool ZxSpectrumMenu::checkExists(const char *file) {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) return false;
  }
  FILINFO fno;
  FRESULT fr = f_stat(file, &fno);
  return fr == FR_OK;
}

bool ZxSpectrumMenu::deleteSave(const char *folder, const char *file) {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) return false;
  }
  std::string fname(folder);
  fname.append("/");
  fname.append(file);
  return f_unlink(fname.c_str()) == FR_OK;
}

bool ZxSpectrumMenu::renameSave(const char *fileo, const char *filen) {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) return false;
  }
  return f_rename(fileo, filen) == FR_OK;
}

void ZxSpectrumMenu::loadDirAlphabetical(const char* folder, PicoSelect *select) {
  FatFsSpiDirReader dirReader(_sdCard, folder);
  int32_t focus = select->focus();
  select->deleteOptions();
  std::vector<std::string> fnames;
  std::vector<const FILINFO*> infos;
  dirReader.foreach([&](const FILINFO* info) { 
    fnames.push_back(info->fname);
  });
  std::sort(fnames.begin(), fnames.end(), [](const std::string& a, const std::string& b) -> bool {
    for (size_t c = 0; c < a.size() and c < b.size(); c++) {
      if (std::tolower(a[c]) != std::tolower(b[c]))
        return (std::tolower(a[c]) < std::tolower(b[c]));
      }
    return a.size() < b.size();
  });
  for (auto &fname : fnames) {
    select->addOption(new PicoOptionText(fname.c_str()));
  };
  select->focus(focus);
}

void ZxSpectrumMenu::showError(std::function<void(PicoPen *pen)> message) {
  _wiz.push(
    &_message, 
    [](PicoPen *pen){ pen->printAt(0, 0, false, "Error:"); },
    true);
  _message.onPaint(message);
}

void ZxSpectrumMenu::confirm(
  std::function<void(PicoPen *pen)> message,
  std::function<void()> yes
) {
  _confirm.focus(0);
  _wiz.push(
    &_confirm, 
    message,
    true);
  _confirmNo.toggle([=]() {
    _wiz.pop(true);
  });
  _confirmYes.toggle([=]() {
    _wiz.pop(true);
    if (yes) yes();
  });
}

void ZxSpectrumMenu::confirm(
  std::function<void(PicoPen *pen)> message,
  std::function<void()> no,
  std::function<void()> yes
) {
  _confirm.focus(0);
  _wiz.push(
    &_confirm, 
    message,
    true);
  _confirmNo.toggle([=]() {
    _wiz.pop(true);
    if (no) no();
  });
  _confirmYes.toggle([=]() {
    _wiz.pop(true);
    if (yes) yes();
  });
}



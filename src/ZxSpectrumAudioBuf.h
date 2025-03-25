#pragma once

// PWM Audio stuff
#define ZX_AUDIO_BUF_SIZE_BITS 7
#define ZX_AUDIO_BUF_SIZE (1<<ZX_AUDIO_BUF_SIZE_BITS)
#define ZX_AUDIO_BUF_MOD_MASK (ZX_AUDIO_BUF_SIZE-1)

// Sample for delayed deliver to PWM output
typedef struct {
  uint16_t vA; 
  uint16_t vB; 
  uint16_t vC; 
  uint16_t s; 
  uint16_t b; 
} zx_audio_sample_t;

typedef struct {
  zx_audio_sample_t sample[ZX_AUDIO_BUF_SIZE];
  uint32_t ri;
  uint32_t wi;
} zx_audio_buf_t;

void zx_audio_buf_init(zx_audio_buf_t *b) {
  b->ri = 0;
  b->wi = 0;
}

inline bool zx_audio_buf_ready_for_write(zx_audio_buf_t *b) {
  return ((b->ri - b->wi) & ZX_AUDIO_BUF_MOD_MASK) != 1; 
}

inline bool zx_audio_buf_ready_for_read(zx_audio_buf_t *b) {
  return b->ri != b->wi; 
}

inline void zx_audio_buf_read_next(zx_audio_buf_t *b) {
  b->ri = (b->ri + 1) & ZX_AUDIO_BUF_MOD_MASK;  
}

inline void zx_audio_buf_write_next(zx_audio_buf_t *b) {
  b->wi = (b->wi + 1) & ZX_AUDIO_BUF_MOD_MASK;  
}

inline zx_audio_sample_t *zx_audio_buf_read_ptr(zx_audio_buf_t *b) {
  return &b->sample[b->ri];
}

inline zx_audio_sample_t *zx_audio_buf_write_ptr(zx_audio_buf_t *b) {
  return &b->sample[b->wi];
}

#include <am.h>
#include <nemu.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)

void __am_audio_init() {
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg)
{
  uint32_t sta = (uint32_t)inl(AUDIO_INIT_ADDR);
  /*最高位为控制位 其余位bufsize*/
  cfg->present = sta >> 31;
  cfg->bufsize = (int)(sta & (0x7fffffff));
}

// 向控制寄存器写入数据 包括 freq channels samples
// 地址已经在/nemu/src/device/audio.c 中通过mmio注册
//  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
//  audio_base=AUDIO_FREQ_ADDR
//  4*3=12 uint32
//   reg_freq,reg_channels,reg_samples,reg_sbuf_size,reg_init,reg_count,nr_reg
void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl)
{
  outl(AUDIO_FREQ_ADDR, ctrl->freq);
  outl(AUDIO_SAMPLES_ADDR, ctrl->samples);
  outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = 0;
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl)
{

  int size = (int)inl(AUDIO_SBUF_SIZE_ADDR);
  uint8_t *start = ctl->buf.start;
  uint8_t *end = ctl->buf.end;
  int i = 0;
  if (size > start-end)
  {

    /*一个Byte一个Byte的传输*/
    for (; start != end; start++)
    {
      outb(AUDIO_SBUF_ADDR + i, *start);
      i++;
    }
  }
  outl(AUDIO_COUNT_ADDR, (int)inl(AUDIO_COUNT_ADDR) + i);
}


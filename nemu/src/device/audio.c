/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;

/* SDL Audio Callback Function */
static void audio_update_handler(void *opaque, Uint8 *stream, int len)
{
  /* read from register by guest program write*/
  if (audio_base[reg_count] > len)
  {
    memcpy(stream, sbuf, len);
    audio_base[reg_count]-=len;
    audio_base[reg_sbuf_size]=CONFIG_SB_SIZE-audio_base[reg_count];
    return;
  }
}
/* the argument read from rigister */
static int audio_config[3];
void audio_init()
{
  SDL_AudioSpec s = {};
  s.format = AUDIO_S16SYS; // 假设系统中音频数据的格式总是使用16位有符号数来表示
  s.channels = audio_config[1];
  s.samples = audio_config[2];
  s.freq = audio_config[0];
  s.userdata = NULL; // 不使用
  s.size = CONFIG_SB_SIZE;
  s.callback = (void *)audio_update_handler;
  SDL_InitSubSystem(SDL_INIT_AUDIO);
  SDL_OpenAudio(&s, NULL);
  SDL_PauseAudio(0);
}

/* 什么时候被调用？ */
/* nemu提供了一个虚拟的物理平台 如qemu一样 */
/* 当程序 也就是在nemu上运行的程序使用 lb lh lw sd等指令*/
/* 这些指令的调用栈如下 */
/*                           是否在pmem中*/
/* vaddr_read-->paddr_read-->pmem_read-->host_read-->guest_to_host*/
/*                        -->mmio_read-->map_read -->invoke_callback-->handler*/
/*                                       根据地址获得IOMap*/

static void audio_io_handler(uint32_t offset, int len, bool is_write)
{
  /* len is lenght */
  /* host program write */
  /* the access addr must in  0xa0000200-0xa000020c */

  /* audio_config 存储三个数据 根据audio_base寄存器分布而分布*/
  if (len == 4 && is_write == 1 && offset % 4 == 0 && offset<=8)
  {
    audio_config[offset / 4] = audio_base[offset / 4];
  }
  /* 当所有数据都获得的时候就初始化 */
  if (audio_config[0] != 0 && audio_config[1] != 0 && audio_config[2] != 0)
  {
    audio_init();
  }
}
void init_audio() {
   /* 6 * uint32_t  32/4=4 */
  /*
  AUDIO_FREQ_ADDR 0x00
  AUDIO_CHANNELS_ADDR 0x04
  AUDIO_SAMPLES_ADDR 0x08
  AUDIO_SBUF_SIZE_ADDR 0x0c
  AUDIO_INIT_ADDR 0x10
  AUDIO_COUNT_ADDR 0x14
  */
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);

  /*tell the host process the audio is regiser*/
  audio_base[4] = 1 << 31 | CONFIG_SB_SIZE;
  /* sb_size */
  audio_base[3] = CONFIG_SB_SIZE;
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);
}

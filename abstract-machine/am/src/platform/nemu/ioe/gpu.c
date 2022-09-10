#include <am.h>
#include <nemu.h>
#include <stdio.h>
#include <string.h>
#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  // int i;
  // int w = inl(VGACTL_ADDR)>>16;  // TODO: get the correct width
  // int h = inl(VGACTL_ADDR)&0xFFFF;  // TODO: get the correct height
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // // for (i = 0; i < w * h; i++) fb[i] = i;
  // outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t width_height = inl(VGACTL_ADDR);
  // printf("$$$$%d %d",width_height&0xFFFF,width_height>>16);

  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = width_height>>16, .height = width_height&0xFFFF,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {

  int ww = inl(VGACTL_ADDR)>>16;  
  // int hh = inl(VGACTL_ADDR)&0xFFFF;  
// int x, y; void *pixels; int w, h; bool sync
  int x= ctl->x;
  int y= ctl->y;
  uint32_t *pixels = (uint32_t *)ctl->pixels;
  int w= ctl->w;
  int h =ctl->h;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;

  for(int i=0;i<h;i++){
    for(int j=0;j<w;j++){
      fb[(i+y)*ww+x+j]=pixels[i*w+j];
    }
    // strncpy(fb+(i+y)*ww+x,pixels+i*w,w);
  }

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

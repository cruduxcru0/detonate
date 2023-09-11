#include "audiodecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <memory>
#include <iostream>

#ifdef LIBRETRO
#define RESAMPLER_IMPLEMENTATION
#include "libretro.h"
#include "resampler.h"
void *resample = NULL;
#else
#include "out_aud.h"
#endif

auddecode *replayer = NULL;
float srate = 0.0;
uint64_t sample_count = 0;
unsigned duration = 0;

const char *get_filename_ext(const char *filename)
{
   const char *dot = strrchr(filename, '.');
   if (!dot || dot == filename)
      return "";
   return dot + 1;
}

std::string auddecode_formats()
{
   std::ostringstream oss;
   char const* sep = "";
   for (int i = 0; auddecode_factory[i].init; ++i) {
   std::unique_ptr<auddecode> replay(auddecode_factory[i].init());
    oss << sep << replay->file_types();
    sep = "|";
   }
   return oss.str();
}

auddecode *make_decoder(const char* filename)
{
   auddecode *replay = NULL;
   for(int i=0;auddecode_factory[i].init;++i)
   {
   replay = auddecode_factory[i].init();
   const char *ext = get_filename_ext(filename);
   if(strcmp(ext,replay->file_types())==0){
   if(!replay->open(filename,&srate,false)){
   delete replay;
   replay=NULL;
   }
   else
   return replay;
   }
   }
}

#ifdef LIBRETRO
void convert_float_to_s16(int16_t *out,
                          const float *in, size_t samples)
{
   size_t i = 0;
   for (; i < samples; i++)
   {
      int32_t val = (int32_t)(in[i] * 0x8000);
      out[i] = (val > 0x7FFF) ? 0x7FFF : (val < -0x8000 ? -0x8000 : (int16_t)val);
   }
}
#endif

bool isplaying=false;
bool music_isplaying(){
    return isplaying;
}

void music_stop()
{
 if(replayer)
    {
        replayer->stop();
        delete replayer;
        replayer = NULL;
        isplaying=false;      
   #ifndef LIBRETRO  
        audio_destroy();
   #else
        resampler_sinc_free(resample);      
   #endif
    }
}

bool music_play(const char* filename)
{
   music_stop();
   sample_count = 0;
   replayer = make_decoder(filename);
   #ifndef LIBRETRO  
   audio_init(0.0,srate,0.0,true);
   #else
   resample = resampler_sinc_init();
   #endif 
   isplaying=true;
   duration=replayer->song_duration();
   return (replayer != NULL)?true:false;
}

uint32_t music_getposition()
{
   return replayer && replayer->is_playing()?uint32_t((1000ull * (sample_count)) / srate):0;
}

uint32_t music_getduration()
{
   return duration;
}

#define BUFSIZE 1024
void music_run()
{
   if(replayer &&replayer->is_playing())
   {
   float *samples2 =NULL;
   unsigned count=0;
   replayer->mix(samples2,count);
   sample_count += count;
   #ifndef LIBRETRO 
   audio_mix(samples2,count);
   #else
   extern retro_audio_sample_batch_t audio_batch_cb;
   if(srate != 44100)
   {
   double ratio = 44100.f/srate;
  int largest_chunk_size = 4096;
  int sampsize= (unsigned int)(ratio*largest_chunk_size*sizeof(float)*2+0.5);
  std::unique_ptr<float[]> output_float = std::make_unique<float[]>(sampsize);
  std::unique_ptr<int16_t[]> samples_int = std::make_unique<int16_t[]>(sampsize);
  float *out_ptr = output_float.get();
  int16_t *int_ptr = samples_int.get();
    struct resampler_data src_data = {0};
    src_data.input_frames = count;
    src_data.ratio = ratio;
    src_data.data_in = samples2;
    src_data.data_out = out_ptr;
    resampler_sinc_process(resample, &src_data);
    convert_float_to_s16(int_ptr,out_ptr,src_data.output_frames*2);
    audio_batch_cb(int_ptr,src_data.output_frames);
   }
   else
   {
   std::unique_ptr<int16_t[]> samples_int = std::make_unique<int16_t[]>(count*2);
   int16_t *int_ptr = samples_int.get();
   convert_float_to_s16(int_ptr, samples2,count*2);
   audio_batch_cb(int_ptr,count);
   }
   #endif

   }
}
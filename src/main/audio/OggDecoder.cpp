#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#define DEFAULT_BUFFER_SIZE 4096


int main(){
  OggVorbis_File vf;
  int eof=0;
  int current_section;
  int ret = ov_open_callbacks(stdin, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE);
  if (ret < 0) {
    switch(ret) {
      // A read from media returned an error.
      case OV_EREAD:
        break;
      // Bitstream does not contain any Vorbis data.
      case OV_ENOTVORBIS:
        break;
      // Vorbis version mismatch.
      case OV_EVERSION:
        break;
      // Invalid Vorbis bitstream header.
      case OV_EBADHEADER:
        break;
      // Internal logic fault; indicates a bug or heap/stack corruption.
      case OV_EFAULT:
        break;
      default:
        break;
    }
  }

  // ref: https://www.xiph.org/vorbis/doc/Vorbis_I_spec.html#x1-810004.3.9
  // ref: https://wiki.libsdl.org/SDL_AudioSpec
  // Channel mapping needed to reorder the audio channels for surround
  vorbis_info *vi = ov_info(&vf, -1);
  int channels = vi->channels;
  long rate = vi->rate;
  double seconds = ov_time_total(&vf, -1);
  
  unsigned char pcmout[DEFAULT_BUFFER_SIZE];

  while(!eof){
    const int USE_LITTLE_ENDIAN = 0;
    const int USE_16BIT_SAMPLES = 2;
    const int USE_UNSIGNED_DATA = 0;
    long ret=ov_read(&vf,pcmout,sizeof(pcmout),USE_LITTLE_ENDIAN,USE_16BIT_SAMPLES,USE_UNSIGNED_DATA,&current_section);
    if (ret > 0) {
      /* we don't bother dealing with sample rate changes, etc, but
         you'll have to*/
      fwrite(pcmout,1,ret,stdout);
    } else {
      switch(ret) {
        case 0:
          /* EOF */
          eof=1;
          break;
        // indicates that an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.
        case OV_EBADLINK:
          fprintf(stderr,"Corrupt bitstream section! Exiting.\n");
          exit(1);
          break;
        // indicates there was an interruption in the data. (one of: garbage between pages, loss of sync followed by recapture, or a corrupt page)
        case OV_HOLE:
          fprintf(stderr,"Interruption in data! Exiting.\n");
          exit(1);
          break;
        default:
          fprintf(stderr,"Weird error %d\n", ret);
          exit(1);
          break;
      }
    }
  }

  ov_clear(&vf);
    
  return(0);
}

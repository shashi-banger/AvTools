
#ifndef __AUDIO_FILE_READER_H__
#define __AUDIO_FILE_READER_H__

#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <unistd.h>
#include "ac_logger.h"
#include "audio_capture_info.h"
#include "data_source.h"

class AudioFileReader : public DataSource<AudioCaptureInfo> {
    enum {
        ERROR_SIZE_INCORRECT = -1,
        ERROR_NOT_IMPLEMENTED = -2,
    };
    SNDFILE     *sf_;
    SF_INFO     sfinfo_;
    int         nc_;
    int64_t     samples_rcvd_;
    public:
        AudioFileReader(char *filename)
        {
            if(access(filename, F_OK) != -1)
            {
                sf_ = sf_open(filename, SFM_READ, &sfinfo_);
                nc_ = sfinfo_.channels;
            }
            else
            {
                ac_log_fatal("%s does not exest\n", filename);
            }
            samples_rcvd_ = 0;
        }
        int ReadBuffer(unsigned char *buf, int   size,   AudioCaptureInfo  *p) {
            int  n_items;
            int  n_items_read;
            int  ret_val = 0;

            n_items = size/sizeof(short);
            if((n_items%nc_) != 0)
            {
                return ERROR_SIZE_INCORRECT;
            }

            n_items_read = sf_read_short(sf_, (short*)buf,  n_items);
            p->ts_ms  = (samples_rcvd_ * 1000)/sfinfo_.samplerate;
            samples_rcvd_ += (n_items_read/nc_);
            ret_val = n_items_read * sizeof(short);
            return ret_val;
        }

        int GetNumChannels()
        {
            return nc_;
        }

        int GetFs()
        {
            return sfinfo_.samplerate;
        }

        int GetFilledBuffer(unsigned char **buf, int  *size, AudioCaptureInfo *p)
        {
            ac_log_fatal("AudioFileReader::GetFilledBuffer Not implemented");
            abort();
            return ERROR_NOT_IMPLEMENTED;
        }

        int ReturnEmptyBuffer(unsigned char *buf, int  size)
        {
            ac_log_fatal("AudioFileReader::ReturnEmptyBuffer Not implemented");
            abort();
            return ERROR_NOT_IMPLEMENTED;
        }
};

#endif //__AUDIO_FILE_READER_H__

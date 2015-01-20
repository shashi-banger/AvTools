
#ifndef __AUDIO_FRAMER_H__
#define __AUDIO_FRAMER_H__

#include "data_source.h"
#include "audio_capture_info.h"
#include "ring_buf.h"


class AudioFramer {
    DataSource<AudioCaptureInfo>  *aud_source_;
    int                           num_samples_in_frame_;
    int                           nb_in_frame_; /* Num bytes in frame */
    int                           overlap_samples_;
    int                           shift_samples_;
    int                           fs_;
    int                           nc_;
    RingBuf                       *rbuf_;
    int64_t                       last_byte_ts_;
    public:
        inline int64_t ConvBytesToMs(int  bytes)
        {
            return ((int64_t)(bytes * 1000)/(nc_ * sizeof(short) * fs_));
        }

        AudioFramer(DataSource<AudioCaptureInfo> *ds, int ns_in_frame, int overlap, int fs, int num_channels) : aud_source_(ds),
                                        num_samples_in_frame_(ns_in_frame),  overlap_samples_(overlap), fs_(fs), nc_(num_channels)
        {
            int    dur_rbuf = 4000; /* 4 seconds */
            int    sizeof_rbuf;

            sizeof_rbuf = (nc_*sizeof(short)) * ((dur_rbuf * fs_)/1000);
            rbuf_ = new RingBuf(sizeof_rbuf,  2 * num_samples_in_frame_ * (nc_ * sizeof(short)));
            nb_in_frame_ =  (sizeof(short) * nc_ * num_samples_in_frame_);
            shift_samples_ = num_samples_in_frame_ - overlap_samples_;
        }

        int  GetNextFrame(unsigned char **buf,  int64_t  *ts)
        {
            int   avail_bytes;
            AudioCaptureInfo        ac_info;
            int                     loc_ret_val = 0;
            int                     nb_avail;

            if(rbuf_->NumBytesAvailToRead() < nb_in_frame_)
            {
                unsigned char  *buf;
                int            ret_val;
                rbuf_->GetFreeBufferArea(nb_in_frame_, &buf, &avail_bytes);
                ret_val = aud_source_->ReadBuffer(buf, nb_in_frame_, &ac_info);
                if(ret_val != nb_in_frame_)
                {
                    printf("End of aud\n");
                }
                last_byte_ts_ = (ac_info.ts_ms +  ConvBytesToMs(ret_val));
                rbuf_->CommitBuffer(buf, ret_val);
            }


            nb_avail = rbuf_->NumBytesAvailToRead();
            if(nb_avail >= nb_in_frame_)
            {
                rbuf_->ReadBuffer(nb_in_frame_, buf,  &avail_bytes);
                rbuf_->ReadBufferDone(shift_samples_ * nc_ * sizeof(short));
                *ts = last_byte_ts_ - ConvBytesToMs(nb_avail);
            }
            else
            {
                loc_ret_val = -1;
            }

            return loc_ret_val;
        }
};

#endif //__AUDIO_FRAMER_H__

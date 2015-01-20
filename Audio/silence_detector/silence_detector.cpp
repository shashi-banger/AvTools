
#include "silence_detector.h"

int  SilenceDetector::CheckForSilence(short *buf, int num_samples, int64_t   ts)
{
    int  i, j;
    int  max_val[32];
    int  abs_val;
    short *loc_buf;
    float  dur;

    loc_buf = buf;
    for(j = 0; j < nc_; j++)
    {
        max_val[j] = abs((int)loc_buf[j]);
    }
    loc_buf += nc_;
    for(i = nc_; i < num_samples; i+= nc_)
    {
        for(j = 0; j < nc_; j++)
        {
            abs_val = abs((int)loc_buf[j]);
            if(abs_val > max_val[j])
            {
                max_val[j] = abs_val;
            }
        }
        loc_buf += nc_;
    }

    dur = (((float)shift_nf_) * 1000)/fs_;
    for(j = 0; j < nc_; j++)
    {
        if(max_val[j] < silence_threshold_)
        {
            //printf("%d %d %d %f\n", j+1, max_val[j], silence_threshold_, silence_duration_[j]);
            silence_duration_[j] += dur;
        }
        else
        {
            if((int)silence_duration_[j] > max_allowed_sil_dur_)
            {
                int64_t   start_ts;

                start_ts = (ts - (int64_t)silence_duration_[j]);
                start_ts = (start_ts < 0) ? 0 : start_ts;
                printf("channel=%d, start=%lldms, duration=%d \n", j+1, start_ts ,  (int)silence_duration_[j]);
            }
            silence_duration_[j] = 0.;
        }
    }
    return 0;
}

int SilenceDetector::DetectAllSilenceInstances()
{
    unsigned char   *buf;
    int64_t          ts = 0;
    int              ret_val;

    while(1)
    {
        ret_val = af_->GetNextFrame(&buf,  &ts);
        if(ret_val == 0)
        {
            CheckForSilence((short*)buf, (nc_ * nf_per_aud_frame_), ts);
        }
        else
        {
            for(int j = 0; j < nc_; j++)
            {
                if((int)silence_duration_[j] > max_allowed_sil_dur_)
                {
                    int64_t   start_ts;

                    start_ts = (ts - (int64_t)silence_duration_[j]);
                    start_ts = (start_ts < 0) ? 0 : start_ts;
                    printf("channel=%d, start=%lldms, duration=%d \n", j+1,  start_ts,  (int)silence_duration_[j]);
                }
            }
            break;
        }
    }
    return 0;
}

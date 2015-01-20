
#ifndef __SILENCE_DETECTOR_H__
#define __SILENCE_DETECTOR_H__

#include <cstring>
#include <math.h>
#include "audio_capture_info.h"
#include "audio_file_reader.h"
#include "audio_framer.h"

class SilenceDetector {
    AudioFileReader     *a_inp_;
    AudioFramer         *af_;
    float               silence_duration_[32]; /* 0 or 1 */
    int                 silence_threshold_;
    double              silence_db_;
    int                 max_allowed_sil_dur_;
    int                 nc_;
    int                 fs_;
    int                 nf_per_aud_frame_;
    int                 overlap_nf_;
    int                 shift_nf_;

    int  CheckForSilence(short *buf, int num_samples, int64_t   ts);

    public:
    SilenceDetector(char   *filename)
    {
        silence_db_ = -40.;
        silence_threshold_ = (int)((1<<15) * pow(10., (silence_db_/20)));
        a_inp_ = new AudioFileReader(filename);
        nc_ = a_inp_->GetNumChannels();
        nf_per_aud_frame_ = 2048;
        overlap_nf_       = 1024;
        shift_nf_         = nf_per_aud_frame_ - overlap_nf_;
        max_allowed_sil_dur_  = 5000; /* 5 seconds */
        fs_ =  a_inp_->GetFs();
        af_    = new AudioFramer(a_inp_, nf_per_aud_frame_, overlap_nf_, fs_, nc_);
        memset(silence_duration_, 0, sizeof(silence_duration_));
    }

    int   DetectAllSilenceInstances();

};

#endif //__SILENCE_DETECTOR_H__

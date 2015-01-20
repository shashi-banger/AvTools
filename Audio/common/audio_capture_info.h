

#ifndef __AUDIO_CAPTURE_INFO_H__
#define __AUDIO_CAPTURE_INFO_H__

#include <stdint.h>

typedef struct AudioCaptureInfo_ {
    int64_t    ts_ms; /* Timestamp since the beginning */
}AudioCaptureInfo;

#endif //__AUDIO_CAPTURE_INFO_H__

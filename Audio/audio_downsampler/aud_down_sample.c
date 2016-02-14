
#include <stdio.h>
#include <stdlib.h>
#include <soxr-lsr.h>
#include <soxr.h>
#include <sndfile.h>

void convert_stereo_to_mono(float *adf, float *adf_mono, int ns)
{
    int i, k;

    for(i = 0,k=0; i < ns; i+=2, k++)
    {
        adf_mono[k] = (adf[i] + adf[i+1])/2;
    }
    return;
}

main(int argc, char *argv[])
{
    if(argc < 4)
    {
        printf("Usage: ./aud_down_sample inp.wav out.wav dest_sampling_freq\n");
        exit(1);
    }

    double  outfs;
    SNDFILE   *af; /* audio file */
    SNDFILE   *af_out;
    SF_INFO    af_info;
    SF_INFO    out_af_info;

    float      adf[2048]; /* audio data frame */
    float      adf_mono[1024];
    float      adf_ds_out[1024]; /* downsampled output */
    int        count;
    soxr_error_t error;
    SRC_DATA   data;
    float      *process;
    int        olen;
    size_t  odone;
    int        ns_in_frame = 1024;
    int        num_inp_samples;

    outfs = atof(argv[3]);
    af = sf_open(argv[1], SFM_READ, &af_info);
    out_af_info = af_info;
    out_af_info.channels = 1;
    out_af_info.samplerate = outfs;
    af_out = sf_open(argv[2], SFM_WRITE, &out_af_info);

    while(1)
    {
        count = sf_read_float(af, adf, ns_in_frame * af_info.channels);
        if(af_info.channels == 2)
        {
            convert_stereo_to_mono(adf, adf_mono, count);
            process = adf_mono;
            num_inp_samples = count >>1;
        }
        else
        {
            num_inp_samples = count;
            process = adf;
        }
        olen = ((num_inp_samples * outfs / (double)af_info.samplerate) + 0.5);
        error = soxr_oneshot((double)af_info.samplerate, outfs, 1, /* Rates and # of chans. */
                             process, num_inp_samples, NULL,                     /* Input. */
                             adf_ds_out, olen, &odone,                    /* Output. */
                             NULL, NULL, NULL);                    /* Default configuration.*/
        printf("olen = %d odone = %d\n", olen, (int)odone);

        sf_write_float(af_out, adf_ds_out, odone);
        if(count < (ns_in_frame * af_info.channels))
        {
            break;
        }
    }
    sf_close(af);
    sf_close(af_out);
}

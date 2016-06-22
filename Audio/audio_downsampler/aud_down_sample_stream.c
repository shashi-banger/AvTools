
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

#define NUM_SAMPLES  1024
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

    float      adf[NUM_SAMPLES * 2]; /* audio data frame */
    float      adf_mono[NUM_SAMPLES];
    float      adf_ds_out[NUM_SAMPLES]; /* downsampled output */
    int        count;
    soxr_error_t error;
    SRC_DATA   data;
    float      *process;
    int        olen;
    size_t  odone;
    int        ns_in_frame = NUM_SAMPLES;
    int        num_inp_samples;
    soxr_t      soxr;
    int        tot_samples = 0;
    int        inp_samples_read;

    outfs = atof(argv[3]);
    af = sf_open(argv[1], SFM_READ, &af_info);
    out_af_info = af_info;
    out_af_info.channels = 1;
    out_af_info.samplerate = outfs;
    af_out = sf_open(argv[2], SFM_WRITE, &out_af_info);

    soxr = soxr_create((double)af_info.samplerate, outfs, 1, &error,
                       NULL, NULL, NULL);

    olen = ((NUM_SAMPLES * outfs / (outfs + (double)af_info.samplerate)) + 0.5);
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
        inp_samples_read += num_inp_samples;

        if(count == 0)
        {
            process = NULL;
        }

        odone = 0;
        do
        {
            error = soxr_process(soxr, process, num_inp_samples, NULL, adf_ds_out, olen, &odone);

            sf_write_float(af_out, adf_ds_out, odone);
            tot_samples += odone;
            printf("olen = %d odone = %d %p del=%f\n", olen, (int)odone, process, ((inp_samples_read*1000./af_info.samplerate) - ((tot_samples*1000.)/outfs)) );
            printf("delat=%lf\n", soxr_delay(soxr));

            num_inp_samples = 0;
        }while(odone >= olen);

        if(count == 0)
        {
            break;
        }
    }

    printf("inp_samples_read=%d, tot_samples=%d\n", inp_samples_read, tot_samples);
    sf_close(af);
    sf_close(af_out);
}


#ifndef __RING_BUF_H__
#define __RING_BUF_H__

#include "ac_logger.h"
#include <cstdlib>

class RingBuf {
    unsigned char    *buf_;
    /* Max bytes that can be stored in the ring buffer */
    int              max_buf_size_;
    /* Actual allocated size in bytes allocated for buf_ */
    int              allocated_buf_size_;
    /* rd_loc_ is the index offset in buf_ fromwhere new data is to be read */
    int              rd_loc_;
    /* wr_loc_ is the index offset where we want to write audio data */
    int              wr_loc_;
    int              start_lock_pos_;
    int              end_lock_pos_;
    /* actual_data_end_pos_ is relevent only when using
     * GetFreeBufferArea(int num_bytes, unsigned char **buf, int  *avail_bytes)
     * Otheriwse it will always be fixed to end_lock_pos_ */
    int              actual_data_end_pos_;
    /* Max number of bytes that can read or written in one call */
    int              max_rw_num_bytes_;
    int              empty_flag_;
    public:
    RingBuf(int max_buf_size, int  max_rw_num_bytes) : max_buf_size_(max_buf_size), max_rw_num_bytes_(max_rw_num_bytes)
    {
        allocated_buf_size_ = max_buf_size_ + (5 * max_rw_num_bytes_);
        buf_ = new unsigned char[allocated_buf_size_];
        if(buf_ == NULL)
        {
            ac_log_fatal("%s: Allocation failed for ring buffer", __func__);
            abort();
        }
        start_lock_pos_ = 3 * max_rw_num_bytes_;
        end_lock_pos_   = start_lock_pos_ + max_buf_size;
        rd_loc_         = start_lock_pos_;
        wr_loc_         = start_lock_pos_;
        actual_data_end_pos_ = end_lock_pos_;
        empty_flag_ = 1;
    }

    ~RingBuf()
    {
        delete buf_;
    }


    /*
     * Initialize buf to point to an area in the ring buffer such that num_bytes
     * worth data is free and can be written into. This function will be called
     * by the client when it requires a free buffer for a specified number of
     * bytes(num_bytes). A CommitBuffer is expected to be called after clent has
     * written data into the free buffer area
     */
    int  GetFreeBufferArea(int num_bytes, unsigned char **buf,  int *avail_bytes);
    /*
     * Same as previous. But avail_bytes corresponds to the maximum number of
     * linear bytes that can be accomodated currently
     */
    int  GetFreeBufferArea(unsigned char **buf,  int *avail_bytes);
    int  CommitBuffer(unsigned char *buf,  int size);
    int  ReadBuffer(int num_bytes, unsigned char **buf, int  *avail_bytes);
    void ReadBufferDone(int  num_bytes);
    int  WriteBuffer(unsigned char *buf, int num_bytes);
    int  PeekBuffer(int  num_bytes,  unsigned char **buf);
    int  NumBytesAvailToRead() const;
    int  NumBytesAvailToWrite() const;
    void DebugDumpParams() const;
};

#endif //__RING_BUF_H__

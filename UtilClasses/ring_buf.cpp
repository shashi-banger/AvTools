
#include <cstdlib>
#include <cstring>
#include "ring_buf.h"

int RingBuf::GetFreeBufferArea(unsigned char **buf, int  *avail_bytes)
{
    int   ret_val = 0;
    if(wr_loc_ < rd_loc_)
    {
        *buf = buf_ + wr_loc_;
        *avail_bytes = (rd_loc_ - wr_loc_);
    }
    else
    {
        if(wr_loc_ == rd_loc_)
        {
            if(empty_flag_)
            {
                *buf = buf_ + wr_loc_;
                *avail_bytes = (end_lock_pos_ - wr_loc_);
                actual_data_end_pos_ = end_lock_pos_;
            }
            else

            {
                *avail_bytes = 0;
                ret_val = -1;
            }
        }
        else
        {
            *buf = buf_ + wr_loc_;
            *avail_bytes = (end_lock_pos_ - wr_loc_);
            actual_data_end_pos_ = end_lock_pos_;
        }
    }
    return ret_val;
}

int RingBuf::GetFreeBufferArea(int num_bytes, unsigned char **buf, int  *avail_bytes)
{
    int   ret_val = 0;

    if(wr_loc_ < rd_loc_)
    {
        if(((rd_loc_ - wr_loc_) < num_bytes))
        {
            *avail_bytes = 0;
            ret_val = -1;
        }
        else
        {
            *buf = buf_ + wr_loc_;
            *avail_bytes = num_bytes;
        }
    }
    else
    {
        if(wr_loc_ == rd_loc_)
        {
            if(empty_flag_)
            {
                *buf = buf_ + wr_loc_;
                *avail_bytes = num_bytes;
            }
            else
            {
                *avail_bytes = 0;
                ret_val = -1;
            }
        }
        else if((end_lock_pos_ - wr_loc_) >= num_bytes)
        {
            *buf = buf_ + wr_loc_;
            *avail_bytes = num_bytes;
        }
        else
        {
            if(((rd_loc_ - start_lock_pos_) >= num_bytes))
            {
                actual_data_end_pos_ = wr_loc_;
                *buf = buf_ + start_lock_pos_;
                *avail_bytes = num_bytes;
                wr_loc_ = start_lock_pos_;
            }
            else
            {
                *avail_bytes = 0;
                ret_val = -1;
            }
        }
    }
    return ret_val;
}

int RingBuf::CommitBuffer(unsigned char *buf, int  size)
{
    if((buf < (buf_ + start_lock_pos_)) ||  (buf >= (buf_ + end_lock_pos_)) )
    {
        ac_log_fatal("%s->%s: Unexpected buf received %p", __FILE__, __func__, buf);
        return  -1;
    }
    wr_loc_ += size;
    if(wr_loc_ >= end_lock_pos_ && (rd_loc_ > start_lock_pos_) )
    {
        wr_loc_ = start_lock_pos_;
    }
    empty_flag_ = 0;
    /*ac_log_info("END of RingBuf::CommitBuffer: [%d %d] %d %d %d ", end_lock_pos_, start_lock_pos_,
            rd_loc_, wr_loc_, size);*/
    return 0;
}

int RingBuf::PeekBuffer(int  num_bytes,  unsigned char **buf)
{
    int num_bytes_avail;

    num_bytes_avail = NumBytesAvailToRead();
    if(num_bytes > num_bytes_avail)
    {
        *buf = 0;
        return -1;
    }

    if((rd_loc_ + num_bytes) >= actual_data_end_pos_)
    {
        int   num_bytes_read;
        int   rd_new_pos;

        num_bytes_read = (actual_data_end_pos_ - rd_loc_);
        rd_new_pos = (start_lock_pos_ - num_bytes_read);
        memcpy((buf_  + rd_new_pos),
                (buf_ + rd_loc_), num_bytes_read);
        *buf = buf_  + rd_new_pos;
        rd_loc_ = (rd_new_pos);
        actual_data_end_pos_ = end_lock_pos_;
    }
    else
    {
        *buf = buf_ + rd_loc_;
    }
    return 0;
}

int RingBuf::ReadBuffer(int  num_bytes,  unsigned char **buf, int  *avail_bytes)
{
    int num_bytes_avail;

    if(num_bytes > max_rw_num_bytes_)
    {
        ac_log_error("%s-%s. num_bytes=%d, max_rw_num_bytes_=%d\n", __FILE__, __func__, num_bytes,
                            max_rw_num_bytes_);
        return -1;
    }
    num_bytes_avail = NumBytesAvailToRead();
    if(num_bytes > num_bytes_avail)
    {
        *avail_bytes = 0;
        return 0;
        //num_bytes = num_bytes_avail;
    }

    if((rd_loc_ + num_bytes) >= actual_data_end_pos_)
    {
        int   num_bytes_read;
        int   rd_new_pos;

        num_bytes_read = (actual_data_end_pos_ - rd_loc_);
        rd_new_pos = (start_lock_pos_ - num_bytes_read);
        memcpy((buf_  + rd_new_pos),
                (buf_ + rd_loc_), num_bytes_read);
        *buf = buf_  + rd_new_pos;
        *avail_bytes = num_bytes;
        rd_loc_ = (rd_new_pos);
        actual_data_end_pos_ = end_lock_pos_;
    }
    else
    {
        *buf = buf_ + rd_loc_;
        *avail_bytes = num_bytes;
    }

    return 0;
}

void RingBuf::ReadBufferDone(int  num_bytes)
{
    rd_loc_ += num_bytes;
    /* When rd_loc_ goes back to wr_loc it means that the ring buffer has become
     * empty again */
    empty_flag_ = (rd_loc_ == wr_loc_) ? 1 : 0;
    return;
}

int RingBuf::WriteBuffer(unsigned char *buf, int num_bytes)
{
    int              num_bytes_to_write;
    int              num_bytes_avail;

    num_bytes_avail = NumBytesAvailToWrite();
    if(num_bytes_avail < num_bytes)
    {
        ac_log_error("%s-%s. num_bytes=%d, num_bytes_avail=%d\n", __FILE__, __func__, num_bytes,
                            num_bytes_avail);
        return -1;
    }

    if((wr_loc_ + num_bytes) >= end_lock_pos_)
    {
        num_bytes_to_write = (end_lock_pos_ - wr_loc_);
        memcpy((buf_ + wr_loc_), buf, num_bytes_to_write);
        actual_data_end_pos_ = end_lock_pos_;
        wr_loc_ = start_lock_pos_;
        memcpy((buf_ + wr_loc_), (buf + num_bytes_to_write),
                (num_bytes - num_bytes_to_write));
        wr_loc_ += (num_bytes - num_bytes_to_write);
    }
    else
    {
        memcpy((buf_ + wr_loc_), buf, num_bytes);
        wr_loc_ += num_bytes;
    }
    empty_flag_ = 0;
    return 0;
}


int RingBuf::NumBytesAvailToRead() const
{
    int              num_bytes_avail;

    if(wr_loc_ >= rd_loc_)
    {
        if(wr_loc_ == rd_loc_)
        {
            if(empty_flag_)
            {
                num_bytes_avail = 0;
            }
            else
            {
                num_bytes_avail = ((wr_loc_ - start_lock_pos_) +
                            (actual_data_end_pos_ - rd_loc_));
            }
        }
        else
        {
            num_bytes_avail = (wr_loc_ - rd_loc_);
        }
    }
    else
    {
        num_bytes_avail = ((wr_loc_ - start_lock_pos_) +
                            (actual_data_end_pos_ - rd_loc_));
    }

    /*if(num_bytes_avail > (3*48000*4))
    {
        DebugDumpParams();
    }*/

    return num_bytes_avail;
}

int RingBuf::NumBytesAvailToWrite() const
{
    int              num_bytes_avail;

    if(wr_loc_ < rd_loc_)
    {
        /* -1 below will ensure that wr_loc_ and rd_loc_ will never become same
         * */
        num_bytes_avail = (rd_loc_ - wr_loc_);
    }
    else
    {
        if(wr_loc_ == rd_loc_)
        {
            if(empty_flag_)
            {
                num_bytes_avail = (end_lock_pos_ - start_lock_pos_);
            }
            else
            {
                num_bytes_avail = 0;
            }
        }
        else
        {
            num_bytes_avail = ((rd_loc_ - start_lock_pos_) +
                                (end_lock_pos_ - wr_loc_));
        }
    }
    return num_bytes_avail;
}

void RingBuf::DebugDumpParams() const
{
    ac_log_info("RBUF_DBG: buf=%p, rd_loc=%d wr_loc=%d start_lock_pos=%d end_lock_pos=%d, actual_data_end_pos=%d",
            buf_, rd_loc_, wr_loc_, start_lock_pos_,  end_lock_pos_, actual_data_end_pos_);
}


#ifndef __DATA_SOURCE_H__
#define __DATA_SOURCE_H__

template <class T>
class DataSource {
    public:
        virtual int ReadBuffer(unsigned char *buf, int size, T* p) = 0;
        virtual int GetFilledBuffer(unsigned char **buf, int  *size, T *p) = 0;
        virtual int ReturnEmptyBuffer(unsigned char *buf, int size) = 0;
};

#endif //__DATA_SOURCE_H__

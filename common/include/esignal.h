#ifndef ESIGNAL_H
#define ESIGNAL_H
enum ErrorID
{
    Err0 = 10,
    ErrorIDMax,
};
void ESignalInit();
int ESetJmp();
void ERaise(int s);
#endif

#include "pch.h"
#include "hash.h"
euint32 ELFHash( char   * str)
{
    euint32  hash  =   0 ;
    euint32  x     =   0 ;

    while  ( * str)
    {
        hash  =  (hash  <<   4 )  +  ( * str ++ );
        if  ((x  =  hash  &   0xF0000000L )  !=   0 )
        {
            hash  ^=  (x  >>   24 );
            hash  &=   ~ x;
        }
    }

    return  (hash  &   0x7FFFFFFF );
}

euint32 calc_hashnr(const char *key, euint length)
{
	if (!length)
		return 0;
    register euint32 nr=1, nr2=4;
    while (length--)
    {
        nr^= (((nr & 63)+nr2)*((euint32) (euint8) *key++))+ (nr << 8);
        nr2+=3;
    }
    return((euint32) nr);
}

// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com
#ifndef OURO_HTTP_UTILTY_H
#define OURO_HTTP_UTILTY_H

#include "common/common.h"

namespace Ouroboros
{
namespace HttpUtility
{
    inline uint8 toHex(const uint8 &x)
    {
        return x > 9 ? x -10 + 'A': x + '0';
    }

    inline uint8 fromHex(const uint8 &x)
    {
        return isdigit(x) ? x-'0' : x-'A'+10;
    }

    inline std::string URLEncode(const std::string &sIn)
    {
        std::string sOut;

        for( size_t ix = 0; ix < sIn.size(); ix++ )
        {
            uint8 buf[4];
            memset( buf, 0, 4 );

            if( isalnum( (uint8)sIn[ix] ) )
            {
                buf[0] = sIn[ix];
            }
            //else if ( isspace( (uint8)sIn[ix] ) ) //Looks like you can encode spaces or both
            //{
            //    buf[0] = '+';
            //}
            else
            {
                buf[0] = '%';
                buf[1] = toHex( (uint8)sIn[ix] >> 4 );
                buf[2] = toHex( (uint8)sIn[ix] % 16);
            }

            sOut += (char *)buf;
        }

        return sOut;
    };

    inline std::string URLDecode(const std::string &sIn)
    {
        std::string sOut;

        for( size_t ix = 0; ix < sIn.size(); ix++ )
        {
            uint8 ch = 0;
            if(sIn[ix]=='%')
            {
                ch = (fromHex(sIn[ix+1])<<4);
                ch |= fromHex(sIn[ix+2]);
                ix += 2;
            }
            else if(sIn[ix] == '+')
            {
                ch = ' ';
            }
            else
            {
                ch = sIn[ix];
            }

            sOut += (char)ch;
        }

        return sOut;
    }
}

}


#endif // OURO_HTTP_UTILTY_H

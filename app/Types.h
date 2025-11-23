//
// Created by Maks930 on 15/11/2025.
//

#ifndef CHIPE_TYPES_H
#define CHIPE_TYPES_H

using u8    =   unsigned char;
using u16   =   unsigned short;
using u32   =   unsigned int;
using i8    =   char;
using i16   =   short;
using i32   =   int;


struct settings {
    //COLOR AS ABGR (AA BB GG RR)
    u32 foreground_color{0xFF'33'FF'33};
    u32 background_color{0xFF'00'00'00};
    u32 target_ips{750};
};

#endif //CHIPE_TYPES_H
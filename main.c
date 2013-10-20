/*
 *  Copyright 2013 David Farrell
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifdef __linux__
#define _BSD_SOURCE
#include <endian.h>
#endif

#ifdef __APPLE__

#include <libkern/OSByteOrder.h>

#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)

#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)

#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

uint16_t get_op(char *OP_str, uint16_t *special)
{
    /* multi-char constants are big endian so must convert OP to big endian
     * before comparing
     */
    int32_t OP = htobe32(*(int32_t *)OP_str);

    switch(OP)
    {
        // basic opcodes
        case 'SET\0':
            return 0x01;
        case 'ADD\0':
            return 0x02;
        case 'SUB\0':
            return 0x03;
        case 'MUL\0':
            return 0x04;
        case 'MLI\0':
            return 0x05;
        case 'DIV\0':
            return 0x06;
        case 'DVI\0':
            return 0x07;
        case 'MOD\0':
            return 0x08;
        case 'MDI\0':
            return 0x09;
        case 'AND\0':
            return 0x0A;
        case 'BOR\0':
            return 0x0B;
        case 'XOR\0':
            return 0x0C;
        case 'SHR\0':
            return 0x0D;
        case 'ASR\0':
            return 0x0E;
        case 'SHL\0':
            return 0x0F;
        case 'IFB\0':
            return 0x10;
        case 'IFC\0':
            return 0x11;
        case 'IFE\0':
            return 0x12;
        case 'IFN\0':
            return 0x13;
        case 'IFG\0':
            return 0x14;
        case 'IFA\0':
            return 0x15;
        case 'IFL\0':
            return 0x16;
        case 'IFU\0':
            return 0x17;
        case 'ADX\0':
            return 0x1A;
        case 'SBX\0':
            return 0x1B;
        case 'STI\0':
            return 0x1E;
        case 'STD\0':
            return 0x1F;
        // special opcodes
        case 'JSR\0':
            *special = 0x01;
            return 0;
        case 'INT\0':
            *special = 0x08;
            return 0;
        case 'IAG\0':
            *special = 0x09;
            return 0;
        case 'IAS\0':
            *special = 0x0A;
            return 0;
        case 'RFI\0':
            *special = 0x0B;
            return 0;
        case 'IAQ\0':
            *special = 0x0C;
            return 0;
        case 'HWN\0':
            *special = 0x10;
            return 0;
        case 'HWQ\0':
            *special = 0x11;
            return 0;
        case 'HWI\0':
            *special = 0x12;
            return 0;
        default:
            return 0xFF;
    }
}

int main(int argc, char **argv)
{
    char OP_str[5];
    //char B_str[9];

    uint16_t OP;
    uint16_t B;
    uint16_t A;

    for(char line[1024]; !feof(stdin);)
    {
        fgets(line, 1024, stdin);
        // TODO: if last char is not new line, read until end of line

        sscanf(line, " %4s", OP_str);

        for(unsigned char i=0; i<strlen(OP_str); ++i)
        {
            OP_str[i] = toupper(OP_str[i]);
        }

        fprintf(stdout, "OP_str=%s\n", OP_str);

        OP = get_op(OP_str, &B);

        if(OP == 0xFF)
        {
            fprintf(stderr, "unrecognized opcode\n");
            break;
        }

        fprintf(stdout, "OP=0x%.4x\n", OP);
        fprintf(stdout, "B =0x%.4x\n", B);
    }

    return 0;
}

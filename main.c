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

uint16_t get_value(char *s, int *pos, int *num_extra, uint16_t *extra)
{
    char unused[2];

    // left opening bracket
    int newpos = 0;
    int is_left_bracket = sscanf(&s[*pos], " %1[[]%n", unused, &newpos) == 1;
    *pos += newpos;

    int first_i;
    char first_s[5] = {0,0,0,0,0};

    // first integer
    newpos = 0;
    int first_is_integer = sscanf(&s[*pos], " %i%n", &first_i, &newpos) == 1;
    *pos += newpos;

    if(!first_is_integer)
    {
        // if first is not integer, first string
        newpos = 0;
        if(sscanf(&s[*pos], " %4[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]%n", first_s, &newpos) != 1)
        {
            //fprintf(stderr, "missing literal or identifier\n");
            return 0xFF;
        }
        *pos += newpos;

        // to upper
        for(unsigned char i=0; i<strlen(first_s); ++i)
        {
            first_s[i] = toupper(first_s[i]);
        }
    }

    // if +
    newpos = 0;
    int is_plus = sscanf(&s[*pos], " %1[+]%n", unused, &newpos) == 1;
    *pos += newpos;

    int second_i;
    char second_s[5] = {0,0,0,0,0};
    int second_is_integer;

    if(is_plus)
    {
        // second integer
        newpos = 0;
        second_is_integer = sscanf(&s[*pos], " %i%n", &second_i, &newpos) == 1;
        *pos += newpos;

        if(!second_is_integer)
        {
            // if second is not integer, first string
            newpos = 0;
            if(sscanf(&s[*pos], " %4[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]%n", second_s, &newpos) != 1)
            {
                //fprintf(stderr, "missing literal or identifier\n");
                return 0xFF;
            }
            *pos += newpos;

            // to upper
            for(unsigned char i=0; i<strlen(second_s); ++i)
            {
                second_s[i] = toupper(second_s[i]);
            }
        }

    }

    // right closing bracket
    newpos = 0;
    int is_right_bracket = sscanf(&s[*pos], " %1[]]%n", unused, &newpos) == 1;
    *pos += newpos;

    // trailing non-whitespace characters
    if(sscanf(&s[*pos], " %1s", unused) == 1)
    {
        //fprintf(stderr, "trailing non-whitespace characters\n");
        return 0xFF;
    }

    // if brackets don't match
    if(is_left_bracket != is_right_bracket)
    {
        //fprintf(stderr, "brackets not matching\n");
        return 0xFF;
    }

    // value conditions

    if(!is_left_bracket && !is_plus)
    {
        if(first_is_integer)
        {
            // literal value
            if(first_i >= -1 && first_i <= 30)
            {
                return (0x21 + first_i);
            }
            // next word literal value
            else
            {
                extra[(*num_extra)++] = first_i;
                return 0x1F;
            }
        }

        int32_t first = htobe32(*(int32_t *)first_s);

        switch(first)
        {
            case 'A\0\0\0':
                return 0x00;
            case 'B\0\0\0':
                return 0x01;
            case 'C\0\0\0':
                return 0x02;
            case 'X\0\0\0':
                return 0x03;
            case 'Y\0\0\0':
                return 0x04;
            case 'Z\0\0\0':
                return 0x05;
            case 'I\0\0\0':
                return 0x06;
            case 'J\0\0\0':
                return 0x02;
            case 'PUSH':
            case 'POP\0':
                return 0x18;
            case 'PEEK':
                return 0x19;
            case 'SP\0\0':
                return 0x1B;
            case 'PC\0\0':
                return 0x1C;
            case 'EX\0\0':
                return 0x1D;
        }
    }
    else if(is_left_bracket && !is_plus)
    {
        if(first_is_integer)
        {
            extra[(*num_extra)++] = first_i;
            return 0x1E;
        }

        int32_t first = htobe32(*(int32_t *)first_s);

        switch(first)
        {
            case 'A\0\0\0':
                return 0x08;
            case 'B\0\0\0':
                return 0x09;
            case 'C\0\0\0':
                return 0x0A;
            case 'X\0\0\0':
                return 0x0B;
            case 'Y\0\0\0':
                return 0x0C;
            case 'Z\0\0\0':
                return 0x0D;
            case 'I\0\0\0':
                return 0x0E;
            case 'J\0\0\0':
                return 0x0F;
            case 'SP\0\0':
                return 0x19;
        }
    }
    else if(is_left_bracket && is_plus && !first_is_integer && second_is_integer)
    {
        int32_t first = htobe32(*(int32_t *)first_s);

        switch(first)
        {
            case 'A\0\0\0':
                extra[(*num_extra)++] = second_i;
                return 0x10;
            case 'B\0\0\0':
                extra[(*num_extra)++] = second_i;
                return 0x11;
            case 'C\0\0\0':
                extra[(*num_extra)++] = second_i;
                return 0x12;
            case 'X\0\0\0':
                extra[(*num_extra)++] = second_i;
                return 0x13;
            case 'Y\0\0\0':
                extra[(*num_extra)++] = second_i;
                return 0x14;
            case 'Z\0\0\0':
                extra[(*num_extra)++] = second_i;
                return 0x15;
            case 'I\0\0\0':
                extra[(*num_extra)++] = second_i;
                return 0x16;
            case 'J\0\0\0':
                extra[(*num_extra)++] = second_i;
                return 0x17;
            case 'SP\0\0':
                extra[(*num_extra)++] = second_i;
                return 0x1A;
        }
    }

    //fprintf(stderr, "invalid value\n");
    return 0xFF;
}

int main(int argc, char **argv)
{
    char OP_str[5] = {0,0,0,0,0};

    uint16_t OP;
    uint16_t B;
    uint16_t A;

    unsigned int line_num = 1;

    for(char line[1024]; !feof(stdin);)
    {
        int num_extra = 0;
        uint16_t extra[2];

        int pos = 0;

        fgets(line, 1024, stdin);

        // ignore rest of line
        if(line[strlen(line) - 1] != EOF && line[strlen(line) - 1] != '\n'
           && line[strlen(line) - 1] != '\r')
        {
            for(;;)
            {
                char c = fgetc(stdin);
                if(c == EOF || c == '\n' || c == '\r') break;
            }
        }

        // opcode
        int newpos = 0;
        if(sscanf(line, " %4[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]%n", OP_str, &newpos) != 1)
        {
            fprintf(stderr, "%u: syntax error at OP\n", line_num);
            break;
        }
        pos += newpos;

        for(unsigned char i=0; i<strlen(OP_str); ++i)
        {
            OP_str[i] = toupper(OP_str[i]);
        }

        OP = get_op(OP_str, &B);
        //fprintf(stdout, "OP=0x%.4x\n", OP);

        // invalid opcode
        if(OP == 0xFF)
        {
            fprintf(stderr, "%u: invalid value for OP\n", line_num);
            break;
        }
        // special opcode
        else if(OP == 0)
        {
            //fprintf(stdout, "B=0x%.4x\n", B);

            A = get_value(line, &pos, &num_extra, extra);

            // syntax error
            if(A == 0xFF)
            {
                fprintf(stderr, "%u: syntax error at A\n", line_num);
                break;
            }

            //fprintf(stdout, "A=0x%.4x\n", A);
        }
        // normal opcode
        else
        {
            char *ptr = strchr(line, ',');

            if(ptr == NULL)
            {
                fprintf(stderr, "%u: missing operand(s)\n", line_num);
                break;
            }

            // split into two strings at comma
            *ptr = '\0';

            B = get_value(line, &pos, &num_extra, extra);

            if(B == 0xFF)
            {
                fprintf(stderr, "%u: syntax error at B\n", line_num);
                break;
            }

            //fprintf(stdout, "B=0x%.4x\n", B);

            int p = 0;
            A = get_value(&ptr[1], &p, &num_extra, extra);

            if(A == 0xFF)
            {
                fprintf(stderr, "%u: syntax error at A\n", line_num);
                break;
            }

            //fprintf(stdout, "A=0x%.4x\n", A);
        }

        uint16_t instruction = OP | (B << 5) | (A << 10);

        // output

        freopen(NULL, "wb", stdout);

        uint16_t be_inst = htobe16(instruction);
        fwrite(&be_inst, sizeof(uint16_t), 1, stdout);

        for(int i=num_extra-1; i>=0; --i)
        {
            uint16_t be = htobe16(extra[i]);
            fwrite(&be, sizeof(uint16_t), 1, stdout);
        }
    }

    return 0;
}

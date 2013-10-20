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

#define _BSD_SOURCE
#include <endian.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv)
{
    char OP_str[5];
    //char B_str[9];

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
    }

    return 0;
}

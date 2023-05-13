#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#define MAX_LINE_LEN 20000

static int git = 0;

/*
 * ansi color esc codes are: 033[XXXm
 * where XXX is any series of semicolon-separated parameters
 *
 * can be used for example:
 * git --no-pager diff --color=always $@ |& t2s -g |& smartless
 */

int main(int argc, char *argv[])
{
    int help = 0;

    if (argc > 1)
    {
        if (strcmp(argv[1], "--git") == 0)
            git = 1;
        else if (strncmp(argv[1], "-g", 2) == 0)
            git = 1;
        else if (strcmp(argv[1], "--help") == 0)
            help = 1;
        else if (strncmp(argv[1], "-h", 2) == 0)
            help = 1;
    }

    if (help)
    {
        fprintf(stdout, "%s [-g|--git]\n", argv[0]);
        fprintf(stdout, "reads from stdin and writes to stdout\n");
        fprintf(stdout, "-g|--git arg accounts for git diff leading -/+ char in tab offsets ...\n");
        return 0;
    }

    char line[MAX_LINE_LEN+1];
    char line2[MAX_LINE_LEN+1];

    while (fgets(line, MAX_LINE_LEN, stdin) != NULL)
    {
        int j = 0;
        int o = 0;
        int in_ansi = 0;
        int gitdigit = 0;
        int rawindex = 0;

        line[MAX_LINE_LEN] = '\0';

        int len = (int)strlen(line);

        if (len >= MAX_LINE_LEN)
        {
            fprintf(stderr, "%s : Error - line too long (binary file?)\n", argv[0]);
            return 1;
        }

        // fprintf(stderr, "%s", line);

        for (int i=0; i<len; i++)
        {
            // fprintf(stderr, "%d %d %c %d %d\n", o, rawindex, (line[i] == '\033' ? '@' : line[i]), i,j);

            if ( (git) && (gitdigit == 0) && (o == 0) && ((line[i] == '+') || (line[i] == '-')) )
            {
                gitdigit = 1;
            }
            else if (line[i] == '\033')
            {
                if ( (!in_ansi) && (len > 1) && (i < len-1) && (line[i+1] == '[') )
                {
                    in_ansi = 1;
                    // fprintf(stderr, "in_ansi on\n");
                }
            }
            else if ( (in_ansi) && (line[i] == 'm') )
            {
                in_ansi = 0;
                // fprintf(stderr, "in_ansi off\n");
                rawindex--;
            }
            else if (!in_ansi)
            {
                o++;
            }

            if (line[i] == '\t')
            {
                // fprintf(stderr, "gitdigit=%d rawindex=%d o=%d i=%d j=%d line[%d] = \\t\n", gitdigit, rawindex, o, i, j, i);

                int z = 0;
                if (gitdigit)
                    z = 4 - ((rawindex-1) % 4);
                else
                    z = 4 - (rawindex % 4);

                // fprintf(stderr,"adding %d spaces ...\n", z);
                // fprintf(stderr, "%c %d %d %d\n", (line[i] == '\033' ? '@' : line[i]), i,j,z);

                for (int k=0; k<z; k++)
                    line2[j++] = ' ';
                rawindex += z;
            }
            else
            {
                line2[j++] = line[i];
                if (!in_ansi)
                    rawindex++;
            }

        }
        line2[j] = '\0';
        fprintf(stdout, "%s", line2);
    }

    return 0;
}

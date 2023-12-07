#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/*
 * Build:
 * $ gcc -o block_banner block_banner.c
 *
 * Option:
 * -t NAME     : table name
 * -m MESSAGE  : message to output
 * -i          : drop table if exists
 *
 * Run and Insert Queries:
 * $ ./block_banner -i -t test -m MerryChristmas | psql
 *
 * Output Table Blocks:
 * $ xxd -u -c 32 \
 *    $(psql -d postgres -Atq -X -c "select current_setting('data_directory') || '/' || pg_relation_filepath('test')")
 *
 * Output Table Blocks with Delays:
 * $ xxd -u -c 32 \
 *    $(psql -d postgres -Atq -X -c "select current_setting('data_directory') || '/' || pg_relation_filepath('test')")  |\
 *    awk '{print $0; system("sleep 0.05")}'
 */

const static char *letters[26][8] = {
	{
		"........#.......",
		".......#.#......",
		"......#...#.....",
		".....#.....#....",
		"....#########...",
		"...#.........#..",
		"..#...........#.",
		".#.............#"
	},
	{
		"..###########...",
		"..#..........#..",
		"..#..........#..",
		"..###########...",
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"..###########...",
	},
	{
		"..############..",
		"..#..........#..",
		"..#.............",
		"..#.............",
		"..#.............",
		"..#.............",
		"..#..........#..",
		"..############.."
	},
	{
		".############...",
		".#...........#..",
		".#............#.",
		".#............#..",
		".#............#..",
		".#............#..",
		".#...........#..",
		".############..."
	},
	{
		"..############..",
		"..#.............",
		"..#.............",
		"..############..",
		"..#.............",
		"..#.............",
		"..#.............",
		"..############.."
	},
	{
		"..############..",
		"..#.............",
		"..#.............",
		"..############..",
		"..#.............",
		"..#.............",
		"..#.............",
		"..#............."
	},
	{
		"..############..",
		"..#..........#..",
		"..#.............",
		"..#.............",
		"..#......#####..",
		"..#..........#..",
		"..#..........#..",
		"..############.."
	},
	{
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"..############..",
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"..#..........#.."
	},
	{
		".......###......",
		"........#.......",
		"........#.......",
		"........#.......",
		"........#.......",
		"........#.......",
		"........#.......",
		".......###......"
	},
	{
		".......###......",
		"........#.......",
		"........#........",
		"........#.......",
		"........#.......",
		"..#.....#.......",
		"..#.....#.......",
		"..#######......."
	},
	{
		"..#.......#.....",
		"..#.....#.......",
		"..#...#.........",
		"..####..........",
		"..#...#.........",
		"..#....#........",
		"..#.....#.......",
		"..#.......#....."
	},
	{
		"..#.............",
		"..#.............",
		"..#.............",
		"..#.............",
		"..#.............",
		"..#.............",
		"..#.............",
		"..############.."
	},
	{
		"..#..........#..",
		"..#.#......#.#..",
		"..#..#....#..#..",
		"..#...#..#...#..",
		"..#....##....#..",
		"..#..........#..",
		"..#..........#..",
		"..#..........#.."
	},
	{
		"..#.........#...",
		"..#.#.......#...",
		"..#..#......#...",
		"..#...#.....#...",
		"..#....#....#...",
		"..#.....#...#...",
		"..#......#..#...",
		"..#.......###..."
	},
	{
		"...##########...",
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"...##########..."
	},
	{
		"..###########...",
		"..#..........#..",
		"..#..........#..",
		"..###########...",
		"..#.............",
		"..#.............",
		"..#.............",
		"..#............."
	},
	{
		"...##########...",
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"..#......#...#..",
		"..#.......#..#..",
		"..#........#.#..",
		"...##########.#."
	},
	{
		"..###########...",
		"..#..........#..",
		"..#..........#..",
		"..###########...",
		"..#.......#.....",
		"..#........#....",
		"..#.........#...",
		"..#..........#.."
	},
	{
		"...###########..",
		"..#.............",
		"..#.............",
		"...##########..",
		".............#..",
		".............#..",
		".............#..",
		"...##########..."
	},
	{
		"..############..",
		"........#.......",
		"........#.......",
		"........#.......",
		"........#.......",
		"........#.......",
		"........#.......",
		".......###......"
	},
	{
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"..#..........#..",
		"...##########..."
	},
	{
		"..#.........#...",
		"..#.........#...",
		"..#.........#...",
		"...#.......#....",
		"....#.....#.....",
		".....#...#......",
		"......#.#.......",
		".......#........"
	},
	{
		"..#..........#..",
		"..#....#.....#..",
		"..#....#.....#..",
		"..#....#.....#..",
		"..#....#.....#..",
		"..#....#.....#..",
		"..#....#.....#..",
		"...##########..."
	},
	{
		"...#........#...",
		"....#......#....",
		".....#....#.....",
		"......#..#......",
		".......##.......",
		"......#..#......",
		".....#....#.....",
		"....#......#...."
	},
	{
		"....#........#..",
		".....#......#...",
		"......#....#....",
		".......#..#.....",
		"........#.......",
		"........#.......",
		"........#.......",
		"........#......."
	},
	{
		"..#############..",
		".............#..",
		"...........#....",
		".........#......",
		".......#........",
		".....#..........",
		"...#............",
		"..###########..."
	}
};

static char *table_name = NULL;
static char *msg = NULL;

#define CHAR_UNMARK "chr(17)"
#define CHAR_MARK	"chr(35)"
#define BLANK_LINE "repeat(" CHAR_UNMARK ", 32)"

void
gen_insert_query(char chr)
{
	int idx = chr - 'A';
	const char **letter = letters[idx];
	char *sep = " ";

	printf("insert into %s (c1, c2, c3, c4, c5, c6, c7, c8) values (0, 0, 0, 0, 0, 0, 0, %s ||",
		   table_name, BLANK_LINE);

	if (chr == ' ')
	{
		/*
		 * On the terminal, we have 32 bytes in each line. A character will be depict
		 * using 8 lines, so we use 256 characters to fill the field.
		 */
		printf("repeat(" CHAR_UNMARK ", 256)");
		goto done;
	}

	for (int i = 0; i < 8; i++)
	{
		int n_consecutives = 1;
		const char *line = letter[i];
		char prev = line[0];

		for (int j = 1; j < 16; j++)
		{
			char cur = line[j];

			if (prev == cur)
			{
				n_consecutives++;
				continue;
			}

			printf("%s repeat(%s, %d)",
				   sep, prev == '.' ? CHAR_UNMARK : CHAR_MARK, n_consecutives * 2);

			prev = cur;
			n_consecutives = 1;
			sep = "|| ";
		}

		printf("%s repeat(%s, %d)",
			   sep, prev == '.' ? CHAR_UNMARK : CHAR_MARK, n_consecutives * 2);
	}

done:
	printf("|| %s);\n", BLANK_LINE);
}

#define Min(i, j) (((i) < (j)) ? (i) : (j))

int
main(int argc, char **argv)
{
	int opt;
	int msg_len, written;
	bool init = false;

	while ((opt = getopt(argc, argv, "it:m:")) != -1)
	{
		switch (opt)
		{
			case 't':
				table_name = strdup(optarg);
				break;
			case 'm':
				msg = strdup(optarg);
				break;
			case 'i':
				init = true;
				break;
			default:
				fprintf(stderr, "invalid option %c\n", opt);
				exit(1);
		}
	}

	if (table_name == NULL)
	{
		fprintf(stderr, "table name (-t) must be specified\n");
		exit(1);
	}
	if (msg == NULL)
	{
		fprintf(stderr, "msg (-m) must be specified\n");
		exit(1);
	}

	if (init)
	{
		printf("drop table if exists %s;\n", table_name);
		printf("create table %s ( "
			   "c1 int4, c2 int4, c3 int4, c4 int4, c5 int4, c6 int4, c7 int4, c8 text, c9 text, c10 text, "
			   "c11 text, c12 text, c13 text, c14 text, c15 text, c16 text, c17 text, c18 text, c19 text, c20 text, "
			   "c21 text, c22 text, c23 text, c24 text, c25 text, c26 text, c27 text, c28 text, c29 text, c30 text, "
			   "c31 text, c32 text, c33 text, c34 text, c35 text, c36 text, c37 text, c38 text, c39 text, c40 text, "
			   "c41 text, c42 text, c43 text, c44 text, c45 text, c46 text, c47 text, c48 text, c49 text, c50 text, "
			   "c51 text, c52 text, c53 text, c54 text, c55 text, c56 text, c57 text, c58 text, c59 text, c60 text, "
			   "c61 text, c62 text, c63 text, c64 text);\n", table_name);
	}

	/* generate INSERT queries */
	msg_len = strlen(msg);
	written = 0;
	while (written < msg_len)
	{
		int len;

		/*
		 * With 8kB blocks, "xxd -c 32" shows 256 lines and each letter use 12 lines
		 * (8 lines for a character, 2 lines for blanks, 2 lines for header and varlena
		 * header). Therefore, at most 21 letters can fit in one 8kB block.
		 */
		len = Min(msg_len - written, 21);

		if (len < 21)
		{
			for (int i = 0; i < 21 - len; i++)
				gen_insert_query(' ');
		}

		for (int i = len; i > 0; i--)
		{
			gen_insert_query(toupper(msg[i + written - 1]));
		}

		written += len;
	}

	printf("checkpoint;\n");

	return 0;
}

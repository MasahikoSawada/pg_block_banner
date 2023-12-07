# pg_block_banner
A toy program showing message in PostgreSQL table blocks.

![](https://github.com/MasahikoSawada/pg_block_banner/blob/main/demo.gif)

# Build

```bash
$ gcc -o block_banner block_banner.c
```

# Run with `psql`

`block_banner` generates and writes INSERT queries to stdout, which can be redirected to `psql` to run.

* `-t` option is the table name.
* `-m` option is the message to show.
* `-i` option writes `DROP TABLE` and `CREATE TABLE` to initialize the table.

```bash
$ ./block_banner -t test -m hello -i | psql
```

# Where is message?

```bash
$ xxd -u -c 32 $(psql -d postgres -Atq -X -c "select current_setting('data_directory') || '/' || pg_relation_filepath('test')")
```

If you want to show it with delays, you can execute instead:

```
$ xxd -u -c 32 $(psql -d postgres -Atq -X -c "select current_setting('data_directory') || '/' || pg_relation_filepath('test')") | awk '{print $0; system("sleep 0.05")}'
```

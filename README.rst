===========================================================
fd-catalog - the file descriptor catalog
===========================================================

fd-catalog is a command pausing after opening various kinds of
files. fd-catalog is designed for testing `lsof
<https://github.com/lsof-org/lsof>` running on Linux, `lsfd
<https://github.com/karelzak/util-linux/pull/1418>`, and
-y option of `strace <https://strace.io/>`.

::

    $ ./fd-catalog &
    [1] 1677961
                     ro-regular-file...3
                        ro-directory...4
                                pipe...5, 6
                 ro-char-dev-devzero...7
              socketpair-unix-stream...8, 9
               socketpair-unix-dgram...10, 11
           socketpair-unix-seqpacket...12, 13

    $ util-linux/lsfd | grep 1677961
    ...
    fd-catalog       1677961             yamato      3  r--  REG               dm-0    88     271210 /etc/passwd
    fd-catalog       1677961             yamato      4  r--  DIR               dm-0    88          2 /
    fd-catalog       1677961             yamato      5  r-- FIFO       nodev:pipefs    14   64655119 pipe:[64655119]
    fd-catalog       1677961             yamato      6  -w- FIFO       nodev:pipefs    14   64655119 pipe:[64655119]
    fd-catalog       1677961             yamato      7  r--  CHR              mem:3    25          4 /dev/null
    fd-catalog       1677961             yamato      8  rw- SOCK       nodev:sockfs     9   64655120 UNIX:[64655120]
    fd-catalog       1677961             yamato      9  rw- SOCK       nodev:sockfs     9   64655121 UNIX:[64655121]
    fd-catalog       1677961             yamato     10  rw- SOCK       nodev:sockfs     9   64655122 UNIX:[64655122]
    fd-catalog       1677961             yamato     11  rw- SOCK       nodev:sockfs     9   64655123 UNIX:[64655123]
    fd-catalog       1677961             yamato     12  rw- SOCK       nodev:sockfs     9   64655124 UNIX:[64655124]
    fd-catalog       1677961             yamato     13  rw- SOCK       nodev:sockfs     9   64655125 UNIX:[64655125]
    ...

    $ lsof -p 1677961
    COMMAND       PID   USER   FD   TYPE             DEVICE SIZE/OFF      NODE NAME
    ...
    fd-catalo 1677961 yamato    3r   REG              253,0     3529    271210 /etc/passwd
    fd-catalo 1677961 yamato    4r   DIR              253,0     4096         2 /
    fd-catalo 1677961 yamato    5r  FIFO               0,13      0t0  64655119 pipe
    fd-catalo 1677961 yamato    6w  FIFO               0,13      0t0  64655119 pipe
    fd-catalo 1677961 yamato    7r   CHR                1,3      0t0         4 /dev/null
    fd-catalo 1677961 yamato    8u  unix 0x00000000e61b5856      0t0  64655120 type=STREAM (CONNECTED)
    fd-catalo 1677961 yamato    9u  unix 0x0000000011b6b68e      0t0  64655121 type=STREAM (CONNECTED)
    fd-catalo 1677961 yamato   10u  unix 0x000000002087170c      0t0  64655122 type=DGRAM (UNCONNECTED)
    fd-catalo 1677961 yamato   11u  unix 0x0000000052735c7f      0t0  64655123 type=DGRAM (UNCONNECTED)
    fd-catalo 1677961 yamato   12u  unix 0x000000009a64540a      0t0  64655124 type=SEQPACKET (CONNECTED)
    fd-catalo 1677961 yamato   13u  unix 0x0000000038a269d9      0t0  64655125 type=SEQPACKET (CONNECTED)
    ...

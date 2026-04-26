# Retro BASIC V2.0

A classic BASIC interpreter written in portable C99, running natively on **macOS, Linux, and Raspberry Pi** — any machine with a standard C compiler. No dependencies, no runtimes, no special toolchain.

Originally inspired by bare-metal 6502 BASIC, it has been fully ported to portable C99 and communicates through your terminal's standard input and output.

```
RETRO BASIC V2.0
FREE BYTES: 4096

READY
10 PRINT "HELLO, WORLD!"
20 END
RUN

HELLO, WORLD!
READY
```

---

## Features

- **26 integer variables** (A through Z) and **26 string variables** (A$ through Z$)
- **Arrays** — one- or two-dimensional, up to 8 arrays via `DIM`
- **4096 bytes of program memory**, up to 256 numbered lines (1–65535)
- **Four loop structures** — `FOR/NEXT`, `WHILE/WEND`, `REPEAT/UNTIL`, `DO/LOOP`
- **Block conditionals** — `IF / THEN / ELSE / ENDIF` with full nesting
- **Subroutines** — `GOSUB` / `RETURN` with 16-level stack
- **String functions** — `LEN`, `LEFT$`, `RIGHT$`, `MID$`, string concatenation with `+`
- **Math functions** — `ABS`, `SGN`, `SQR`, `RND`, `MIN`, `MAX`
- **Screen and graphics** — `CLS`, `LOCATE`, `COLOR`, `DRAW`, `LINE`, `RECT`, `FILL`, `PAUSE`, `CURSOR`
- **Save and load programs** to plain-text `.bas` files
- **Run `.bas` files directly** from the command line
- **Direct mode** — type any statement at the `READY` prompt for immediate execution
- **Pipe-friendly** — reads from stdin, writes to stdout

---

## Building

Requires only `cc`, `gcc`, or `clang`. No other dependencies.

```sh
cd Retro-BASIC
make
```

The binary is created at `build/basic`. To use a specific compiler:

```sh
make CC=gcc
make CC=clang
```

To clean and rebuild from scratch:

```sh
make clean && make
```

---

## Running

### Interactive mode

```sh
./build/basic
```

You are greeted with the `READY` prompt. Type BASIC statements, or type a line number followed by a statement to build a program.

### Run a file directly

```sh
./build/basic myprogram.bas
```

The interpreter loads and runs the file, then drops into the `READY` prompt. If the file cannot be found, an error is printed.

---

## The BASIC Language

### Variables

| Type | Names | Size |
|---|---|---|
| Integer | `A` – `Z` | one per letter |
| String | `A$` – `Z$` | up to 63 characters |
| Array | `DIM name(n)` or `DIM name(n,m)` | up to 8 arrays |

### Operators

| Category | Operators |
|---|---|
| Arithmetic | `+`  `-`  `*`  `/`  `MOD`  `^` |
| Comparison | `=`  `<>`  `<`  `>`  `<=`  `>=` |
| Boolean | `AND`  `OR`  `NOT` |

Operator precedence follows standard mathematical rules: `^` binds tightest, then unary `-`, then `*` `/` `MOD`, then `+` `-`, then comparisons, then `NOT`, then `AND`, then `OR`.

### System commands

| Command | Description |
|---|---|
| `LIST` | List the current program |
| `LIST 10-50` | List lines 10 through 50 |
| `RUN` | Run the current program |
| `NEW` | Clear the program and all variables |
| `SAVE filename` | Save the program to a `.bas` file |
| `LOAD filename` | Load a `.bas` file, replacing the current program |

`LOAD` and `SAVE` accept paths with spaces — no quoting or escaping required:

```basic
LOAD /Users/jacob/My Programs/hello.bas
SAVE /Users/jacob/My Programs/hello.bas
```

### Statements

```basic
PRINT expr [; expr] [, expr]     ' print to screen
INPUT ["prompt";] var             ' read from keyboard
LET var = expr                    ' assignment (LET is optional)
DIM name(n [, m])                 ' declare array
GOTO line                         ' unconditional branch
GOSUB line / RETURN               ' subroutine call
IF cond THEN stmt                 ' single-line conditional
IF cond THEN                      ' block conditional
  ...
ELSE
  ...
ENDIF
FOR var = start TO end [STEP n]   ' counted loop
NEXT var
WHILE cond                        ' condition-first loop
WEND
REPEAT                            ' condition-last loop
UNTIL cond
DO                                ' flexible loop
  ...
LOOP [WHILE cond | UNTIL cond]
POKE addr, val                    ' write to simulated memory
REM text                          ' comment
END                               ' end program
STOP                              ' break with line number
```

### Built-in functions

```basic
ABS(n)          ' absolute value
SGN(n)          ' sign: -1, 0, or 1
SQR(n)          ' square root
RND(n)          ' random integer 0 to n-1
MIN(a, b)       ' smaller of a and b
MAX(a, b)       ' larger of a and b
PEEK(addr)      ' read from simulated 64KB memory

LEN(s$)         ' string length
LEFT$(s$, n)    ' first n characters
RIGHT$(s$, n)   ' last n characters
MID$(s$, pos, n)' substring from pos, length n
```

---

## Screen Control and Graphics

All graphics commands use ANSI escape sequences and work with any modern terminal (macOS Terminal, iTerm2, GNOME Terminal, PuTTY, Windows Terminal, and others). Coordinates are `column, row` with `1, 1` at the top-left.

| Command | Description |
|---|---|
| `CLS` | Clear the screen |
| `LOCATE row, col` | Move the cursor |
| `COLOR fg [, bg]` | Set text color (0–7 normal, 8–15 bright, -1 reset) |
| `DRAW x, y, expr` | Draw a string or character at column x, row y |
| `LINE x1, y1, x2, y2 [, char$]` | Draw a line (Bresenham, default char `*`) |
| `RECT x1, y1, x2, y2 [, char$]` | Draw a rectangle outline |
| `FILL x1, y1, x2, y2 [, char$]` | Fill a rectangular area |
| `PAUSE ms` | Wait N milliseconds |
| `CURSOR n` | Show (`1`) or hide (`0`) the terminal cursor |

**Color values:**

| 0 Black | 1 Red | 2 Green | 3 Yellow |
|---|---|---|---|
| 4 Blue | 5 Magenta | 6 Cyan | 7 White |
| 8–15 | Bright versions | -1 | Reset to default |

### Animation example

```basic
10 CLS
20 CURSOR 0
30 X = 1
40 COLOR 2
50 DRAW X, 12, "O"
60 PAUSE 80
70 COLOR 0
80 DRAW X, 12, " "
90 X = X + 1
100 IF X > 78 THEN X = 1
110 GOTO 40
```

Press **Ctrl-C** to stop.

---

## Sample Programs

### Hello World
```basic
10 PRINT "HELLO, WORLD!"
20 END
```

### Guess the Number
```basic
10 REM GUESS THE NUMBER
20 S = RND(100) + 1
30 PRINT "I AM THINKING OF A NUMBER BETWEEN 1 AND 100."
40 T = 0
50 REPEAT
60   T = T + 1
70   INPUT "YOUR GUESS" G
80   IF G < S THEN PRINT "TOO LOW"
90   IF G > S THEN PRINT "TOO HIGH"
100 UNTIL G = S
110 PRINT "CORRECT! YOU GOT IT IN ";T;" TRIES!"
120 END
```

### Fibonacci Sequence
```basic
10 REM FIBONACCI NUMBERS
20 A = 0
30 B = 1
40 PRINT A
50 WHILE B < 10000
60   PRINT B
70   C = A + B
80   A = B
90   B = C
100 WEND
110 END
```

### Bubble Sort
```basic
10 REM BUBBLE SORT
20 DIM A(10)
30 FOR I = 0 TO 9
40   A(I) = RND(100)
50 NEXT I
60 PRINT "BEFORE: "
70 GOSUB 200
80 FOR I = 0 TO 8
90   FOR J = 0 TO 8
100    IF A(J) > A(J+1) THEN GOSUB 300
110  NEXT J
120 NEXT I
130 PRINT "AFTER:  "
140 GOSUB 200
150 END
200 FOR K = 0 TO 9
210   PRINT A(K);" ";
220 NEXT K
230 PRINT
240 RETURN
300 T = A(J)
310 A(J) = A(J+1)
320 A(J+1) = T
330 RETURN
```

---

## Saving and Loading Programs

Programs are saved as plain-text `.bas` files in the same format shown by `LIST`. You can open and edit them in any text editor.

```
READY
10 PRINT "MY PROGRAM"
20 END
SAVE myprog.bas
OK
LOAD myprog.bas
OK
LIST
10 PRINT "MY PROGRAM"
20 END
```

---

## Technical Notes

- Program memory: 4096 bytes, up to 256 lines
- String variables: 63 characters maximum
- GOSUB call stack: 16 levels deep
- FOR, WHILE, REPEAT, DO stacks: 8 levels each
- IF nesting: 8 levels
- Simulated memory: 64KB array (PEEK/POKE do not access real hardware addresses)
- Timing: `PAUSE` uses standard C `clock()` — no OS-specific calls
- Graphics: ANSI escape sequences only — no curses or platform libraries
- Build standard: C99 (`-std=c99`), zero warnings with `-Wall -Wextra`

---

## Project Structure

```
Retro-BASIC/
├── main.c        Entry point, banner, command-line file loading
├── io.c/h        Terminal I/O (putchar, fgets, number formatting)
├── vars.c/h      Variables, arrays, strings, simulated memory
├── expr.c/h      Expression parser with full operator precedence
├── program.c/h   Program storage, line numbering, LIST, SAVE, LOAD
├── commands.c/h  All BASIC statement execution and graphics
├── repl.c/h      Interactive REPL and direct mode
├── config.h      Shared constants (memory sizes, stack depths)
└── Makefile      Standard C build (gcc or clang, no dependencies)
```

---

## Full Documentation

See [`manual.md`](manual.md) for the complete 32-chapter Programmer's Manual including:

- Full language reference for every statement and function
- Detailed error message guide
- Screen control and graphics reference with examples
- Building from source on macOS, Linux, and Raspberry Pi
- Quick-reference card (Appendix A)
- Sample programs (Appendix B)

---

## License
MIT liecense see LICENSE file for details

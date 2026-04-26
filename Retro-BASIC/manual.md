# R E T R O &ensp; B A S I C &ensp; V 2 . 0

## Programmer's Manual

**For macOS, Linux, and Raspberry Pi**

(C) 2026 Retro Computing

---

## Table of Contents

1. [Introduction](#chapter-1-introduction)
2. [Getting Started](#chapter-2-getting-started)
3. [Entering and Editing Programs](#chapter-3-entering-and-editing-programs)
4. [System Commands](#chapter-4-system-commands)
5. [Variables and Data Types](#chapter-5-variables-and-data-types)
6. [Numeric Variables](#chapter-6-numeric-variables)
7. [String Variables](#chapter-7-string-variables)
8. [Arrays](#chapter-8-arrays)
9. [Arithmetic Operators](#chapter-9-arithmetic-operators)
10. [Comparison Operators](#chapter-10-comparison-operators)
11. [Boolean Logic Operators](#chapter-11-boolean-logic-operators)
12. [Operator Precedence](#chapter-12-operator-precedence)
13. [The PRINT Statement](#chapter-13-the-print-statement)
14. [The INPUT Statement](#chapter-14-the-input-statement)
15. [The LET Statement](#chapter-15-the-let-statement)
16. [The GOTO Statement](#chapter-16-the-goto-statement)
17. [The IF...THEN...ELSE...ENDIF Statement](#chapter-17-the-ifthenelseendif-statement)
18. [The FOR...NEXT Loop](#chapter-18-the-fornext-loop)
19. [The WHILE...WEND Loop](#chapter-19-the-whilewend-loop)
20. [The REPEAT...UNTIL Loop](#chapter-20-the-repeatuntil-loop)
21. [The DO...LOOP Structure](#chapter-21-the-doloop-structure)
22. [GOSUB and RETURN](#chapter-22-gosub-and-return)
23. [Built-In Numeric Functions](#chapter-23-built-in-numeric-functions)
24. [Built-In String Functions](#chapter-24-built-in-string-functions)
25. [Direct Memory Access](#chapter-25-direct-memory-access)
26. [Screen Control and Graphics](#chapter-26-screen-control-and-graphics)
27. [The REM Statement](#chapter-27-the-rem-statement)
28. [Program Control Statements](#chapter-28-program-control-statements)
29. [Direct Mode](#chapter-29-direct-mode)
30. [Error Messages](#chapter-30-error-messages)
31. [Technical Notes](#chapter-31-technical-notes)
32. [Building from Source](#chapter-32-building-from-source)
- [Appendix A: Quick Reference Card](#appendix-a-quick-reference-card)
- [Appendix B: Sample Programs](#appendix-b-sample-programs)

---

## Chapter 1: Introduction

> *Welcome to RETRO BASIC V2.0 -- a classic BASIC interpreter for modern machines!*

Retro BASIC is a full-featured BASIC language interpreter that runs natively on **macOS, Linux, and Raspberry Pi** -- any system with a standard C compiler. Originally inspired by bare-metal 6502 BASIC, it has been ported to portable C99 and communicates through your terminal's standard input and output. No special hardware is required.

Whether you are a seasoned programmer or just beginning your journey into the world of computing, Retro BASIC provides a friendly and capable environment for writing programs, performing calculations, and exploring what classic BASIC can do.

Retro BASIC provides:

- 26 integer variables (A through Z)
- 26 string variables (A$ through Z$)
- Up to 8 arrays, one- or two-dimensional
- 4096 bytes of program memory (up to 256 lines)
- Full arithmetic with proper operator precedence
- Multiple loop structures (FOR, WHILE, REPEAT, DO)
- Subroutines with GOSUB/RETURN
- Block IF/ELSE/ENDIF conditional logic
- String manipulation functions
- Direct memory access with PEEK and POKE
- Screen control with CLS and LOCATE

---

## Chapter 2: Getting Started

When you launch Retro BASIC, it greets you with its startup banner:

```
RETRO BASIC V2.0
FREE BYTES: 4096

READY
```

The `READY` prompt means the interpreter is waiting for your command. You may type a statement for immediate execution (called **direct mode**), or you may type a line number followed by a statement to store it as part of a program.

Try typing the following and pressing RETURN:

```basic
PRINT 2 + 2
```

The computer responds:

```
4
READY
```

Congratulations -- you have just given your computer its first instruction!

Now try entering a short program:

```basic
10 PRINT "HELLO, WORLD!"
20 END
RUN
```

The computer responds:

```
HELLO, WORLD!
READY
```

You have written and executed your first Retro BASIC program.

---

## Chapter 3: Entering and Editing Programs

Programs in Retro BASIC are composed of numbered lines. Each line begins with a line number (1 through 65535), followed by a BASIC statement.

### Entering Lines

Simply type a line number, a space, and then the statement:

```basic
10 PRINT "FIRST LINE"
20 PRINT "SECOND LINE"
30 END
```

Lines are automatically sorted by line number in memory. If you type a new line with the same number as an existing line, it replaces the old line.

### Deleting Lines

To delete a line, type its line number alone and press RETURN:

```
20
```

This removes line 20 from the program.

### Inserting Lines

Because lines are sorted by number, you can insert new lines between existing ones by choosing an intermediate number:

```basic
15 PRINT "INSERTED BETWEEN 10 AND 20"
```

It is good practice to number your lines by tens (10, 20, 30...) so you have room to insert lines later.

### Line Editing

While typing a line, you may use the **BACKSPACE** or **DELETE** key to erase characters. The cursor moves back and the character is removed from the screen and the input buffer.

### Maximum Line Length

Each line may be up to **127 characters** long.

---

## Chapter 4: System Commands

System commands are typed at the `READY` prompt without a line number. They are executed immediately.

### RUN

Executes the program currently in memory, starting from the lowest line number.

```
RUN
```

### LIST

Displays the program currently stored in memory.

```
LIST
```

You may list a single line:

```
LIST 20
```

Or a range of lines:

```
LIST 10-50
```

### NEW

Erases the entire program from memory and clears all variables. The computer responds with `OK` to confirm.

```
NEW
OK
READY
```

> **CAUTION:** There is no way to recover a program after typing `NEW`. Use `SAVE` to write your program to disk before clearing memory.

### SAVE

Writes the current program to a file on the filesystem. The filename may be given with or without quotation marks.

```
SAVE "myprog.bas"
SAVE myprog.bas
SAVE /home/user/programs/hello.bas
```

The interpreter responds `OK` on success. If the file cannot be created (for example, a bad path or no write permission), the error `?CANNOT SAVE FILE` is shown.

> **NOTE:** The file is plain text in the same format shown by `LIST`. You can open and edit it in any text editor.

### LOAD

Reads a program from a file on the filesystem and replaces the current program in memory. All existing program lines are cleared before loading.

```
LOAD "myprog.bas"
LOAD myprog.bas
LOAD /home/user/programs/hello.bas
```

Paths with spaces work without any special treatment. Quotes are accepted but not required:

```
LOAD /Users/jacob/My Programs/hello.bas
LOAD "/Users/jacob/My Programs/hello.bas"
```

The interpreter responds `OK` on success. If the file cannot be opened (for example, it does not exist), the error `?CANNOT LOAD FILE` is shown.

After a successful `LOAD`, use `LIST` to verify the program and `RUN` to execute it.

> **NOTE:** `LOAD` calls `NEW` internally — all variables are reset and the previous program is erased before the file is read.

---

## Chapter 5: Variables and Data Types

Retro BASIC supports three kinds of data storage:

| Type | Names | Range / Size |
|---|---|---|
| Integer variable | `A` -- `Z` | -32768 to 32767 |
| String variable | `A$` -- `Z$` | Up to 63 characters |
| Array | `A()` -- `Z()` | Up to 64 elements total |

Variables do not need to be declared before use. All numeric variables are initialized to `0` and all string variables are initialized to empty (`""`) when the computer starts or when `NEW` is executed.

---

## Chapter 6: Numeric Variables

There are 26 numeric variables, named with a single letter from `A` to `Z`. Each stores a 16-bit signed integer, meaning values from **-32768** to **32767**.

```basic
10 LET A = 100
20 LET B = 200
30 LET C = A + B
40 PRINT C
50 END
```

Output:

```
300
```

You may also assign a variable without the `LET` keyword:

```basic
10 A = 42
20 PRINT A
```

---

## Chapter 7: String Variables

String variables are identified by a letter followed by a dollar sign: `A$`, `B$`, `C$`, and so on up to `Z$`. Each string variable can hold up to **63 characters**.

### Assigning Strings

```basic
10 LET A$ = "HELLO"
20 PRINT A$
30 END
```

Output:

```
HELLO
```

### String Concatenation

Strings can be joined together with the `+` operator:

```basic
10 A$ = "RETRO "
20 B$ = "BASIC"
30 C$ = A$ + B$
40 PRINT C$
50 END
```

Output:

```
RETRO BASIC
```

### Printing String Literals

Enclose text in double quotation marks within a `PRINT` statement:

```basic
PRINT "GREETINGS, PROGRAM!"
```

See [Chapter 24](#chapter-24-built-in-string-functions) for built-in string functions (`LEN`, `LEFT$`, `RIGHT$`, `MID$`).

---

## Chapter 8: Arrays

Arrays allow you to store multiple values under a single variable name, accessed by index. Before using an array, you must declare it with the `DIM` statement.

### One-Dimensional Arrays

```basic
10 DIM A(10)
20 A(0) = 42
30 A(1) = 99
40 PRINT A(0)
50 PRINT A(1)
60 END
```

This creates an array `A` with 10 elements, numbered 0 through 9.

### Two-Dimensional Arrays

```basic
10 DIM M(4,4)
20 M(0,0) = 1
30 M(1,1) = 1
40 M(2,2) = 1
50 M(3,3) = 1
60 PRINT "IDENTITY SET"
70 END
```

This creates a 4-by-4 matrix.

### Limits

- Up to **8** different arrays may be declared
- The total number of elements in any single array must not exceed **64**
- Array indices start at **0**
- Accessing an element outside the declared size returns `0` silently

---

## Chapter 9: Arithmetic Operators

Retro BASIC supports the following arithmetic operators:

| Operator | Meaning | Example | Result |
|---|---|---|---|
| `+` | Addition | `3 + 5` | `8` |
| `-` | Subtraction | `10 - 4` | `6` |
| `*` | Multiplication | `6 * 7` | `42` |
| `/` | Division | `20 / 4` | `5` |
| `MOD` | Modulus | `17 MOD 5` | `2` |
| `^` | Exponent | `2 ^ 8` | `256` |
| `-` | Negation (unary) | `-5` | `-5` |

**Division** is integer division. The fractional part is discarded:

```basic
PRINT 7 / 2
```

Output: `3`

**Division by zero** is protected. If you attempt to divide by zero, the result is `0` and no error is raised.

**Exponentiation** raises the left value to the power of the right value. Only positive integer exponents are supported.

```basic
PRINT 3 ^ 4
```

Output: `81`

---

## Chapter 10: Comparison Operators

Comparison operators compare two values and return a result: **-1** for TRUE, **0** for FALSE. This follows the convention of classic BASIC interpreters where TRUE is represented as -1 (all bits set).

| Operator | Meaning |
|---|---|
| `=` | Equal to |
| `<>` | Not equal to |
| `<` | Less than |
| `>` | Greater than |
| `<=` | Less than or equal to |
| `>=` | Greater than or equal to |

Example:

```basic
10 A = 10
20 B = 20
30 PRINT A < B
40 PRINT A > B
50 PRINT A = B
60 END
```

Output:

```
-1
0
0
```

---

## Chapter 11: Boolean Logic Operators

Boolean operators combine or modify logical values. They operate on the binary representation of integers (bitwise operations).

| Operator | Meaning | Example |
|---|---|---|
| `AND` | Bitwise AND | `A > 5 AND B < 10` |
| `OR` | Bitwise OR | `A = 1 OR B = 1` |
| `NOT` | Complement | `NOT A` |

When used with comparisons (which return -1 or 0), `AND` and `OR` behave as logical operators:

```basic
10 A = 15
20 IF A > 10 AND A < 20 THEN PRINT "IN RANGE"
30 END
```

Output:

```
IN RANGE
```

---

## Chapter 12: Operator Precedence

Expressions are evaluated according to the following precedence rules, from highest to lowest:

| Priority | Operators | Description |
|---|---|---|
| 1 (highest) | `( )` | Parentheses |
| 2 | `ABS()` `SQR()` etc. | Functions |
| 3 | `-x` `NOT x` | Unary minus, NOT |
| 4 | `^` | Exponentiation |
| 5 | `*` `/` `MOD` | Multiplication, division, modulus |
| 6 | `+` `-` | Addition, subtraction |
| 7 | `=` `<>` `<` `>` `<=` `>=` | Comparisons |
| 8 (lowest) | `AND` `OR` | Boolean logic |

When in doubt, use parentheses to make your intentions clear:

```basic
PRINT (2 + 3) * 4
```

Output: `20`

Without parentheses, multiplication is performed first:

```basic
PRINT 2 + 3 * 4
```

Output: `14`

---

## Chapter 13: The PRINT Statement

The `PRINT` statement displays text, numbers, and the results of expressions on the screen.

### Printing Text

```basic
PRINT "WELCOME TO RETRO BASIC"
```

### Printing Numbers

```basic
PRINT 42
PRINT 3 + 4 * 5
```

### Printing Variables

```basic
10 A = 100
20 PRINT A
```

### Printing Multiple Items

Use a semicolon (`;`) to print items next to each other with no space:

```basic
10 A = 5
20 PRINT "THE VALUE IS ";A
```

Output:

```
THE VALUE IS 5
```

Use a comma (`,`) to advance to the next tab position:

```basic
PRINT "NAME","SCORE"
PRINT "ALICE",95
PRINT "BOB",87
```

### Suppressing the Newline

Normally, `PRINT` advances to the next line after output. To keep the cursor on the same line, end with a semicolon:

```basic
10 PRINT "ENTER YOUR NAME: ";
20 INPUT N$
```

### Printing Strings and String Functions

```basic
10 A$ = "HELLO WORLD"
20 PRINT LEFT$(A$, 5)
30 PRINT RIGHT$(A$, 5)
40 PRINT MID$(A$, 7, 5)
```

Output:

```
HELLO
WORLD
WORLD
```

---

## Chapter 14: The INPUT Statement

The `INPUT` statement pauses program execution and waits for the user to type a value from the keyboard.

### Numeric Input

```basic
10 PRINT "HOW OLD ARE YOU";
20 INPUT A
30 PRINT "YOU ARE ";A;" YEARS OLD"
40 END
```

The computer displays a question mark (`?`) and waits. The user types a number and presses RETURN.

### String Input

Add a dollar sign to the variable name:

```basic
10 INPUT "WHAT IS YOUR NAME"; N$
20 PRINT "HELLO, ";N$
30 END
```

### Custom Prompt

You may include a prompt string before the variable:

```basic
10 INPUT "ENTER A NUMBER: " A
```

---

## Chapter 15: The LET Statement

The `LET` statement assigns a value to a variable.

```basic
10 LET A = 42
20 LET B = A * 2
30 LET C = A + B
```

The keyword `LET` is optional. You may write:

```basic
10 A = 42
20 B = A * 2
```

### String Assignment

```basic
10 LET A$ = "HELLO"
20 A$ = "WORLD"
```

### Array Assignment

```basic
10 DIM X(10)
20 LET X(0) = 99
30 X(5) = 42
```

---

## Chapter 16: The GOTO Statement

The `GOTO` statement transfers execution to the specified line number.

```basic
10 PRINT "THIS REPEATS FOREVER"
20 GOTO 10
```

Press **Ctrl-C** in your terminal to break out of an infinite loop.

`GOTO` should be used sparingly. Structured loop commands (`FOR`, `WHILE`, `REPEAT`, `DO`) are preferred for clarity.

If the target line does not exist, the error `?UNDEF'D LINE` is displayed and the program stops.

---

## Chapter 17: The IF...THEN...ELSE...ENDIF Statement

The `IF` statement tests a condition and controls which statements are executed based on the result.

### Single-Line IF...THEN

```basic
10 A = 5
20 IF A > 3 THEN PRINT "A IS GREATER THAN THREE"
30 END
```

If the condition is true, the statement after `THEN` is executed. If false, execution continues on the next line.

### IF...THEN with Line Number

```basic
10 A = 5
20 IF A > 10 THEN 50
30 PRINT "A IS SMALL"
40 GOTO 60
50 PRINT "A IS BIG"
60 END
```

If the condition is true, execution jumps to the specified line number.

### Block IF...THEN...ELSE...ENDIF

For multi-line conditional blocks, place `THEN` at the end of the `IF` line (with nothing after it), and close the block with `ENDIF`:

```basic
10 INPUT "ENTER A NUMBER: " A
20 IF A > 0 THEN
30 PRINT "POSITIVE"
40 ELSE
50 PRINT "NOT POSITIVE"
60 ENDIF
70 END
```

### Nested IF Blocks

`IF` blocks may be nested up to **8 levels** deep:

```basic
10 A = 15
20 IF A > 10 THEN
30 IF A > 20 THEN
40 PRINT "VERY LARGE"
50 ELSE
60 PRINT "MODERATELY LARGE"
70 ENDIF
80 ELSE
90 PRINT "SMALL"
100 ENDIF
110 END
```

Output:

```
MODERATELY LARGE
```

---

## Chapter 18: The FOR...NEXT Loop

The `FOR`...`NEXT` loop repeats a block of statements a specified number of times.

### Basic Syntax

```basic
10 FOR I = 1 TO 10
20 PRINT I
30 NEXT I
```

This prints the numbers 1 through 10.

### Using STEP

By default, the loop variable increases by 1 each time. Use `STEP` to change the increment:

```basic
10 FOR I = 0 TO 100 STEP 10
20 PRINT I
30 NEXT I
```

This prints 0, 10, 20, ... 100.

### Counting Backwards

Use a negative `STEP` to count down:

```basic
10 FOR I = 10 TO 1 STEP -1
20 PRINT I
30 NEXT I
40 PRINT "LIFTOFF!"
```

### Nested FOR Loops

`FOR` loops may be nested. Always use different variable names, and make sure `NEXT` statements appear in the correct order (innermost first):

```basic
10 FOR Y = 1 TO 3
20 FOR X = 1 TO 5
30 PRINT X * Y;" ";
40 NEXT X
50 PRINT
60 NEXT Y
```

Output:

```
1 2 3 4 5
2 4 6 8 10
3 6 9 12 15
```

### Limits

`FOR` loops may be nested up to **8 levels** deep.

---

## Chapter 19: The WHILE...WEND Loop

The `WHILE`...`WEND` loop repeats a block of statements as long as a condition is true. The condition is tested at the beginning of each pass, so if the condition is false initially, the body is never executed.

```basic
10 A = 1
20 WHILE A <= 10
30 PRINT A
40 A = A + 1
50 WEND
60 END
```

This prints the numbers 1 through 10.

### Skipping the Loop

If the condition is false from the start, the entire block is skipped:

```basic
10 A = 100
20 WHILE A < 10
30 PRINT "THIS NEVER PRINTS"
40 WEND
50 PRINT "DONE"
60 END
```

Output:

```
DONE
```

### Limits

`WHILE` loops may be nested up to **8 levels** deep.

---

## Chapter 20: The REPEAT...UNTIL Loop

The `REPEAT`...`UNTIL` loop executes a block of statements and then tests a condition. The body always executes at least once. The loop continues until the condition becomes true.

```basic
10 A = 1
20 REPEAT
30 PRINT A
40 A = A + 1
50 UNTIL A > 10
60 END
```

This prints the numbers 1 through 10.

### Comparison with WHILE

| Feature | WHILE...WEND | REPEAT...UNTIL |
|---|---|---|
| Condition tested | Before body | After body |
| Minimum executions | 0 | 1 |
| Exits when condition | Becomes false | Becomes true |

### Limits

`REPEAT` loops may be nested up to **8 levels** deep.

---

## Chapter 21: The DO...LOOP Structure

The `DO`...`LOOP` is the most flexible loop structure. The condition can be tested at the end of the loop using either `WHILE` or `UNTIL`.

### DO...LOOP WHILE

Repeats as long as the condition is true:

```basic
10 A = 1
20 DO
30 PRINT A
40 A = A + 1
50 LOOP WHILE A <= 5
60 END
```

Output: `1 2 3 4 5`

### DO...LOOP UNTIL

Repeats until the condition becomes true:

```basic
10 A = 1
20 DO
30 PRINT A
40 A = A + 1
50 LOOP UNTIL A > 5
60 END
```

Output: `1 2 3 4 5`

### Infinite DO...LOOP

If `LOOP` appears without `WHILE` or `UNTIL`, the loop repeats forever. Use `GOTO` or `STOP` to exit.

```basic
10 DO
20 PRINT "FOREVER"
30 LOOP
```

### Limits

`DO` loops may be nested up to **8 levels** deep.

---

## Chapter 22: GOSUB and RETURN

`GOSUB` and `RETURN` allow you to write subroutines -- reusable blocks of code that can be called from anywhere in your program.

```basic
10 PRINT "MAIN PROGRAM"
20 GOSUB 100
30 PRINT "BACK IN MAIN"
40 END
100 REM -- SUBROUTINE --
110 PRINT "INSIDE SUBROUTINE"
120 RETURN
```

Output:

```
MAIN PROGRAM
INSIDE SUBROUTINE
BACK IN MAIN
```

### How It Works

When the computer encounters `GOSUB`, it remembers where it was (the return address) and jumps to the specified line. When `RETURN` is reached, execution resumes at the line after the original `GOSUB`.

### Nested Subroutines

Subroutines may call other subroutines. The return addresses are stored on a stack, up to **16 levels** deep.

```basic
10 GOSUB 100
20 END
100 PRINT "LEVEL 1"
110 GOSUB 200
120 RETURN
200 PRINT "LEVEL 2"
210 RETURN
```

### Errors

| Error | Cause |
|---|---|
| `?RETURN WITHOUT GOSUB` | A `RETURN` was encountered without a matching `GOSUB` |
| `?GOSUB OVERFLOW` | More than 16 nested `GOSUB` calls |
| `?UNDEF'D LINE` | The target line does not exist |

---

## Chapter 23: Built-In Numeric Functions

Retro BASIC provides several built-in numeric functions. All functions take their argument in parentheses.

### ABS(n)

Returns the absolute value of *n*.

```basic
PRINT ABS(-42)
```

Output: `42`

### SGN(n)

Returns the sign of *n*: `1` if positive, `-1` if negative, `0` if zero.

```basic
PRINT SGN(-7)
PRINT SGN(0)
PRINT SGN(15)
```

Output:

```
-1
0
1
```

### SQR(n)

Returns the integer square root of *n* (the largest integer whose square does not exceed *n*).

```basic
PRINT SQR(144)
PRINT SQR(10)
```

Output:

```
12
3
```

### RND(n)

Returns a pseudo-random number.

- If *n* > 0, returns a random number from **0** to **n-1**.
- If *n* <= 0, returns a random number from **0** to **32767**.

```basic
10 FOR I = 1 TO 5
20 PRINT RND(100)
30 NEXT I
```

This prints 5 random numbers between 0 and 99.

To simulate a six-sided die:

```basic
PRINT RND(6) + 1
```

### MIN(a, b)

Returns the smaller of two values.

```basic
PRINT MIN(10, 25)
```

Output: `10`

### MAX(a, b)

Returns the larger of two values.

```basic
PRINT MAX(10, 25)
```

Output: `25`

---

## Chapter 24: Built-In String Functions

Retro BASIC provides functions for examining and manipulating strings.

### LEN(s$)

Returns the number of characters in the string variable.

```basic
10 A$ = "HELLO"
20 PRINT LEN(A$)
```

Output: `5`

### LEFT$(s$, n)

Returns the first *n* characters of a string.

```basic
10 A$ = "RETRO BASIC"
20 PRINT LEFT$(A$, 5)
```

Output: `RETRO`

### RIGHT$(s$, n)

Returns the last *n* characters of a string.

```basic
10 A$ = "RETRO BASIC"
20 PRINT RIGHT$(A$, 5)
```

Output: `BASIC`

### MID$(s$, start, length)

Returns a substring beginning at position *start* (1-based) for *length* characters. If *length* is omitted, returns everything from *start* to the end.

```basic
10 A$ = "RETRO BASIC"
20 PRINT MID$(A$, 7, 5)
30 PRINT MID$(A$, 7)
```

Output:

```
BASIC
BASIC
```

### String Concatenation

Use the `+` operator to join strings:

```basic
10 A$ = "GOOD "
20 B$ = "MORNING"
30 PRINT A$ + B$
```

Output: `GOOD MORNING`

String functions can be combined:

```basic
10 A$ = "HELLO WORLD"
20 PRINT LEFT$(A$, 5) + " " + RIGHT$(A$, 5)
```

Output: `HELLO WORLD`

---

## Chapter 25: Direct Memory Access

Retro BASIC provides two statements for reading and writing individual bytes of memory. `PEEK` and `POKE` operate on a **simulated 64KB memory space** (addresses 0 through 65535). This means you can freely experiment without any risk of crashing the machine.

### PEEK(address)

Returns the byte value (0-255) stored at the specified address.

```basic
PRINT PEEK(100)
```

`PEEK` can be used in any expression:

```basic
10 A = PEEK(100)
20 IF A > 0 THEN PRINT "SLOT HAS DATA"
```

### POKE address, value

Writes a byte value (0-255) to the specified address.

```basic
POKE 100, 42
PRINT PEEK(100)
```

Output: `42`

You can use PEEK and POKE to pass data between subroutines, store lookup tables, or implement simple data structures.

> **NOTE:** On a modern system, PEEK and POKE use a 64KB simulated memory array. No real hardware addresses are accessed. Addresses outside 0-65535 are silently ignored.

---

## Chapter 26: Screen Control and Graphics

Retro BASIC includes commands for controlling the display and drawing text-mode graphics. All commands send standard ANSI escape sequences and work with any terminal that supports them (macOS Terminal, iTerm2, GNOME Terminal, PuTTY, and others).

> **NOTE:** All screen positions use **column, row** order, where column 1, row 1 is the top-left corner of the terminal. Most terminals are 80 columns wide and 24 rows tall.

### CLS

Clears the entire screen and moves the cursor to the top-left corner.

```basic
10 CLS
20 PRINT "FRESH SCREEN!"
```

### LOCATE row, column

Moves the cursor to the specified row and column. Row 1, column 1 is the top-left corner. Useful for placing text precisely without drawing a full graphic.

```basic
10 CLS
20 LOCATE 10, 5
30 PRINT "I AM AT ROW 10, COLUMN 5"
```

### COLOR fg [, bg]

Sets the foreground (text) and optionally background color using ANSI color codes.

| Value | Color |
|---|---|
| 0 | Black |
| 1 | Red |
| 2 | Green |
| 3 | Yellow |
| 4 | Blue |
| 5 | Magenta |
| 6 | Cyan |
| 7 | White |
| 8–15 | Bright versions of the above |
| -1 | Reset all colors to default |

```basic
10 COLOR 2
20 PRINT "THIS IS GREEN"
30 COLOR 1, 7
40 PRINT "RED TEXT ON WHITE BACKGROUND"
50 COLOR -1
60 PRINT "BACK TO NORMAL"
```

### DRAW x, y, expr

Positions the cursor at column `x`, row `y` and draws a string or character there. `expr` can be a string literal, a string variable, or an integer (treated as an ASCII character code).

```basic
10 CLS
20 COLOR 3
30 DRAW 10, 5, "HELLO!"
40 DRAW 10, 6, A$
50 DRAW 20, 8, 42
```

Line 50 draws the character with ASCII code 42 (`*`) at column 20, row 8.

### LINE x1, y1, x2, y2 [, char$]

Draws a straight line from (x1, y1) to (x2, y2) using Bresenham's line algorithm. The optional `char$` specifies the character to draw with (default `*`).

```basic
10 CLS
20 COLOR 4
30 LINE 1, 1, 40, 20
40 LINE 40, 1, 1, 20, "+"
```

### RECT x1, y1, x2, y2 [, char$]

Draws the outline of a rectangle with corners at (x1, y1) and (x2, y2). The optional `char$` specifies the character to use (default `*`).

```basic
10 CLS
20 COLOR 6
30 RECT 5, 3, 35, 15
40 RECT 5, 3, 35, 15, "#"
```

### FILL x1, y1, x2, y2 [, char$]

Fills a rectangular area from (x1, y1) to (x2, y2) with a character (default space ` `).

```basic
10 CLS
20 COLOR 5
30 FILL 5, 3, 35, 15, "."
```

To erase a region, fill it with spaces:

```basic
FILL 5, 3, 35, 15
```

### PAUSE ms

Pauses execution for `ms` milliseconds. Essential for controlling animation speed.

```basic
10 PAUSE 500
```

Pause 500ms (half a second). Typical animation frames use 50–100ms.

### CURSOR n

Shows (`n` = 1) or hides (`n` = 0) the terminal cursor. Hide the cursor during animation to avoid flickering.

```basic
10 CURSOR 0
20 REM ... draw animation frames ...
30 CURSOR 1
```

---

### Animation Pattern

Combine the graphics commands to build smooth text-mode animations:

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

This bounces an `O` across the middle of the screen. Press **Ctrl-C** to stop.

---

## Chapter 27: The REM Statement

The `REM` (remark) statement lets you add comments to your program. Everything after `REM` on that line is ignored by the interpreter.

```basic
10 REM THIS IS A COMMENT
20 REM CALCULATE THE SUM
30 A = 10
40 B = 20
50 C = A + B
60 PRINT C
70 END
```

Comments do not affect program execution but they do use program memory. Use them wisely to document important sections of your code.

---

## Chapter 28: Program Control Statements

### END

Terminates program execution and returns to the `READY` prompt.

```basic
10 PRINT "THE END"
20 END
```

A program will also stop when it runs past the last line, but it is good practice to include an explicit `END` statement.

### STOP

Halts execution and prints the line number where the program stopped, then returns to `READY`. Useful for debugging.

```basic
10 A = 42
20 STOP
30 PRINT "THIS LINE IS NOT REACHED"
```

Output:

```
BREAK IN 20
READY
```

---

## Chapter 29: Direct Mode

Many statements can be typed at the `READY` prompt for immediate execution without a line number. This is called **direct mode**.

The following commands work in direct mode:

| Command | Description |
|---|---|
| `PRINT` | Display values and text |
| `LET` | Assign a variable |
| `DIM` | Declare an array |
| `INPUT` | Read from keyboard |
| `PEEK` | Read a memory location |
| `POKE` | Write to a memory location |
| `CLS` | Clear the screen |
| `RUN` | Execute the stored program |
| `LIST` | Display the stored program |
| `NEW` | Erase program and clear variables |
| `A = expr` | Direct variable assignment |

Direct mode is ideal for quick calculations, testing expressions, and examining or setting variables.

```basic
PRINT 256 * 256
LET A = 42
PRINT A
PRINT SQR(A)
DIM X(10)
X(0) = 99
PRINT X(0)
```

---

## Chapter 30: Error Messages

When something goes wrong, Retro BASIC displays an error message. If the error occurs during program execution, the line number is included.

| Message | Meaning |
|---|---|
| `?SYNTAX ERROR` | The statement could not be understood |
| `?SYNTAX ERROR IN 50` | Syntax error on line 50 |
| `?UNDEF'D LINE IN 30` | GOTO or GOSUB to a line that does not exist |
| `?RETURN WITHOUT GOSUB` | RETURN without a matching GOSUB |
| `?GOSUB OVERFLOW` | Too many nested GOSUBs (max 16) |
| `?FOR OVERFLOW` | Too many nested FOR loops (max 8) |
| `?NEXT WITHOUT FOR` | NEXT without a matching FOR |
| `?WHILE OVERFLOW` | Too many nested WHILE loops (max 8) |
| `?WEND WITHOUT WHILE` | WEND without a matching WHILE |
| `?REPEAT OVERFLOW` | Too many nested REPEAT loops (max 8) |
| `?UNTIL WITHOUT REPEAT` | UNTIL without a matching REPEAT |
| `?DO OVERFLOW` | Too many nested DO loops (max 8) |
| `?LOOP WITHOUT DO` | LOOP without a matching DO |
| `?BAD SUBSCRIPT` | Array index error or DIM failure |
| `?OUT OF MEMORY` | Program too large for memory |
| `BREAK IN 20` | STOP encountered on line 20 |

---

## Chapter 31: Technical Notes

Retro BASIC V2.0 is a portable C99 application that runs on any modern POSIX system: macOS, Linux, Raspberry Pi (any model), and any x86 or ARM machine with a C compiler.

### System Limits

| Resource | Limit |
|---|---|
| Program storage | 4096 bytes |
| Maximum program lines | 256 |
| Maximum line length | 127 characters |
| Integer variables | 26 (A-Z) |
| String variables | 26 (A$-Z$, 63 chars each) |
| Arrays | 8 (max 64 elements each) |
| GOSUB stack depth | 16 |
| FOR / WHILE / REPEAT / DO stack depth | 8 each |
| IF nesting depth | 8 |
| Simulated memory (PEEK/POKE) | 65536 bytes |

### Program Storage Format

Programs are stored in a contiguous buffer. Each line is stored as:

| Byte(s) | Content |
|---|---|
| 1 | Line number high byte |
| 2 | Line number low byte |
| 3...n | ASCII text of the line |
| n+1 | Null terminator (`$00`) |

Lines are kept in sorted order by line number. The interpreter maintains an index of up to **256 lines** for fast lookup during execution.

### I/O

The interpreter reads from **standard input** and writes to **standard output**. This means it works naturally as a pipeline:

```sh
echo 'PRINT 2 + 2' | ./build/basic
```

`CLS` and `LOCATE` send ANSI escape sequences. They work with any ANSI-compatible terminal (macOS Terminal, Linux console, PuTTY, etc.).

---

## Chapter 32: Building from Source

Retro BASIC is written in standard **C99** and builds with any modern C compiler. No special toolchain is required.

### Requirements

- `gcc`, `clang`, or any C99-compatible compiler
- `make`

### Platforms Tested

- macOS (Apple Silicon and Intel)
- Raspberry Pi (all models, 32-bit and 64-bit Raspberry Pi OS)
- Linux (x86-64, ARM)
- Any POSIX system with a C compiler

### Source Files

| File | Purpose |
|---|---|
| `config.h` | Configuration and constants |
| `io.c` / `io.h` | Standard I/O (stdin/stdout) |
| `vars.c` / `vars.h` | Variables, arrays, string functions, simulated memory |
| `expr.c` / `expr.h` | Expression parser |
| `program.c` / `program.h` | Program storage and line management |
| `commands.c` / `commands.h` | BASIC command execution |
| `repl.c` / `repl.h` | Interactive command loop |
| `main.c` | Entry point |

### Building

```sh
cd Retro-BASIC
make
```

The output executable is `build/basic`. Run it directly:

```sh
./build/basic
```

### Choosing a Compiler

```sh
make CC=gcc
make CC=clang
```

### Clean Build

```sh
make clean
make
```

### Running Programs from a File

Because the interpreter reads from standard input, you can run a saved program file directly:

```sh
./build/basic < myprog.bas
```

---

## Appendix A: Quick Reference Card

### System Commands

| Command | Syntax |
|---|---|
| Run program | `RUN` |
| List program | `LIST [n][-m]` |
| Clear memory | `NEW` |
| Save to file | `SAVE filename` |
| Load from file | `LOAD filename` |

### Statements

| Statement | Syntax |
|---|---|
| Print | `PRINT expr [; expr] [, expr]` |
| Input | `INPUT ["prompt";] var` |
| Assign | `LET var = expr` or `var = expr` |
| Dimension | `DIM v(n [,m])` |
| Branch | `GOTO line` |
| Subroutine | `GOSUB line` / `RETURN` |
| Conditional | `IF cond THEN stmt` or `IF cond THEN` ... `[ELSE]` ... `ENDIF` |
| Memory write | `POKE addr, val` |
| Clear screen | `CLS` |
| Move cursor | `LOCATE row, col` |
| Set color | `COLOR fg [, bg]` |
| Draw at position | `DRAW x, y, expr` |
| Draw line | `LINE x1, y1, x2, y2 [, char$]` |
| Draw rectangle | `RECT x1, y1, x2, y2 [, char$]` |
| Fill rectangle | `FILL x1, y1, x2, y2 [, char$]` |
| Wait | `PAUSE ms` |
| Cursor on/off | `CURSOR n` |
| Comment | `REM text` |
| End program | `END` |
| Break | `STOP` |

### Loops

| Loop | Syntax |
|---|---|
| FOR | `FOR v = a TO b [STEP s]` ... `NEXT v` |
| WHILE | `WHILE cond` ... `WEND` |
| REPEAT | `REPEAT` ... `UNTIL cond` |
| DO | `DO` ... `LOOP [WHILE cond \| UNTIL cond]` |

### Operators

| Category | Operators |
|---|---|
| Arithmetic | `+` `-` `*` `/` `MOD` `^` |
| Comparison | `=` `<>` `<` `>` `<=` `>=` |
| Boolean | `AND` `OR` `NOT` |

### Functions

| Function | Returns |
|---|---|
| `ABS(n)` | Absolute value |
| `SGN(n)` | Sign (-1, 0, 1) |
| `SQR(n)` | Integer square root |
| `RND(n)` | Random number |
| `MIN(a,b)` | Smaller value |
| `MAX(a,b)` | Larger value |
| `PEEK(addr)` | Byte at address |
| `LEN(s$)` | String length |
| `LEFT$(s$,n)` | First n characters |
| `RIGHT$(s$,n)` | Last n characters |
| `MID$(s$,s,l)` | Substring |

### Variables

| Type | Names | Capacity |
|---|---|---|
| Integer | `A` -- `Z` | -32768 to 32767 |
| String | `A$` -- `Z$` | 63 characters |
| Array | `A()` -- `Z()` | DIM first, max 64 elements |

---

## Appendix B: Sample Programs

### Multiplication Table

```basic
10 REM MULTIPLICATION TABLE
20 CLS
30 FOR Y = 1 TO 10
40 FOR X = 1 TO 10
50 P = X * Y
60 IF P < 10 THEN PRINT " ";
70 PRINT P;" ";
80 NEXT X
90 PRINT
100 NEXT Y
110 END
```

### Guess the Number

```basic
10 REM GUESS THE NUMBER
20 S = RND(100) + 1
30 PRINT "I AM THINKING OF A NUMBER"
40 PRINT "BETWEEN 1 AND 100."
50 T = 0
60 REPEAT
70 T = T + 1
80 INPUT "YOUR GUESS" G
90 IF G < S THEN PRINT "TOO LOW"
100 IF G > S THEN PRINT "TOO HIGH"
110 UNTIL G = S
120 PRINT "CORRECT! YOU GOT IT IN ";T;" TRIES!"
130 END
```

### Fibonacci Sequence

```basic
10 REM FIBONACCI NUMBERS
20 A = 0
30 B = 1
40 PRINT A
50 WHILE B < 10000
60 PRINT B
70 C = A + B
80 A = B
90 B = C
100 WEND
110 END
```

### Subroutine Demo

```basic
10 REM SUBROUTINE DEMONSTRATION
20 FOR I = 1 TO 5
30 N = I
40 GOSUB 100
50 NEXT I
60 END
100 REM -- FACTORIAL --
110 F = 1
120 FOR J = 1 TO N
130 F = F * J
140 NEXT J
150 PRINT N;"! = ";F
160 RETURN
```

### Array Sorting (Bubble Sort)

```basic
10 REM BUBBLE SORT
20 DIM A(10)
30 FOR I = 0 TO 9
40 A(I) = RND(100)
50 NEXT I
60 PRINT "BEFORE: "
70 GOSUB 200
80 REM -- SORT --
90 FOR I = 0 TO 8
100 FOR J = 0 TO 8
110 IF A(J) > A(J+1) THEN GOSUB 300
120 NEXT J
130 NEXT I
140 PRINT "AFTER:  "
150 GOSUB 200
160 END
200 REM -- PRINT ARRAY --
210 FOR K = 0 TO 9
220 PRINT A(K);" ";
230 NEXT K
240 PRINT
250 RETURN
300 REM -- SWAP --
310 T = A(J)
320 A(J) = A(J+1)
330 A(J+1) = T
340 RETURN
```

### String Fun

```basic
10 REM STRING DEMONSTRATION
20 A$ = "RETRO BASIC IS GREAT"
30 PRINT "FULL: ";A$
40 PRINT "LENGTH: ";LEN(A$)
50 PRINT "FIRST 5: ";LEFT$(A$, 5)
60 PRINT "LAST 5: ";RIGHT$(A$, 5)
70 PRINT "MIDDLE: ";MID$(A$, 7, 5)
80 B$ = LEFT$(A$, 5) + " " + RIGHT$(A$, 5)
90 PRINT "COMBINED: ";B$
100 END
```

---

> *This manual was written for RETRO BASIC V2.0.*
>
> *May your programs be free of bugs and your line numbers always in order.*
>
> *HAPPY COMPUTING!*

# novalang

Programming language for a study project in the course 'Compiler Construction'. For the language specification, see the [docs](docs/docs.pdf).

## Build

To build the project, you need to have the following tools installed:

- Bison
- Flex
- Make

To build the project, run the following command:

```bash
make
```

This will compile the source code and create an executable called `novalang`.

## Run

To run the program, use the following command:

```bash
./novalang <input-file>
```

This will run the program and parse the input file. After the type check was successful, the program will be executed.

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

## Tests

The project contains several tests to ensure the correctness of the interpreter. To run the tests, use the following command:

```bash
make test
```

The test folder follows a specific structure to be able to just test for the successful execution of the program, a specific output or input. For more information on the structure, you can check out the [test script](tests.sh).

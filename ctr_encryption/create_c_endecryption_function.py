#! /usr/bin/python3.5

f = open("endecryption_direct_access.c", "w")

with open("ctr_encryption.c", "r") as fin:
    define = fin.readlines()[9].replace("\n", "").split(" ")[-1]
BITS_SIZE = int(define)
size = 1<<BITS_SIZE

f.write('#include "endecryption_direct_access.h"\n\n')
f.write("typedef uint16_t u16;")
f.write("void encrypt_block_jumps_hardcoded(uint8_t* b, size_t size, u16** ss) {\n")
f.write("  u16* s;\n")
f.write("  u16 t;\n")
for i in range(0, BITS_SIZE):
    f.write("  s=ss[{}];\n".format(i))
    jump = 1<<i
    for jump_pos in range(0, size, jump*2):
        for j in range(0, jump):
            f.write("  t=s[b[0x{:X}]<<8|b[0x{:X}]];".format(jump_pos+j, jump_pos+j+jump))
            f.write(" b[0x{:X}]=t>>8;".format(jump_pos+j))
            f.write(" b[0x{:X}]=t;\n".format(jump_pos+j+jump))
f.write("}\n\n")

f.write("void decrypt_block_jumps_hardcoded(uint8_t* b, size_t size, u16** ss) {\n")
f.write("  u16* s;\n")
f.write("  u16 t;\n")
for i in range(BITS_SIZE-1, -1, -1):
    f.write("  s=ss[{}];\n".format(i))
    jump = 1<<i
    for jump_pos in range(0, size, jump*2):
        for j in range(0, jump):
            f.write("  t=s[b[0x{:X}]<<8|b[0x{:X}]];".format(jump_pos+j, jump_pos+j+jump))
            f.write(" b[0x{:X}]=t>>8;".format(jump_pos+j))
            f.write(" b[0x{:X}]=t;\n".format(jump_pos+j+jump))
f.write("}\n")

f.close()

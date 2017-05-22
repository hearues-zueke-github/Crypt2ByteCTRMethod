#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/fs.h>

#include "ctr_encryption.h"

#define BLOCK_BITS 8

uint64_t rdtsc() {
  uint64_t a, d;
  __asm__ volatile ("mfence");
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  a = (d<<32) | a;
  __asm__ volatile ("mfence");
  return a;
}

void set_sboxes_16_bits(uint16_t* sbox_enc, uint16_t* sbox_dec) {
    for (int i = 0; i < 0x10000-1; i++) {
        sbox_enc[i] = i+1;
    }
    sbox_enc[0x10000-1] = 0;

    for (int k = 0; k < 4; k++) {
        for (int i = 0 ; i < 0x10000-1; i++) {
            if ((uint32_t)rand() % 2 == 0) {
                continue;
            }
            int j = i+1+((uint32_t)rand())%(0x10000-1-i);
            if ((sbox_enc[i] != j) && (sbox_enc[j] != i)) {
                int t = sbox_enc[i];
                sbox_enc[i] = sbox_enc[j];
                sbox_enc[j] = t;
            }
        }
    }

    for (int i = 0; i < 0x10000; i++) {
        sbox_dec[sbox_enc[i]] = i;
    }
}

void set_random_block(uint8_t* block, size_t size) {
    for (int i = 0; i < size; i++) {
        block[i] = rand();
    }
}

void calc_hash_block(uint8_t* i1, uint8_t* i2, uint8_t* o, int size) {
    o[0] = i1[0]^i2[0];
    for (int i = 1; i < size; i++) {
        o[i] = (i1[i]^i2[i])+o[i-1]+i;
    }
    o[0] = (i1[0]^i2[0])+o[size-1];
    o[1] = (i1[1]^i2[1])+o[0];
    for (int i = 2; i < size; i++) {
        o[i] = (i1[i]^i2[i])+o[i-2]+o[i-1]+1;
    }
}

void calc_xor_block(uint8_t* i1, uint8_t* i2, uint8_t* o, int size) {
    for (int i = 0; i < size; i++) {
        o[i] = i1[i]^i2[i];
    }
}

void define_sboxes(char* file_name, uint16_t*** sboxes_enc_ref, uint16_t*** sboxes_dec_ref, uint64_t block_bits) {
    int hash_block_size = 1<<8;
    int block_amounts = 0x20000/hash_block_size;
    uint8_t* hash_block = malloc(hash_block_size);

    uint16_t** sboxes_enc = malloc(sizeof(uint16_t*)*block_bits);
    uint16_t** sboxes_dec = malloc(sizeof(uint16_t*)*block_bits);
    for (int i = 0; i < block_bits; i++) {
        sboxes_enc[i] = malloc(sizeof(uint16_t)*0x10000);
        sboxes_dec[i] = malloc(sizeof(uint16_t)*0x10000);
    }

    for (int i = 0; i < block_bits; i++) {
        set_sboxes_16_bits((void*)sboxes_enc[i], (void*)sboxes_dec[i]);
    }
    FILE* fout = fopen(file_name, "wb");
    uint64_t file_size = (0x20000*block_bits+hash_block_size*block_bits)*2;
    fwrite(&block_bits, 1, sizeof(uint64_t), fout);
    fwrite(&file_size, 1, sizeof(uint64_t), fout);
    for (int i = 0; i < block_bits; i++) {
        uint8_t* sbox_enc = (uint8_t*)sboxes_enc[i];
        uint8_t* sbox_dec = (uint8_t*)sboxes_dec[i];
        
        memset(hash_block, 0, hash_block_size);
        calc_hash_block(hash_block, sbox_enc, hash_block, hash_block_size);
        for (int j = 1; j < block_amounts; j++) {
            calc_hash_block(hash_block, sbox_enc+j*hash_block_size, hash_block, hash_block_size);
        }
        fwrite(hash_block, 1, hash_block_size, fout);

        memset(hash_block, 0, hash_block_size);
        calc_hash_block(hash_block, sbox_dec, hash_block, hash_block_size);
        for (int j = 1; j < block_amounts; j++) {
            calc_hash_block(hash_block, sbox_dec+j*hash_block_size, hash_block, hash_block_size);
        }
        fwrite(hash_block, 1, hash_block_size, fout);
    }
    for (int i = 0; i < block_bits; i++) {
        fwrite(sboxes_enc[i], 1, 0x20000, fout);
        fwrite(sboxes_dec[i], 1, 0x20000, fout);
    }
    fclose(fout);

    free(hash_block);

    *sboxes_enc_ref = sboxes_enc;
    *sboxes_dec_ref = sboxes_dec;
}

int get_sboxes(char* file_name, uint16_t*** sboxes_enc_ref, uint16_t*** sboxes_dec_ref, uint64_t* block_bits) {
    int hash_block_size = 1<<8;
    int block_amounts = 0x20000/hash_block_size;
    uint8_t* hash_block = malloc(hash_block_size);

    int is_hash_ok = 1;
    FILE* fin = fopen(file_name, "rb");

    struct stat file_stat;
    if (fstat(fileno(fin),&file_stat)== -1) {
        printf("File stat failed!\n");
        return -4;
    }
    uint64_t file_size = file_stat.st_size-sizeof(uint64_t)*2;

    uint64_t block_bits_read;
    uint64_t file_size_read;
    fread(&block_bits_read, 1, sizeof(uint64_t), fin);
    fread(&file_size_read, 1, sizeof(uint64_t), fin);

    if (block_bits_read == 0 || block_bits_read > 20) {
        printf("block_bits_read is %zx, should be between 1 and 19\n", block_bits_read);
        return 1;
    }
    *block_bits = block_bits_read;
    uint64_t precise_file_size = (hash_block_size*block_bits_read+0x20000*block_bits_read)*2;
    if (file_size_read != precise_file_size) {
        printf("file_size_read is not equal to precise_file_size!\n");
        return 1;
    }
    if (file_size < precise_file_size) {
        printf("file_size is smaller than precise_file_size!\n");
        return 1;
    }

    uint16_t** sboxes_enc = malloc(sizeof(uint16_t*)*block_bits_read);
    uint16_t** sboxes_dec = malloc(sizeof(uint16_t*)*block_bits_read);
    for (int i = 0; i < block_bits_read; i++) {
        sboxes_enc[i] = malloc(sizeof(uint16_t)*0x10000);
        sboxes_dec[i] = malloc(sizeof(uint16_t)*0x10000);
    }

    uint8_t** hash_blocks_enc = malloc(sizeof(uint8_t*)*block_bits_read);
    uint8_t** hash_blocks_dec = malloc(sizeof(uint8_t*)*block_bits_read);
    for (int i = 0; i < block_bits_read; i++) {
        hash_blocks_enc[i] = malloc(hash_block_size);
        hash_blocks_dec[i] = malloc(hash_block_size);
    }
    for (int i = 0; i < block_bits_read; i++) {
        fread(hash_blocks_enc[i], 1, hash_block_size, fin);
        fread(hash_blocks_dec[i], 1, hash_block_size, fin);
    }
    for (int i = 0; i < block_bits_read; i++) {
        fread(sboxes_enc[i], 1, 0x20000, fin);
        fread(sboxes_dec[i], 1, 0x20000, fin);
    }
    for (int i = 0; i < block_bits_read; i++) {
        uint8_t* sbox_enc = (uint8_t*)sboxes_enc[i];
        uint8_t* sbox_dec = (uint8_t*)sboxes_dec[i];

        memset(hash_block, 0, hash_block_size);
        calc_hash_block(hash_block, sbox_enc, hash_block, hash_block_size);
        for (int j = 1; j < block_amounts; j++) {
            calc_hash_block(hash_block, sbox_enc+j*hash_block_size, hash_block, hash_block_size);
        }
        if (memcmp(hash_block, hash_blocks_enc[i], hash_block_size)) {
            is_hash_ok = 0;
            break;
        }

        memset(hash_block, 0, hash_block_size);
        calc_hash_block(hash_block, sbox_dec, hash_block, hash_block_size);
        for (int j = 1; j < block_amounts; j++) {
            calc_hash_block(hash_block, sbox_dec+j*hash_block_size, hash_block, hash_block_size);
        }
        if (memcmp(hash_block, hash_blocks_dec[i], hash_block_size)) {
            is_hash_ok = 0;
            break;
        }
    }
    fclose(fin);
    for (int i = 0; i < block_bits_read; i++) {
        free(hash_blocks_enc[i]);
        free(hash_blocks_dec[i]);
    }
    free(hash_blocks_enc);
    free(hash_blocks_dec);
    free(hash_block);
    
    if (!is_hash_ok) {
        printf("The hash of one or more sboxes is not OK!!!\n");
        for (int i = 0; i < block_bits_read; i++) {
            free(sboxes_enc[i]);
            free(sboxes_dec[i]);
        }
        free(sboxes_enc);
        free(sboxes_dec);

        return 1;
    }

    *sboxes_enc_ref = sboxes_enc;
    *sboxes_dec_ref = sboxes_dec;

    return 0;
}

void encrypt_block_jumps(uint8_t* block, size_t size, uint16_t** sboxes, uint64_t block_bits) {
    assert(size % 2 == 0);
    for (int i = 0; i < block_bits; i++) {
        uint16_t* sbox = sboxes[i];
        int jump = 1<<i;
        for (int jump_pos = 0; jump_pos < size; jump_pos+=jump*2) {
            for (int j = 0; j < jump; j++) {
                uint16_t t = sbox[((uint16_t)block[jump_pos+j])<<8|((uint16_t)block[jump_pos+j+jump])];
                block[jump_pos+j] = (t>>8)&0xFF;
                block[jump_pos+j+jump] = (uint8_t)(t&0xFF);
            }
        }
    }
}

void decrypt_block_jumps(uint8_t* block, size_t size, uint16_t** sboxes, uint64_t block_bits) {
    assert(size % 2 == 0);
    for (int i = block_bits; i > 0; i--) {
        uint16_t* sbox = sboxes[i-1];
        int jump = 1<<(i-1);
        for (int jump_pos = 0; jump_pos < size; jump_pos+=jump*2) {
            for (int j = 0; j < jump; j++) {
                uint16_t t = sbox[((uint16_t)block[jump_pos+j])<<8|((uint16_t)block[jump_pos+j+jump])];
                block[jump_pos+j] = (t>>8)&0xFF;
                block[jump_pos+j+jump] = (uint8_t)(t&0xFF);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    srand(rdtsc());

    if (argc < 5) {
        printf("usage: ./<program> <key_file_name> (encryption|decryption) <input_file_name> <output_file_name> ...");
        return -1;
    }

    uint16_t** sboxes_enc;
    uint16_t** sboxes_dec;

    uint64_t block_bits = BLOCK_BITS;
    char* key_file_name = argv[1];
    char* modus_string = argv[2];
    char* input_file_name = argv[3];
    char* output_file_name = argv[4];
    if (access(key_file_name, F_OK) != -1) {
        if (get_sboxes(key_file_name, &sboxes_enc, &sboxes_dec, &block_bits)) {
            return -2;
        }
    } else {
        printf("File '%s' was not found, creating new key... \n", key_file_name);
        define_sboxes(key_file_name, &sboxes_enc, &sboxes_dec, block_bits);
        printf("Successfully created file '%s'\n", key_file_name);
    }

    int modus = -1;
    if (strcmp(modus_string, "encryption") == 0) {
        modus = 0;
    } else if (strcmp(modus_string, "decryption") == 0) {
        modus = 1;
    } else {
        printf("Modus %s is not possible! Choose 'encryption' or 'decryption'!\n", modus_string);
        
        for (int i = 0; i < block_bits; i++) {
            free(sboxes_enc[i]);
            free(sboxes_dec[i]);
        }
        free(sboxes_enc);
        free(sboxes_dec);

        return -1;
    }

    if (access(input_file_name, F_OK) == -1) {
        printf("Input file '%s' was not found!\n", input_file_name);
        for (int i = 0; i < block_bits; i++) {
            free(sboxes_enc[i]);
            free(sboxes_dec[i]);
        }
        free(sboxes_enc);
        free(sboxes_dec);
        return -3;    
    }

    uint64_t block_size = 1<<block_bits;
    uint8_t* block = malloc(sizeof(uint8_t)*block_size);
    uint8_t* iv_block = malloc(sizeof(uint8_t)*block_size);
    memset(block, 0, block_size);
    FILE* fin = fopen(input_file_name, "rb");
    
    struct stat file_stat;
    if (fstat(fileno(fin),&file_stat)== -1) {
        printf("File stat failed!\n");
        for (int i = 0; i < block_bits; i++) {
            free(sboxes_enc[i]);
            free(sboxes_dec[i]);
        }
        free(sboxes_enc);
        free(sboxes_dec);

        free(block);
        free(iv_block);
        return -4;
    }
    uint64_t file_size = file_stat.st_size;

    FILE* fout = fopen(output_file_name, "wb");

    uint64_t idx = 0;
    if (modus == 0) {
        set_random_block(iv_block, block_size);
        fwrite(iv_block, 1, block_size, fout);
        fwrite(&block_bits, 1, sizeof(uint64_t), fout);
        fwrite(&file_size, 1, sizeof(uint64_t), fout);
        while (fread(block, 1, block_size, fin) > 0) {
            ((uint64_t*)iv_block)[0] = idx;
            calc_xor_block(block, iv_block, block, block_size);
            encrypt_block_jumps(block, block_size, sboxes_enc, block_bits);
            calc_xor_block(block, iv_block, block, block_size);
            fwrite(block, 1, block_size, fout);
            memset(block, 0, block_size);
            idx++;
        }
    } else {
        if (file_size < sizeof(uint64_t)*2+block_size) {
            printf("Inputfile should contain the block_bits and file_size numbers as uint64_t!\n");
            for (int i = 0; i < block_bits; i++) {
                free(sboxes_enc[i]);
                free(sboxes_dec[i]);
            }
            free(sboxes_enc);
            free(sboxes_dec);

            free(block);
            free(iv_block);

            return -5;
        }
        file_size -= sizeof(uint64_t)*2+block_size;
        fread(iv_block, 1, block_size, fin);
        uint64_t block_bits_read;
        uint64_t file_size_read;
        fread(&block_bits_read, 1, sizeof(uint64_t), fin);
        fread(&file_size_read, 1, sizeof(uint64_t), fin);
        if (block_bits_read != block_bits) {
            printf("For decryption block_bits and block_bits_read should be the same!\n");
            for (int i = 0; i < block_bits; i++) {
                free(sboxes_enc[i]);
                free(sboxes_dec[i]);
            }
            free(sboxes_enc);
            free(sboxes_dec);

            free(block);
            free(iv_block);

            return -6;
        }
        if (file_size < file_size_read || file_size-file_size_read > block_size) {
            printf("file_size_read is not correct!\n");
            for (int i = 0; i < block_bits; i++) {
                free(sboxes_enc[i]);
                free(sboxes_dec[i]);
            }
            free(sboxes_enc);
            free(sboxes_dec);

            free(block);
            free(iv_block);

            return -7;
        }
        uint64_t last_block_size = file_size_read % block_size;
        int read_blocks = file_size_read / block_size;
        while (fread(block, 1, block_size, fin) > 0) {
            ((uint64_t*)iv_block)[0] = idx;
            calc_xor_block(block, iv_block, block, block_size);
            decrypt_block_jumps(block, block_size, sboxes_dec, block_bits);
            calc_xor_block(block, iv_block, block, block_size);
            fwrite(block, 1, idx<read_blocks?block_size:last_block_size, fout);
            memset(block, 0, block_size);
            idx+=1;
        }
    }

    fclose(fin);
    fclose(fout);

    for (int i = 0; i < block_bits; i++) {
        free(sboxes_enc[i]);
        free(sboxes_dec[i]);
    }
    free(sboxes_enc);
    free(sboxes_dec);

    free(block);
    free(iv_block);

    printf("Successfully %scrypted file '%s' in file '%s'\n", (modus==0)?"en":"de", input_file_name, output_file_name);

    return 0;
}

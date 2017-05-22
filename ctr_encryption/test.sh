#! /bin/bash

make
./ctr_encryption bla.hex encryption test_file.hex test_file_en.hex
./ctr_encryption bla.hex decryption test_file_en.hex test_file_de.hex

./ctr_encryption bla.hex encryption python3.5 t/python3.5.enc
./ctr_encryption bla.hex decryption t/python3.5.enc python3.5.dec

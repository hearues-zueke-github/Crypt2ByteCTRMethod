#! /bin/bash

# param1   Name of the file
# return   Without the .c Extension
function getFileArgument {
  local name=$1
  local len=${#name}
  local subname=${name:0:$[len-2]}
  local fullarg="${name} -o ${subname}"
  echo $fullarg
}

# compiler="gcc"
# flags="-Wall"
# files=()
# files+=("utils.c")
# files+=("create_key_2B_1k.c")
# files+=("crypt_program_16B_1k.c")

# i=0
# # Get the len of Array files
# len=${#files[@]}
# while [ $i -lt $len ]; do
#   fullarg=$(getFileArgument ${files[$i]})
#   # Get the whole program with arguments
#   fullprogram="${compiler} ${flags} ${fullarg}"
#   # Print and evaluate the current program
#   echo $fullprogram
#   eval $fullprogram

#   i=$[i+1]
# done


# gcc -Wall create_key_2B_1k.c utils.c -o create_key_2B_1k
# gcc -Wall crypt_program_16B_1k.c utils.c -o crypt_program_16B_1k

compiler="gcc"
flags="-Wall"

commands=()
commands+=("create_key_2B_1k.c utils.c crypt_2B_1k.c -o create_key_2B_1k")
commands+=("crypt_program_2B_1k.c utils.c crypt_2B_1k.c -o crypt_program_2B_1k")
commands+=("check_key_2B_1k.c utils.c crypt_2B_1k.c -o check_key_2B_1k")
commands+=("test_blocks_generic.c utils.c -o test_blocks_generic")
commands+=("test_create_array.c utils.c -o test_create_array")
commands+=("test_pointers_generic.c utils.c -o test_pointers_generic")
commands+=("test_loop_max_depth.c utils.c -o test_loop_max_depth")
len=${#commands[@]}

# Compile all commands
i=0
while [ $i -lt $len ]; do
  fullprogram="${compiler} ${flags} ${commands[$i]}"
  # echo ""
  echo $fullprogram
  eval $fullprogram
  i=$[i+1]
done


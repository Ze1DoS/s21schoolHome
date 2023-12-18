#!/bin/zsh

CAT_EXECUTABLE="./../../cat/s21_cat"
ORIGINAL_CAT="/bin/cat"
flags=(-b -n -e -s -t)

RED='\033[0;31m' 
GREEN='\033[0;32m' 
NC='\033[0m'

SUCCESS="${GREEN}SUCCESS${NC}"
FAILED="${RED}FAILED${NC}"

list_input_files=('examples/*' examples/Example10-XML.txt examples/Example11-CSV.txt)

for input_files in "${list_input_files[@]}"; do

   s21_cat_output=$("$CAT_EXECUTABLE" $input_files)

        original_cat_output=$("$ORIGINAL_CAT" $input_files)

        s21_cat_output_file=$(mktemp)
        original_cat_output_file=$(mktemp)

        echo "$s21_cat_output" > "$s21_cat_output_file"
        echo "$original_cat_output" > "$original_cat_output_file"

        if [ "$s21_cat_output" == "$original_cat_output" ]; then
          result=$SUCCESS
        else
          result=$FAILED
        fi

        echo -e "CAT:$input_files:Test case with no flag: $result"

        rm -f "$s21_cat_output_file" "$original_cat_output_file"


  for flag in "${flags[@]}"; do
        s21_cat_output=$("$CAT_EXECUTABLE" "$flag" $input_files)

        original_cat_output=$("$ORIGINAL_CAT" "$flag" $input_files)

        s21_cat_output_file=$(mktemp)
        original_cat_output_file=$(mktemp)

        echo "$s21_cat_output" > "$s21_cat_output_file"
        echo "$original_cat_output" > "$original_cat_output_file"

        if [ "$s21_cat_output" == "$original_cat_output" ]; then
          result=$SUCCESS
        else
          result=$FAILED
        fi

        echo -e "CAT:$input_files: Test case with flag $flag: $result"

        rm -f "$s21_cat_output_file" "$original_cat_output_file"
  done
done

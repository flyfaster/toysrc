bits 64

section .rodata

global __binary_dict_txt_start
global __binary_dict_txt_end
global __binary_dict_txt_size

__binary_dict_txt_start:   incbin "dict.txt"
__binary_dict_txt_end:
__binary_dict_txt_size:    dd $-__binary_dict_txt_start

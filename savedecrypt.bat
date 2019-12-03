cd C:\DSCSTools\
if exist 0000.bin copy /y 0000.bin backup\0000.bin
if exist slot_0000.bin copy /y slot_0000.bin backup\slot_0000.bin
if exist system_graphic.bin copy /y system_graphic.bin backup\system_graphic.bin
if exist system_data.bin copy /y system_data.bin backup\system_data.bin
if exist 0000.bin DSCSTools --savedecrypt 0000.bin 0000_dec.bin
if exist slot_0000.bin DSCSTools --savedecrypt slot_0000.bin slot_0000_dec.bin
if exist system_graphic.bin DSCSTools --savedecrypt system_graphic.bin system_graphic_dec.bin
if exist system_data.bin DSCSTools --savedecrypt system_data.bin system_data_dec.bin
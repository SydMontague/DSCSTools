cd C:\DSCSTools\
if exist 0000.bin copy /y 0000.bin backup\0000.bin
if exist 0001.bin copy /y 0001.bin backup\0001.bin
if exist 0002.bin copy /y 0002.bin backup\0002.bin
if exist slot_0000.bin copy /y slot_0000.bin backup\slot_0000.bin
if exist slot_0001.bin copy /y slot_0001.bin backup\slot_0001.bin
if exist slot_0002.bin copy /y slot_0002.bin backup\slot_0002.bin
if exist system_graphic.bin copy /y system_graphic.bin backup\system_graphic.bin
if exist system_data.bin copy /y system_data.bin backup\system_data.bin
if exist 0000.bin DSCSTools --savedecrypt 0000.bin 0000_dec.bin
if exist 0001.bin DSCSTools --savedecrypt 0001.bin 0001_dec.bin
if exist 0002.bin DSCSTools --savedecrypt 0002.bin 0002_dec.bin
if exist slot_0000.bin DSCSTools --savedecrypt slot_0000.bin slot_0000_dec.bin
if exist slot_0001.bin DSCSTools --savedecrypt slot_0001.bin slot_0001_dec.bin
if exist slot_0002.bin DSCSTools --savedecrypt slot_0002.bin slot_0002_dec.bin
if exist system_graphic.bin DSCSTools --savedecrypt system_graphic.bin system_graphic_dec.bin
if exist system_data.bin DSCSTools --savedecrypt system_data.bin system_data_dec.bin

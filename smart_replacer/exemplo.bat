cls
call Release\sixel_smart_replacer.exe -i file_in/PAG05102017600039580270.rem -o file_out/PAG05102017600039580270.rem -r header,161-163,seq:REMESSA -r header,144-151,01112017 -r custom:8-8.14-14=3A,94-101,01112017 -r custom:8-8.14-14=3B,128-135,01112017

rem for %%f in (.\file_in\*.rem) do	@echo %%f



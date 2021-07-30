REM libcurl built by .\buildconf.bat && cd winbuild && nmake /f Makefile.vc mode=static
REM then copy from /build the /lib and /include folders

cl.exe /DEBUG /Zi /Ox /MD main.c /Iinclude .\lib\libcurl_a.lib  ws2_32.lib crypt32.lib advapi32.lib wldap32.lib Normaliz.lib

REM invocation for the modified libcurl lib which clears some buffers (Code https://github.com/nico-abram/curl/tree/zeromem )
REM cl.exe /DEBUG /Zi /Ox /MD main.c /Iinclude .\lib\libcurl_a_debug.lib  ws2_32.lib crypt32.lib advapi32.lib wldap32.lib Normaliz.lib

REM ignore this, some notes for myself in windbg
REM bp `main.c:20` "j ((poi(ptr))=0x000001dfe139a025) ''; 'gc' " 
REM bp `main.c:13` "j ((poi(memory_ptr)-poi(memory))>0n12000 & (poi(memory_ptr)-poi(memory))<0n13000) ''; 'gc' " 
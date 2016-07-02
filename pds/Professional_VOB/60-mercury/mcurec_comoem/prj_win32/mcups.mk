
mcups.dll: dlldata.obj mcu_p.obj mcu_i.obj
	link /dll /out:mcups.dll /def:mcups.def /entry:DllMain dlldata.obj mcu_p.obj mcu_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del mcups.dll
	@del mcups.lib
	@del mcups.exp
	@del dlldata.obj
	@del mcu_p.obj
	@del mcu_i.obj

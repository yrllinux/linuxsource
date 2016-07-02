
mculicenseps.dll: dlldata.obj mculicense_p.obj mculicense_i.obj
	link /dll /out:mculicenseps.dll /def:mculicenseps.def /entry:DllMain dlldata.obj mculicense_p.obj mculicense_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del mculicenseps.dll
	@del mculicenseps.lib
	@del mculicenseps.exp
	@del dlldata.obj
	@del mculicense_p.obj
	@del mculicense_i.obj

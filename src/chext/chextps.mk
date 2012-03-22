
chextps.dll: dlldata.obj chext_p.obj chext_i.obj
	link /dll /out:chextps.dll /def:chextps.def /entry:DllMain dlldata.obj chext_p.obj chext_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del chextps.dll
	@del chextps.lib
	@del chextps.exp
	@del dlldata.obj
	@del chext_p.obj
	@del chext_i.obj

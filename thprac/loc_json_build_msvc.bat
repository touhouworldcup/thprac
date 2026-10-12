@echo off
cl /I..\lib\yyjson /nologo /EHsc /Zi /Gy /O2 /std:c++20 loc_json.cpp /Fe:loc_json.exe /link /INCREMENTAL:NO /OPT:REF

@echo off
pushd Cm
call make_sources.bat
popd
pushd Syntax
call make_sources.bat
popd

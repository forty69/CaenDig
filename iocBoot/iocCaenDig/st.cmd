#!../../bin/linux-x86_64/CaenDig

## You may have to change CaenDig to something else
## everywhere it appears in this file

< envPaths

## Register all support components
dbLoadDatabase "../../dbd/CaenDig.dbd"
CaenDig_registerRecordDeviceDriver pdbbase

CaenDigConfig(CD1, 0, 0, 0, 0)

iocInit

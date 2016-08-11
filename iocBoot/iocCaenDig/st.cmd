#!../../bin/linux-x86_64/CaenDig

## You may have to change CaenDig to something else
## everywhere it appears in this file

< envPaths

## Register all support components
dbLoadDatabase "$(CAEN_DIG)/dbd/CaenDig.dbd"
CaenDig_registerRecordDeviceDriver pdbbase

CaenDigConfig(CD1, 0, 0, 0, 0)

dbLoadRecords("$(CAEN_DIG)/db/CaenDig.template", "P=CaenDig:,R=Dig1:, PORT=CD1")

iocInit

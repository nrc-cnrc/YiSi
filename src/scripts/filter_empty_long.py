import sys

SRC=open(sys.argv[1],"r").readlines()
REF=open(sys.argv[2],"r").readlines()
SOUT=open(sys.argv[3],"w")
ROUT=open(sys.argv[4],"w")

for i in range(len(SRC)):
    s=SRC[i].strip()
    r=REF[i].strip()
    ls = len(s)
    lr = len(r)
    if ((ls>0 and lr>0) and (ls <1000 and lr<1000)):
        SOUT.write(SRC[i])
        ROUT.write(REF[i])

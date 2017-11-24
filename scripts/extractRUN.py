#!/usr/bin/python3

import glob
import subprocess
import os 
from zenipy import calendar,message,error,warning,question,entry,password,file_selection,scale,color_selection,zlist



def run_and_show( CMD ):
    print("=================================================")
    print("|  ",CMD)
    print("-------------------------------------------------")
    out = subprocess.check_output( CMD.split() )
    print( out.decode("utf8").rstrip() )
    print("-------------------------------------------------")


print("Select the directory with raw runs:")

#dir=file_selection(directory=True)
dir="/home/ojr/DATA/NASTRO/20171115_dp26mg/201117:160945,nastro-0_16"
dir="/home/ojr/DATA/NASTRO/20171115_dp26mg/231117:164316,nastro-0_26"
#dir="./a_1"
#dir="/home/ojr/DATA/NASTRO/20171115_dp26mg/231117:164316,nastro-0"
print( dir )

print("i... getting basename and RUNNUM")

base=os.path.basename(dir)
try:
    runnum=int(base.split("_")[-1])
except ValueError:
    #Handle the exception
    print( '!... runnum Please, an integer')
    quit()
runnum="{:03d}".format(runnum)
print("i...", base, "RUN=",runnum)


#CMD=' du -bs '+dir+'/*_u | cut  -f 1 | awk \'{total = total + $1}END{printf("%d",total)}\' '

files = glob.glob(dir+"/*_u")
CMD='du -bs '+" ".join(files)
print(CMD)
psize = subprocess.Popen( CMD.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
#size = subprocess.check_output( CMD.split() ,shell=True)
#print("i... size of _u files is ",size)

communicateRes = psize.communicate()
stdOutValue, stdErrValue = communicateRes
res=stdOutValue.decode("utf8").rstrip().split("\n")
res=[ int(x.split()[0]) for x in res ]
print(res)
size=sum(res)
print('i... total expected size in bytes:',size)


runbase="RUN_"+str(runnum) 
dest=os.path.expanduser( "~/"+runbase)

if os.path.exists(dest):
    print("!... FILE EXISTS")
    run_and_show("ls -ltrh "+dest)
    esize=os.path.getsize( dest )
    print("i... file exists SIZE_OLD x  SIZE_EXPECT:: ",esize," x ", size)
    if esize==size:
        print("i... it must be the same file... doing nothing")
        #quit()
else:
    if os.path.exists(dest+".bz2"):
        print("i... file "+dest+" exists and is BZIPPED2 ...")
        run_and_show( "bunzip2 -v "+dest+".bz2")
        quit()
    else:
        files = glob.glob(dir+"/*.vme_u")
        files.sort(key=os.path.getmtime)   # SORT BY TIME

        with open( dest, "ab") as myfile:
            tsize=0
            for i in files:
                esize=os.path.getsize( i )
                tsize=tsize+esize
                print("+... {:30s} {:9d}   {:9d}/{:9d} bytes".format(os.path.basename(i),esize, tsize, size) )
                with open(i, "rb") as file2:
                    myfile.write(file2.read())
#################################################                    
print("\n\n")
run_and_show("ls -ltrh "+dest)

workdir=os.path.dirname( dir )+"_acq/plucqisition/"
outdir =os.path.dirname( dir )+"_root/"
outname=outdir+runbase+".root"

print("\n\n cd "+workdir)
print("\nroot -l")
#zhfile.xml
print('ZH_data(-1,"'+dest+'","'+outname+'","zhfile_26mg.xml") ')
print(".q\n\n")

print("cd")
print("ls -1 RUN_* | grep -v bz2 | xargs -n 1 pbzip2 -v -9 ")
print("cd -\n\n")

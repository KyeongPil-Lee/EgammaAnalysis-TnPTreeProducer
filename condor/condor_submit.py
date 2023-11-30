import os
import string

home = os.environ['HOME']

def makefile(njob):
        myfilename = "condor_test.condor"
        myfile = open(myfilename,"w")
        myfile.write("universe = vanilla\n")
        myfile.write("executable = "+home+"/work/private/Tag_and_Probe/tree_producer/lxplus9/CMSSW_13_3_0/src/EgammaAnalysis/TnPTreeProducer/condor/exec.sh\n")
        myfile.write("arguments = $(ProcId) 0 \n")
        #myfile.write("x509userproxy = X509_USER_PROXY")
        #myfile.write("use_x509userproxy = True")
        myfile.write("output = "+home+"/work/private/Tag_and_Probe/tree_producer/lxplus9/CMSSW_13_3_0/src/EgammaAnalysis/TnPTreeProducer/condor/logs/$(ProcId).out\n")
        myfile.write("error = "+home+"/work/private/Tag_and_Probe/tree_producer/lxplus9/CMSSW_13_3_0/src/EgammaAnalysis/TnPTreeProducer/condor/logs/$(ProcId).err\n")
        myfile.write("log  = "+home+"/work/private/Tag_and_Probe/tree_producer/lxplus9/CMSSW_13_3_0/src/EgammaAnalysis/TnPTreeProducer/condor/logs/$(ProcId).log\n")
        myfile.write("\n")
        myfile.write("request_memory = 1000 \n")
        myfile.write("+MaxRuntime = 244000\n")
        myfile.write("queue %d\n"%(njob))
        myfile.close()
   
if __name__ == "__main__":
        print("PREPARING JOBS")
        makefile(1)
        print("JOBS PREPARED")

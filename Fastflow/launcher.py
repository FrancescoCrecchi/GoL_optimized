from datetime import datetime
import subprocess
import os
import sys, getopt

#Plot part
import matplotlib
# Force matplotlib to not use any Xwindows backend.
matplotlib.use('Agg')
import matplotlib.pyplot as plt

#Directory and filename
directory = os.path.join(os.getcwd(), "Tests")
testsDayDir = directory + "/" + datetime.now().strftime("%d.%m.%Y")
testsDir = testsDayDir + "/" + datetime.now().strftime("%H:%M:%S")
outputFileName = testsDir + "/logs.txt"

#Test vars
maxThreads = -1
runs = 6 #how many tests per configuration
step = 10 #worker step
threshold = 32 #doubling worker threshold

mic = False #does the test is running on mic?
micN = -1

#GameOfLife default vars
dim = 1024
nIt = 1000
nW = 0

#Plot vars
Tseq = -1;
T_1 = -1;
Speedup_n = [] #List of couples (n, sp_n)
Scalability_n = [] #List of couples (n, scalab_n)
Efficiency_n = [] #List of couples (n, eff_n)
Latency_n = [] #List of couples (n, lat_n)

def AVG(times):
	times = sorted(times)
	#Removing the Min and the Max
	times = times[1:-1]
	#Compute the AVG
	return sum(times) / float(len(times))

def usage():
	print "python launcher.py [-h] -d boardDim -n numIter --maxThreads N --nRuns testsPerConfig --step s --threshold t [--mic 0/1]"

def main(argv):

	try:
		opts, args = getopt.getopt(argv,"hd:n:",["maxThreads=","nRuns=", "step=", "threshold=", "mic="])
	except getopt.GetoptError as err:
		 # print help information and exit:
		 print str(err) # will print something like "option -a not recognized"
		 usage()
		 sys.exit(2)

	global dim, nIt, maxThreads, runs, mic, outputFileName, Tseq, T_1, Speedup_n, Scalability_n, Efficiency_n, step, threshold

	for o, a in opts:
	 	if o == "-h":
			usage()
			sys.exit()
		elif o == "-d":	
			dim = int(a)
		elif o == "-n":
			nIt = int(a)
		elif o == "--maxThreads":
			maxThreads = int(a)
		elif o == "--nRuns":
			runs = int(a)	
		elif o == "--step":
			step = int(a)
		elif o == "--threshold":
			threshold = int(a)
		elif o == "--mic":
			mic = True
			micN = int(a)
		else:
			print str(err) # will print something like "option -a not recognized"
		     	usage()
		 	sys.exit(2)

	#Compile invoking the right target architecture
	cmd = ["make"]

	#Cleaning...
	cmd = cmd + ["clean"]
	p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
	out, err = p.communicate()
	assert err == ""

	#Then..
	cmd = ["make"]
	exe = "GameOfLife"
	others = "OTHERS="

	#Mic case
	if mic :
		others = others + "-mmic -DNO_DEFAULT_MAPPING"
		cmd = cmd + [exe] + [others]
		p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
		out, err = p.communicate()
		# assert err == ""
		print err

		cmd = ["scp", exe, "mic" + str(micN) + ":"]
		p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
		out, err = p.communicate()
		#assert err == ""
		print err

	else:
		p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
		out, err = p.communicate()
		#assert err == ""
		print err

	# Create dir (if any)
	if not os.path.exists(directory):
		os.makedirs(directory)
	if not os.path.exists(testsDayDir):
		os.makedirs(testsDayDir)	
	if not os.path.exists(testsDir):
		os.makedirs(testsDir)

	#Logging part
	with open(outputFileName, "w") as fout:
		nW = 0

		#Logging the test configuration
		fout.write("# Test configuration: \n")

		msg = "# Running on "
		if mic:
			msg = msg + "mic " + str(micN)	
		else :
			msg = msg + "host"

		fout.write(msg + "\n")
				
		fout.write("# GameOfLife vars: \n")
		fout.write("# \t-dim = " + str(dim) + "\n")
		fout.write("# \t-nIter = " + str(nIt) +"\n")
		fout.write("# Test vars:\n")
		fout.write("# \t-maxThreads = " + str(maxThreads) + "\n")
		fout.write("# \t-runs = " + str(runs) + "\n")
		fout.write("\n")


		cmd = ["./" + exe] + [str(dim), str(nIt), str(nW)]
		#Running the program and getting the results
		if mic :
			cmd = ["ssh", "mic"+str(micN)] + cmd

		#Main loop
		while nW <= maxThreads:

			#Setting the nW parameter
			if mic :
				cmd [5] = str(nW)
 			else:
				cmd[3] = str(nW)

			fout.write("# Testing with nW = " + str(nW) + " : \n")
			fout.flush() 

			elapsed_times = []
			for i in range(0, runs):

				#DEBUG PRINT!
				print cmd

				output,err = subprocess.Popen(cmd, stdout=subprocess.PIPE).communicate()
				assert err == None

				fout.write(output)
				fout.flush()

				elapsed = int(output)
				elapsed_times.append(elapsed)

			#Once you get all the running time compute the AVG
			avg_t = AVG(elapsed_times)
			fout.write("=> AVG elapsed time : " + str(avg_t) + "\n\n")
			fout.flush()

			if nW == 0:
				#Sequential version of the program
				Tseq = avg_t
				nW = nW + 1

				Speedup_n.append((nW, 0))
				Efficiency_n.append((nW, 1))
				Scalability_n.append((nW, 0))

			else:
				#Calculate sp(n) & eff(n)
				sp_n = Tseq/avg_t
				Speedup_n.append((nW, sp_n))
				Efficiency_n.append((nW, sp_n/nW))
				#Calculate scalab(n)
				if nW == 1:
					T_1 = avg_t
				Scalability_n.append((nW, T_1/avg_t))

				if nW >= threshold:
					nW = nW + step #Go step bt step!
				else:
					nW = nW * 2 #Twice the workers!
			Latency_n.append((nW, avg_t))

		fout.close()

		#Generate plots:
		
		#Latency plot
		aux = [list(t) for t in zip(*Latency_n)]
		n = aux[0]
		lat_n = aux[1]

		plt.title("Latency graph")
		plt.plot(n, lat_n, color='red', label="completion time")
		plt.xlabel("n")
		plt.ylabel("T(n)")
		plt.legend(loc='upper right')
		plt.savefig(testsDir + "/Latency")
		plt.close()

		#Speedup plot		
		aux = [list(t) for t in zip(*Speedup_n)]
		n = aux[0]
		sp_n = aux[1]

		plt.title("Speedup graph")
		plt.plot(n, sp_n, color='red', label="measured speedup")
		plt.plot(n, n, color='black', label="ideal speedup")
		plt.xlabel("n")
		plt.ylabel("Sp(n)")
		plt.legend(loc='upper right')
		plt.savefig(testsDir + "/Speedup")
		plt.close()

		#Scalability
		aux = [list(t) for t in zip(*Scalability_n)]
		n = aux[0]
		scalab_n = aux[1]

		plt.title("Scalability graph")
		plt.plot(n, scalab_n, color='red', label="measured scalab")
		plt.plot(n, n, color='black', label="ideal scalab")
		plt.xlabel("n")
		plt.ylabel("Scalab(n)")
		plt.legend(loc='upper right')
		plt.savefig(testsDir + "/Scalability")
		plt.close()

		#Efficiency
		aux = [list(t) for t in zip(*Efficiency_n)]
		n = aux[0]
		eff_n = aux[1]

		plt.title("Efficiency graph")
		plt.plot(n, eff_n, color='red', label="measured efficiency")
		aux = [1]*len(n)
		plt.plot(n, aux, color='black', label="ideal efficiency")
		plt.xlabel("n")
		plt.ylabel("Eff(n)")
		plt.legend(loc='upper right')
		plt.savefig(testsDir + "/Efficiency")
		plt.close()

if __name__ == "__main__":
	if len(sys.argv) < 2 :
		usage()
		sys.exit(2)

	main(sys.argv[1:])

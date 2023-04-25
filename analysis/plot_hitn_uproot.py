import sys
import uproot
#import matplotlib.pyplot as plt


# Get the input file name from the command line
if len(sys.argv) < 2:
    print("Usage: python plot_hitn.py <input file name>")
    sys.exit(1)

FILENAME = sys.argv[1]
# open the root file
file = uproot.open(FILENAME)

print(file.classnames)

# # Access the histogram
# hist = file["hit.n"]

# # Get the hisogram as numpy array
# data = hist.values()
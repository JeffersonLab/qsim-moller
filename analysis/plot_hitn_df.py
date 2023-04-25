"""
Plots the histogram of hit.n
"""

import sys
from ROOT import RDataFrame
from ROOT import TCanvas


# Get the input file name from the command line
if len(sys.argv) < 2:
    print("Usage: python plot_hitn.py <input file name>")
    sys.exit(1)

FILENAME = sys.argv[1]

# Create a ROOT dataframe
df = RDataFrame("T", FILENAME)

# Create a histogram
h = df.Histo1D("hit.n", "", 70, 0,70)
h.GetXaxis().SetRangeUser(0,70)

# Draw the histogram
c = TCanvas()
h.Draw()
c.Draw()

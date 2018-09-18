#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mlp
from scipy.optimize import curve_fit

from optparse import OptionParser

# initial settings
mlp.rcParams['axes.linewidth'] = 2
# give tick labels a little more space from axis
mlp.rcParams['xtick.major.pad'] = 12
mlp.rcParams['ytick.major.pad'] = 12

#---------------------------------------------------------------------------------------------------
def gaussian(x, mean, amplitude, standard_deviation):
    return amplitude * np.exp( - ((x - mean) / standard_deviation) ** 2)

#---------------------------------------------------------------------------------------------------
def readDataFromFile(name,i_column,min,max):

    with open(name+".dat","r") as file:
        data = file.read()
        
    values = []
    for line in data.split("\n"):
        f = line.split(' ')

        # do we have a valid column?
        if len(f)>i_column:
            value = float(f[i_column])
            if len(f)>i_column and value>min and value<max:
                values.append(float(f[i_column]))

    return values

#---------------------------------------------------------------------------------------------------
# define and get all command line arguments
parser = OptionParser()
parser.add_option("-n", "--name",  dest="name",  default='temp',                help="name of plot")
parser.add_option("-x", "--xtitle",dest="xtitle",default='Time [milli seconds]',help="x axis title")
parser.add_option("-y", "--ytitle",dest="ytitle",default='analog values',       help="y axis title")
(options, args) = parser.parse_args()

# get my data
values = readDataFromFile(options.name,8,3000,4000)

# bins
xmin = int(0.8*min(values))
xmax = int(1.2*max(values))
nbins = 25
print " min=%d, max=%d, nb=%d"%(xmin,xmax,nbins)

# make the histogram plot
n, bins, patches = plt.hist(values,bins=nbins,facecolor="lightblue",ec="blue")
plt.ylim(ymax=n.max()*1.1)

# prepare the fit
bin_centers = bins[:-1] + np.diff(bins) / 2
popt, covar = curve_fit(gaussian, bin_centers, n,
                    p0 = [sum(values)/len(values), sum(values), sum(values)/len(values)])
print popt
print covar
x_interval_for_fit = np.linspace(bins[0], bins[-1], 10000)
plt.plot(x_interval_for_fit, gaussian(x_interval_for_fit, *popt), linewidth=4)

# titles on the x and y axes
plt.xlabel('Volume [mm$^3$]', fontsize=26)
plt.ylabel('Number of Measurements', fontsize=26)

# tick marker sizes
ax = plt.gca()
ax.xaxis.set_tick_params(labelsize=20)
ax.yaxis.set_tick_params(labelsize=20)

ymin,ymax = ax.get_ylim()
dy = ymax-ymin
xmin,xmax = ax.get_xlim()
dx = xmax-xmin

plt.text(xmin+0.02*dx,ymax-0.06*dy, r'Mean: %.0f$\pm$%.0f'%(popt[0],covar[0][0]**0.5), fontsize=20)
plt.text(xmin+0.02*dx,ymax-0.12*dy, r'Width: %.0f$\pm$%.0f'%(popt[1],covar[1][1]**0.5), fontsize=20)
plt.text(xmin+0.02*dx,ymax-0.18*dy, r'Integral: %.0f$\pm$%.0f'%(popt[2],covar[2][2]**0.5), fontsize=20)
#plt.text(xmin+0.02*dx,ymax-0.08*dy, r'Volume [TB]: %5.2f'%(totalSizeTb), fontsize=20)
#plt.text(xmin+0.02*dx,ymax-0.12*dy, r'Exitcode:    %4d'%(ecode), fontsize=20)
#plt.text(xmin+0.02*dx,ymax-0.16*dy, r'Past:        %s'%(options.window), fontsize=20)
#
#plt.text(xmin-0.06*dx,ymin-0.10*dy, r'EC: %d'%(ecode), fontsize=34,color=color)

# save plot for later viewing
plt.subplots_adjust(top=0.99, right=0.99, bottom=0.2, left=0.07)
plt.savefig(options.name+".png",bbox_inches='tight',dpi=400)

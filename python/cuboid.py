#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mlp

from optparse import OptionParser

# initial settings
mlp.rcParams['axes.linewidth'] = 2

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
#plt.hist(values,bins='auto',facecolor='blue')
#n, bins, patches = plt.hist(values,nbins,range=(xmin,xmax),facecolor='blue')
n, bins, patches = plt.hist(values,bins=nbins,facecolor='blue')
plt.ylim(ymax=n.max()*1.1)
         
#ymin,ymax = ax.get_ylim()
#dy = ymax-ymin
#xmin,xmax = ax.get_xlim()
#dx = xmax-xmin
#
#plt.text(xmin+0.02*dx,ymax-0.04*dy, r'Transfers:   %d'%(len(sizes)), fontsize=20)
#plt.text(xmin+0.02*dx,ymax-0.08*dy, r'Volume [TB]: %5.2f'%(totalSizeTb), fontsize=20)
#plt.text(xmin+0.02*dx,ymax-0.12*dy, r'Exitcode:    %4d'%(ecode), fontsize=20)
#plt.text(xmin+0.02*dx,ymax-0.16*dy, r'Past:        %s'%(options.window), fontsize=20)
#
#plt.text(xmin-0.06*dx,ymin-0.10*dy, r'EC: %d'%(ecode), fontsize=34,color=color)

# save plot for later viewing
plt.savefig(options.name+".png",bbox_inches='tight',dpi=400)

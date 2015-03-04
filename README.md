## Junior Lab Tools

Some tools to perform data analysis. Some examples of data are also stored


## Requirements

### Operating system

root runs on all major operating systems like Linux/Windows/iOS.

### Packages

* CERN root package: https://root.cern.ch/drupal/content/downloading-root
* the gcc and more specifically gcc-c++ compilers are needed so root can create shared libraries of your programs, and
* the MitRootStyle package (https://github.com/cpausmit/MitRootStyle) needs to be installed and configured so the plots look reasonable without too much effort


## Installation

### Linux (ScientificLinux/Fedora/CentOS)

* sudo yum install root
* sudo yum install gcc gcc-c++
* mkdir JLab
* cd JLab
* git clone https://github.com/cpausmit/MitRootStyle
* cd MitRootStyle; ./install.sh; source setup.sh; cd ..
* git clone https://github.com/cpausmit/JLabTools

## Examples

* cd JLabTools/root/
* root -l ./cubiod.C+
* root -l ./cubiodUnbinned.C+

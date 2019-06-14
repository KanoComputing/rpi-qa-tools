#
# Wrapper module to import the qa-hid python interface modules
#

# The python interface module for qa-findimage
import sys
sys.path.append('../python/qa_tools')
from hid import *

# Use the Git development qa-hid tool, not the one installed on the system
import hid
hid.QA_HID = 'sudo ../tools/qa-hid/qa-hid'

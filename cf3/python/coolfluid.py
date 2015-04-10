import sys
if sys.version_info < (2, 5):
  import dl
else:
  import ctypes as dl
# This is needed to get MPI symbols to be visible
flags = sys.getdlopenflags()
sys.setdlopenflags(flags | dl.RTLD_GLOBAL)

# Import the C++ module
from libcoolfluid_python import *

# Import unit test module
from check import *

# restore the dlopen flags to default
sys.setdlopenflags(flags)

#initiate the CF3 environment. Note: there is no argv if executed from the ScriptEngine
if sys.__dict__.has_key('argv'):
  Core.initiate(sys.argv)

# shortcut for root
root = Core.root()

# shortcut for environment
env = Core.environment()

# shortcut for libraries
libs = Core.libraries()

# shortcut for tools
tools = Core.tools()

def interactive(banner="INTERACTIVE MODE - Use Ctrl-D to continue or exit() to exit"):
  """provides an interactive python shell when called in a script
  """
  import readline, code, sys, time
  # use exception trick to pick up the current frame
  try:
    raise None
  except:
    frame = sys.exc_info()[2].tb_frame.f_back

  # evaluate commands in current namespace
  namespace = frame.f_globals.copy()
  namespace.update(frame.f_locals)

  try:
    class_name = frame.f_locals['self'].__class__.__name__
    banner += "\nnote: keyboard() was called from within class \"" + class_name + "\"\n      use help("+class_name+") for more information on this class"
  except KeyError:
    class_name = None

  code.interact(banner="\n[%02d:%02d] " % (time.localtime().tm_hour, time.localtime().tm_min)+banner, local=namespace)

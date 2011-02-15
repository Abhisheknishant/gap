import commands, os, glob, sys, string

# parse options and set up environment

vars = Variables()
vars.Add('cflags', 'Supply additional CFLAGS', "")
vars.Add(BoolVariable("debug", "Set for debug builds", 0))
vars.Add(BoolVariable("profile",
  "Set for profiling with google performance tools", 0))
vars.Add(EnumVariable("abi", "Set to 32 or 64 depending on platform", 'auto',
  allowed_values=('32', '64', 'auto')))
vars.Add(EnumVariable("gmp", "Use GMP: yes, no, or system", "yes",
  allowed_values=("yes", "no", "system")))
vars.Add(EnumVariable("gc", "Use GC: yes, no, or system", "yes",
  allowed_values=("yes", "no", "system")))
vars.Add('preprocess', 'Use source preprocessor', "")

GAP = DefaultEnvironment(variables=vars)

compiler = GAP["CC"]
platform = commands.getoutput("cnf/config.guess")
build_dir = "bin/" + platform + "-" + compiler

try: os.makedirs(build_dir)
except: pass

try: os.unlink("bin/current")
except: pass

try: os.symlink(platform+"-"+compiler, "bin/current")
except: pass
 

def abi_from_config(config_header_file):
  global GAP
  try:
    config_file_contents = open(config_header_file).read()
  except:
    config_file_contents = ""

  abi = GAP["abi"]
  if "SIZEOF_VOID_P 8" in config_file_contents:
    abi = '64'
  elif "SIZEOF_VOID_P 4" in config_file_contents:
    abi = '32'
  return abi, config_file_contents != ""


# Create config.h if we don't have it and determine ABI

config_header_file = build_dir + "/config.h"
default_abi, has_config = abi_from_config(config_header_file)
changed_abi = GAP["abi"] != "auto" and GAP["abi"] != default_abi
if changed_abi:
  default_abi = GAP["abi"]

if not has_config or "config" in COMMAND_LINE_TARGETS or changed_abi:
  if GAP["abi"] != "auto":
    os.environ["CC"] = compiler 
    os.environ["CFLAGS"] = " -m" + GAP["abi"]
  os.system("./configure")
  os.system("cd "+build_dir+"; sh ../../cnf/configure.out")
  os.system("test -w bin/gap.sh && chmod ugo+x bin/gap.sh")
  if GAP["abi"] != "auto":
    del os.environ["CC"]
    del os.environ["CFLAGS"]

default_abi, has_config = abi_from_config(config_header_file)
if not has_config:
  print "=== Configuration file wasn't created ==="
  Exit(1)
GAP["abi"] = default_abi

GAP.Command("config", [], "") # Empty builder for the config target

# Which external libraries do we need?

conf = Configure(GAP)
libs = ["gmp", "gc"]
if conf.CheckLib("pthread"):
  libs.append("pthread")
if conf.CheckLib("rt"):
  libs.append("rt")
if conf.CheckLib("m"):
  libs.append("m")
if conf.CheckLib("dl"):
  libs.append("dl")
if GAP["gc"] == "system":
  if conf.CheckLib("gc"):
    compile_gc = False
  else:
    print "=== No system gc library found, using internal one. ==="
    compile_gc = True
elif GAP["gc"] == "yes":
  compile_gc = True
else:
  compile_gc = False
  libs.remove("gc")
if GAP["gmp"] == "system":
  if conf.CheckLib("gmp"):
    compile_gmp = False
  else:
    print "=== No system gmp library found, using internal one. ==="
    compile_gmp = True
elif GAP["gmp"] == "yes":
  compile_gmp = True
else:
  compile_gmp = False
  libs.remove("gmp")
conf.Finish()
if GAP["profile"]:
  libs.append("profiler")

# Construct command line options

cflags = ""
if not GAP["debug"]:
  cflags = "-O2"
cflags += " -g"
cflags += " -m"+GAP["abi"]
cflags += " -DCONFIG_H"
if "gc" in libs:
  cflags += " -DGC_THREADS"
else:
  cflags += " -DDISABLE_GC"
if "gmp" in libs:
  cflags += " -DUSE_GMP"

if GAP["cflags"]:
  cflags += " " + GAP["cflags"]


GAP.Append(CCFLAGS=cflags, LINKFLAGS=cflags)

# Building external libraries

abi_path = "extern/"+GAP["abi"]+"bit"
GAP.Append(RPATH=os.path.join(os.getcwd(), abi_path, "lib"))

def build_external(libname):
  global abi_path
  if GetOption("help") or GetOption("clean"):
    return
  try:
    os.makedirs(abi_path)
  except:
    pass
  jobs = GetOption("num_jobs")
  if os.system("cd " + abi_path + ";"
          + "tar xzf ../" + libname + ".tar.gz;"
	  + "cd " + libname + ";"
	  + "./configure --prefix=$PWD/.. && make -j " + str(jobs) + " && make install") != 0:
    print "=== Failed to build " + libname + " ==="
    sys.exit(1)

if compile_gmp and glob.glob(abi_path + "/lib/libgmp.*") == []:
  os.environ["ABI"] = GAP["abi"]
  build_external("gmp-4.2.2")
  del os.environ["ABI"]

if compile_gc and glob.glob(abi_path + "/lib/libgc.*") == []:
  if commands.getoutput("uname -s") != "Darwin":
    os.environ["CC"] = GAP["CC"]+" -m"+GAP["abi"]
  else:
    os.environ["CC"] = GAP["CC"]+" -m"+GAP["abi"] + " -D_XOPEN_SOURCE"
  build_external("gc-7.2alpha2")
  del os.environ["CC"]

# Adding paths for external libraries

options = { }
include_path = build_dir+":." # for config.h
if compile_gc or compile_gmp:
  options["LIBPATH"] = abi_path + "/lib"
  include_path += ":" + abi_path + "/include"
options["CPPPATH"] = include_path
options["OBJPREFIX"] = "../" + build_dir + "/"

# uname file generator

sysinfo_os, sysinfo_host, sysinfo_version, sysinfo_os_full, sysinfo_arch = \
  os.uname()

sysinfo_header = [
  "#ifndef _SYSINFO_H",
  "#define SYSINFO_OS " + sysinfo_os,
  "#define SYSINFO_OS_"+ string.upper(sysinfo_os)+" 1",
  "#define SYSINFO_VERSION " + sysinfo_version,
  "#define SYSINFO_ARCH " + sysinfo_arch,
  "#define SYSINFO_ARCH_"+ string.upper(sysinfo_arch)+" 1",
  "#endif /* _SYSINFO_H */"
]

def SysInfoBuilder(target, source, env):
  file = open(target[0].get_abspath(), "w")
  for line in sysinfo_header:
    file.write(line+"\n")
  file.close()

# Building binary from source

preprocess = GAP["preprocess"]

source = glob.glob("src/*.c")
source.remove("src/gapw95.c")
if preprocess:
  import os, stat
  try: os.mkdir("gen")
  except: pass
  pregen = glob.glob("src/*.[ch]")
  gen = map(lambda s: "gen/"+s[4:], pregen)
  for i in range(len(pregen)):
    GAP.Command(gen[i], pregen[i],
        preprocess + " $SOURCE >$TARGET")
  source = map(lambda s: "gen/"+s[4:], source)

source.append("extern/jenkins/jhash.o")

GAP.Command("extern/include/jhash.h", "extern/jenkins/jhash.h",
            "cp -f $SOURCE $TARGET")
GAP.Command("extern/include/sysinfo.h", [], SysInfoBuilder)
GAP.Object("extern/jenkins/jhash.o", "extern/jenkins/jhash.c")
GAP.Program(build_dir + "/gap", source, LIBS=libs, **options)

#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# EXEFS_SRC is the optional input directory containing data copied into exefs, if anything this normally should only contain "main.npdm".
#---------------------------------------------------------------------------------
TARGET		:=	sys-tweak
BUILD		:=	build
OUTDIR		:=	out
RESOURCES	:=	res
SOURCES		:=	src lib/inih
DATA		:=	data
INCLUDES	:=	src lib/inih
EXEFS_SRC	:=	exefs_src

DEFINES	:=	-DDISABLE_IPC -DTARGET="\"$(TARGET)\"" -DATMOSPHERE_BOARD_NINTENDO_SWITCH -DATMOSPHERE_IS_STRATOSPHERE \
			-DATMOSPHERE_ARCH_ARM64 -DATMOSPHERE_CPU_ARM_CORTEX_A57 -DATMOSPHERE_BOARD_NINTENDO_NX -DATMOSPHERE_OS_HORIZON

#---------------------------------------------------------------------------------
# options for features
#---------------------------------------------------------------------------------
FEATURES := NSVM_SAFE NSAM_CONTROL
TOGGLES := LOGGING
#---------------------------------------------------------------------------------
ENABLED_FEATURES := $(foreach feat,$(FEATURES),$(if $(or $(FEAT_$(feat)),$(FEAT_ALL)),$(feat)))
DEFINES += $(foreach feat,$(ENABLED_FEATURES),-DHAVE_$(feat))
ENABLED_TOGGLES := $(foreach toggle,$(TOGGLES),$(if $(or $(TOGL_$(feat)),$(TOGL_ALL)),$(toggle)))
DEFINES += $(foreach toggle,$(ENABLED_TOGGLES),-DENABLE_$(toggle))
#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -O2 -ffunction-sections \
			$(ARCH) $(DEFINES)

CFLAGS	+=	$(INCLUDE) -D__SWITCH__

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++17

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lnx

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBAMS := $(CURDIR)/lib/ams
LIBSTRAT_SOURCES := sf/cmif sf/hipc os os/impl result hos diag sm ams spl/smc

LIBDIRS	:= $(PORTLIBS) $(LIBNX) $(LIBAMS)/libstratosphere $(LIBAMS)/libvapours

SOURCES += $(foreach dir,$(LIBSTRAT_SOURCES),lib/ams/libstratosphere/source/$(dir))

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(OUTDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export BUILD_EXEFS_SRC := $(TOPDIR)/$(EXEFS_SRC)

export APP_JSON := $(TOPDIR)/$(RESOURCES)/sysmodule.json

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -n "$(ENABLED_FEATURES)" ] || (echo "Please enable at least one feature with FEAT_X env vars, where X can be (ALL $(FEATURES))" 1>&2; exit 1)
	@echo "* ENABLED_FEATURES: $(ENABLED_FEATURES)"
	@echo "* ENABLED_TOGGLES: $(ENABLED_TOGGLES)"
	#@$(MAKE) -C $(LIBAMS)/libstratosphere
	@[ -d $@ ] || mkdir -p $@
	@[ -d $(OUTDIR) ] || mkdir -p $(OUTDIR)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	#@$(MAKE) -C $(LIBAMS) clean
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).kip $(TARGET).nsp $(TARGET).npdm $(TARGET).nso $(TARGET).elf $(OUTDIR)


#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

all: $(OUTPUT).nsp

$(OUTPUT).nsp: $(OUTPUT).nso $(OUTPUT).npdm

$(OUTPUT).elf: $(OFILES)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------

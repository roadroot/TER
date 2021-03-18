SRCDIR    = src
BUILDDIR  = build
TARGET    = trafic

SRCEXT    = cpp
ALLSRCS   = $(wildcard $(SRCDIR)/*.$(SRCEXT))
SATSRCS   = $(wildcard $(SRCDIR)/*Solver.$(SRCEXT))
SOURCES   = $(filter-out $(SATSRCS),  $(ALLSRCS))
OBJECTS   = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

CXX       = g++
CFLAGS    = -Wall -pthread -Wno-parentheses -Wno-sign-compare -std=c++14
COPTIMIZE = -O3
LFLAGS    = -Wall -pthread
IFLAGS    = -I include
LDFLAGS   = -pthread

CFLAGS   += $(COPTIMIZE)
CFLAGS   += -D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS
CFLAGS   += -D CONE_OF_INFLUENCE
CFLAGS   += -D DCCO_VIA_DCAD
CFLAGS   += -D SECO_VIA_SEGR
CFLAGS   += -D DSCO_VIA_DCGR
CFLAGS   += -D GR_IN_ST
CFLAGS   += -D AD_IN_ST
#CFLAGS   += -D CO_IN_ST
CFLAGS   += -D ST_EXISTS_STG
#CFLAGS   += -D ST_EXISTS_SST
#CFLAGS   += -D ADD_UNITS
CFLAGS   += -D CHECK_POS
CFLAGS   += -D CHECK_NEG

SAT_SOLVER = glucose
#SAT_SOLVER = glucose-inc
#SAT_SOLVER = cryptominisat

GLUCOSE    = lib/glucose-syrup-4.1
CMSAT      = lib/cryptominisat

ifeq ($(SAT_SOLVER), glucose)
	CFLAGS  += -D SAT_GLUCOSE
	IFLAGS  += -I $(GLUCOSE)
	LFLAGS  += -lz
	OBJECTS += $(BUILDDIR)/GlucoseSolver.o $(BUILDDIR)/Solver.oc
	SAT_DIR  = $(GLUCOSE)
else ifeq ($(SAT_SOLVER), glucose-inc)
	CFLAGS  += -D SAT_GLUCOSE -D INCREMENTAL
	IFLAGS  += -I $(GLUCOSE)
	LFLAGS  += -lz
	OBJECTS += $(BUILDDIR)/GlucoseSolver.o $(BUILDDIR)/Solver.oc
	SAT_DIR  = $(GLUCOSE)
else ifeq ($(SAT_SOLVER), cryptominisat)
	CFLAGS  += -D SAT_CMSAT
	IFLAGS  += -I $(CMSAT)/build/include
	LFLAGS  += -Wl,-rpath,$(CMSAT)/build/lib -L $(CMSAT)/build/lib -lcryptominisat5
	OBJECTS += $(BUILDDIR)/CryptoMiniSatSolver.o
	CMSAT_BUILD = $(CMSAT)/build
else
	$(error No SAT solver specified.)
endif

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@echo "$(CXX) $(OBJECTS) -o $(TARGET) $(LFLAGS)"; $(CXX) $(OBJECTS) -o $(TARGET) $(LFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling..."
	@mkdir -p $(BUILDDIR)
	@echo "$(CXX) $(CFLAGS) $(IFLAGS) -c -o $@ $<"; $(CXX) $(CFLAGS) $(IFLAGS) -c -o $@ $<

$(BUILDDIR)/%.oc: $(SAT_DIR)/core/%.cc
	@echo "Compiling core..."
	@echo "$(CXX) $(CFLAGS) $(IFLAGS) -c -o $@ $<"; $(CXX) $(CFLAGS) $(IFLAGS) -c -o $@ $<

#.ONESHELL:
cmsat:
	@echo "Compiling CryptoMiniSat..."
	cd lib && wget https://github.com/msoos/cryptominisat/archive/45b803706d75981538fe0f8ca46bb5fd23ff30db.zip && \
	unzip 45b803706d75981538fe0f8ca46bb5fd23ff30db.zip && rm 45b803706d75981538fe0f8ca46bb5fd23ff30db.zip && \
	mv cryptominisat-45b803706d75981538fe0f8ca46bb5fd23ff30db cryptominisat && \
	cd cryptominisat && \
	mkdir -p build && cd build && \
	cmake .. && \
	make

clean:
	@echo "Cleaning..."
	@echo "rm -rf $(BUILDDIR) $(TARGET)"; rm -rf $(BUILDDIR) $(TARGET) && if [ -d $(CMSAT) ]; then rm -rf $(CMSAT_BUILD); fi

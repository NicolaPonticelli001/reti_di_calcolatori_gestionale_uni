# Specifica il compilatore
CXX = g++
# Opzioni per il compilatore
CXXFLAGS = -Wall -std=c++11

# Percorsi delle cartelle
SRCDIR = .
LIBDIR = ./libs
INCLUDEDIR = $(LIBDIR)/include
OBJDIR = $(LIBDIR)/obj

# Ottieni tutti i file main nella cartella "codice"
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
# Genera i nomi degli eseguibili
EXECUTABLES = $(patsubst $(SRCDIR)/%.cpp,%,$(SOURCES))

# Ottieni tutti i file .cpp presenti nella cartella "libs/src"
LIBSOURCES = $(wildcard $(LIBDIR)/src/*.cpp)
# Genera i nomi dei file .o corrispondenti
LIBOBJECTS = $(patsubst $(LIBDIR)/src/%.cpp,$(OBJDIR)/%.o,$(LIBSOURCES))

# Target predefinito
all: $(EXECUTABLES)

# Regola generica per la compilazione degli eseguibili
$(EXECUTABLES): % : $(SRCDIR)/%.cpp $(LIBOBJECTS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) $< $(LIBOBJECTS) -lsqlite3 -o $@

# Regola per la compilazione dei file .o delle librerie
$(OBJDIR)/%.o: $(LIBDIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Pulisci tutti gli eseguibili e i file .o
clean:
	rm -f $(EXECUTABLES) $(OBJDIR)/*.o

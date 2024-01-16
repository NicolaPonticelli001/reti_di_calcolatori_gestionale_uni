# Nome del programma
TARGET = server_uni

# Compilatore C++
CXX = g++

# Opzioni di compilazione
CXXFLAGS = -std=c++11 -Wall

# Librerie da collegare
LDLIBS = -lsqlite3

# Directory per i file header (.hpp)
INC_DIR = libs/include

# Directory per i file di implementazione (.cpp)
SRC_DIR = libs/src

# Directory per i file oggetto (.o)
OBJ_DIR = libs/obj

# Trova tutti i file sorgente (.cpp) nella directory SRC_DIR
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Genera la lista dei file oggetto (.o) basata sui file sorgente
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Aggiungi il file principale al programma
SOURCES += server_uni.cpp

# Regola per generare l'eseguibile
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDLIBS)

# Regola generica per generare i file oggetto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC_DIR)/%.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(INC_DIR)

# Pulizia dei file intermedi e dell'eseguibile
clean:
	rm -f $(TARGET) $(OBJECTS)

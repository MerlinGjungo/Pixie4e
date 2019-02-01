# Define in your make file
# TARGET = your_target
# SRC = list_of_sources_for_target

OBJ_EXT=.o
OD=$(ARCH)/

CPP_SRCS = $(filter %.cpp,$(SRC))
C_SRCS = $(filter %.c,$(SRC))
CPP_OBJS = $(addsuffix $(OBJ_EXT), $(addprefix $(OD), $(basename $(notdir $(CPP_SRCS)))))
C_OBJS = $(addsuffix $(OBJ_EXT), $(addprefix $(OD), $(basename $(notdir $(C_SRCS)))))

ifeq ($(DEBUG),1)
    DEBFLAGS = -g -DDEBUG
else
    DEBFLAGS = -O2
endif

C_FLAGS += -I"$(WDSRC)/include" -I"$(WDSRC)" -D$(ARCH) $(DEBFLAGS) -c
# need also thread support for interrupt handling
L_FLAGS += -lpthread -pthread

all: $(ARCH) $(OD)$(TARGET)

CPP_FLAGS += $(C_FLAGS)

$(CPP_OBJS) : $(OD)%$(OBJ_EXT): $(filter %$(basename $(notdir $@)).cpp,$(CPP_SRCS))
	$(CXX) $(CPP_FLAGS) -o $@ $(filter %$(basename $(notdir $@)).cpp,$(CPP_SRCS))
$(C_OBJS) : $(OD)%$(OBJ_EXT): $(filter %$(basename $(notdir $@)).c,$(C_SRCS))
	$(CC) $(C_FLAGS) -o $@ $(filter %$(basename $(notdir $@)).c,$(C_SRCS))

$(ARCH):
	-@ mkdir $@ &

$(OD)$(TARGET): $(C_OBJS) $(CPP_OBJS)
	$(CXX) $(L_FLAGS) $^ -o $@

clean:
	rm -rf *~ core $(OD)$(TARGET) $(CPP_OBJS) $(C_OBJS) $(ARCH)/*


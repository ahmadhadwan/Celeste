CC=gcc
CFLAGS=-Ofast -Wall
LFLAGS=-lglfw -lm -ldl -lfreetype -lpthread -lopenal -lvorbisfile

AR=ar

BIN_DIR=./bin/
OBJS_DIR=$(BIN_DIR)objs/
EXT_OBJS_DIR=$(OBJS_DIR)ext/
CORE_DIR=./core/
INC_DIR=./include/
EXT_DIR=./ext/
SANDBOX_DIR=./sandbox/

MACROS=-DCELESTE_GLFW -DCELESTE_GORILLA_AUDIO -DCELESTE_AUDIO_ASYNC -DCELESTE_PTHREAD
CFLAGS+=-I$(INC_DIR) -I$(EXT_DIR) $(MACROS)

CORE_SRCS=$(wildcard $(CORE_DIR)*.c)
CORE_OBJS=$(addprefix $(OBJS_DIR), $(notdir $(CORE_SRCS:.c=.o)))

GLAD_SRCS=$(wildcard $(EXT_DIR)glad/*.c)
GLAD_OBJS=$(addprefix $(EXT_OBJS_DIR), $(notdir $(GLAD_SRCS:.c=.o)))

STB_SRCS=$(wildcard $(EXT_DIR)stb/*.c)
STB_OBJS=$(addprefix $(EXT_OBJS_DIR), $(notdir $(STB_SRCS:.c=.o)))

FREETYPE2_DIR=/usr/include/freetype2/
FREETYPE_GL_SRCS=$(wildcard $(EXT_DIR)freetype-gl/*.c)
FREETYPE_GL_OBJS=$(addprefix $(EXT_OBJS_DIR), $(notdir $(FREETYPE_GL_SRCS:.c=.o)))

GORILLA_COMMON_SRCS=$(wildcard $(EXT_DIR)gorilla/common/*.c)
GORILLA_COMMON_OBJS=$(addprefix $(EXT_OBJS_DIR), $(notdir $(GORILLA_COMMON_SRCS:.c=.o)))
GORILLA_DEVICES_SRCS=$(wildcard $(EXT_DIR)gorilla/devices/ga_openal.c)
GORILLA_DEVICES_OBJS=$(addprefix $(EXT_OBJS_DIR), $(notdir $(GORILLA_DEVICES_SRCS:.c=.o)))
GORILLA_SRCS=$(wildcard $(EXT_DIR)gorilla/*.c)
GORILLA_OBJS=$(addprefix $(EXT_OBJS_DIR), $(notdir $(GORILLA_SRCS:.c=.o))) $(GORILLA_COMMON_OBJS) $(GORILLA_DEVICES_OBJS)

CELESTE_CORE=$(BIN_DIR)libceleste.a
SANDBOX=$(BIN_DIR)sandbox

# For Windows Builds
WINCC=x86_64-w64-mingw32-gcc
WINMACROS=-DCELESTE_GLFW -DCELESTE_GORILLA_AUDIO -DCELESTE_AUDIO_ASYNC
WINCFLAGS=-Ofast -Wall -I$(INC_DIR) -I$(EXT_DIR) $(WINMACROS)
WINLFLAGS=-lglfw3 -lgdi32 -lm -lpthread -lOpenAL32 -lfreetype-6
WINAR=x86_64-w64-mingw32-ar
WINBIN_DIR=./winbuild/
WINOBJS_DIR=$(WINBIN_DIR)objs/
WINEXT_OBJS_DIR=$(WINOBJS_DIR)ext/
WINCORE_SRCS=$(wildcard $(CORE_DIR)*.c)
WINCORE_OBJS=$(addprefix $(WINOBJS_DIR), $(notdir $(WINCORE_SRCS:.c=.o)))
WINGLAD_SRCS=$(wildcard $(EXT_DIR)glad/*.c)
WINGLAD_OBJS=$(addprefix $(WINEXT_OBJS_DIR), $(notdir $(WINGLAD_SRCS:.c=.o)))
WINSTB_SRCS=$(wildcard $(EXT_DIR)stb/*.c)
WINSTB_OBJS=$(addprefix $(WINEXT_OBJS_DIR), $(notdir $(WINSTB_SRCS:.c=.o)))
WINFREETYPE2_DIR=./ext/freetype2/
WINFREETYPE2_OBJS=$(addprefix $(WINEXT_OBJS_DIR), $(notdir $(WINFREETYPE2_SRCS:.c=.o)))
WINFREETYPE_GL_SRCS=$(wildcard $(EXT_DIR)freetype-gl/*.c)
WINFREETYPE_GL_OBJS=$(addprefix $(WINEXT_OBJS_DIR), $(notdir $(WINFREETYPE_GL_SRCS:.c=.o)))
WINGORILLA_COMMON_SRCS=$(wildcard $(EXT_DIR)gorilla/common/*.c)
WINGORILLA_COMMON_OBJS=$(addprefix $(WINEXT_OBJS_DIR), $(notdir $(WINGORILLA_COMMON_SRCS:.c=.o)))
WINGORILLA_DEVICES_SRCS=$(wildcard $(EXT_DIR)gorilla/devices/ga_openal.c)
WINGORILLA_DEVICES_OBJS=$(addprefix $(WINEXT_OBJS_DIR), $(notdir $(WINGORILLA_DEVICES_SRCS:.c=.o)))
WINGORILLA_SRCS=$(wildcard $(EXT_DIR)gorilla/*.c)
WINGORILLA_OBJS=$(addprefix $(WINEXT_OBJS_DIR), $(notdir $(WINGORILLA_SRCS:.c=.o))) $(WINGORILLA_COMMON_OBJS) $(WINGORILLA_DEVICES_OBJS)
WINOGG_SRCS=$(wildcard $(EXT_DIR)ogg/*.c)
WINOGG_OBJS=$(addprefix $(WINEXT_OBJS_DIR), $(notdir $(WINOGG_SRCS:.c=.o)))
WINVORBIS_SRCS=$(wildcard $(EXT_DIR)vorbis/*.c)
WINVORBIS_OBJS=$(addprefix $(WINEXT_OBJS_DIR), $(notdir $(WINVORBIS_SRCS:.c=.o)))
WINCELESTE_CORE=$(WINBIN_DIR)libceleste.a
WINSANDBOX=$(WINBIN_DIR)sandbox.exe

all: dirs $(CELESTE_CORE) $(SANDBOX)

$(CELESTE_CORE): $(CORE_OBJS) $(GLAD_OBJS) $(STB_OBJS) $(FREETYPE_GL_OBJS) $(GORILLA_OBJS)
	$(AR) -rcs $@ $^

$(OBJS_DIR)%.o: $(CORE_DIR)%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(SANDBOX): $(OBJS_DIR)sandbox.o $(CELESTE_CORE)
	$(CC) -o $@ $< -L$(BIN_DIR) -lceleste $(LFLAGS)

$(OBJS_DIR)%.o: $(SANDBOX_DIR)%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(EXT_OBJS_DIR)%.o: $(EXT_DIR)glad/%.c
	$(CC) -c -Ofast -I$(EXT_DIR) -o $@ $<

$(EXT_OBJS_DIR)%.o: $(EXT_DIR)stb/%.c
	$(CC) -c -Ofast -I$(EXT_DIR) -o $@ $<

$(EXT_OBJS_DIR)%.o: $(EXT_DIR)freetype-gl/%.c
	$(CC) -c -Ofast -I$(EXT_DIR) -I$(FREETYPE2_DIR) -o $@ $<

$(EXT_OBJS_DIR)%.o: $(EXT_DIR)gorilla/%.c
	$(CC) -c -Ofast -DENABLE_OPENAL -I$(EXT_DIR) -o $@ $<

$(EXT_OBJS_DIR)%.o: $(EXT_DIR)gorilla/common/%.c
	$(CC) -c -Ofast -DENABLE_OPENAL -I$(EXT_DIR) -o $@ $<

$(EXT_OBJS_DIR)%.o: $(EXT_DIR)gorilla/devices/%.c
	$(CC) -c -Wno-incompatible-pointer-types -Ofast -DENABLE_OPENAL -I$(EXT_DIR) -o $@ $<

# Windows
win: windirs $(WINCELESTE_CORE) $(WINSANDBOX)

$(WINCELESTE_CORE): $(WINCORE_OBJS) $(WINGLAD_OBJS) $(WINSTB_OBJS) $(WINFREETYPE_GL_OBJS) $(WINGORILLA_OBJS) $(WINOGG_OBJS) $(WINVORBIS_OBJS)
	$(WINAR) -rcs $@ $^

$(WINOBJS_DIR)%.o: $(CORE_DIR)%.c
	$(WINCC) -c $(WINCFLAGS) -I$(EXT_DIR) $(WINCFLAGS) -o $@ $<

$(WINSANDBOX): $(WINOBJS_DIR)sandbox.o $(WINCELESTE_CORE)
	$(WINCC) -o $@ $< -L$(WINBIN_DIR) -lceleste $(WINLFLAGS)

$(WINOBJS_DIR)%.o: $(SANDBOX_DIR)%.c
	$(WINCC) -c $(WINCFLAGS) -o $@ $<

$(WINEXT_OBJS_DIR)%.o: $(EXT_DIR)glad/%.c
	$(WINCC) -c -Ofast -I$(EXT_DIR) -o $@ $<

$(WINEXT_OBJS_DIR)%.o: $(EXT_DIR)stb/%.c
	$(WINCC) -c -Ofast -I$(EXT_DIR) -o $@ $<

$(WINEXT_OBJS_DIR)%.o: $(EXT_DIR)freetype-gl/%.c
	$(WINCC) -c -Ofast -I$(EXT_DIR) -I$(WINFREETYPE2_DIR) -o $@ $<

$(WINEXT_OBJS_DIR)%.o: $(EXT_DIR)gorilla/%.c
	$(WINCC) -c -Ofast -DENABLE_OPENAL -I$(EXT_DIR) -o $@ $<

$(WINEXT_OBJS_DIR)%.o: $(EXT_DIR)gorilla/common/%.c
	$(WINCC) -c -Ofast -I$(EXT_DIR) -o $@ $<

$(WINEXT_OBJS_DIR)%.o: $(EXT_DIR)gorilla/devices/%.c
	$(WINCC) -c -Wno-incompatible-pointer-types -Ofast -I$(EXT_DIR) -o $@ $<

$(WINEXT_OBJS_DIR)%.o: $(EXT_DIR)ogg/%.c
	$(WINCC) -c -Ofast -I$(EXT_DIR) -o $@ $<

$(WINEXT_OBJS_DIR)%.o: $(EXT_DIR)vorbis/%.c
	$(WINCC) -c -Ofast -I$(EXT_DIR) -I$(EXT_DIR)vorbis/ -o $@ $<

dirs:
	@if [ ! -d $(BIN_DIR) ]; then echo "Creating $(BIN_DIR) directory.." ; mkdir $(BIN_DIR); fi
	@if [ ! -d $(OBJS_DIR) ]; then echo "Creating $(OBJS_DIR) directory.." ; mkdir $(OBJS_DIR); fi
	@if [ ! -d $(EXT_OBJS_DIR) ]; then echo "Creating $(EXT_OBJS_DIR) directory.." ; mkdir $(EXT_OBJS_DIR); fi

windirs:
	@if [ ! -d $(WINBIN_DIR) ]; then echo "Creating $(WINBIN_DIR) directory.." ; mkdir $(WINBIN_DIR); fi
	@if [ ! -d $(WINOBJS_DIR) ]; then echo "Creating $(WINOBJS_DIR) directory.." ; mkdir $(WINOBJS_DIR); fi
	@if [ ! -d $(WINEXT_OBJS_DIR) ]; then echo "Creating $(WINEXT_OBJS_DIR) directory.." ; mkdir $(WINEXT_OBJS_DIR); fi

clean:
	@echo "Cleaning $(BIN_DIR) directory.."
	@rm -f $(CORE_OBJS) $(CELESTE_CORE) $(SANDBOX)

winclean:
	@echo "Cleaning $(WINBIN_DIR) directory.."
	@rm -f $(WINCORE_OBJS) $(WINCELESTE_CORE) $(WINSANDBOX)

.PHONY: all win dirs windirs clean winclean

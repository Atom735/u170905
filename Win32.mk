.PHONY : all objDir clean

CC := $(TOOL_CC)
AR := $(TOOL_AR)

OUT_NAME := a.exe

CPPFLAGS := -DDEBUG -D_DEBUG -DUNICODE -D_UNICODE
CFLAGS := -O0 -g3 -Wall -std=c99
LDFLAGS := -lOpenGL32 -lGDI32

DIR_INCLUDES := \
	src \
    $(wildcard libs/*/include)

DIR_LIBS :=
    # libs

SRC_FILES := \
	src/main.c \
	src/log.c \
	src/sm.c \
	src/utilities.c \
	src/gl.c

OBJECTS := $(addprefix obj/, $(addsuffix .o, $(basename $(SRC_FILES))))
OBJECTS_DIRS := $(sort $(dir $(OBJECTS)))

CFLAGS  += $(addprefix -I, $(DIR_INCLUDES))
LDFLAGS += $(addprefix -L, $(DIR_LIBS))

all : objDir $(OUT_NAME)

$(OUT_NAME) : $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJECTS) : obj/%.o : %.c
	$(CC) -o $@ -c $< $(CPPFLAGS) $(CFLAGS)

objDir :
	-mkdir -p $(OBJECTS_DIRS)

clean :
	-rm -rf obj $(OUT_NAME)



# AR
# Программа работы с архивами; по умолчанию, `ar'.
# AS
# Ассемблер; по умолчанию, `as'.
# CC
# Компилятор Си; по умолчанию, `cc'.
# CXX
# Компилятор C++; по умолчанию, `g++'.
# CO
# Программа для извлечения файлов из RCS; по умолчанию, `co'.
# CPP
# Препроцессор языка Си, выдающий результат на стандартный вывод; по умолчанию, `$(CC) -E'.
# FC
# Препроцессор и компилятор для Фортрана и Ратфора; по умолчанию, `f77'.
# GET
# Программа для извлечения файлов из SCCS; по умолчанию, `get'.
# LEX
# Программа для преобразования Lex-грамматики в текст на языках Си или Ратфор; по умолчанию - `lex'.
# PC
# Компилятор Паскаля; по умолчанию, `pc'.
# YACC
# Программа для преобразования Yacc-грамматики в текст на Си; по умолчанию - `yacc'.
# YACCR
# Программа для преобразования Yacc-грамматики в текст на языке Ратфор; по умолчанию - `yacc -r'.
# MAKEINFO
# Программа для преобразования исходного файла формата Texinfo в файл формата Info; по умолчанию, `makeinfo'.
# TEX
# Программа для преобразования исходных файлов на TeX в файлы формата DVI; по умолчанию - `tex'.
# TEXI2DVI
# Программа, преобразующая исходные файлы в формате Texinfo, в DVI-файлы программы TeX; по умолчанию - `texi2dvi'.
# WEAVE
# Программа, преобразующая текст из формата Web в формат TeX; по умолчанию - `weave'.
# CWEAVE
# Программа, преобразующая текст на Си-Web в формат TeX; по умолчанию - `cweave'.
# TANGLE
# Программа, преобразующая текст на Web в Паскаль; по умолчанию - `tangle'.
# CTANGLE
# Программа, преобразующая текст на Си-Web в текст на Си; по умолчанию - `ctangle'.
# RM
# Команда удаления файла; по умолчанию, `rm -f'.
# Ниже приведена таблица переменных, содержащих дополнительные параметры для перечисленных выше программ. По умолчанию, значением этих переменных является пустая строка (если не указано другое).
#
# ARFLAGS
# Опции, передаваемые программе, манипулирующей с архивными файлам; по умолчанию `rv'.
# ASFLAGS
# Дополнительные параметры, передаваемые ассемблеру (при его явном вызове для файлов `.s' и `.S').
# CFLAGS
# Дополнительные параметры, передаваемые компилятору Си.
# CXXFLAGS
# Дополнительные параметры, передаваемые компилятору C++.
# COFLAGS
# Дополнительные параметры, передаваемые программе co (входящей в систему RCS).
# CPPFLAGS
# Дополнительные параметры, передаваемые препроцессору языка Си и программам, его использующим (компиляторам Си и Фортрана).
# FFLAGS
# Дополнительные параметры для компилятора Фортрана.
# GFLAGS
# Дополнительные параметры, передаваемые программе get (входящей в систему SCCS).
# LDFLAGS
# Дополнительные параметры, передаваемые компиляторам, когда предполагается вызов компоновщика `ld'.
# LFLAGS
# Дополнительные параметры, передаваемые программе Lex.
# PFLAGS
# Дополнительные параметры, передаваемые компилятору Паскаля.
# RFLAGS
# Дополнительные параметры, передаваемые компилятору Фортрана при компиляции программ на Ратфоре.
# YFLAGS
# Дополнительные параметры, передаваемые программе Yacc.

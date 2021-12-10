default-main/main.d:
main/main.o  : main/main.cpp h/server-socket/server.h h/Base/basedef.h \
 h/Base/err.h
	@echo "         CXX   $(notdir $<)"
	@$(CXX) $(PROGECTOR_CFLAG) $(PROGECTOR_DFLAG) $(PROGECTOR_INCLUDES) -Imain/ -c $(filter %.cpp, $^) -o $(filter %.o, $@)

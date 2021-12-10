default-framework/server-socket/server.d:
framework/server-socket/server.o  : framework/server-socket/server.cpp h/server-socket/server.h \
 h/Base/basedef.h h/Base/err.h
	@echo "         CXX   $(notdir $<)"
	@$(CXX) $(PROGECTOR_CFLAG) $(PROGECTOR_DFLAG) $(PROGECTOR_INCLUDES) -Iframework/server-socket/ -c $(filter %.cpp, $^) -o $(filter %.o, $@)

default-framework/client-socket/client.d:
framework/client-socket/client.o  : framework/client-socket/client.cpp h/Base/basedef.h \
 h/Base/err.h h/client-socket/client.h
	@echo "         CXX   $(notdir $<)"
	@$(CXX) $(PROGECTOR_CFLAG) $(PROGECTOR_DFLAG) $(PROGECTOR_INCLUDES) -Iframework/client-socket/ -c $(filter %.cpp, $^) -o $(filter %.o, $@)

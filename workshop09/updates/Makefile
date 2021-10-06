# capture goal1 then make the remainder PHONY and depend on donothing
goal1:=$(firstword $(MAKECMDGOALS))
unique_goals:=$(filter-out $(goal1),$(sort $(MAKECMDGOALS)))
ifneq ($(words $(unique_goals)),0)
    .PHONY: $(unique_goals)
    $(eval $(unique_goals):donothing ;@true)
endif

# default target is all
all: test 
	@true

clean:
	@rm -f *.out tests/chips/*-tested-*

donothing notest regenerate working:
	@true

compiled: HelloWorld/Main.vm Sum/Main.vm Average/Main.vm SDev/Main.vm

%/Main.vm: %/*.jack
	JackCompiler.sh $(<D)

less Less show Show:
	@bash bin/run-tests.bash $(MAKECMDGOALS)

live test Test: compiled
	@bash bin/run-tests.bash $(MAKECMDGOALS)

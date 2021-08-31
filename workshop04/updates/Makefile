# # capture goal1 then make the remainder PHONY and depend on donothing
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
	@rm -f *.out

donothing notest working:
	@true

less Less live regenerate show Show test Test:
	@bash bin/run-tests.bash $(MAKECMDGOALS)


// test header so that variables named in workshop
// questions will have known addresses we can test
	@x
	@y
	@sum
	@ptr
	@numbers
	@arr
// add your instructions after this line
	@x
	M=0
	@sum
	M=0
(LOOP)
	@x
	D=M
	@10
	D=D-A // D = x - 10
	@END
	D;JGE // if (x - 10) >= 0 goto end
	@x
	D=M
	@sum
	M=D+M // sum = sum +x;
	@x
	M=M+1 // x = x +1
	@LOOP
	0;JMP
(END)
	@END
	0;JMP
	


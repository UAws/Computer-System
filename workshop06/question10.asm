// test header so that variables named in workshop
// questions will have known addresses we can test
	@x
	@y
	@sum
	@ptr
	@numbers
	@arr
// add your instructions after this line
	@ptr 
	A=M
	D=M
	@x 
	M=D // x = *ptr
	@ptr
	M=M+1 // ptr++
(END)
	@END
	0;JMP
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
	M=1
	@10
	D=A
	@y
	M=D
	@sum
	M=0
	@x
	D=M
	@sum
	M=D+M
	@y
	D=M
	@sum
	M=D+M
(END)
	@END
	0;JMP



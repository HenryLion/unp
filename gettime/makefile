all : daytimetcpcli daytimetcpsrv
.PHONY : all
daytimetcpcli : daytimetcpcli.c
	gcc -o $@ $^ -lunp
daytimetcpsrv : daytimetcpsrv.c
	gcc -o $@ $^ -lunp

clean:
	rm daytimetcpcli daytimetcpsrv

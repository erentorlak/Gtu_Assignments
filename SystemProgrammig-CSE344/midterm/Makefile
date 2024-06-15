.PHONY: all clean

all: neHosServer neHosClient

neHosServer: neHosServer.c
	gcc -w -pthread neHosServer.c -o neHosServer

neHosClient: neHosClient.c
	gcc -w -pthread neHosClient.c -o neHosClient

clean:
	rm -f *.txt neHosServer neHosClient

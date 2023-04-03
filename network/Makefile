all:
	gcc server.c ./cJSON/cJSON.c json_parse.c -o server -lm -I./cJSON
	gcc client.c ./cJSON/cJSON.c json_parse.c -o client -lm -I./cJSON
#	gcc ./cJSON/cJSON.c json_parse.c -o parse -lm -I./cJSON

clean:
	rm -rf a.out server client

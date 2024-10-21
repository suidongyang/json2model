all: json2model
json2model: json2model.o property_list.o

.PHONY:
clean: 
	rm -r *.o json2model

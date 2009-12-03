SUBDIRS = src

all:
	@for dir in $(SUBDIRS) ; do (cd $$dir ; $(MAKE)) ; done

ircd:
	@cd src ; $(MAKE) ; cd ..

clean:
	@for dir in $(SUBDIRS) ; do (cd $$dir ; $(MAKE) clean) ; done
